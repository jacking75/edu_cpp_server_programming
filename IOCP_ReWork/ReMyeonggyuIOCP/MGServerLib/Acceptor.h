#pragma once

#include "Thread.h"
#include "Asios.h"

class Acceptor : public Thread
{
public:
	Acceptor(INetworkSender* theAsynchEventManager, AsynchSocket* prototype, const char* address, u_short port);

	virtual ~Acceptor() = default;

	void Run() override;  //acceptor loop
			

protected:
	virtual void logmsg(char * format,...);


private:
	AsynchSocket*		m_prototype; //socket creator : prototype patterns
	SOCKET				socketValue; //listen socket
	INetworkSender*		theAsynchEventManager;
	char				filestr[1024];
};



