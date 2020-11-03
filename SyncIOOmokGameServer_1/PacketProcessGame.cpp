#include <tuple>
#include <iostream>
#include "User.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	NServerNetLib::ERROR_CODE PacketProcess::GamePut(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktPutStoneReq*)packetInfo.pRefData;
		NCommon::PktPutStoneRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::PUT_STONE_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto room = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		auto result = room->OmokGame->GamePutStone(pUser->GetIndex(), reqPkt->x, reqPkt->y);

	}
}