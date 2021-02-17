#pragma once
#include "../MGServerLib/Asios.h"
#include <map>

class MGServerReceiver : public INetworkReceiver
{
	typedef std::map<ULONG_PTR, ASSOCKDESCEX> SessionMap;

public:
	MGServerReceiver(void) = default;
	virtual ~MGServerReceiver(void) = default;

	virtual void notifyRegisterSocket(ASSOCKDESCEX& sockdesc, SOCKADDR_IN& ip) override;
	
	virtual void notifyReleaseSocket(ASSOCKDESCEX& sockdesc) override;
	
	virtual void notifyMessage(ASSOCKDESCEX& sockdesc, size_t length, char* data) override;

	virtual void notifyConnectingResult(INT32 requestID, ASSOCKDESCEX& sockdesc, DWORD error) override 
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

