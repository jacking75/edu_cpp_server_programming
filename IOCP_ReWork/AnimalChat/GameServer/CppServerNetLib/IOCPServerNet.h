#pragma once

#include <vector>
#include <thread>
#include <mutex>

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "CommonDefine.h"
#include "Performance.h"
#include "Connection.h"
#include "MessagePool.h"
#include "MiniDump.h"

namespace NetLib
{
	//TODO 너무 크니 적절하게 분해하자

	//TODO 모든 세션을 주기적으로 조사 기능 구현하기
	// - 연결이 끊어졌는데 재 사용하고 있는지 조사하기
	
	struct OVERLAPPED_EX;
	class Connection;
	class MessagePool;

	class IOCPServerNet
	{
	public:
		IOCPServerNet() {}
		~IOCPServerNet() {}

	public:
		virtual E_NET_RESULT StartServer(NetConfig netConfig)
		{
			LogFuncPtr((int)LogLevel::eINFO, "Start Server Completion");
						
			m_NetConfig = netConfig;
			
			auto result = CreateListenSocket();
			if (result != E_NET_RESULT::Success)
			{
				return result;
			}

			result = CreateHandleIOCP();
			if (result != E_NET_RESULT::Success)
			{
				return result;
			}

			if (!CreateMessageManager())
			{
				return E_NET_RESULT::Fail_Create_Message_Manager;
			}

			if (!LinkListenSocketIOCP())
			{
				return E_NET_RESULT::Fail_Link_IOCP;
			}

			if (!CreateConnections())
			{
				return E_NET_RESULT::Fail_Create_Connection;
			}

			if (!CreatePerformance())
			{
				return E_NET_RESULT::Fail_Create_Performance;
			}

			if (!CreateWorkThread())
			{
				return E_NET_RESULT::Fail_Create_WorkThread;
			}

			LogFuncPtr((int)LogLevel::eINFO, "Start Server Completion");
			return E_NET_RESULT::Success;
		}

		virtual void EndServer(void)
		{
			LogFuncPtr((int)LogLevel::eINFO, "IOCPServer::EndServer - Start");

			if (m_hWorkIOCP != INVALID_HANDLE_VALUE)
			{
				m_IsRunWorkThread = false;
				CloseHandle(m_hWorkIOCP);
								
				for (int i = 0; i < m_WorkThreads.size(); ++i)
				{
					if (m_WorkThreads[i].get()->joinable())
					{
						m_WorkThreads[i].get()->join();
					}
				}												
			}
			

			if (m_hLogicIOCP != INVALID_HANDLE_VALUE)
			{
				//PostQueuedCompletionStatus(m_hLogicIOCP, 0, 0, nullptr);
				CloseHandle(m_hLogicIOCP);
			}

			if (m_ListenSocket != INVALID_SOCKET)
			{
				closesocket(m_ListenSocket);
				m_ListenSocket = INVALID_SOCKET;
			}

			WSACleanup();


			DestoryConnections();

			if (m_pMsgPool != nullptr)
			{
				delete m_pMsgPool;
			}

			LogFuncPtr((int)LogLevel::eINFO, "IOCPServer::EndServer - Completion");
		}

		
		bool ProcessNetworkMessage(OUT INT8& msgOperationType, OUT INT32& connectionIndex, char* pBuf, OUT INT16& copySize)
		{
			Message* pMsg = nullptr;
			Connection* pConnection = nullptr;
			DWORD ioSize = 0;

			auto result = GetQueuedCompletionStatus(
				m_hLogicIOCP,
				&ioSize,
				reinterpret_cast<PULONG_PTR>(&pConnection),
				reinterpret_cast<LPOVERLAPPED*>(&pMsg),
				INFINITE);

			if (result == false)
			{
				return false;
			}

			
			switch (pMsg->OperationType)
			{
			case OP_CONNECTION:
				DoPostConnection(pConnection, pMsg, msgOperationType, connectionIndex);
				break;
			case OP_CLOSE:
				//TODO 재 사용에 딜레이를 주도록 한다. 이유는 재 사용으로 가능 도중 IOCP 워크 스레드에서 이 세션이 호출될 수도 있다. 
				DoPostClose(pConnection, pMsg, msgOperationType, connectionIndex);
				break;
			case OP_RECV_PACKET:
				DoPostRecvPacket(pConnection, pMsg, msgOperationType, connectionIndex, pBuf, copySize, ioSize);
				m_pMsgPool->DeallocMsg(pMsg);
				break;
			}
						
			return true;
		}

