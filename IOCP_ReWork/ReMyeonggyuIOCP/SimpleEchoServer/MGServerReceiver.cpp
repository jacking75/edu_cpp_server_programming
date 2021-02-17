#include "stdafx.h"
#include "MGServerReceiver.h"

void MGServerReceiver::notifyRegisterSocket(ASSOCKDESCEX& sockdesc, SOCKADDR_IN& ip)
{
	UNREFERENCED_PARAMETER(ip);

	printf("Connected %llu\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.insert(std::make_pair(sockdesc.assockUid, sockdesc));
}

void MGServerReceiver::notifyReleaseSocket(ASSOCKDESCEX& sockdesc)
{
	printf("Disconnected %llu\n", sockdesc.assockUid);

	Synchronized es(&m_SessionLock);
           
	m_SessionMap.erase(sockdesc.assockUid);

	sockdesc.psender->releaseSocketUniqueId(sockdesc.assockUid);
}

void MGServerReceiver::notifyMessage(ASSOCKDESCEX& sockdesc, size_t length, char* data)
{
	char* pbuffer = new char[length]; 
	memcpy(pbuffer, data, length);

	if( 'q' == pbuffer[0] )
		sockdesc.psender->disconnectSocket(sockdesc);
	else
		sockdesc.psender->postingSend(sockdesc, length, pbuffer);

	delete pbuffer;
}

void MGServerReceiver::Process()
{
	Synchronized es(&m_SessionLock);

	SessionMap::iterator iter = m_SessionMap.begin();

	for(;iter != m_SessionMap.end(); ++iter)
	{
		//for(int i = 1; i <= 20; ++i)
		{
			iter->second.psender->postingSend(iter->second, 6, "Hello");
		}
	}
}