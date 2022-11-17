#include "stdafx.h"
#include "SendThread.h"
#include "NetworkFacade.h"
#include "MessageHeader.h"
#include "TimerUtil.h"
#include "IOStatus.h"

//////////////////////////
// SendThread
//////////////////////////

SendThread::SendThread() : _ioStatus(NULL), _aimSendByte(0)
{
}

SendThread::~SendThread()
{
}

bool SendThread::Begin()
{
	return true;
}

void SendThread::End()
{
}

void SendThread::Update()
{
	_ioStatus->Print();

	if ((unsigned int)_ioStatus->GetSentBytes() >= _aimSendByte)
	{
		EventSleep(1000);
		//Reset();
		return;
	}

	char buffer[1024] = {0,};
	unsigned int sendSize = rand()%1000 + 4;
	sendSize = 50;
	for(int i=0;i<10;++i)
	{
		MakeRandomBuffer(buffer, sendSize);
		ACE_Message_Block* block = new ACE_Message_Block(sendSize);
		block->copy(buffer, sendSize);
		unsigned int streamId = 0;
		
		if (NetworkInstance->SendRequest(streamId, block, false))
			_ioStatus->AddSendBytes(sendSize);
		else
			printf("SendRequest fail bytes(%d).\n", sendSize);
	}

	EventSleep(30);
}

void SendThread::MakeRandomBuffer( char* buffer, unsigned int len )
{
	for(unsigned int i=0; i<len; ++i)
		buffer[i] = rand()%255;
}

void SendThread::EventSleep(int ms)
{
	ACE_Time_Value waitTime(ms/1000, (ms%1000)*1000);
	ACE_Event eventObj;
	eventObj.wait(&waitTime, 0);
}

void SendThread::SetIoStatus( IOStatus* ioStatus )
{
	_ioStatus = ioStatus;
}

void SendThread::SetAimSendByte( unsigned int byte )
{
	_aimSendByte = byte;
}

void SendThread::Reset()
{
	_aimSendByte = 0;
	_ioStatus->Reset();
}