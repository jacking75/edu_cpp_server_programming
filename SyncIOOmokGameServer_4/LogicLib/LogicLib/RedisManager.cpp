#pragma comment(lib,"hiredis")
#include "RedisManager.h"
#include "hiredis.h"
#include "sstream"

namespace OmokServerLib
{
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

		m_RedisThread = std::make_unique<std::thread>([&] {ExecuteCommandProcess(); });

		return ERROR_CODE::NONE;
	}

	void RedisManager::ExecuteCommandProcess()
	{
		while (m_Connection != nullptr)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);

			if (m_RequestQueue.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(m_SendCheckTick));
				continue;
			}

			CommandRequest request = m_RequestQueue.front();
			m_RequestQueue.pop();
			
			CommandResponse result;
			result.m_ErrorCode = ERROR_CODE::NONE;

			
			std::string commandString = CommandRequestToString(request);
			if (commandString == "")
			{
				result.m_ErrorCode = ERROR_CODE::REDIS_COMMAND_FAIL;
				m_ResponseQueue.push(result);
				continue;
			}

			redisReply* reply = (redisReply*)redisCommand(m_Connection, commandString.c_str());
			
			if (reply == nullptr)
			{
				result.m_ErrorCode = ERROR_CODE::REDIS_GET_FAIL;
				m_ResponseQueue.push(result);
				freeReplyObject(reply);
				continue;
			}

			if (reply->str == nullptr)
			{
				result.m_Result = "";
				m_ResponseQueue.push(result);
				freeReplyObject(reply);
				continue;
			}

			result.m_Result = reply->str;

			if (reply->type == REDIS_REPLY_ERROR)
			{
				result.m_ErrorCode = ERROR_CODE::REDIS_GET_FAIL;
				m_ResponseQueue.push(result);
				freeReplyObject(reply);
				continue;
			}

			m_ResponseQueue.push(result);
			freeReplyObject(reply);
		}
	}
	void RedisManager::ExecuteCommandAsync(const CommandRequest& request)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_RequestQueue.push(request);
	}

	CommandResponse RedisManager::GetCommandResult()
	{
		CommandResponse response;
		UINT32 waitTick = 0;
		while (waitTick < m_ReceiveCheckTimeOut)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			if (m_ResponseQueue.empty())
			{
				lock.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(m_SendCheckTick));
				waitTick += m_SendCheckTick;
				continue;
			}

			response = m_ResponseQueue.front();
			m_ResponseQueue.pop();
			return response;
		}

		response.m_ErrorCode = ERROR_CODE::REDIS_RECEIVE_TIME_OUT;
		return response;
	}

	std::string RedisManager::CommandRequestToString(const CommandRequest& request)
	{

		if (request.m_CommandBodySize < sizeof(Get))
		{
			return "";
		}

		std::string output = "GET";

		Get* get = reinterpret_cast<Get*>(request.m_CommandBody);
		auto resultString = output + get->m_Key;

		return resultString;

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
