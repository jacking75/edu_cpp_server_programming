#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstring>
#include <vector>
#include <deque>
#include <iostream>
#include "TcpNetwork.h"


namespace NServerNetLib
{
	TcpNetwork::TcpNetwork()
	{

	}

	TcpNetwork::~TcpNetwork()
	{
		for (auto& client : m_ClientSessionPool)
		{
			if (client.pRecvBuffer) {
				delete[] client.pRecvBuffer;
			}

			if (client.pSendBuffer) {
				delete[] client.pSendBuffer;
			}
		}
	}

	NET_ERROR_CODE TcpNetwork::Init(const ServerConfig* pConfig)
	{
		std::memcpy(&m_Config, pConfig, sizeof(ServerConfig));


		auto initRet = InitServerSocket();
		if (initRet != NET_ERROR_CODE::NONE)
		{
			return initRet;
		}

		auto bindListenRet = BindListen(pConfig->Port, pConfig->BackLogCount);
		if (bindListenRet != NET_ERROR_CODE::NONE)
		{
			return bindListenRet;
		}

		FD_ZERO(&m_Readfds);
		FD_SET(m_ServerSockfd, &m_Readfds);

		auto sessionPoolSize = CreateSessionPool(pConfig->MaxClientCount + pConfig->ExtraClientCount);

		return NET_ERROR_CODE::NONE;
	}

	int TcpNetwork::CreateSessionPool(const int maxClientCount)
	{
		for (int i = 0; i < maxClientCount; ++i)
		{
			TcpSession session;
			session.Init(i);
			session.pRecvBuffer = new char[m_Config.MaxClientRecvBufferSize];
			session.pSendBuffer = new char[m_Config.MaxClientSendBufferSize];

			m_ClientSessionPool.push_back(session);
			m_ClientSessionPoolIndex.push_back(i);
		}

		return maxClientCount;
	}
	int TcpNetwork::AllocClientSessionIndex()
	{
		if (m_ClientSessionPoolIndex.empty())
		{
			return -1;
		}

		int index = m_ClientSessionPoolIndex.front();
		m_ClientSessionPoolIndex.pop_front();
		return index;
	}

	void TcpNetwork::ReleaseSessionIndex(const int index)
	{
		m_ClientSessionPoolIndex.push_back(index);
		m_ClientSessionPool[index].Init(index);
	}

	void TcpNetwork::Run()
	{
		while (1) {
			fd_set read_set;
			read_set = m_Readfds;
			timeval timeout{ 0, 1000 }; //tv_sec, tv_usec

			auto selectResult = select(0, &read_set, nullptr, nullptr, &timeout);

			if (RunCheckSelectResult(selectResult) == false)
			{
				continue;
			}

			if (FD_ISSET(m_ServerSockfd, &read_set))
			{
				NewSession();
			}
			RunCheckSelectClients(read_set);
		}
	}

	NET_ERROR_CODE TcpNetwork::NewSession()
	{
		auto tryCount = 0;

		do
		{
			++tryCount;

			struct sockaddr_in client_adr;

			socklen_t client_len = sizeof(client_adr);
			auto client_sockfd = accept(m_ServerSockfd, (struct sockaddr*)&client_adr, &client_len);

			if (client_sockfd == INVALID_SOCKET)
			{
				return NET_ERROR_CODE::ACCEPT_API_ERROR;
			}

			std::cout << std::endl;
			std::cout << "클라이언트 연결 " << inet_ntoa(client_adr.sin_addr) <<":"<<client_adr.sin_port<< std::endl;
			
			auto newSessionIndex = AllocClientSessionIndex();
			if (newSessionIndex < 0)
			{
				CloseSession(SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY, client_sockfd, -1);
				return NET_ERROR_CODE::ACCEPT_MAX_SESSION_COUNT;
			}
		
			SetNonBlockSocket(client_sockfd);

			FD_SET(client_sockfd, &m_Readfds);

			ConnectedSession(newSessionIndex, client_sockfd);

		} while (tryCount < FD_SETSIZE);

		return NET_ERROR_CODE::NONE;
	}

