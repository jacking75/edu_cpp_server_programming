#pragma comment(lib,"hiredis")
#include "RedisManager.h"
#include "sstream"
#include "PacketDef.h"
#include <string>
namespace OmokServerLib
{

	ERROR_CODE RedisManager::ConfirmLogin(RedisRequestInfo redisRequestInfo)
	{
		auto reqPkt = (NCommon::PktLogInReq*)redisRequestInfo.CommandBody;
		NCommon::PktLogInRes resPkt;

		std::string userId(reqPkt->szID);
		auto commandString = "GET" + userId;

		redisReply* reply = (redisReply*)redisCommand(m_Connection, commandString.c_str());

		if (reply == nullptr || reply->str == nullptr || reply->type == REDIS_REPLY_ERROR)
		{
			resPkt.SetError(ERROR_CODE::REDIS_GET_FAIL);
			freeReplyObject(reply);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return ERROR_CODE::REDIS_GET_FAIL;
		}

		if (reply->str == reqPkt->szPW)
		{
			resPkt.SetError(ERROR_CODE::NONE);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
		}

		freeReplyObject(reply);
		return ERROR_CODE::NONE;
	}
	
	std::optional <CommandRequest> RedisManager::GetCommandResult()
	{
		std::lock_guard<std::mutex> lock(m_Mutex);

		if (m_RedisRequestQueue.empty() == false)
		{
			CommandRequest packetInfo = m_RedisRequestQueue.front();
			m_RedisRequestQueue.pop_front();
			return packetInfo;
		}

		return std::nullopt;
	}

}
