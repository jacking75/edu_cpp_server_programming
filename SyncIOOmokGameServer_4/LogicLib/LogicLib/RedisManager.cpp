#include "RedisManager.h"

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

		Init();

		return ERROR_CODE::NONE;
	}

	void RedisManager::Init()
	{
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
}
