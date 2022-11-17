// StressClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <ace/ACE.h>
#include "NetworkFacade.h"
#ifdef WIN32
#include "ProactorFactory.h"
#else
#include "ReactorFactory.h"
#endif
#include "SimpleConfig.h"
#include "SendThread.h"
#include "MessageHeader.h"
#include "IOStatus.h"

IOStatus g_ioStatus;
SendThread g_sendThread;
int g_sendThreadCount = 0;

bool NetworkInit(const HalfNetwork::SimpleConfig& configReader, unsigned char queueID)
{
	unsigned short port = configReader.GetValue<unsigned short>(ACE_TEXT("ServerPort"));
	HalfNetwork::SystemConfig config;
	config.Worker_Thread_Count = configReader.GetValue<unsigned char>(ACE_TEXT("WorkerThread"));
	config.Receive_Buffer_Len = configReader.GetValue<unsigned int>(ACE_TEXT("ReceiveBufferLength"));
	config.Interval_Send_Term = configReader.GetValue<unsigned int>(ACE_TEXT("UpdateTerm"));
	config.Send_Mode = HalfNetwork::eSM_Direct;
	g_sendThreadCount = configReader.GetValue<int>(ACE_TEXT("SendThread"));
	unsigned int aimSendByte = configReader.GetValue<unsigned int>(ACE_TEXT("AimSendByte"));
	g_sendThread.SetAimSendByte(aimSendByte);

	HALF_LOG(ConsoleLogger, ACE_TEXT("Port(%d)"), port);
	HALF_LOG(ConsoleLogger, ACE_TEXT("WorkerThread Count(%d)"), config.Worker_Thread_Count);
	HALF_LOG(ConsoleLogger, ACE_TEXT("SendThread Count(%d)"), g_sendThreadCount);
	HALF_LOG(ConsoleLogger, ACE_TEXT("Receive buffer length(%d)"), config.Receive_Buffer_Len);
	HALF_LOG(ConsoleLogger, ACE_TEXT("Update term(%d)"), config.Interval_Send_Term);
	HALF_LOG(ConsoleLogger, ACE_TEXT("Aim SendByte(%d)"), aimSendByte);

#ifdef WIN32
	if (false == NetworkInstance->Create<HalfNetwork::ProactorFactory>())
#else
	if (false == NetworkInstance->Create<HalfNetwork::ReactorFactory>())
#endif
		return false;
	if (false == NetworkInstance->Open(&config))
		return false;

	HALF_LOG(ConsoleLogger, ACE_TEXT("Network start"), 0);
	return true;
}

void NetworkFini()
{
	NetworkInstance->Close();
	NetworkInstance->Destroy();
}

void OnConnect(unsigned int streamID, const char* ip)
{
	g_sendThread.SetIoStatus(&g_ioStatus);
	g_sendThread.Open(g_sendThreadCount);
}

void OnRead(unsigned int streamID, char* buffer, size_t length)
{
	g_ioStatus.AddRecvBytes((long)length);
}

void OnClose(unsigned int streamID)
{
}

void ProcessRecvQueue(unsigned char queue_id)
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
			OnConnect(postee.stream_id, payloadBlock->rd_ptr());
			break;
		case eMH_Read:
			OnRead(postee.stream_id, payloadBlock->rd_ptr(), payloadBlock->length());
			break;
		case eMH_Close:
			OnClose(postee.stream_id);
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

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
	srand((signed)time(NULL));
	ACE::init();

	const ACE_TCHAR* ConfigFilename = ACE_TEXT("StressClientConfig.txt");
	HalfNetwork::SimpleConfig configReader;

	ConsoleLogger->SetConsoleLog();
	ConsoleLogger->AppendThreadId();

	if (false == configReader.ReadFile(ConfigFilename))
	{
		HALF_LOG(ConsoleLogger, ACE_TEXT("Read config fail(%s)"), ConfigFilename);
		return 0;
	}
	const unsigned char QueueID = 103;
	if (false == NetworkInit(configReader, QueueID))
	{
		HALF_LOG(ConsoleLogger, ACE_TEXT("NetworkInit fail."), 0);
		return 0;
	}

	unsigned int connectionCount = configReader.GetValue<unsigned int>(ACE_TEXT("ConnectionCount"));
	unsigned short port = configReader.GetValue<unsigned short>(ACE_TEXT("ServerPort"));
	tstring serverIp = configReader.GetValue<tstring>(ACE_TEXT("ServerIp"));
	if (false == NetworkInstance->Connect(serverIp.c_str(), port, QueueID))
	{
		HALF_LOG(ConsoleLogger, ACE_TEXT("Connect fail."), 0);
		return 0;
	}

	while(true)
		ProcessRecvQueue(QueueID);

	NetworkFini();
	g_sendThread.Close();
	ACE::fini();
	return 0;
}

