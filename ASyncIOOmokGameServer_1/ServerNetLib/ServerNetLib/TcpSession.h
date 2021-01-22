#include "Define.h"
#include <basetsd.h>
#include "NetErrorCode.h"
#include <optional>
#include <mutex>
#include <WinSock2.h>
#include <queue>

namespace NServerNetLib
{
	class TcpSession
	{

	public:
		TcpSession()
		{
			ZeroMemory(&m_RecvOverlappedEx, sizeof(stOverlappedEx));
			m_Socket = INVALID_SOCKET;
		}

		~TcpSession() = default;

		void Init(const UINT32 index, HANDLE iocpHandle);

		UINT32 GetIndex() { return m_Index; }

		bool IsConnectd() { return m_IsConnect == 1; }

		SOCKET GetSock() { return m_Socket; }

		UINT64 GetLatestClosedTimeSec() { return m_LatestClosedTimeSec; }

		char* RecvBuffer() { return m_RecvBuf; }

		bool OnConnect(HANDLE iocpHandle, SOCKET socket);

		void Close(bool bIsForce);

		bool PostAccept(SOCKET listenSock, const UINT64 curTimeSec);

		bool AcceptCompletion();

		bool BindIOCompletionPort(HANDLE iocpHandle);

		bool BindRecv();

		bool SendMsg(const short bodySize, char* pMsg);

		void SendCompleted(const UINT32 dataSize);

		void SetPacketData(const UINT32 dataSize, char* pData);

		RecvPacketInfo GetPacket();

	private:
		bool SendIO();

		bool SetSocketOption();

		INT32 m_Index = 0;

		HANDLE m_IOCPHandle = INVALID_HANDLE_VALUE;

		INT64 m_IsConnect = 0;

		UINT64 m_LatestClosedTimeSec = 0;

		SOCKET m_Socket;

		stOverlappedEx	m_AcceptContext;

		char m_AcceptBuf[64];

		stOverlappedEx	m_RecvOverlappedEx;	

		char m_RecvBuf[MAX_SOCK_RECVBUF];

		std::mutex m_SendLock;

		std::queue<stOverlappedEx*> m_SendDataqueue;

		UINT32 m_PakcetDataBufferWPos = 0;

		UINT32 m_PakcetDataBufferRPos = 0;

		char* m_PakcetDataBuffer = nullptr;

		std::mutex m_PakcetDataBufferPosMutex;
		
	};
}
