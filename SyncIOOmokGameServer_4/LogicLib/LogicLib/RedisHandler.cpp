#pragma comment(lib,"hiredis")
#include "RedisManager.h"
#include "hiredis.h"
#include "sstream"

namespace OmokServerLib
{
	/*
	void RedisHandler::ExecuteCommandProcess()
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
	void RedisHandler::ExecuteCommandAsync(const CommandRequest& request)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_RequestQueue.push(request);
	}

	CommandResponse RedisHandler::GetCommandResult()
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

	std::string RedisHandler::CommandRequestToString(const CommandRequest& request)
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
	*/

	ERROR_CODE RedisManager::ConfirmLogin(PacketInfo packinfo)
	{



	}
	/*
	bool RedisManager::GetCommandResult()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (m_ResponseQueue.empty())
		{
			lock.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(m_SendCheckTick));
			continue;
		}

		response = m_ResponseQueue.front();
		m_ResponseQueue.pop();
		return response;

		response.m_ErrorCode = ERROR_CODE::REDIS_RECEIVE_TIME_OUT;
		return response;
	}*/

}
