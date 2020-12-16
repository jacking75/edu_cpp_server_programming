#include "OmokServer.h"
#include <iostream>

namespace OmokServerLib
{
	OmokServer::OmokServer()
	{

	}

	OmokServer::~OmokServer()
	{
		if (m_pNetwork) 
		{
			m_pNetwork->Release();
		}
	}

	ERROR_CODE OmokServer::Init(const NServerNetLib::ServerConfig Config)
	{
		m_pLogger = std::make_unique<NServerNetLib::Logger>();
		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();

		m_pLogger->info("LoadConfig Port : {} , BackLog : {} ", Config.Port, Config.BackLogCount);

		auto result = m_pNetwork->Init(Config, m_pLogger.get());

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
		m_pUserMgr->Init(Config.MaxClientCount);

		m_pRedisMgr = std::make_unique<RedisManager>();
		auto redisResult = m_pRedisMgr->Connect(Config.RedisAddress.c_str(), Config.RedisPortNum);

		if (redisResult != ERROR_CODE::NONE)
		{
			m_pLogger->error("Redis Connect Fail");
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}
		m_pRedisMgr->Init(m_pUserMgr.get());
		m_pRedisMgr->SendPacketFunc = sendPacketFunc;

		m_pRoomMgr = std::make_unique<RoomManager>();
		m_pRoomMgr->SendPacketFunc = sendPacketFunc;
		m_pRoomMgr->Init(Config.MaxRoomCountByLobby, m_pNetwork.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->SendPacketFunc = sendPacketFunc;
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(),m_pRoomMgr.get(),m_pRedisMgr.get(), m_pLogger.get(), Config);
	
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
		//TODO 최흥배
		// 이렇게 하면 무의미한 CPU 소모가 있지 않을까요? 
		while (m_IsRun)
		{
			// TODO 최흥배
			// 상태 조사를 아주 짧은 시간마다 할 필요 없습니다.
			// 그리고 한번 조사할 때 모든 객체를 조사하면 StateCheck() 호출 때 마다 CPU 사용률이 평균을 넘게 됩니다.
			// 그래서 조사할 때 한번에 다하는 것 보다 나누어서 하는 것이 좋습니다.
			// 예를들면 전체 객체 수가 1000개 라면  100 밀리센컨드 마다 200개씩 나누어서 조사합니다.
			// 이렇게 하면 StateCheck() 호출에 따른 부하가 작아집니다. 단점은 객체마다 시간이 조금 오바할 수 있는데 이것은 밀리세컨드 단위라서 무시해도 괜찮습니다.
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