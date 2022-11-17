#pragma once

#include <ace/Atomic_Op_T.h>

namespace HalfNetwork 
{
	class CycleChecker;
}

class IOStatus
{
public:
	IOStatus();
	~IOStatus();

	void AddSendBytes(long value);
	void AddRecvBytes(long value);
	void Print();
	long GetSentBytes();
	void Reset();


private:
	ACE_Atomic_Op<ACE_Thread_Mutex, long> _sendBytes;
	ACE_Atomic_Op<ACE_Thread_Mutex, long> _recvBytes;
	HalfNetwork::CycleChecker*	_printCycle;
};
