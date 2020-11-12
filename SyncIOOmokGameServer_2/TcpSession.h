#include "ErrorCode.h"
#include "Define.h"
#include <basetsd.h>

namespace NServerNetLib
{
	class TcpSession
	{

	public:

		TcpSession() = default;
		~TcpSession() = default;

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
	
	};
}
