#pragma once

#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>


#include <vector>
#include <deque>
#include <unordered_map>
#include "ErrorCode.h"
#include "Define.h"
#include "TcpSession.h"

namespace NServerNetLib
{
	enum class SOCKET_CLOSE_CASE : short
	{
		SESSION_POOL_EMPTY = 1,
		SELECT_ERROR = 2,
		SOCKET_RECV_ERROR = 3,
		SOCKET_RECV_BUFFER_PROCESS_ERROR = 4,
		SOCKET_SEND_ERROR = 5,
		FORCING_CLOSE = 6,
	};

	class TcpNetwork
	{
	public:
		TcpNetwork();
		virtual ~TcpNetwork();

		NET_ERROR_CODE Init(const ServerConfig* pConfig);

		void Run();
		void Stop();

		size_t ConnectSessionCount() { return ConnectedSessions.size(); }

	protected:
		void RunCheckSelectClients(fd_set& read_set);
		bool RunCheckSelectResult(const int result);
		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);
		int CreateSessionPool(const int maxClientCount);

		NET_ERROR_CODE InitServerSocket();
		NET_ERROR_CODE BindListen(short port, int backlogCount);
		NET_ERROR_CODE NewSession();
		NET_ERROR_CODE SetNonBlockSocket(const SOCKET sock);
		NET_ERROR_CODE RecvSocket(const int sessionIndex);

		void SetSockOption(const SOCKET fd);
		NET_ERROR_CODE ConnectedSession(const int sessionIndex, const SOCKET fd);
		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
		int AllocClientSessionIndex();
		void ReleaseSessionIndex(const int index);

	protected:

		ServerConfig m_Config;

		SOCKET m_MaxSockFD = 0;
		SOCKET m_ServerSockfd;
		fd_set m_Readfds;

		int64_t m_ConnectSeq = 0;

		std::vector<TcpSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;

		// 연결된 세션들 저장한 컨테이너
		std::unordered_map<int64_t, TcpSession*> ConnectedSessions;

	};
}
