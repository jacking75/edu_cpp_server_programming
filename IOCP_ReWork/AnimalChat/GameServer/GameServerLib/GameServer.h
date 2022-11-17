#pragma once

#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <memory>

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>


namespace NetLib
{
	class IOCPServerNet;
}

struct User;
class ChatServer
{
public:
	ChatServer();
	~ChatServer();

private:
	typedef void(ChatServer::*PROCESS_RECV_PACKET_FUNCTION)(INT32, char*, INT16);
	std::unordered_map<short/*packetID*/, PROCESS_RECV_PACKET_FUNCTION> m_PacketProcesser;

public:
	void Run(void);

private:
	void PostMessagesThreadFunction(void);

	bool Init(void);

	void ConnectConnection(INT32 connectionIndex);
	void DisconnectConnection(INT32 connectionIndex);
	void CommandRecvPacket(INT32 connectionIndex, char* pBuf, INT16 copySize);

	void RegisterProcessPacketFunc(void);
	void ProcessPacketLogin(INT32 connectionIndex, char* pBodyData, INT16 bodySize);
	void ProcessPacketSelectCharacter(INT32 connectionIndex, char* pBodyData, INT16 bodySize);
	/*void ProcessPacketRoomNew(INT32 connectionIndex, char* pBuf, INT16 copySize);
	void ProcessPacketRoomEnter(INT32 connectionIndex, char* pBuf, INT16 copySize);
	void ProcessPacketRoomLeave(INT32 connectionIndex, char* pBuf, INT16 copySize);
	void ProcessPacketRoomChat(INT32 connectionIndex, char* pBuf, INT16 copySize);*/

private:
	NetLib::IOCPServerNet* m_IOCPServerNet = nullptr;

	bool m_IsSuccessStartServer = false;

	bool m_IsEchoTestMode = false;

	int m_PostMessagesThreadsCount = 0;
	std::vector<std::unique_ptr<std::thread>> m_PostMessagesThreads;

	int m_MaxPacketSize = 0;
	int m_MaxConnectionCount = 0;
	std::unordered_set<int/*connectionIndex*/> m_Users;
	std::unordered_set<std::string> m_UserIDs;
	std::vector<std::unique_ptr<User>> m_UsersInfo;

	int m_MaxRoomCount = 100;
	std::vector<std::unordered_set<int/*connectionIndex*/>> m_Rooms;
};