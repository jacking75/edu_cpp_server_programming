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

		NET_ERROR_CODE Init(const ServerConfig pConfig);
		NET_ERROR_CODE SendData(const int sessionIndex, const short packetId, const short bodySize, const char* pMsg);

		void Run();
		void Stop();

		const int backLogLoop = 50;
		RecvPacketInfo GetReceivePacket();

		
	protected:
		void RunCheckSelectClients(fd_set& read_set, fd_set& write_set);
		bool RunCheckSelectResult(const int result);
		int CreateSessionPool(const int maxClientCount);

		NET_ERROR_CODE InitServerSocket();
		NET_ERROR_CODE BindListen(short port, int backlogCount);
		NET_ERROR_CODE NewSession();
		NET_ERROR_CODE SetNonBlockSocket(const SOCKET sock);
		NET_ERROR_CODE RecvSocket(const int sessionIndex);
		NET_ERROR_CODE RecvBufferProcess(const int sessionIndex);
		NetError FlushSendBuff(const int sessionIndex);
		NetError SendSocket(const SOCKET fd, const char* pMsg, const int size);
		void RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set);
		void ConnectedSession(const int sessionIndex, const SOCKET fd);
		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
		int AllocClientSessionIndex();
		void ReleaseSessionIndex(const int index);
		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);
		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);


	protected:

		ServerConfig m_Config;

		SOCKET m_ServerSockfd;
		fd_set m_Readfds;

		int64_t m_ConnectSeq = 0;

		std::vector<TcpSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;

		std::deque<RecvPacketInfo> m_PacketQueue;
	};
}
