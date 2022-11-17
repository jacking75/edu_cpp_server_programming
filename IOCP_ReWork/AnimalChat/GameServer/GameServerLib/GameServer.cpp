#include <iostream>
#include <string>

#include "../CppServerNetLib/IOCPServerNet.h"

#include "GameServer.h"

#include "../Common/Packet.h"
#include "User.h"

ChatServer::ChatServer()
{	
}

ChatServer::~ChatServer()
{	
}

void ChatServer::Run(void)
{
	NetLib::NetConfig netConfig;
	netConfig.m_PortNumber = 32452;
	netConfig.m_WorkThreadCount = 4;
	netConfig.m_MaxRecvOverlappedBufferSize = 1024;
	netConfig.m_MaxSendOverlappedBufferSize = 1024;
	netConfig.m_MaxRecvConnectionBufferCount = 4019;
	netConfig.m_MaxSendConnectionBufferCount = 4019;
	netConfig.m_MaxPacketSize = 1024;
	netConfig.m_MaxConnectionCount = 1024;
	netConfig.m_MaxMessagePoolCount = 1024;
	netConfig.m_ExtraMessagePoolCount = 128;
	netConfig.m_PerformancePacketMillisecondsTime = 0;
	netConfig.m_PostMessagesThreadsCount = 1;

	m_IOCPServerNet = new NetLib::IOCPServerNet;

	NetLib::E_NET_RESULT result = m_IOCPServerNet->StartServer(netConfig);
	if (result == NetLib::E_NET_RESULT::Success)
	{
		m_MaxPacketSize = m_IOCPServerNet->GetMaxPacketSize();
		m_MaxConnectionCount = m_IOCPServerNet->GetMaxConnectionCount();
		m_PostMessagesThreadsCount = m_IOCPServerNet->GetPostMessagesThreadsCount();
				
		if (Init())
		{
			m_IsSuccessStartServer = true;

			//스레드 생성
			for (int i = 0; i < m_PostMessagesThreadsCount; ++i)
			{
				m_PostMessagesThreads.push_back(std::make_unique<std::thread>(&ChatServer::PostMessagesThreadFunction, this));
			}
		}
	}

	
	if (m_IsSuccessStartServer == false)
	{
		std::cout << "===서버 동작 실패===" << std::endl;
	}
	
	std::cout << "아무 키를 누르면 종료" << std::endl;

	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);
		break;
	}

	if (m_IsSuccessStartServer)
	{
		m_IOCPServerNet->EndServer();

		m_IsSuccessStartServer = false;

		for (int i = 0; i < m_PostMessagesThreads.size(); ++i)
		{
			m_PostMessagesThreads[i].get()->join();
		}
	}
}

void ChatServer::PostMessagesThreadFunction(void)
{
	if (m_MaxPacketSize <= 0)
	{
		return;
	}

	char* pBuf = new char[m_MaxPacketSize];
	ZeroMemory(pBuf, sizeof(char) * m_MaxPacketSize);

	while (true)
	{
		if (!m_IsSuccessStartServer)
		{
			return;
		}

		ZeroMemory(pBuf, sizeof(char) * m_MaxPacketSize);

		INT8 operationType = 0;
		INT32 connectionIndex = 0;
		INT16 copySize = 0;

		if (!m_IOCPServerNet->ProcessNetworkMessage(operationType, connectionIndex, pBuf, copySize))
		{
			return;
		}

		switch (operationType)
		{
		case NetLib::OP_CONNECTION:
			ConnectConnection(connectionIndex);
			break;
		case NetLib::OP_CLOSE:
			DisconnectConnection(connectionIndex);
			break;
		case NetLib::OP_RECV_PACKET:
			CommandRecvPacket(connectionIndex, pBuf, copySize);
			break;
		}
	}
}

bool ChatServer::Init(void)
{
	if (m_MaxConnectionCount < 0 || m_MaxRoomCount < 0 || m_PostMessagesThreadsCount < 0)
	{
		return false;
	}

	//패킷 처리 함수 등록
	RegisterProcessPacketFunc();

	//유저 정보 생성
	for (int i = 0; i < m_MaxConnectionCount; ++i)
	{
		m_UsersInfo.push_back(std::make_unique<User>());
	}

	//방 생성
	for (int i = 0; i < m_MaxRoomCount; ++i)
	{
		std::unordered_set<int/*connectionIndex*/> Users;
		m_Rooms.push_back(Users);
	}

	return true;
}

