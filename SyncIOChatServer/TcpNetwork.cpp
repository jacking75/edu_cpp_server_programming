#include <cstring>
#include <vector>
#include <deque>

#include "TcpNetwork.h"


namespace NServerNetLib
{
	TcpNetwork::TcpNetwork()
	{		
	}
	
	TcpNetwork::~TcpNetwork() 
	{		
	}
		
	void TcpNetwork::Run()
	{		
		fd_set read_set;
		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec

        auto selectResult = select(0, &read_set, nullptr, nullptr, &timeout);	
	}

	void TcpNetwork::Stop()
	{

	}

}