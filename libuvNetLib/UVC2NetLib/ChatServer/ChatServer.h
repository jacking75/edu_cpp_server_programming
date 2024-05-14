#pragma once

#include "PacketManager.h"
#include "Packet.h"
#include "../UVC2NetLibz/UVTCPServer.h"

class ChatServer : public UVC2NetLibz::TCPServer
{
public:
	ChatServer() = default;
	~ChatServer() = default;

	int Init(UVC2NetLibz::NetConfig netConfig_);
	
	void StartAsync();
	
	void End();

	static void WriteLog(const int eLogInfoType, const char* szLogMsg)
	{
		printf("[%s] %s\r\n", UVC2NetLibz::LogLevelStr[eLogInfoType], szLogMsg);
	}

	static void WriteLog2(const int eLogInfoType, const char* szOutputString, ...)
	{		
		char logmsg[256] = { 0, };

		va_list	argptr;
		va_start(argptr, szOutputString);
		vsprintf_s(logmsg, 256, szOutputString, argptr);
		va_end(argptr);

		printf("[%s] %s\r\n", UVC2NetLibz::LogLevelStr[eLogInfoType], logmsg);
	}

private:
	virtual void OnConnect(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) override;
		
	virtual void OnClose(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) override;
	
	virtual void OnReceive(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_,
			const uint16_t dataSize_, char* pData) override;
	
	static void WriteToLog(UVC2NetLibz::NetConfig netConfig);
	

	UVC2NetLibz::NetConfig mNetConfig;

	std::thread mRunThread;

	std::unique_ptr<PacketManager> m_pPacketManager;
};