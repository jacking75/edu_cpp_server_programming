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
		//TODO 최흥배
		// 이미 로그인 한 유저인데 또 로그인 요청을 보낸 경우 잘 방어하고 있나요?

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