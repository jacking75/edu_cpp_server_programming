#pragma comment(lib, "hiredis")
#pragma warning(push)
#pragma warning(disable : 4200)
#include <hiredis.h>
#pragma warning(pop)
#include "RedisManager.h"
#include "PacketDef.h"
#include "User.h"
namespace OmokServerLib
{
	RedisManager::RedisManager()
	{

	}

	RedisManager::~RedisManager()
	{
		Disconnect();
	}

	ERROR_CODE RedisManager::Connect(const char* ipAddress, const int portNum)
	{
		if (m_Connection != nullptr)
		{
			return ERROR_CODE::REDIS_ALREADY_CONNECT_STATE;
		}

		m_Connection = redisConnect(ipAddress, portNum);
		if (m_Connection == nullptr)
		{
			return ERROR_CODE::REDIS_CONNECT_FAIL;
		}
		if (m_Connection->err)
		{
			Disconnect();
			return ERROR_CODE::REDIS_CONNECT_FAIL;
		}


		return ERROR_CODE::NONE;
	}

	void RedisManager::Init(UserManager* pUserMgr)
	{
		m_pRefUserMgr = pUserMgr;

		for (int i = 0; i < (int)REDIS_TASK_ID_MAX; ++i)
		{
			PacketFuncArray[i] = nullptr;
		}

		PacketFuncArray[(int)RedisTaskID::confirmLogin] = &RedisManager::ConfirmLogin;

		m_IsRun = true;
		m_RedisThread = std::make_unique<std::thread>([&] {RedisProcessThread(); });

	}


	void RedisManager::Process(RedisRequestInfo redisRequestInfo)
	{
		auto redisTaskID = redisRequestInfo.redisTaskID;

		if (PacketFuncArray[redisTaskID] == nullptr)
		{
			return;
		}

		(this->*PacketFuncArray[redisTaskID])(redisRequestInfo);

	}

	void RedisManager::Stop()
	{
		m_IsRun = false;
	}


	void RedisManager::RedisProcessThread()
	{
		while (m_IsRun)
		{
			auto packetInfo = GetCommandResult();

			if (packetInfo.has_value() == false)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			else
			{
				Process(packetInfo.value());
			}
		}
	}

	void RedisManager::InsertRedisRequestQueue(RedisRequestInfo redisRequestInfo)
	{
		m_RedisRequestQueue.push_back(redisRequestInfo);
	}

	void RedisManager::Disconnect()
	{
		m_RedisThread->join();
		if (m_Connection != nullptr)
		{
			redisFree(m_Connection);
		}
		m_Connection = nullptr;
	}

	ERROR_CODE RedisManager::ConfirmLogin(RedisRequestInfo redisRequestInfo)
	{
		auto reqPkt = (NCommon::PktLogInReq*)redisRequestInfo.CommandBody;
		NCommon::PktLogInRes resPkt;

		std::string userId(reqPkt->szID);
		auto commandString = "GET " + userId;

		redisReply* reply = (redisReply*)redisCommand(m_Connection, commandString.c_str());

		if (reply == nullptr || reply->str == nullptr || reply->type == REDIS_REPLY_ERROR)
		{
			resPkt.SetError(ERROR_CODE::REDIS_GET_FAIL);
			freeReplyObject(reply);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return ERROR_CODE::REDIS_GET_FAIL;
		}

		std::string userInputPW(reqPkt->szPW);
		std::string userRedisPW(reply->str);

		if (userRedisPW == userInputPW)
		{
			auto addRet = m_pRefUserMgr->AddUser(redisRequestInfo.sessionIndex, reqPkt->szID);
			auto userInfo = m_pRefUserMgr->GetUser(redisRequestInfo.sessionIndex);
			userInfo.second->SetLogin();

			resPkt.SetError(ERROR_CODE::NONE);
			freeReplyObject(reply);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return ERROR_CODE::NONE;
		}

		freeReplyObject(reply);
		resPkt.SetError(ERROR_CODE::REDIS_LOGIN_PW_INCORRECT);
		SendPacketFunc(redisRequestInfo.sessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);

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
