#pragma once

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
#include "logger.h"
#include "../../LogicLib/LogicLib/Config.h"
#include <concurrent_queue.h>

using namespace Concurrency;

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
		TcpNetwork()  = default;
		~TcpNetwork();

		NET_ERROR_CODE Init(OmokServerLib::Config* config, Logger* pLogger);

		NET_ERROR_CODE BindandListen(short port, int backlogCount);

		bool Run();

		bool SendMsg(const int sessionIndex, const short bodySize, char* pMsg);

		int ClientSessionPoolSize() { return (int)m_ClientSessionPool.size(); }

		void Release();

		void CloseSocket(const int sessionIndex, bool isForce);

		std::optional <RecvPacketInfo> GetPacketData();

		std::optional <RecvPacketInfo> GetSystemPacketData();

	private: 
		void DestroyThread();

		void OnConnect(const UINT32 clientIndex);

		void OnClose(const UINT32 clientIndex);

		void OnReceive(const UINT32 clientIndex, const UINT32 size, char* pData);

		int CreateSessionPool(const int maxClientCount);

		bool CreateWokerThread();

		TcpSession* GetClientInfo(const UINT32 clientIndex);

		bool CreateAccepterThread();

		void WokerThread();

		void AccepterThread();

		concurrent_queue <RecvPacketInfo> m_PacketQueue;

		std::mutex m_ReceivePacketMutex;

		UINT32 maxIOWorkerThreadCount = 0;

		std::vector<TcpSession*> m_ClientSessionPool;

		SOCKET m_ListenSocket = INVALID_SOCKET;

		int	m_ClientCnt = 0;

		std::vector<std::thread> m_IOWorkerThreads;

		std::thread	m_AccepterThread;

		HANDLE	m_IOCPHandle = INVALID_HANDLE_VALUE;

		bool	m_IsWorkerRun = true;

		bool	m_IsAccepterRun = true;

		std::deque<UINT32> m_InComingPacketUserIndexQueue;

		std::mutex m_InComingPacketQueueMutex;

		OmokServerLib::Config* m_Config;

		Logger* m_pRefLogger;

		void AddSystemPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);
	};
}
