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
		auto reqPkt = (NCommon::PktLogInReq*)packetInfo.pRefData;
		NCommon::PktLogInRes resPkt;

		CommandRequest redisRequestInfo;
		redisRequestInfo.sessionIndex = packetInfo.SessionIndex;
		redisRequestInfo.redisTaskID = (int)RedisTaskID::confirmLogin;
		redisRequestInfo.CommandBody = packetInfo.pRefData;

		m_pRefRedisMgr->InsertRedisRequestQueue(redisRequestInfo);

		return ERROR_CODE::NONE;
	}
	
}