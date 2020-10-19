#include "TcpSession.h"


namespace NServerNetLib
{
	TcpSession::TcpSession()
	{		
	}
	
	TcpSession::~TcpSession()
	{
		
	}

	void TcpSession::Init(const int index)
	{
		mIndex = index;

		Seq = 0;
		SocketFD = 0;

		RemainingDataSize = 0;
		PrevReadPosInRecvBuffer = 0;
		SendSize = 0;

		Seq = 0;
		SocketFD = 0;

		pRecvBuffer = nullptr;
		RemainingDataSize = 0;
		PrevReadPosInRecvBuffer = 0;

		pSendBuffer = nullptr;
		SendSize = 0;
	}
	
}