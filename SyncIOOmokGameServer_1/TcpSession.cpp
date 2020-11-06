#include "TcpSession.h"


namespace NServerNetLib
{	

	void TcpSession::Init(const int index)
	{
		mIndex = index;
		SocketFD = -1;
		SendSize = 0;

		pRecvBuffer = nullptr;
		pSendBuffer = nullptr;

		RemainingDataSize = 0;
		PrevReadPosInRecvBuffer = 0;
	}
	
}