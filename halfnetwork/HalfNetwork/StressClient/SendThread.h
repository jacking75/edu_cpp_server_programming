#pragma once

#include "ThreadPool.h"

namespace HalfNetwork 
{
	class CycleChecker;
	class SimpleConfig;
}

class IOStatus;

class SendThread : public HalfNetwork::ThreadPool
{
public:
	SendThread();
	virtual ~SendThread();

public:
	virtual bool	Begin();
	virtual void	Update();
	virtual void	End();
	void SetIoStatus(IOStatus* ioStatus);
	void SetAimSendByte(unsigned int byte);

protected:
	void MakeRandomBuffer( char* buffer, unsigned int len );
	void EventSleep(int ms);
	void Reset();
	

private:
	IOStatus* _ioStatus;
	unsigned int _aimSendByte;
};
