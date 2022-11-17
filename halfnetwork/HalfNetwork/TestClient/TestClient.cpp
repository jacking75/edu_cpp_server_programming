// TestClient.cpp : Defines the entry point for the console application.
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
#include "NetworkThread.h"
//#include <HalfNetwork/MemoryObject.h>
//#include <HalfNetwork/Logger.h>


void EventSleep(int ms)
{
	ACE_Time_Value waitTime(ms/1000, (ms%1000)*1000);
	ACE_Event eventObj;
	eventObj.wait(&waitTime, 0);
}

bool NetworkInit(const HalfNetwork::SimpleConfig& configReader, unsigned char queueID)
{
	unsigned short port = configReader.GetValue<unsigned short>(ACE_TEXT("ServerPort"));
	HalfNetwork::SystemConfig config;
	config.Worker_Thread_Count = configReader.GetValue<unsigned char>(ACE_TEXT("WorkerThread"));
	config.Receive_Buffer_Len = configReader.GetValue<unsigned int>(ACE_TEXT("ReceiveBufferLength"));
	config.Interval_Send_Term = configReader.GetValue<unsigned int>(ACE_TEXT("UpdateTerm"));
	config.Send_Mode = HalfNetwork::eSM_Interval;
	config.ZombieConnectionTerm = 0;

	HALF_LOG(ConsoleLogger, ACE_TEXT("Port(%d)"), port);
	HALF_LOG(ConsoleLogger, ACE_TEXT("WorkerThread Count(%d)"), config.Worker_Thread_Count);
	HALF_LOG(ConsoleLogger, ACE_TEXT("Receive buffer length(%d)"), config.Receive_Buffer_Len);
	HALF_LOG(ConsoleLogger, ACE_TEXT("Update term(%d)"), config.Interval_Send_Term);
	
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

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
	srand((signed)time(NULL));
	ACE::init();

	const ACE_TCHAR* ConfigFilename = ACE_TEXT("TestClientConfig.txt");
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
		HALF_LOG(ConsoleLogger, ACE_TEXT("NetworkInit Fail."), 0);
		return 0;
	}

	unsigned int connectionCount = configReader.GetValue<unsigned int>(ACE_TEXT("ConnectionCount"));
	unsigned short port = configReader.GetValue<unsigned short>(ACE_TEXT("ServerPort"));
	tstring serverIp = configReader.GetValue<tstring>(ACE_TEXT("ServerIp"));
	NetworkThread netThread(configReader);
	netThread.SetReceiveQueueId(QueueID);
	netThread.Open(1);
	
	HALF_LOG(ConsoleLogger, ACE_TEXT("ConnectCount = %d, start connect."), connectionCount);
	for(unsigned int i=0; i<connectionCount; ++i)
	{
		bool result = NetworkInstance->Connect(serverIp.c_str(), port, QueueID);
		EventSleep(configReader.GetValue<int>(ACE_TEXT("ConnectTerm")));
	}
	HALF_LOG(ConsoleLogger, ACE_TEXT("connect end"), 0);
	
	if (configReader.GetValue<bool>(ACE_TEXT("DisconnectTest")))
	{
		while(true)
		{
			int fillupConnection = connectionCount - netThread.GetCurrentConnection();
			HALF_LOG(ConsoleLogger, 
				ACE_TEXT("Connection count(%ld) fillup(%d)"), 
				netThread.GetCurrentConnection(), fillupConnection);
			//HalfNetwork::MemoryPoolInstance->Dump();
			for(int i=0; i<fillupConnection; ++i)
			{
				if (false == NetworkInstance->Connect(serverIp.c_str(), port, QueueID))
					HALF_LOG(ConsoleLogger, ACE_TEXT("Connection fail"), 0);
				EventSleep(configReader.GetValue<int>(ACE_TEXT("ConnectTerm")));
			}
			EventSleep(1000);
		}
	}

	getchar();

	NetworkFini();
	netThread.Close();
	ACE::fini();
	return 0;
}

