#include "PacketProcess.h"
#include "PacketDef.h"

namespace OmokServerLib
{
	
	ERROR_CODE PacketProcess::RoomEnter(PacketInfo packetInfo)
	{
		auto reqPkt = (OmokServerLib::PktRoomEnterReq*)packetInfo.pRefData;
		OmokServerLib::PktRoomEnterRes resPkt;

		//TODO 최흥배
		// 다른 패킷 처리 함수에서 비슷한 코드가 있습니다. 중복을 제거해주세요 
		//-> 26줄 FindRoom 에서 reqPkt->RoomIndex를 통해 방을 찾으므로 다른 함수들과 달라 묶기에 무리가 있는 것 같습니다.
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;
		
		if (errorCode != ERROR_CODE::NONE) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_ENTER_RES, errorCode);
			return errorCode;
		}
		
		auto pRoom = m_pRefRoomMgr->FindRoom(reqPkt->RoomIndex);

		if (pRoom.has_value() == false) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_ENTER_RES, ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);
			return ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX;
		}
		//
		auto enterRet = pRoom.value()->EnterUser(pUser);

		if (enterRet != ERROR_CODE::NONE) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_ENTER_RES, enterRet);
			return enterRet;
		}

		pUser->EnterRoom(pRoom.value()->GetIndex());

		pRoom.value()->NotifyEnterUserInfo(packetInfo.SessionIndex, pUser->GetID().c_str());

		SendPacketFunc(packetInfo.SessionIndex, (short)OmokServerLib::PACKET_ID::ROOM_ENTER_RES, sizeof(resPkt), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}

	ERROR_CODE PacketProcess::RoomLeave(PacketInfo packetInfo)
	{
		OmokServerLib::PktRoomLeaveRes resPkt;

		//TODO 최흥배
		// 다른 패킷 처리 함수에서 비슷한 코드가 있습니다. 중복을 제거해주세요
		//-> 해결

		auto findResult = FindUserAndRoom(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_LEAVE_RES, ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);

		if (findResult.has_value() == false)
		{
			return ERROR_CODE::USER_ROOM_FIND_ERROR;
		}

		auto pUser = findResult.value().first;
		auto pRoom = findResult.value().second;	

		if (pUser->IsCurDomainInLogIn() == true)
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_LEAVE_RES, ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN);
			return ERROR_CODE::ROOM_LEAVE_INVALID_DOMAIN;
		}

		auto userIndex = pUser->GetIndex();

		auto leaveRet = pRoom->LeaveUser(userIndex, packetInfo.SessionIndex, pUser->GetID().c_str());
		if (leaveRet != ERROR_CODE::NONE) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_LEAVE_RES, leaveRet);
			return leaveRet;
		}

		pUser->LeaveRoom();

		SendPacketFunc(packetInfo.SessionIndex, (short)OmokServerLib::PACKET_ID::ROOM_LEAVE_RES, sizeof(resPkt), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}


	ERROR_CODE PacketProcess::RoomChat(PacketInfo packetInfo)
	{
		auto reqPkt = (OmokServerLib::PktRoomChatReq*)packetInfo.pRefData;
		OmokServerLib::PktRoomChatRes resPkt;
		
		//TODO 최흥배
		// 다른 패킷 처리 함수에서 비슷한 코드가 있습니다. 중복을 제거해주세요
		//->해결

		auto findResult = FindUserAndRoom(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_CHAT_RES, ERROR_CODE::ROOM_ENTER_INVALID_ROOM_INDEX);

		if (findResult.has_value() == false)
		{
			return ERROR_CODE::USER_ROOM_FIND_ERROR;
		}

		auto pUser = findResult.value().first;
		auto pRoom = findResult.value().second;

		if (pUser->IsCurDomainInLogIn() == true) 
		{
			SendPacketSetError(packetInfo.SessionIndex, OmokServerLib::PACKET_ID::ROOM_CHAT_RES, ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN);
			return ERROR_CODE::ROOM_CHAT_INVALID_DOMAIN;
		}
		//

	    pRoom->NotifyChat(pUser->GetSessioIndex(), pUser->GetID().c_str(), reqPkt->Msg);	
		SendPacketFunc(packetInfo.SessionIndex, (short)OmokServerLib::PACKET_ID::ROOM_CHAT_RES, sizeof(resPkt), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}

}