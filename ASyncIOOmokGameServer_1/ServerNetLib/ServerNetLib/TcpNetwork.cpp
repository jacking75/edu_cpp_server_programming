#pragma once
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include "TcpNetwork.h"
#include "Define.h"
#include <thread>
#include <vector>

namespace NServerNetLib
{
	TcpNetwork::~TcpNetwork()
	{
		for (auto& client : m_ClientSessionPool)
		{
			delete client;
		}

		WSACleanup();
		DestroyThread();
	}


	NET_ERROR_CODE TcpNetwork::Init(OmokServerLib::Config* config, Logger* pLogger)
	{
		m_Config = config;
		m_pRefLogger = pLogger;

		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);


		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (INVALID_SOCKET == m_ListenSocket)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		maxIOWorkerThreadCount = config->maxIOWorkerThreadCount;

		auto result = BindandListen(config->port, config->backLogCount);
		if (result != NET_ERROR_CODE::NONE)
		{
			return result;
		}

		auto sessionPoolSize = CreateSessionPool(config->maxClientCount + config->extraClientCount);

		m_pRefLogger->info("Network Init | SessionPoolSize : {}", sessionPoolSize);
		return NET_ERROR_CODE::NONE;
	}


	NET_ERROR_CODE TcpNetwork::BindandListen(short port, int backlogCount)
	{
		SOCKADDR_IN		stServerAddr;
		stServerAddr.sin_family = AF_INET;
		stServerAddr.sin_port = htons(port);

		stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);


		int nRet = bind(m_ListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
		if (0 != nRet)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
		}

		nRet = listen(m_ListenSocket, backlogCount);
		if (0 != nRet)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_FAIL;
		}

		m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, maxIOWorkerThreadCount);
		if (NULL == m_IOCPHandle)
		{
			m_pRefLogger->error("BindandListen | CreateIoCompletionPort(): {}", GetLastError());
			return NET_ERROR_CODE::SERVER_CREATE_IO_COMPLETION_PORT_FAIL;
		}

		auto hIOCPHandle = CreateIoCompletionPort((HANDLE)m_ListenSocket, m_IOCPHandle, (UINT32)0, 0);
		if (nullptr == hIOCPHandle)
		{
			m_pRefLogger->error("BindandListen | listen socket IOCP bind(): {}", WSAGetLastError());
			return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_IOCP_BIND_FAIL;
		}

		m_pRefLogger->info("BindListen | Listen. mIOCPHandle({})", m_IOCPHandle);
		return NET_ERROR_CODE::NONE;
	}

	bool TcpNetwork::Run()
	{
		bool bRet = CreateWokerThread();
		if (false == bRet)
		{
			return false;
		}

		bRet = CreateAccepterThread();
		if (false == bRet)
		{
			return false;
		}

		return true;
	}

	void TcpNetwork::DestroyThread()
	{
		m_IsWorkerRun = false;
		CloseHandle(m_IOCPHandle);

		for (auto& th : m_IOWorkerThreads)
		{
			if (th.joinable())
			{
				th.join();
			}
		}

		m_IsAccepterRun = false;
		closesocket(m_ListenSocket);

		if (m_AccepterThread.joinable())
		{
			m_AccepterThread.join();
		}
	}

	bool TcpNetwork::SendMsg(const int sessionIndex, const short bodySize, char* pMsg)
	{
		auto pClient = GetClientInfo(sessionIndex);
		return pClient->SendMsg(bodySize, pMsg);
	}

    void TcpNetwork::OnConnect(const UINT32 sessionIndex)
	{
		m_pRefLogger->info("OnConnect | connect client index : {}", sessionIndex);
		AddSystemPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CONNECT_SESSION, 0, nullptr);
	}

	void TcpNetwork::OnClose(const UINT32 sessionIndex)
	{
		m_pRefLogger->info("OnClose | close client index : {}", sessionIndex);
		AddSystemPacketQueue(sessionIndex, (short)PACKET_ID::NTF_SYS_CLOSE_SESSION, 0, nullptr);
	}

	void TcpNetwork::OnReceive(const UINT32 sessionIndex, const UINT32 size, char* pData)
	{
		m_pRefLogger->info("OnReceive | receive client index : {}, datasize : {}", sessionIndex, size);
		auto session = GetClientInfo(sessionIndex);
		session->SetPacketData(size, pData);

		std::lock_guard<std::mutex> guard(m_InComingPacketQueueMutex);
		m_InComingPacketUserIndexQueue.push_back(sessionIndex);
	}

	std::optional <RecvPacketInfo> TcpNetwork::GetPacketData()
	{
		int userIndex = 0;

		std::lock_guard<std::mutex> guard(m_InComingPacketQueueMutex);
		if (m_InComingPacketUserIndexQueue.empty())
		{
			return std::nullopt;
		}

		userIndex = m_InComingPacketUserIndexQueue.front();
		m_InComingPacketUserIndexQueue.pop_front();
		
		auto pUser = m_ClientSessionPool[userIndex];
		auto packetData = pUser->GetPacket();
		packetData.SessionIndex = userIndex;
		return packetData;
	}

	std::optional <RecvPacketInfo> TcpNetwork::GetSystemPacketData()
	{
		std::lock_guard<std::mutex> lock(m_ReceivePacketMutex);

		if (m_PacketQueue.empty() == false)
		{
			RecvPacketInfo packetInfo;
			m_PacketQueue.try_pop(packetInfo);
			return packetInfo;
		}

		return std::nullopt;
	}

	void TcpNetwork::AddSystemPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos)
	{
		std::lock_guard<std::mutex> lock(m_ReceivePacketMutex);

		RecvPacketInfo packetInfo;
		packetInfo.SessionIndex = sessionIndex;
		packetInfo.PacketId = pktId;
		packetInfo.PacketBodySize = bodySize;
		packetInfo.pRefData = pDataPos;

		m_PacketQueue.push(packetInfo);
	}

	int TcpNetwork::CreateSessionPool(const int maxClientCount)
	{
		for (int i = 0; i < maxClientCount; ++i)
		{
			auto client = new TcpSession;
			client->Init(i, m_IOCPHandle);

			m_ClientSessionPool.push_back(client);
		}

		return maxClientCount;
	}

	bool TcpNetwork::CreateWokerThread()
	{
		auto threadCount = (maxIOWorkerThreadCount * 2) + 1;
		for (UINT32 i = 0; i < threadCount; i++)
		{
			m_IOWorkerThreads.emplace_back([this]() { WokerThread(); });
		}

		return true;
	}

	TcpSession* TcpNetwork::GetClientInfo(const UINT32 clientIndex)
	{
		return m_ClientSessionPool[clientIndex];
	}

	bool TcpNetwork::CreateAccepterThread()
	{
		m_AccepterThread = std::thread([this]() { AccepterThread(); });

		return true;
	}

	void TcpNetwork::WokerThread()
	{
		TcpSession* pClientInfo = nullptr;
		BOOL bSuccess = TRUE;
		DWORD dwIoSize = 0;
		LPOVERLAPPED lpOverlapped = NULL;

		while (m_IsWorkerRun)
		{
			bSuccess = GetQueuedCompletionStatus(m_IOCPHandle,
				&dwIoSize,					
				(PULONG_PTR)&pClientInfo,		
				&lpOverlapped,				
				INFINITE);					


			if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
			{
				m_IsWorkerRun = false;
				continue;
			}

			if (NULL == lpOverlapped)
			{
				continue;
			}

			auto pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		
			if (FALSE == bSuccess || (0 == dwIoSize && IOOperation::ACCEPT != pOverlappedEx->m_eOperation))
			{
				m_pRefLogger->info("WokerThread | socket disconnect : {}", (int)pClientInfo->GetSock());
				CloseSocket(pClientInfo->GetIndex(), false);
				continue;
			}


			if (IOOperation::ACCEPT == pOverlappedEx->m_eOperation)
			{
				pClientInfo = GetClientInfo(pOverlappedEx->SessionIndex);
				if (pClientInfo->AcceptCompletion())
				{
					++m_ClientCnt;
					OnConnect(pClientInfo->GetIndex());
				}
				else
				{
					CloseSocket(pClientInfo->GetIndex(), true);
				}
			}

			else if (IOOperation::RECV == pOverlappedEx->m_eOperation)
			{
				OnReceive(pClientInfo->GetIndex(), dwIoSize, pClientInfo->RecvBuffer());
				pClientInfo->BindRecv();
			}

			else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
			{
				pClientInfo->SendCompleted(dwIoSize);
			}

			else
			{
				m_pRefLogger->info("WokerThread | client exception : {}", pClientInfo->GetIndex());
			}
		}
	}

	void TcpNetwork::AccepterThread()
	{
		while (m_IsAccepterRun)
		{
			auto curTimeSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

			for (auto client : m_ClientSessionPool)
			{
				if (client->IsConnectd())
				{
					continue;
				}

				if ((UINT64)curTimeSec < client->GetLatestClosedTimeSec())
				{
					continue;
				}

				auto diff = curTimeSec - client->GetLatestClosedTimeSec();
				if (diff <= RE_USE_SESSION_WAIT_TIMESEC)
				{
					continue;
				}

				client->PostAccept(m_ListenSocket, curTimeSec);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(32));
		}
	}

	void TcpNetwork::CloseSocket(const int sessionIndex, bool isForce)
	{
		auto session = GetClientInfo(sessionIndex);

		if (session->IsConnectd() == false)
		{
			return;
		}

		session->Close(isForce);

		OnClose(sessionIndex);
	}

	void TcpNetwork::Release()
	{
		WSACleanup();
	}
};