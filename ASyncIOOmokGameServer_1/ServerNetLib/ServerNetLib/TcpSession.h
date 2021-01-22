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

		void Init(const int index, HANDLE iocpHandle);

		int GetIndex() { return m_Index; }

		bool IsConnectd() { return m_IsConnect == 1; }

		SOCKET GetSock() { return m_Socket; }

		UINT64 GetLatestClosedTimeSec() { return m_LatestClosedTimeSec; }

		char* RecvBuffer() { return m_RecvBuf; }

		bool OnConnect(HANDLE iocpHandle, SOCKET socket);

		void Close();

		bool PostAccept(SOCKET listenSock, const UINT64 curTimeSec);

		bool AcceptCompletion();

		bool BindIOCompletionPort(HANDLE iocpHandle);

		bool BindRecv();

		bool SendMsg(const short bodySize, char* pMsg);

		void SendCompleted(const int dataSize);

		void SetPacketData(const int dataSize, char* pData);

		RecvPacketInfo GetPacket();

	private:
		bool SendIO();

		bool SetSocketOption();

		int m_Index = 0;

		HANDLE m_IOCPHandle = INVALID_HANDLE_VALUE;

		int m_IsConnect = 0;

		UINT64 m_LatestClosedTimeSec = 0;

		SOCKET m_Socket;

		stOverlappedEx	m_AcceptContext;

		char m_AcceptBuf[64];

		stOverlappedEx	m_RecvOverlappedEx;	

		char m_RecvBuf[MAX_SOCK_RECVBUF];

		std::mutex m_SendLock;

		std::queue<stOverlappedEx*> m_SendDataqueue;

		int m_PakcetDataBufferWPos = 0;

		int m_PakcetDataBufferRPos = 0;

		char* m_PakcetDataBuffer = nullptr;

		std::mutex m_PakcetDataBufferPosMutex;
		
	};
}
