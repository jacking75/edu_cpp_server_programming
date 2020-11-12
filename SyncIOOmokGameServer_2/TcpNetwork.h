#pragma once

#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>


#include <vector>
#include <deque>
#include "NetErrorCode.h"
#include "Define.h"
#include "TcpSession.h"
#include <optional>
#include <thread>
#include <mutex>


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
		 ~TcpNetwork();

		NET_ERROR_CODE Init(const ServerConfig pConfig);

		void SendData(const int sessionIndex, const short packetId, const short bodySize, char* pMsg);

		NET_ERROR_CODE Run();

		void Release();

		const int backLogLoop = 50;

		std::optional <RecvPacketInfo> GetReceivePacket();

		void SelectProcess();

		void SendProcess();
	
	protected:

		void RunCheckSelectClients(fd_set& read_set);

		int CreateSessionPool(const int maxClientCount);

		NET_ERROR_CODE InitServerSocket();

		NET_ERROR_CODE BindListen(short port, int backlogCount);

		NET_ERROR_CODE NewSession();

		NET_ERROR_CODE SetNonBlockSocket(const SOCKET sock);

		NET_ERROR_CODE RecvSocket(const int sessionIndex);

		NET_ERROR_CODE SendToClient(const int sessionIndex, char* pSendBuffer, const short bodySize);

		NET_ERROR_CODE RecvBufferProcess(const int sessionIndex);

		void ConnectedSession(const int sessionIndex, const SOCKET fd);

		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);

		int AllocClientSessionIndex();

		void ReleaseSessionIndex(const int index);

		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);

		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);

		bool mIsRunning = false;

		std::unique_ptr<std::thread> mSelectThread;

		std::unique_ptr<std::thread> mSendThread;

		std::mutex mReceivePacketMutex;

		std::mutex mSendPacketMutex;

	protected:

		ServerConfig m_Config;

		SOCKET m_ServerSockfd;

		fd_set m_Readfds;

		int64_t m_ConnectSeq = 0;

		std::vector<TcpSession> m_ClientSessionPool;

		std::deque<int> m_ClientSessionPoolIndex;

		std::deque<RecvPacketInfo> m_PacketQueue;
		std::deque<RecvPacketInfo> m_SendPacketQueue;
	};
}
