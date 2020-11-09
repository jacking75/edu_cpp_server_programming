#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	
	ERROR_CODE PacketProcess::RoomEnter(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktRoomEnterReq*)packetInfo.pRefData;
		NCommon::PktRoomEnterRes resPkt;

		auto temp = reqPkt->RoomIndex;
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}
		Room* pRoom = nullptr;

		pRoom = m_pRefRoomMgr->FindRoom(reqPkt->RoomIndex);
		if (pRoom == nullptr) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		auto enterRet = pRoom->EnterUser(pUser);
		if (enterRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(enterRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return enterRet;
		}

		pUser->EnterRoom(pRoom->GetIndex());

		pRoom->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;
	}

	//TODO : 게임중에 나갔을때 처리
	ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		NCommon::PktRoomLeaveRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();

		if (pUser->IsCurDomainInLogIn() == true)
		{
			resPkt.SetError(ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN;
		}
	
		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (pRoom == nullptr) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		auto leaveRet = pRoom->LeaveUser(userIndex);
		if (leaveRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(leaveRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return leaveRet;
		}

		pUser->LeaveRoom();
		pRoom->NotifyLeaveUserInfo(packetInfo.SessionIndex,pUser->GetID().c_str());

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;
	}


	ERROR_CODE PacketProcess::RoomChat(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktRoomChatReq*)packetInfo.pRefData;
		NCommon::PktRoomChatRes resPkt;
		
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInRoom() == false) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN;
		}

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

	    pRoom->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);
		//pRoom->NotifyChat(-1, pUser->GetID().c_str(), reqPkt->Msg);
		
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return ERROR_CODE::NONE;
	}

}