#pragma once
#include "../MGServerLib/Define.h"
#include <map>

class EchoNetworkEventHandler : public INetworkReceiver
{
	typedef std::map<ULONG_PTR, AsyncSocketDescEx> SessionMap;

public:
	EchoNetworkEventHandler(void) = default;
	virtual ~EchoNetworkEventHandler(void) = default;

	virtual void notifyRegisterSocket(AsyncSocketDescEx& sockdesc, SOCKADDR_IN& ip) override;
	
	virtual void notifyReleaseSocket(AsyncSocketDescEx& sockdesc) override;
	
	virtual void notifyMessage(AsyncSocketDescEx& sockdesc, size_t length, char* data) override;

	virtual void notifyConnectingResult(INT32 requestID, AsyncSocketDescEx& sockdesc, DWORD error) override 
	{
		UNREFERENCED_PARAMETER(requestID);
		UNREFERENCED_PARAMETER(sockdesc);
		UNREFERENCED_PARAMETER(error);
	}


	void Process();

protected:

private:
//멀티쓰레드로 접근해서
	SessionMap m_SessionMap;
	CriticalSectionLockWrapper m_SessionLock;
};

