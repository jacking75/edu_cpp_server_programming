#include "PacketProcess.h"
#include "PacketDef.h"


namespace ChatServerLib
{
	NServerNetLib::ERROR_CODE PacketProcess::MatchUser(PacketInfo packetInfo)
	{
		NCommon::PktMatchRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);

		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != NServerNetLib::ERROR_CODE::NONE) {
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		auto userIndex = pUser->GetIndex();

		auto pRoom = m_pRefRoomMgr->FindProperRoom();

		if (pRoom == nullptr) {
			resPkt.SetError(NServerNetLib::ERROR_CODE::MATCHING_FAIL);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		pUser->EnterRoom(pRoom->GetIndex());
		pRoom->NotifyEnterUserInfo(userIndex, pUser->GetID().c_str());

		pUser->SetBlack();
		pRoom->m_BlackStoneUserIndex = userIndex;

		pRoom->OmokGame->init(userIndex);
		
		strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1), pUser->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);
		pRoom->SendToAllUser((short)NCommon::PACKET_ID::MATCH_USER_RES, sizeof(resPkt), (char*)&resPkt);

		return NServerNetLib::ERROR_CODE::NONE;

	}
}