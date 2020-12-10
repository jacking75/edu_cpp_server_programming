#pragma once


#include "PacketID.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "../../ServerNetLib/ServerNetLib/Define.h"
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
#include <functional>
#include "RedisManager.h"

namespace OmokServerLib
{		
	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		typedef ERROR_CODE(PacketProcess::* PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)NCommon::PACKET_ID::MAX];

	public:

		PacketProcess() = default;
		~PacketProcess() = default;

		ERROR_CODE NtfSysConnectSession(PacketInfo packetInfo);

		ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);

		void Init(NServerNetLib::TcpNetwork* pNetwork, UserManager* pUserMgr, RoomManager* pRoomMgr, RedisManager* pRedisMgr, NServerNetLib::Logger* pLogger, NServerNetLib::ServerConfig pConfig);

		void Process(PacketInfo packetInfo);

		ERROR_CODE Login(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);

		ERROR_CODE RoomChat(PacketInfo packetInfo);

		ERROR_CODE MatchUser(PacketInfo packetInfo);

		ERROR_CODE GamePut(PacketInfo packetInfo);

		ERROR_CODE GameReady(PacketInfo packetInfo);

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;		

	private:

		UserManager* m_pRefUserMgr;

		RoomManager* m_pRefRoomMgr;

		RedisManager* m_pRefRedisMgr;

		NServerNetLib::Logger* m_pRefLogger;

		std::string SetWinUserID(Room* pRoom, ERROR_CODE endResult);

		std::pair<ERROR_CODE, std::string> PutStone(Room* pRoom, int x, int y, int sessionIndex);

		ERROR_CODE UserSetGame(User* pUser, int sessionIndex);
	};
}