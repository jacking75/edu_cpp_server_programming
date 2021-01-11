#include "Define.h"
#include <basetsd.h>
#include "NetErrorCode.h"
#include <optional>
#include <mutex>
#include <WinSock2.h>

namespace NServerNetLib
{
	class TcpSession
	{

	public:

		TcpSession() {}
		~TcpSession() {}

		void Init(const int index);

		bool IsConnected() 
		{ 
			return SocketFD != -1 ? true : false;
		}
	
		UINT64 SocketFD = -1;

		char* pRecvBuffer = nullptr;

		char* pSendBuffer = nullptr;

		int SendSize = 0;

		int RemainingDataSize = 0;

		int PrevReadPosInRecvBuffer = 0;

		int mIndex = 0;

		NET_ERROR_CODE SendSessionData(int maxClientSendBufferSize, const short packetId, const short bodySize, char* pMsg);

		NET_ERROR_CODE RecvSessionData();

		NET_ERROR_CODE FlushSendBuffer();

		NET_ERROR_CODE RecvBuffer(int sessionIndex);

		void DeleteClientBuffer();

		void NewSessionBuffer(int maxClientRecvBufferSize, int maxClientSendBufferSize);


	private :

		std::optional <int> SendSocket();

		std::mutex sendPacketMutex;
	};
}