		void SendPacket(const INT32 connectionIndex, const void* pSendPacket, const INT16 packetSize)
		{
			auto pConnection = GetConnection(connectionIndex);
			if (pConnection == nullptr)
			{
				return;
			}
			
			char* pReservedSendBuf = nullptr;
			auto result = pConnection->ReservedSendPacketBuffer(&pReservedSendBuf, packetSize);
			if (result == E_NET_RESULT::ReservedSendPacketBuffer_Not_Connected)
			{
				return;
			}
			else if (result == E_NET_RESULT::ReservedSendPacketBuffer_Empty_Buffer)
			{
				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
				return;
			}

			CopyMemory(pReservedSendBuf, pSendPacket, packetSize);

			if (pConnection->PostSend(packetSize) == false)
			{
				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
			}
		}

		int GetMaxPacketSize(void) { return m_NetConfig.m_MaxPacketSize; }
		int GetMaxConnectionCount(void) { return m_NetConfig.m_MaxConnectionCount; }
		int GetPostMessagesThreadsCount(void) { return m_NetConfig.m_PostMessagesThreadsCount; }


	private:		
		E_NET_RESULT CreateListenSocket(void)
		{
			WSADATA wsaData;
			auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (result != 0)
			{
				return E_NET_RESULT::fail_create_listensocket_startup;
			}

			m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
			if (m_ListenSocket == INVALID_SOCKET)
			{
				return E_NET_RESULT::fail_create_listensocket_socket;
			}

			SOCKADDR_IN	addr;
			ZeroMemory(&addr, sizeof(SOCKADDR_IN));

			addr.sin_family = AF_INET;
			addr.sin_port = htons(m_NetConfig.m_PortNumber);
			addr.sin_addr.s_addr = htonl(INADDR_ANY);

			if (::bind(m_ListenSocket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR)
			{
				return E_NET_RESULT::fail_create_listensocket_bind;
			}

			if (::listen(m_ListenSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				return E_NET_RESULT::fail_create_listensocket_listen;
			}

			return E_NET_RESULT::Success;
		}

		E_NET_RESULT CreateHandleIOCP(void)
		{
			m_hWorkIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
			if (m_hWorkIOCP == INVALID_HANDLE_VALUE)
			{
				return E_NET_RESULT::fail_handleiocp_work;
			}

			m_hLogicIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);
			if (m_hLogicIOCP == INVALID_HANDLE_VALUE)
			{
				return E_NET_RESULT::fail_handleiocp_logic;
			}

			return E_NET_RESULT::Success;
		}

		bool CreateMessageManager(void)
		{
			m_pMsgPool = new MessagePool(m_NetConfig.m_MaxMessagePoolCount, m_NetConfig.m_ExtraMessagePoolCount);
			if (!m_pMsgPool->CheckCreate())
			{
				return false;
			}

			return true;
		}

		bool LinkListenSocketIOCP(void)
		{
			auto hIOCPHandle = CreateIoCompletionPort(
				reinterpret_cast<HANDLE>(m_ListenSocket),
				m_hWorkIOCP,
				0,
				0);

			if (hIOCPHandle == INVALID_HANDLE_VALUE || m_hWorkIOCP != hIOCPHandle)
			{
				return false;
			}

			return true;
		}


		bool CreateConnections(void)
		{
			ConnectionNetConfig config;
			config.MaxSendConnectionBufferCount = m_NetConfig.m_MaxSendConnectionBufferCount;
			config.MaxRecvConnectionBufferCount = m_NetConfig.m_MaxRecvConnectionBufferCount;
			config.MaxRecvOverlappedBufferSize = m_NetConfig.m_MaxRecvOverlappedBufferSize;
			config.MaxSendOverlappedBufferSize = m_NetConfig.m_MaxSendOverlappedBufferSize;

			for (int i = 0; i < m_NetConfig.m_MaxConnectionCount; ++i)
			{
				auto pConnection = new Connection();
				pConnection->Init(m_ListenSocket, i, config);
				m_Connections.push_back(pConnection);
			}

			return true;
		}

		void DestoryConnections(void)
		{
			for (int i = 0; i < m_NetConfig.m_MaxConnectionCount; ++i)
			{
				delete m_Connections[i];				
			}
		}

		Connection* GetConnection(const INT32 connectionIndex)
		{
			if (connectionIndex < 0 || connectionIndex >= m_NetConfig.m_MaxConnectionCount)
			{
				return nullptr;
			}

			return m_Connections[connectionIndex];
		}


		bool CreatePerformance(void)
		{
			if (m_NetConfig.m_PerformancePacketMillisecondsTime == INVALID_VALUE)
			{
				return false;
			}

			m_Performance = std::make_unique<Performance>();
			m_Performance->Start(m_NetConfig.m_PerformancePacketMillisecondsTime);

			return true;
		}

		bool CreateWorkThread(void)
		{
			if (m_NetConfig.m_WorkThreadCount == INVALID_VALUE)
			{
				return false;
			}

			for (int i = 0; i < m_NetConfig.m_WorkThreadCount; ++i)
			{
				m_WorkThreads.push_back(std::make_unique<std::thread>([&]() {WorkThread(); }));
			}

			return true;
		}

		void WorkThread(void)
		{
			while (m_IsRunWorkThread)
			{
				DWORD ioSize = 0;
				OVERLAPPED_EX* pOverlappedEx = nullptr;
				Connection* pConnection = nullptr;

				//TODO: 모든 GetQueuedCompletionStatus를 GetQueuedCompletionStatusEx 버전을 사용하도록 변경. 
				auto result = GetQueuedCompletionStatus(
					m_hWorkIOCP,
					&ioSize,
					reinterpret_cast<PULONG_PTR>(&pConnection),
					reinterpret_cast<LPOVERLAPPED*>(&pOverlappedEx),
					INFINITE);

				if (pOverlappedEx == nullptr)
				{
					if (WSAGetLastError() != 0 && WSAGetLastError() != WSA_IO_PENDING)
					{
						char logmsg[128] = { 0, };
						sprintf_s(logmsg, "IOCPServer::WorkThread - GetQueuedCompletionStatus(). error:%d", WSAGetLastError());
						LogFuncPtr((int)LogLevel::eERROR, logmsg);
					}
					continue;
				}

				if (!result || (0 == ioSize && OP_ACCEPT != pOverlappedEx->OverlappedExOperationType))
				{
					HandleExceptionWorkThread(pConnection, pOverlappedEx);
					continue;
				}

				switch (pOverlappedEx->OverlappedExOperationType)
				{
				case OP_ACCEPT:
					DoAccept(pOverlappedEx);
					break;
				case OP_RECV:
					DoRecv(pOverlappedEx, ioSize);
					break;
				case OP_SEND:
					DoSend(pOverlappedEx, ioSize);
					break;
				}
			}
		}

		E_NET_RESULT PostNetMessage(Connection* pConnection, Message* pMsg, const DWORD packetSize = 0)
		{
			if (m_hLogicIOCP == INVALID_HANDLE_VALUE || pMsg == nullptr)
			{
				return E_NET_RESULT::fail_message_null;
			}

			auto result = PostQueuedCompletionStatus(
				m_hLogicIOCP,
				packetSize,
				reinterpret_cast<ULONG_PTR>(pConnection),
				reinterpret_cast<LPOVERLAPPED>(pMsg));

			if (!result)
			{
				char logmsg[256] = { 0, };
				sprintf_s(logmsg, "IOCPServer::PostLogicMsg - PostQueuedCompletionStatus(). error:%d, MsgType:%d", WSAGetLastError(), pMsg->OperationType);
				LogFuncPtr((int)LogLevel::eERROR, logmsg);
				return E_NET_RESULT::fail_pqcs;
			}
			return E_NET_RESULT::Success;
		}

		void HandleExceptionWorkThread(Connection* pConnection, const OVERLAPPED_EX* pOverlappedEx)
		{
			if (pOverlappedEx == nullptr)
			{
				return;
			}
						
			//Connection 접속 종료 시 남은 IO 처리
			switch (pOverlappedEx->OverlappedExOperationType)
			{
			case OP_ACCEPT:
				pConnection->DecrementAcceptIORefCount();
				break;
			case OP_RECV:
				pConnection->DecrementRecvIORefCount();
				break;
			case OP_SEND:
				pConnection->DecrementSendIORefCount();
				break;
			}

			if (pConnection->CloseComplete())
			{
				HandleExceptionCloseConnection(pConnection);
			}

			return;
		}
				
		void HandleExceptionCloseConnection(Connection* pConnection)
		{
			if (pConnection == nullptr)
			{
				return;
			}
									
			pConnection->DisconnectConnection();

			pConnection->SetNetStateDisConnection();
						
			if (PostNetMessage(pConnection, pConnection->GetCloseMsg()) != E_NET_RESULT::Success)
			{
				pConnection->ResetConnection();
			}
		}

		void DoAccept(const OVERLAPPED_EX* pOverlappedEx)
		{
			auto pConnection = GetConnection(pOverlappedEx->ConnectionIndex);
			if (pConnection == nullptr)
			{
				return;
			}

			pConnection->DecrementAcceptIORefCount();

			if (pConnection->SetNetAddressInfo() == false)
			{
				char logmsg[128] = { 0, };
				sprintf_s(logmsg, "IOCPServer::DoAccept - GetAcceptExSockaddrs(). error:%d", WSAGetLastError());
				LogFuncPtr((int)LogLevel::eERROR, logmsg);

				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
				return;
			}
		
			if (!pConnection->BindIOCP(m_hWorkIOCP))
			{
				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
				return;
			}

			pConnection->SetNetStateConnection();
			
			auto result = pConnection->PostRecv(pConnection->RecvBufferBeginPos(), 0);
			if (result != E_NET_RESULT::Success)
			{
				char logmsg[128] = { 0, };
				sprintf_s(logmsg, "IOCPServer::PostRecv. Call pConnection->PostRecv. error:%d", WSAGetLastError());
				LogFuncPtr((int)LogLevel::eERROR, logmsg);
				
				HandleExceptionCloseConnection(pConnection);
				return;
			}

			if (PostNetMessage(pConnection, pConnection->GetConnectionMsg()) != E_NET_RESULT::Success)
			{
				pConnection->DisconnectConnection();
				pConnection->ResetConnection();
				return;
			}
		}

		void DoRecv(OVERLAPPED_EX* pOverlappedEx, const DWORD ioSize)
		{
			const int PACKET_HEADER_LENGTH = 5;
			const int PACKET_SIZE_LENGTH = 2;
			const int PACKET_TYPE_LENGTH = 2;

			Connection* pConnection = GetConnection(pOverlappedEx->ConnectionIndex);
			if (pConnection == nullptr)
			{
				return;
			}

			//TODO: 아래 코드들은 거의 대분 Connection 클래스에 함수를 만들어서 그쪽으로 넣어야 하지 않을까?. 함수도 너크 크지 않게 하고...
			pConnection->DecrementRecvIORefCount();

			short packetSize = 0;
			int remainByte = pOverlappedEx->OverlappedExRemainByte;
			char* pCurrent = nullptr;
			char* pNext = nullptr;

			pOverlappedEx->OverlappedExWsaBuf.buf = pOverlappedEx->pOverlappedExSocketMessage;
			pOverlappedEx->OverlappedExRemainByte += ioSize;

			if (PACKET_HEADER_LENGTH <= pOverlappedEx->OverlappedExRemainByte)
			{
				CopyMemory(&packetSize, pOverlappedEx->OverlappedExWsaBuf.buf, PACKET_SIZE_LENGTH);
			}			

			if (packetSize <= 0 || packetSize > pConnection->RecvBufferSize())
			{
				char logmsg[128] = { 0, };
				sprintf_s(logmsg, "IOCPServer::DoRecv. Arrived Wrong Packet.");
				LogFuncPtr((int)LogLevel::eERROR, logmsg);

				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
				return;
			}

			pOverlappedEx->OverlappedExTotalByte = packetSize;

			//아직 데이터를 받지 못한 상황
			if ((pOverlappedEx->OverlappedExRemainByte < static_cast<DWORD>(packetSize)))
			{
				remainByte = pOverlappedEx->OverlappedExRemainByte;
				pNext = pOverlappedEx->OverlappedExWsaBuf.buf;
			}
			//하나 이상의 데이터를 다 받은 상황
			else
			{
				pCurrent = pOverlappedEx->OverlappedExWsaBuf.buf;
				auto currentSize = packetSize;

				remainByte = pOverlappedEx->OverlappedExRemainByte;

				Message* pMsg = m_pMsgPool->AllocMsg();
				if (pMsg == nullptr)
				{
					return;
				}

				pMsg->SetMessage(OP_RECV_PACKET, pCurrent);
				if (PostNetMessage(pConnection, pMsg, currentSize) != E_NET_RESULT::Success)
				{
					m_pMsgPool->DeallocMsg(pMsg);
					return;
				}

				remainByte -= currentSize;
				pNext = pCurrent + currentSize;

				while (true)
				{
					//TODO 패킷 분해 부분을 가상 함수로 만들기 
					if (remainByte >= PACKET_HEADER_LENGTH)
					{
						CopyMemory(&packetSize, pNext, PACKET_SIZE_LENGTH);
						currentSize = packetSize;

						if (0 >= packetSize || packetSize > pConnection->RecvBufferSize())
						{
							char logmsg[128] = { 0, }; sprintf_s(logmsg, "IOCPServer::DoRecv. Arrived Wrong Packet.");
							LogFuncPtr((int)LogLevel::eERROR, logmsg);

							if (pConnection->CloseComplete())
							{
								HandleExceptionCloseConnection(pConnection);
							}
							return;
						}
						pOverlappedEx->OverlappedExTotalByte = currentSize;
						if (remainByte >= currentSize)
						{
							pMsg = m_pMsgPool->AllocMsg();
							if (pMsg == nullptr)
							{
								return;
							}

							pMsg->SetMessage(OP_RECV_PACKET, pNext);
							if (PostNetMessage(pConnection, pMsg, currentSize) != E_NET_RESULT::Success)
							{
								m_pMsgPool->DeallocMsg(pMsg);
								return;
							}

							remainByte -= currentSize;
							pNext += currentSize;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}

			if (pConnection->PostRecv(pNext, remainByte) != E_NET_RESULT::Success)
			{
				if (pConnection->CloseComplete())
				{
					HandleExceptionCloseConnection(pConnection);
				}
			}
		}

		void DoSend(OVERLAPPED_EX* pOverlappedEx, const DWORD ioSize)
		{
			Connection* pConnection = GetConnection(pOverlappedEx->ConnectionIndex);
			if (pConnection == nullptr)
			{
				return;
			}

			
			pConnection->DecrementSendIORefCount();

			pOverlappedEx->OverlappedExRemainByte += ioSize;

			//모든 메세지 전송하지 못한 상황
			if (static_cast<DWORD>(pOverlappedEx->OverlappedExTotalByte) > pOverlappedEx->OverlappedExRemainByte)
			{
				pConnection->IncrementSendIORefCount();

				pOverlappedEx->OverlappedExWsaBuf.buf += ioSize;
				pOverlappedEx->OverlappedExWsaBuf.len -= ioSize;

				ZeroMemory(&pOverlappedEx->Overlapped, sizeof(OVERLAPPED));

				DWORD flag = 0;
				DWORD sendByte = 0;
				auto result = WSASend(
					pConnection->GetClientSocket(),
					&(pOverlappedEx->OverlappedExWsaBuf),
					1,
					&sendByte,
					flag,
					&(pOverlappedEx->Overlapped),
					NULL);

				if (result == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING)
				{
					pConnection->DecrementSendIORefCount();

					char logmsg[128] = { 0, };
					sprintf_s(logmsg, "IOCPServer::DoSend. WSASend. error:%d", WSAGetLastError());
					LogFuncPtr((int)LogLevel::eERROR, logmsg);

					if (pConnection->CloseComplete())
					{
						HandleExceptionCloseConnection(pConnection);
					}
					return;
				}
			}
			//모든 메세지 전송한 상황
			else
			{
				pConnection->SendBufferSendCompleted(pOverlappedEx->OverlappedExTotalByte);
				pConnection->SetEnableSend();
				
				if (pConnection->PostSend(0) == false)
				{
					if (pConnection->CloseComplete())
					{
						HandleExceptionCloseConnection(pConnection);
					}
				}
			}
		}

		void DoPostConnection(Connection* pConnection, const Message* pMsg, OUT INT8& msgOperationType, OUT INT32& connectionIndex)
		{
			if (pConnection->IsConnect() == false)
			{
				return;
			}

			msgOperationType = pMsg->OperationType;
			connectionIndex = pConnection->GetIndex();

			char logmsg[128] = { 0, };
			sprintf_s(logmsg, "IOCPServer::DoPostConnection. Connect Connection. Index:%d", pConnection->GetIndex());
			LogFuncPtr((int)LogLevel::eDEBUG, logmsg);
		}

		void DoPostClose(Connection* pConnection, const Message* pMsg, OUT INT8& msgOperationType, OUT INT32& connectionIndex)
		{
			msgOperationType = pMsg->OperationType;
			connectionIndex = pConnection->GetIndex();

			char logmsg[128] = { 0, };
			sprintf_s(logmsg, "IOCPServer::DoPostClose. Disconnect Connection. Index:%d", pConnection->GetIndex());
			LogFuncPtr((int)LogLevel::eDEBUG, logmsg);

			pConnection->ResetConnection();
		}

		void DoPostRecvPacket(Connection* pConnection, const Message* pMsg, OUT INT8& msgOperationType, OUT INT32& connectionIndex, char* pBuf, OUT INT16& copySize, const DWORD ioSize)
		{
			if (pMsg->pContents == nullptr)
			{
				return;
			}

			msgOperationType = pMsg->OperationType;
			connectionIndex = pConnection->GetIndex();
			CopyMemory(pBuf, pMsg->pContents, ioSize);

			copySize = static_cast<INT16>(ioSize);

			pConnection->RecvBufferReadCompleted(ioSize);

			m_Performance.get()->IncrementPacketProcessCount();
		}


	private:
		NetConfig m_NetConfig;

		std::mutex m_MUTEX;
		SOCKET m_ListenSocket = INVALID_SOCKET;

		std::vector<Connection*> m_Connections;
			
		HANDLE m_hWorkIOCP = INVALID_HANDLE_VALUE;
		HANDLE m_hLogicIOCP = INVALID_HANDLE_VALUE;
		bool m_IsRunWorkThread = true;
		std::vector<std::unique_ptr<std::thread>> m_WorkThreads;

		MessagePool* m_pMsgPool = nullptr;

		std::unique_ptr<Performance> m_Performance;
	};
}