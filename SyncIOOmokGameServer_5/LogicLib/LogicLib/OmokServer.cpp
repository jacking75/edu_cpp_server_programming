#include "OmokServer.h"
#include "User.h"
#include "Config.h"
#include <iostream>

namespace OmokServerLib
{
	OmokServer::~OmokServer()
	{
		if (m_pNetwork) 
		{
			m_pNetwork->Release();
		}
	}

	ERROR_CODE OmokServer::Init()
	{
		m_pConfig = std::make_unique<Config>();
		m_pConfig->Load();

		m_pLogger = std::make_unique<NServerNetLib::Logger>();
		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();

		m_pLogger->info("LoadConfig Port : {} , BackLog : {} ", m_pConfig->port, m_pConfig->backLogCount);

		auto result = m_pNetwork->Init(m_pConfig.get(), m_pLogger.get());

		if (result != NServerNetLib::NET_ERROR_CODE::NONE)
		{
			m_pLogger->error("Network Init Fail");
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}

		auto sendPacketFunc = [&](const int sessionIndex, const short packetId, const short bodySize, char* pMsg)
		{
			m_pNetwork->SendData(sessionIndex, packetId, bodySize, pMsg);
		};

		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(m_pConfig->maxClientCount);

		m_pRedisMgr = std::make_unique<RedisManager>();
		auto redisResult = m_pRedisMgr->Connect(m_pConfig->redisAddress.value().c_str(), m_pConfig->redisPortNum);

		if (redisResult != ERROR_CODE::NONE)
		{
			m_pLogger->error("Redis Connect Fail");
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}
		m_pRefConUserMgr = std::make_unique<ConnectedUserManager>();
		m_pRefConUserMgr->Init(m_pNetwork->ClientSessionPoolSize(), m_pNetwork.get(), m_pLogger.get());

		m_pRedisMgr->Init(m_pUserMgr.get(), m_pRefConUserMgr.get());
		m_pRedisMgr->SendPacketFunc = sendPacketFunc;

		m_pRoomMgr = std::make_unique<RoomManager>();
		m_pRoomMgr->SendPacketFunc = sendPacketFunc;
		m_pRoomMgr->Init(m_pConfig->maxRoomCountByLobby, m_pNetwork.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->SendPacketFunc = sendPacketFunc;
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(),m_pRoomMgr.get(),m_pRedisMgr.get(), m_pLogger.get(), m_pRefConUserMgr.get());
	
		m_IsRun = true;

		m_pLogger->info("Init Success. Server Run");

		return ERROR_CODE::NONE;
	}
	
	void OmokServer::Stop()
	{
		m_IsRun = false;
	}

	ERROR_CODE OmokServer::Run()
	{
		m_pNetwork->Run();

		mainThread = std::make_unique<std::thread>([&]() {MainProcessThread(); });

		std::cout << "press any key to exit..." << "\n";
		getchar();

		Stop();

		mainThread->join();

		return ERROR_CODE::NONE;
	}

	void OmokServer::MainProcessThread()
	{	
		while (m_IsRun)
		{
			m_pPacketProc->StateCheck();

			auto packetInfo = m_pNetwork->GetReceivePacket();

			if (packetInfo.has_value() == false)
			{
				continue;
			}
			else
			{
				m_pPacketProc->Process(packetInfo.value());
			}
		}
	}
}