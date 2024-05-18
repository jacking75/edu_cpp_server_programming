#include "stdafx.h"
#include "EchoNetworkEventHandler.h"

void EchoNetworkEventHandler::notifyRegisterSocket(AsyncSocketDescEx& sockdesc, SOCKADDR_IN& ip)
{
	UNREFERENCED_PARAMETER(ip);

	printf("Connected %llu\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.insert(std::make_pair(sockdesc.assockUid, sockdesc));
}

void EchoNetworkEventHandler::notifyReleaseSocket(AsyncSocketDescEx& sockdesc)
{
	printf("Disconnected %llu\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.erase(sockdesc.assockUid);

	sockdesc.psender->releaseSocketUniqueId(sockdesc.assockUid);
}

void EchoNetworkEventHandler::notifyMessage(AsyncSocketDescEx& sockdesc, size_t length, char* data)
{
	char* pbuffer = new char[length]; 
	memcpy(pbuffer, data, length);

	if( 'q' == pbuffer[0] )
		sockdesc.psender->disconnectSocket(sockdesc);
	else
		sockdesc.psender->postingSend(sockdesc, length, pbuffer);

	delete pbuffer;
}

void EchoNetworkEventHandler::Process()
{
	Synchronized es(&m_SessionLock);

	SessionMap::iterator iter = m_SessionMap.begin();

	for(;iter != m_SessionMap.end(); ++iter)
	{
		iter->second.psender->postingSend(iter->second, 6, "Hello");
	}
}