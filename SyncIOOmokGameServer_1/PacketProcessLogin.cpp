#include <tuple>
#include <iostream>
#include "User.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	
	NServerNetLib::ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktLogInReq*)packetInfo.pRefData;
		NCommon::PktLogInRes resPkt;

		auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);

		if (addRet != NServerNetLib::ERROR_CODE::NONE) 
		{
			resPkt.SetError(addRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return addRet;
		}
		
		resPkt.ErrorCode = (short)addRet;
		auto result = m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);

		return NServerNetLib::ERROR_CODE::NONE;
	}
	
}