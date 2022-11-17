#include "stdafx.h"
#include "NetworkThread.h"
#include "NetworkFacade.h"
#include "MessageHeader.h"
#include "TimerUtil.h"
#include "SimpleConfig.h"
#include "EchoBackChecker.h"

//////////////////////////
// NetworkThread
//////////////////////////

NetworkThread::NetworkThread(HalfNetwork::SimpleConfig& config) : _configObj(config)
{
	_bufferChecker = new EchoBackChecker();
	_disconnectCycle = new HalfNetwork::CycleChecker();
	_debugMsgCycle = new HalfNetwork::CycleChecker(3000);
	_disconnectCycle->Set(_configObj.GetValue<unsigned int>(ACE_TEXT("DisconnectTerm")));
	_sendTerm = _configObj.GetValue<unsigned int>(ACE_TEXT("SendTerm"));
}

NetworkThread::~NetworkThread()
{
	SAFE_DELETE(_bufferChecker);
	SAFE_DELETE(_disconnectCycle);
	SAFE_DELETE(_debugMsgCycle);
}

bool NetworkThread::Begin()
{
	return true;
}

void NetworkThread::End()
{
}

void NetworkThread::Update()
{
	_ProcessRecvQueue(_queueId);
}

void NetworkThread::_ProcessRecvQueue(unsigned char queue_id)
{
	using namespace HalfNetwork;
	ACE_Message_Block* headBlock = NULL;
	ACE_Message_Block* commandBlock = NULL;

	bool receiveData = NetworkInstance->PopAllMessage(queue_id, &headBlock, -1);
	if (false == receiveData)
		return;

	commandBlock = headBlock;
	while(NULL != commandBlock)
	{
		MessagePostee postee;
		memcpy(&postee, commandBlock->rd_ptr(), sizeof(MessagePostee));
		commandBlock->rd_ptr(sizeof(MessagePostee));
		ACE_Message_Block* payloadBlock = commandBlock->cont();

		switch(postee.command)
		{
		case eMH_Establish:
			_OnConnect(postee.stream_id, payloadBlock->rd_ptr());
			break;
		case eMH_Read:
			_OnRead(postee.stream_id, payloadBlock->rd_ptr(), payloadBlock->length());
			break;
		case eMH_Close:
			_OnClose(postee.stream_id);
			break;
		case eMH_Custom:
			// process user defined data
			break;
		default:
			break;
		}
		commandBlock = payloadBlock->cont();
	}
	headBlock->release();
}

void NetworkThread::_OnConnect(unsigned int streamID, const char* ip)
{
	HALF_LOG(
		ConsoleLogger, 
		ACE_TEXT("OnConnect(%d)"), streamID);
	SendDummyData(streamID);
}

void NetworkThread::_OnRead(unsigned int streamID, char* buffer, size_t length)
{
	ACE_DEBUG ((LM_DEBUG, "[%t] OnRead(%d, %d).\n", streamID, length));
	if (_DisconnectTest(streamID))
		return;

	_EchoTest(streamID, buffer, length);
	SendDummyData(streamID);
}

bool NetworkThread::_DisconnectTest( unsigned int streamID ) 
{
	if (false == _configObj.GetValue<bool>(ACE_TEXT("DisconnectTest")))
		return false;

	if (_disconnectCycle->IsExpire())
	{
		NetworkInstance->CloseStream(streamID);
		return true;
	}

	return false;
}

void NetworkThread::_EchoTest( unsigned int streamID, char* buffer, size_t length )
{
	if (false == _configObj.GetValue<bool>(ACE_TEXT("EchoTest")))
		return;
	int elaspedTick = 0;
	if (false == _bufferChecker->IsValidBuffer(streamID, buffer, (unsigned int)length, elaspedTick))
	{
		HALF_LOG(
			ConsoleLogger, 
			ACE_TEXT("Verify fail. Elasped(%d) ElementCount(%d)"), 
			elaspedTick, _bufferChecker->GetElementCount());
	}
	else
	{
		if (_debugMsgCycle->IsExpire())
		{
			HALF_LOG(
				ConsoleLogger, 
				ACE_TEXT("Verify success. Elasped(%d) ElementCount(%d)"), 
				elaspedTick, _bufferChecker->GetElementCount());
		}
	}
}

void NetworkThread::_OnClose(unsigned int streamID)
{
	HALF_LOG(
		ConsoleLogger, 
		ACE_TEXT("OnClose(%d)"), streamID);
}

long NetworkThread::GetCurrentConnection()
{
	HalfNetwork::NetworkFacadeInfo info;
	NetworkInstance->GetInformation(info);
	return info.service_count;
}

void NetworkThread::SetReceiveQueueId( unsigned char id )
{
	_queueId = id;
}

void NetworkThread::MakeRandomBuffer( char* buffer, unsigned int len )
{
	for(unsigned int i=0; i<len; ++i)
		buffer[i] = rand()%255;
}

void NetworkThread::SendDummyData( unsigned int streamId )
{
	char buffer[1024] = {0,};
	unsigned int sendSize = rand()%1000 + 4;
	MakeRandomBuffer(buffer, sendSize);

	if (_configObj.GetValue<bool>(ACE_TEXT("EchoTest")))
		_bufferChecker->AddSendBuffer(streamId, buffer, sendSize);

	NetworkInstance->SendReserve(streamId, buffer, sendSize, _sendTerm);
}