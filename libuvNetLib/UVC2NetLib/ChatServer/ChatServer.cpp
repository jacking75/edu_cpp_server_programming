#include "ChatServer.h"

int ChatServer::Init(UVC2NetLibz::NetConfig netConfig_)
{
	UVC2NetLibz::LogFuncPtr = [](const int eLogInfoType, const char* szLogMsg) {
		ChatServer::WriteLog(eLogInfoType, szLogMsg);
	};

	ChatServer::WriteToLog(netConfig_);

	WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::Init() - Start");

	if (auto ret = UVC2NetLibz::TCPServer::Init(netConfig_); ret != 0)
	{
		return ret;
	}

	mNetConfig = netConfig_;

	WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::Init() - Success");
	return 0;
}

void ChatServer::StartAsync()
{
	WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::StartAsync()");

	auto sendPacketFunc = [&](uint32_t clientIndex_, uint16_t packetSize, char* pSendPacket)
	{
		SendMsg(clientIndex_, packetSize, pSendPacket);
	};

	m_pPacketManager = std::make_unique<PacketManager>();
	m_pPacketManager->SendPacketFunc = sendPacketFunc;
	m_pPacketManager->Init(mNetConfig.MaxConnectionCount);
	m_pPacketManager->Run();

	mRunThread = std::thread([&]() { Start(); });
}

void ChatServer::End()
{
	WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::End() - Start ~");

	m_pPacketManager->End();
	
	UVC2NetLibz::TCPServer::End();

	if (mRunThread.joinable())
	{
		mRunThread.join();
	}

	WriteLog((int)UVC2NetLibz::LogLevel::info, "ChatServer::End() - Finish");
}


void ChatServer::OnConnect(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) 
{
	printf("[OnConnect] - SessionIndex(%d), UniqueId(%I64u)\n", sessionIndex_, sessionUniqueId_);

	PacketInfo packet{ sessionIndex_, (UINT16)PACKET_ID::SYS_USER_CONNECT, 0 };
	m_pPacketManager->PushSystemPacket(packet);
}

void ChatServer::OnClose(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) 
{
	printf("[OnConnect] - SessionIndex(%d), UniqueId(%I64u)\n", sessionIndex_, sessionUniqueId_);

	PacketInfo packet{ sessionIndex_, (UINT16)PACKET_ID::SYS_USER_DISCONNECT, 0 };
	m_pPacketManager->PushSystemPacket(packet);
}

void ChatServer::OnReceive(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_,
	const uint16_t dataSize_, char* pData)
{
	printf("[OnReceive] - SessionIndex(%d), UniqueId(%I64u), DataSzie(%d)\n", sessionIndex_, sessionUniqueId_, dataSize_);

	m_pPacketManager->ReceivePacketData(sessionIndex_, dataSize_, pData);
}


void ChatServer::WriteToLog(UVC2NetLibz::NetConfig netConfig)
{
	char logmsg[256] = { 0, };

	sprintf_s(logmsg, "NetConfig - PortNumber: %d", netConfig.PortNumber);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);
		
	sprintf_s(logmsg, "NetConfig - MaxConnectionCount: %d", netConfig.MaxConnectionCount);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - netConfig.Conn.MaxRecvPacketSize: %d", netConfig.Conn.MaxRecvPacketSize);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - netConfig.Conn.MaxTry1RecvBufferSize: %d", netConfig.Conn.MaxTry1RecvBufferSize);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - Conn.MAXRecvCountPer1Sec: %d", netConfig.Conn.MAXRecvCountPer1Sec);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - Conn.MaxRecvBufferSize: %d", netConfig.Conn.MaxRecvBufferSize);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - Conn.MaxSendPacketSize: %d", netConfig.Conn.MaxSendPacketSize);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - Conn.MAXSendCountPer1Sec: %d", netConfig.Conn.MAXSendCountPer1Sec);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);

	sprintf_s(logmsg, "NetConfig - Conn.MaxSendBufferSize: %d", netConfig.Conn.MaxSendBufferSize);
	WriteLog((int)UVC2NetLibz::LogLevel::info, logmsg);
}