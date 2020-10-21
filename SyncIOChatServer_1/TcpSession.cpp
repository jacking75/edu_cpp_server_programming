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
		SocketFD = -1;

		pRecvBuffer = nullptr;
		pSendBuffer = nullptr;
	}
	
}