void ChatServer::ConnectConnection(INT32 connectionIndex)
{
	if (m_IsEchoTestMode)
	{
		return;
	}

	auto iter = m_Users.find(connectionIndex);
	if (iter == m_Users.end())
	{
		(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_CONNECT;
		m_Users.insert(connectionIndex);
	}
}

void ChatServer::DisconnectConnection(INT32 connectionIndex)
{
	if (m_IsEchoTestMode)
	{
		return;
	}

	auto currentState = (m_UsersInfo[connectionIndex].get())->UserState;
	switch (currentState)
	{
	case E_USER_STATE_ROOM:
	{
		auto roomNumber = (m_UsersInfo[connectionIndex].get())->RoomNumber;
		auto iter_RoomNumber = m_Rooms[roomNumber].find(connectionIndex);
		if (iter_RoomNumber != m_Rooms[roomNumber].end())
		{
			iter_RoomNumber = m_Rooms[roomNumber].erase(iter_RoomNumber);
		}
	}
	case E_USER_STATE_LOGIN:
	case E_USER_STATE_CONNECT:
	{
		auto iter_Connection = m_Users.find(connectionIndex);
		if (iter_Connection != m_Users.end())
		{
			iter_Connection = m_Users.erase(iter_Connection);
		}

		auto iter_ID = m_UserIDs.find((m_UsersInfo[connectionIndex].get())->UserID);
		if (iter_ID != m_UserIDs.end())
		{
			iter_ID = m_UserIDs.erase(iter_ID);
		}

		(m_UsersInfo[connectionIndex].get())->Clear();
		break;
	}
	default: return;
	}
}

void ChatServer::CommandRecvPacket(INT32 connectionIndex, char* pBuf, INT16 copySize)
{
	auto pHeader = reinterpret_cast<NCommon::PktHeader*>(pBuf);

	auto iter = m_PacketProcesser.find(pHeader->Id);
	if (iter != m_PacketProcesser.end())
	{
		char* pBody = nullptr;
		INT16 bodySize = 0;

		if (copySize > NCommon::PacketHeaderSize)
		{
			bodySize = copySize - NCommon::PacketHeaderSize;
			pBody = (pBuf + NCommon::PacketHeaderSize);
		}
		
		(this->*(iter->second))(connectionIndex, pBody, bodySize);
	}
}

void ChatServer::RegisterProcessPacketFunc(void)
{
	m_PacketProcesser[(UINT16)NCommon::PACKET_ID::LOGIN_IN_REQ] = &ChatServer::ProcessPacketLogin;
	m_PacketProcesser[(UINT16)NCommon::PACKET_ID::SEL_CHARECTER_REQ] = &ChatServer::ProcessPacketSelectCharacter;
}

void ChatServer::ProcessPacketLogin(INT32 connectionIndex, char* pBodyData, INT16 bodySize)
{
	if (bodySize < 1 || bodySize != sizeof(NCommon::PktLogInReq))
	{
		return;
	}

	auto pReqPacket = reinterpret_cast<NCommon::PktLogInReq*>(pBodyData);
	
	NCommon::PktLogInRes resPacket;
	resPacket.TotalSize = sizeof(NCommon::PktLogInRes);
	resPacket.Id = (UINT16)NCommon::PACKET_ID::LOGIN_IN_RES;
	resPacket.ErrorCode = (INT16)NCommon::ERROR_CODE::NONE;

	auto currentUserState = (m_UsersInfo[connectionIndex].get())->UserState;
	if (currentUserState == E_USER_STATE_NONE)
	{
		resPacket.ErrorCode = (INT16)NCommon::ERROR_CODE::LOGIN_NOT_CONNECTED_STATE;
	}
	else if (currentUserState == E_USER_STATE_CONNECT)
	{
		std::string reqPacketID(pReqPacket->szID);
		auto iter = m_UserIDs.find(reqPacketID);
		if (iter == m_UserIDs.end())
		{
			m_UserIDs.insert(reqPacketID);
			(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_LOGIN;
			(m_UsersInfo[connectionIndex].get())->UserID = reqPacketID;
		}
		else
		{
			resPacket.ErrorCode = (INT16)NCommon::ERROR_CODE::LOGIN_SAME_ID;
		}
	}
	else
	{
		resPacket.ErrorCode = (INT16)NCommon::ERROR_CODE::LOGIN_ALLREADY_LOGIN;
	}
	

	m_IOCPServerNet->SendPacket(connectionIndex, &resPacket, resPacket.TotalSize);
}

void ChatServer::ProcessPacketSelectCharacter(INT32 connectionIndex, char* pBodyData, INT16 bodySize)
{
	if (bodySize < 1 || bodySize != sizeof(NCommon::PktSelCharacterReq))
	{
		return;
	}

	auto pReqPacket = reinterpret_cast<NCommon::PktSelCharacterReq*>(pBodyData);

	NCommon::PktSelCharacterRes resPacket;
	resPacket.TotalSize = sizeof(NCommon::PktSelCharacterRes);
	resPacket.Id = (UINT16)NCommon::PACKET_ID::SEL_CHARECTER_RES;
	resPacket.ErrorCode = (INT16)NCommon::ERROR_CODE::NONE;
	resPacket.CharCode = pReqPacket->CharCode;

	//TODO
	// 로그인 상태인 경우만 ok
	// 선택한 캐릭터 번호를 저장한다


	m_IOCPServerNet->SendPacket(connectionIndex, &resPacket, resPacket.TotalSize);
}

//void ChatServer::ProcessPacketRoomNew(INT32 connectionIndex, char* pBuf, INT16 copySize)
//{
//	if (copySize != sizeof(ROOM_NEW_REQUEST_PACKET))
//	{
//		return;
//	}
//
//	ROOM_NEW_REQUEST_PACKET* pReqPacket = reinterpret_cast<ROOM_NEW_REQUEST_PACKET*>(pBuf);
//	UNREFERENCED_PARAMETER(pReqPacket);
//
//	ROOM_NEW_RESPONSE_PACKET resPacket;
//	resPacket.PacketLength = sizeof(ROOM_NEW_RESPONSE_PACKET);
//	resPacket.PacketId = ROOM_NEW_RESPONSE;
//	resPacket.RoomNumber = -1;
//
//	auto currentUserState = (m_UsersInfo[connectionIndex].get())->UserState;
//	if (currentUserState == E_USER_STATE_NONE)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_NONE;
//	}
//	else if (currentUserState == E_USER_STATE_CONNECT)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOGIN;
//	}
//	else if (currentUserState == E_USER_STATE_LOGIN)
//	{
//		auto roomNumber = -1;
//		for (int i = 0; i < m_MaxRoomCount; ++i)
//		{
//			if (m_Rooms[i].size() == 0)
//			{
//				roomNumber = i;
//				break;
//			}
//		}
//
//		if (roomNumber == -1)
//		{
//			resPacket.Result = E_PACKET_RESULT_FAIL_MAX_MAKE_ROOM;
//		}
//		else
//		{
//			resPacket.Result = E_PACKET_RESULT_SUCCESS;
//			resPacket.RoomNumber = roomNumber;
//
//			m_Rooms[roomNumber].insert(connectionIndex);
//			(m_UsersInfo[connectionIndex].get())->RoomNumber = roomNumber;
//			(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_ROOM;
//		}
//	}
//	else if (currentUserState == E_USER_STATE_ROOM)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_ROOM;
//	}
//
//	m_IOCPServer->SendPacket(connectionIndex, &resPacket, resPacket.PacketLength);
//}

//void ChatServer::ProcessPacketRoomEnter(INT32 connectionIndex, char* pBuf, INT16 copySize)
//{
//	if (copySize != sizeof(ROOM_ENTER_REQUEST_PACKET))
//	{
//		return;
//	}
//
//	ROOM_ENTER_REQUEST_PACKET* pReqPacket = reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pBuf);
//
//	ROOM_ENTER_RESPONSE_PACKET resPacket;
//	resPacket.PacketLength = sizeof(ROOM_ENTER_RESPONSE_PACKET);
//	resPacket.PacketId = ROOM_ENTER_RESPONSE;
//
//	auto currentUserState = (m_UsersInfo[connectionIndex].get())->UserState;
//	if (currentUserState == E_USER_STATE_NONE)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_NONE;
//	}
//	else if (currentUserState == E_USER_STATE_CONNECT)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOGIN;
//	}
//	else if (currentUserState == E_USER_STATE_LOGIN)
//	{
//		int roomNumber = pReqPacket->RoomNumber;
//		if (roomNumber == -1 || roomNumber >= m_MaxRoomCount)
//		{
//			resPacket.Result = E_PACKET_RESULT_FAIL_WRONG_ROOM_NUMBER;
//		}
//		else
//		{
//			if (m_Rooms[pReqPacket->RoomNumber].size() > 0)
//			{
//				resPacket.Result = E_PACKET_RESULT_SUCCESS;
//
//				m_Rooms[pReqPacket->RoomNumber].insert(connectionIndex);
//				(m_UsersInfo[connectionIndex].get())->RoomNumber = pReqPacket->RoomNumber;
//				(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_ROOM;
//			}
//			else
//			{
//				resPacket.Result = E_PACKET_RESULT_FAIL_NOT_EXIST_ROOM;
//			}
//		}
//	}
//	else if (currentUserState == E_USER_STATE_ROOM)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_ROOM;
//	}
//
//	m_IOCPServer->SendPacket(connectionIndex, &resPacket, resPacket.PacketLength);
//}
//
//void ChatServer::ProcessPacketRoomLeave(INT32 connectionIndex, char* pBuf, INT16 copySize)
//{
//	if (copySize != sizeof(ROOM_LEAVE_REQUEST_PACKET))
//	{
//		return;
//	}
//
//	ROOM_LEAVE_REQUEST_PACKET* pReqPacket = reinterpret_cast<ROOM_LEAVE_REQUEST_PACKET*>(pBuf);
//	UNREFERENCED_PARAMETER(pReqPacket);
//
//	ROOM_LEAVE_RESPONSE_PACKET resPacket;
//	resPacket.PacketLength = sizeof(ROOM_LEAVE_RESPONSE_PACKET);
//	resPacket.PacketId = ROOM_LEAVE_RESPONSE;
//
//	auto currentUserState = (m_UsersInfo[connectionIndex].get())->UserState;
//	if (currentUserState == E_USER_STATE_NONE)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_NONE;
//	}
//	else if (currentUserState == E_USER_STATE_CONNECT)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOGIN;
//	}
//	else if (currentUserState == E_USER_STATE_LOGIN)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOBBY;
//	}
//	else if (currentUserState == E_USER_STATE_ROOM)
//	{
//		auto roomNumber = (m_UsersInfo[connectionIndex].get())->RoomNumber;
//		auto iter = m_Rooms[roomNumber].find(connectionIndex);
//		if (iter != m_Rooms[roomNumber].end())
//		{
//			resPacket.Result = E_PACKET_RESULT_SUCCESS;
//
//			iter = m_Rooms[roomNumber].erase(iter);
//
//			(m_UsersInfo[connectionIndex].get())->RoomNumber = -1;
//			(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_LOGIN;
//		}
//		else
//		{
//			resPacket.Result = E_PACKET_RESULT_FAIL;
//
//			(m_UsersInfo[connectionIndex].get())->RoomNumber = -1;
//			(m_UsersInfo[connectionIndex].get())->UserState = E_USER_STATE_LOGIN;
//		}
//	}
//
//	m_IOCPServer->SendPacket(connectionIndex, &resPacket, resPacket.PacketLength);
//}

//void ChatServer::ProcessPacketRoomChat(INT32 connectionIndex, char* pBuf, INT16 copySize)
//{
//	if (copySize != sizeof(ROOM_CHAT_REQUEST_PACKET))
//	{
//		return;
//	}
//
//	ROOM_CHAT_REQUEST_PACKET* pReqPacket = reinterpret_cast<ROOM_CHAT_REQUEST_PACKET*>(pBuf);
//
//	ROOM_CHAT_RESPONSE_PACKET resPacket;
//	resPacket.PacketLength = sizeof(ROOM_CHAT_RESPONSE_PACKET);
//	resPacket.PacketId = ROOM_CHAT_RESPONSE;
//
//	auto currentUserState = (m_UsersInfo[connectionIndex].get())->UserState;
//	if (currentUserState == E_USER_STATE_NONE)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_NONE;
//	}
//	else if (currentUserState == E_USER_STATE_CONNECT)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOGIN;
//	}
//	else if (currentUserState == E_USER_STATE_LOGIN)
//	{
//		resPacket.Result = E_PACKET_RESULT_FAIL_IN_LOBBY;
//	}
//	else if (currentUserState == E_USER_STATE_ROOM)
//	{
//		resPacket.Result = E_PACKET_RESULT_SUCCESS;
//
//		for (auto roomConnectionIndex : m_Rooms[(m_UsersInfo[connectionIndex].get())->RoomNumber])
//		{
//			ROOM_CHAT_NOTIFY_PACKET notPacket;
//			notPacket.PacketLength = sizeof(ROOM_CHAT_NOTIFY_PACKET);
//			notPacket.PacketId = ROOM_CHAT_NOTIFY;
//			CopyMemory(notPacket.Message, pReqPacket->Message, sizeof(notPacket.Message));
//			CopyMemory(notPacket.UserID, (m_UsersInfo[connectionIndex].get())->UserID.c_str(), sizeof(notPacket.UserID));
//
//			m_IOCPServer->SendPacket(roomConnectionIndex, &notPacket, notPacket.PacketLength);
//		}
//	}
//
//	m_IOCPServer->SendPacket(connectionIndex, &resPacket, resPacket.PacketLength);
//}