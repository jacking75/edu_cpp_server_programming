#pragma once


#include "PacketID.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "../../ServerNetLib/ServerNetLib/Define.h"
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
#include <functional>
#include "RedisManager.h"
#include "ConnectedUserManager.h"

namespace OmokServerLib
{		
	class PacketProcess
	{
		using PacketInfo = NServerNetLib::RecvPacketInfo;
		typedef ERROR_CODE(PacketProcess::* PacketFunc)(PacketInfo);
		PacketFunc PacketFuncArray[(int)OmokServerLib::PACKET_ID::MAX];

	public:

		PacketProcess() = default;
		~PacketProcess() = default;

		ERROR_CODE NtfSysConnectSession(PacketInfo packetInfo);

		ERROR_CODE NtfSysCloseSession(PacketInfo packetInfo);

		void Init(NServerNetLib::TcpNetwork* pNetwork, 
					UserManager* pUserMgr,
					RoomManager* pRoomMgr, 
					RedisManager* pRedisMgr, 
					NServerNetLib::Logger* pLogger, 
					ConnectedUserManager* m_pConUserMgr);

		void Process(PacketInfo packetInfo);

		ERROR_CODE Login(PacketInfo packetInfo);

		ERROR_CODE RoomEnter(PacketInfo packetInfo);

		ERROR_CODE RoomLeave(PacketInfo packetInfo);

		ERROR_CODE RoomChat(PacketInfo packetInfo);

		ERROR_CODE MatchUser(PacketInfo packetInfo);

		ERROR_CODE GamePut(PacketInfo packetInfo);

		ERROR_CODE GameReady(PacketInfo packetInfo);

		void SendPacketSetError(int sessionIndex, OmokServerLib::PACKET_ID packetID, ERROR_CODE errorCode);

		std::function<void(const int,const short, char*)> SendPacketFunc;		

		void StateCheck();

	private:

		UserManager* m_pRefUserMgr;

		RoomManager* m_pRefRoomMgr;

		RedisManager* m_pRefRedisMgr;

		ConnectedUserManager* m_pRefConUserMgr;

		NServerNetLib::Logger* m_pRefLogger;

		std::string SetWinUserID(Room* pRoom, ERROR_CODE endResult);

		std::pair<ERROR_CODE, std::string> PutStone(Room* pRoom, int x, int y, int sessionIndex);

		std::optional <std::pair<User*, Room*>> FindUserAndRoom(int sessionIndex, OmokServerLib::PACKET_ID packetID, ERROR_CODE roomErrorCode);
	};
}