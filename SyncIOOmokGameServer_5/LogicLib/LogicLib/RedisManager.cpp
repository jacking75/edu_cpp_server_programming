#include "RedisManager.h"
#include "PacketDef.h"
#include "User.h"

#include "../../../ThirdParty/RedisCpp-hiredis/CRedisConn.h"
#pragma comment(lib, "hiredis")
#pragma warning(push)
#pragma warning(disable : 4200)
#include <hiredis.h>
#pragma warning(pop)


namespace OmokServerLib
{
	RedisManager::~RedisManager()
	{
		Disconnect();
	}

	ERROR_CODE RedisManager::Connect(const char* ipAddress, const int portNum)
	{
		con = new RedisCpp::CRedisConn;

		if (!con->connect(ipAddress, portNum))
		{
			return ERROR_CODE::REDIS_CONNECT_FAIL;
		}

		return ERROR_CODE::NONE;
	}

	void RedisManager::Init(UserManager* pUserMgr, ConnectedUserManager* pConUserMgr)
	{
		m_pRefUserMgr = pUserMgr;
		m_pRefConUserMgr = pConUserMgr;

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
	}

	ERROR_CODE RedisManager::ConfirmLogin(RedisRequestInfo redisRequestInfo)
	{
		auto reqPkt = (OmokServerLib::PktLogInReq*)redisRequestInfo.commandBody;
		OmokServerLib::PktLogInRes resPkt;

		std::string userID = reqPkt->szID;
		std::string password;
		
		if (con->get(userID, password) == false)
		{
			resPkt.SetError(ERROR_CODE::REDIS_GET_FAIL);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)OmokServerLib::PACKET_ID::LOGIN_IN_RES, sizeof(OmokServerLib::PktLogInRes), (char*)&resPkt);
			return ERROR_CODE::REDIS_GET_FAIL;
		}

		if (password == reqPkt->szPW)
		{
			auto addRet = m_pRefUserMgr->AddUser(redisRequestInfo.sessionIndex, reqPkt->szID);
			auto userInfo = m_pRefUserMgr->GetUser(redisRequestInfo.sessionIndex);

			userInfo.second->SetLogin();
			m_pRefConUserMgr->SetLogin(redisRequestInfo.sessionIndex);

			resPkt.SetError(ERROR_CODE::NONE);
			SendPacketFunc(redisRequestInfo.sessionIndex, (short)OmokServerLib::PACKET_ID::LOGIN_IN_RES, sizeof(OmokServerLib::PktLogInRes), (char*)&resPkt);
			return ERROR_CODE::NONE;
		}

		resPkt.SetError(ERROR_CODE::REDIS_LOGIN_PW_INCORRECT);
		SendPacketFunc(redisRequestInfo.sessionIndex, (short)OmokServerLib::PACKET_ID::LOGIN_IN_RES, sizeof(OmokServerLib::PktLogInRes), (char*)&resPkt);
		
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
