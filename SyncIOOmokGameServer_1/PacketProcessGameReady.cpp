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

		pUser->SetReady();
		auto pRoom = m_pRefRoomMgr->FindRoom(pUser->GetRoomIndex());

		auto userIndex = pUser->GetIndex();
		for (auto iter : pRoom->m_UserList)
		{
			if (iter->GetIndex() != userIndex)
			{
				if (iter->IsCurDomainInReady()) 
				{
					m_pRefUserMgr->GetUser(iter->GetSessioIndex()).second->SetGame();
					pUser->SetGame();

					//TODO :  검은돌 랜덤 선정
					pRoom->m_BlackStoneUserIndex = packetInfo.SessionIndex;
					pRoom->m_TurnIndex = packetInfo.SessionIndex;
					pRoom->OmokGame->init();
					pRoom->OmokGame->initType();

					strncpy_s(resPkt.UserID, (NCommon::MAX_USER_ID_SIZE + 1),m_pRefUserMgr->GetUser(pRoom->m_BlackStoneUserIndex).second->GetID().c_str(), NCommon::MAX_USER_ID_SIZE);
					m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
					pRoom->NotifyGameStart(packetInfo.SessionIndex, m_pRefUserMgr->GetUser(pRoom->m_BlackStoneUserIndex).second->GetID().c_str());
				}
				else
				{
					resPkt.SetError(ERROR_CODE::NOT_READY_EXIST);
					m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::GAME_START_RES, sizeof(resPkt), (char*)&resPkt);
				}
			}
 		}

		return ERROR_CODE::NONE;
	}
}
