#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	ERROR_CODE PacketProcess::GameReady(PacketInfo packetInfo)
	{
		NCommon::PktGameReadyRes resPkt;

		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		auto errorCode = userInfo.first;
		auto pUser = userInfo.second;

		if (errorCode != ERROR_CODE::NONE)
		{
			resPkt.SetError(errorCode);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}
		auto checkReadyResult = m_pRefUserMgr->CheckReady(pUser);

		if (checkReadyResult != ERROR_CODE::NONE)
		{
			resPkt.SetError(checkReadyResult);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			return errorCode;
		}

		pUser->SetReady();

		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());
		auto pOpponentUser = m_pRefUserMgr->GetUser(std::abs(pUser->GetSessioIndex() - 1)).second;

		if (pOpponentUser->IsCurDomainInReady() == true)
		{
			pOpponentUser->SetGame();
			pUser->SetGame();

			//TODO :  검은돌 랜덤 선정
			pRoom->m_OmokGame->m_BlackStoneUserIndex = packetInfo.SessionIndex;
			pRoom->m_OmokGame->m_TurnIndex = packetInfo.SessionIndex;
			pRoom->m_OmokGame->init();
			pRoom->m_OmokGame->initType();

			strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1),pUser->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
			pRoom->NotifyGameStart(packetInfo.SessionIndex, pUser->GetID().c_str());
		}
		else
		{
			resPkt.SetError(ERROR_CODE::NOT_READY_EXIST);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
		}
	

		return ERROR_CODE::NONE;
	}
}
