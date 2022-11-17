#include "stdafx.h"
#include "IOStatus.h"
#include "TimerUtil.h"

IOStatus::IOStatus() : _sendBytes(0), _recvBytes(0)
{
	_printCycle = new HalfNetwork::CycleChecker(2000);
}

IOStatus::~IOStatus()
{
	SAFE_DELETE(_printCycle);
}

void IOStatus::AddSendBytes( long value )
{
	_sendBytes += value;
}

void IOStatus::AddRecvBytes( long value )
{
	_recvBytes += value;
}

void IOStatus::Print()
{
	if (false == _printCycle->IsExpire())
		return;
	
	printf("IOStatus::Print\n");
	printf("SentBytes(%d)\n", _sendBytes.value());
	printf("RecvBytes(%d)\n", _recvBytes.value());
}

long IOStatus::GetSentBytes()
{
	return _sendBytes.value();
}

void IOStatus::Reset()
{
	_recvBytes = 0;
	_sendBytes = 0;
}