#include <tuple>

#include <cstring>
#include <iostream>
#include "PacketProcess.h"
#include "PacketDef.h"
#include "User.h"
#include "UserManager.h"
#include "ErrorCode.h"
#include "Room.h"

namespace ChatServerLib
{
	
	NServerNetLib::ERROR_CODE PacketProcess::RoomEnter(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktRoomEnterReq*)packetInfo.pRefData;
		NCommon::PktRoomEnterRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}
		Room* pRoom = nullptr;

		pRoom = m_pRefRoomMgr->FindRoom(reqPkt->RoomIndex);
		if (pRoom == nullptr) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		auto enterRet = pRoom->EnterUser(pUser);
		if (enterRet != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(enterRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return enterRet;
		}

		pUser->EnterRoom(pRoom->GetIndex());

		pRoom->NotifyEnterUserInfo(pUser->GetIndex(), pUser->GetID().c_str());

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
		return NServerNetLib::ERROR_CODE::NONE;
	}

	NServerNetLib::ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		NCommon::PktRoomLeaveRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();

		if (pUser->IsCurDomainInRoom() == false) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN;
		}
	
		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (pRoom == nullptr) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		auto leaveRet = pRoom->LeaveUser(userIndex);
		if (leaveRet != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(leaveRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return leaveRet;
		}

		pUser->LeaveRoom();

		pRoom->NotifyLeaveUserInfo(pUser->GetID().c_str());

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
		return NServerNetLib::ERROR_CODE::NONE;
	}


	NServerNetLib::ERROR_CODE PacketProcess::RoomChat(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktRoomChatReq*)packetInfo.pRefData;
		NCommon::PktRoomChatRes resPkt;
		
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInRoom() == false) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN;
		}

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		if (pRoom == nullptr) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return NServerNetLib::ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		pRoom->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);

		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
		return NServerNetLib::ERROR_CODE::NONE;
	}

}