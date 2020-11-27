#include "OmokServer.h"
#include <iostream>

namespace ChatServerLib
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
		m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
		auto result = m_pNetwork->Init(Config);

		if (result != NServerNetLib::NET_ERROR_CODE::NONE)
		{
			return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
		}

		auto sendPacketFunc = [&](const int sessionIndex, const short packetId, const short bodySize, char* pMsg)
		{
			m_pNetwork->SendData(sessionIndex, packetId, bodySize, pMsg);
		};

		m_pUserMgr = std::make_unique<UserManager>();
		m_pUserMgr->Init(Config.MaxClientCount);

		m_pRoomMgr = std::make_unique<RoomManager>();
		m_pRoomMgr->SendPacketFunc = sendPacketFunc;
		m_pRoomMgr->Init(Config.MaxRoomCountByLobby, m_pNetwork.get());

		m_pPacketProc = std::make_unique<PacketProcess>();
		m_pPacketProc->SendPacketFunc = sendPacketFunc;
		m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(),m_pRoomMgr.get(), Config);
	
		m_IsRun = true;

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