#include <tuple>
#include <iostream>
#include "User.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
		if (m_pRefConUserMgr->CheckUserLogin(packetInfo.SessionIndex) == true)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::LOGIN_IN_RES, ERROR_CODE::ALREADY_LOGIN_STATE);
			return ERROR_CODE::ALREADY_LOGIN_STATE;
		}

		CommandRequest redisRequestInfo;
		redisRequestInfo.SessionIndex = packetInfo.SessionIndex;
		redisRequestInfo.RedisTaskID = (int)RedisTaskID::confirmLogin;
		redisRequestInfo.CommandBody = packetInfo.pRefData;

		m_pRefRedisMgr->InsertRedisRequestQueue(redisRequestInfo);

		return ERROR_CODE::NONE;
	}
	
}