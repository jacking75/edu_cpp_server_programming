#include "ErrorCode.h"
#include "Define.h"

namespace NServerNetLib
{
	class TcpSession
	{

	public:
		TcpSession();
		virtual ~TcpSession();

		void Init(const int index);
		bool IsConnected() { return SocketFD != 0 ? true : false; }

		long long Seq = 0;
		unsigned long long	SocketFD = 0;

		char* pRecvBuffer = nullptr;
		int     RemainingDataSize = 0;
		int     PrevReadPosInRecvBuffer = 0;

		char* pSendBuffer = nullptr;
		int     SendSize = 0;
		int mIndex = 0;

	private:
		
	
	};
}
