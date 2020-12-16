#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
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
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}
		
		auto pRoom = m_pRefRoomMgr->FindRoom(reqPkt->RoomIndex);

		if (pRoom.has_value() == false) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

		auto enterRet = pRoom.value()->EnterUser(pUser);

		if (enterRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(enterRet);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);
			return enterRet;
		}

		pUser->EnterRoom(pRoom.value()->GetIndex());

		pRoom.value()->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

		SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		NCommon::PktRoomLeaveRes resPkt;

		//TODO 최흥배
		// 이 부분 코드 다른 요청 처리할 때도 비슷한 코드가 있네요. 코드 중복이 많습니다. 함수로 만들어서 중복을 제거해주세요
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE) 
		{
			resPkt.SetError(errorCode);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();

		if (pUser->IsCurDomainInLogIn() == true)
		{
			resPkt.SetError(ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN;
		}
	
		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		if (pRoom.has_value() == false) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}
		//

		auto leaveRet = pRoom.value()->LeaveUser(userIndex, packetInfo.SessionIndex, pUser->GetID().c_str());
		if (leaveRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(leaveRet);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);
			return leaveRet;
		}

		pUser->LeaveRoom();

		SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);

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
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		if (pUser->IsCurDomainInLogIn() == true) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN;
		}

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		if (pRoom.has_value() == false) 
		{
			resPkt.SetError(ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}

	    pRoom.value()->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);	
		SendPacketFunc(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}

}