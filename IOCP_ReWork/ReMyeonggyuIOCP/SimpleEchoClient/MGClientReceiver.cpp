#include "stdafx.h"
#include "MGClientReceiver.h"


MGClientReceiver::MGClientReceiver(void)
{
}


MGClientReceiver::~MGClientReceiver(void)
{
}

void MGClientReceiver::notifyRegisterSocket(ASSOCKDESCEX& sockdesc, SOCKADDR_IN& ip)
{
	printf(" Connected %d\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.insert(std::make_pair(sockdesc.assockUid, sockdesc));
}

void MGClientReceiver::notifyReleaseSocket(ASSOCKDESCEX& sockdesc)
{
	printf("Disconnected %d\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.erase(sockdesc.assockUid);

	sockdesc.psender->releaseSocketUniqueId(sockdesc.assockUid);
}

void MGClientReceiver::notifyMessage(ASSOCKDESCEX& sockdesc, size_t length, char* data)
{
	char* pbuffer = new char[length]; 
	memcpy(pbuffer, data, length);
	pbuffer[length-1] = 0;

	printf("%s\n", pbuffer);
	delete pbuffer;
}

void MGClientReceiver::notifyConnectingResult(INT32 requestID, ASSOCKDESCEX& sockdesc, DWORD error)
{
	if(error == 0)
	{
		m_SocketDesc = sockdesc;
		printf("Connected\n");
	}
	
}

bool MGClientReceiver::ProcessEcho()
{
	
		m_SocketDesc.psender->postingSend(m_SocketDesc, 5, "Echo");
	

	return true;
}