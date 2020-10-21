#include "ErrorCode.h"
#include "Define.h"
#include <basetsd.h>

namespace NServerNetLib
{
	class TcpSession
	{

	public:
		TcpSession();
		virtual ~TcpSession();

		void Init(const int index);
		bool IsConnected() { return SocketFD != 0 ? true : false; }

		UINT64 SocketFD = -1;

		char* pRecvBuffer = nullptr;
		char* pSendBuffer = nullptr;

		int mIndex = 0;

	private:
		
	
	};
}