	void TcpNetwork::ConnectedSession(const int sessionIndex, const SOCKET fd)
	{

		TcpSession* session = &m_ClientSessionPool[sessionIndex];
		session->SocketFD = fd;
	}


	NET_ERROR_CODE TcpNetwork::SetNonBlockSocket(const SOCKET sock)
	{
		unsigned long mode = 1;

		if (ioctlsocket(sock, FIONBIO, &mode) == SOCKET_ERROR)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_FIONBIO_FAIL;
		}

		return NET_ERROR_CODE::NONE;
	}

	bool TcpNetwork::RunCheckSelectResult(const int result)
	{
		if (result == 0)
		{
			return false;
		}
		else if (result == -1)
		{
			return false;
		}

		return true;
	}

	void TcpNetwork::RunCheckSelectClients(fd_set& read_set)
	{
		for (int i = 0; i < m_ClientSessionPool.size(); ++i)
		{
			auto& session = m_ClientSessionPool[i];

			if (session.IsConnected() == false) {
				continue;
			}

			SOCKET fd = session.SocketFD;
			auto sessionIndex = session.mIndex;

			auto retReceive = RunProcessReceive(sessionIndex, fd, read_set);
			/*
			if (retReceive == false) {
				continue;
			}
			write
			*/
		}
	}
	bool TcpNetwork::RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set)
	{
		if (!FD_ISSET(fd, &read_set))
		{
			return true;
		}

		auto ret = RecvSocket(sessionIndex);
		if (ret != NET_ERROR_CODE::NONE)
		{
			CloseSession(SOCKET_CLOSE_CASE::SOCKET_RECV_ERROR, fd, sessionIndex);
			std::cout << "클라이언트 접속 끊어짐" << std::endl;
			return false;
		}
		return true;
	}

	NET_ERROR_CODE TcpNetwork::RecvSocket(const int sessionIndex)
	{
		auto& session = m_ClientSessionPool[sessionIndex];
		auto fd = static_cast<SOCKET>(session.SocketFD);

		if (session.IsConnected() == false)
		{
			return NET_ERROR_CODE::RECV_PROCESS_NOT_CONNECTED;
		}

		int recvPos = 0;
		auto recvSize = recv(fd, &session.pRecvBuffer[recvPos], (MAX_PACKET_BODY_SIZE * 2), 0);
//		std::cout << "recv 완료" << std::endl;
		if (recvSize == 0) 
		{
			return NET_ERROR_CODE::RECV_REMOTE_CLOSE;
		}

		if (recvSize < 0)
		{
			auto netError = WSAGetLastError();

			if (netError != WSAEWOULDBLOCK)
			{
				return NET_ERROR_CODE::RECV_API_ERROR;
			}
			else
			{
				return NET_ERROR_CODE::NONE;
			}
		}

		std::cout << "RECEIVE Session Index [ " << sessionIndex << " ] [ " << recvSize << " ] Bytes" << '\n';


		return NET_ERROR_CODE::NONE;
	}

	void TcpNetwork::CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex)
	{
		if (closeCase == SOCKET_CLOSE_CASE::SESSION_POOL_EMPTY)
		{
			closesocket(sockFD);

			FD_CLR(sockFD, &m_Readfds);
			return;
		}

		if (m_ClientSessionPool[sessionIndex].IsConnected() == false) {
			return;
		}

		closesocket(sockFD);

		FD_CLR(sockFD, &m_Readfds);

		m_ClientSessionPool[sessionIndex].Init(sessionIndex);

		ReleaseSessionIndex(sessionIndex);

	}

	NET_ERROR_CODE TcpNetwork::InitServerSocket()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		}

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		}

		return NET_ERROR_CODE::NONE;
	}

	NET_ERROR_CODE TcpNetwork::BindListen(short port, int backlogCount)
	{
		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(port);

		if (bind(m_ServerSockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
		}

		auto netError = SetNonBlockSocket(m_ServerSockfd);
		if (netError != NET_ERROR_CODE::NONE)
		{
			return netError;
		}

		if (listen(m_ServerSockfd, backlogCount) == SOCKET_ERROR)
		{
			return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_FAIL;
		}

	}


	void TcpNetwork::Stop()
	{

	}

}