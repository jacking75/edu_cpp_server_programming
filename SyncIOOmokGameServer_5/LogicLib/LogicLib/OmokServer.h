#pragma once

#include "PacketProcess.h"
#include "RedisManager.h"

namespace OmokServerLib
{
	class OmokServer
	{
	public:
		OmokServer();
		~OmokServer();

		ERROR_CODE Run();

		void Stop();

		ERROR_CODE Init(const NServerNetLib::ServerConfig pConfig);

		void MainProcessThread();

	private:

		bool m_IsRun = false;

		std::unique_ptr<NServerNetLib::TcpNetwork> m_pNetwork;

		std::unique_ptr<NServerNetLib::Logger> m_pLogger;

		std::unique_ptr<UserManager> m_pUserMgr;

		std::unique_ptr<RoomManager> m_pRoomMgr;

		std::unique_ptr<PacketProcess> m_pPacketProc;

		std::unique_ptr<RedisManager> m_pRedisMgr;

		std::unique_ptr<std::thread> mainThread;

	};
}
