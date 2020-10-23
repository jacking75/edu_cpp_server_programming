#pragma once

#include <memory>

#include "Packet.h"
#include "PacketProcess.h"

namespace ChatServerLib
{
	class ChatServer
	{
	public:
		ChatServer();
		~ChatServer();

		void Run();

		void Stop();
		NServerNetLib::ERROR_CODE Init(const NServerNetLib::ServerConfig* pConfig);

	private:
		bool m_IsRun = false;
		std::unique_ptr<NServerNetLib::TcpNetwork> m_pNetwork;
		std::unique_ptr<PacketProcess> m_pPacketProc;
	};
}
