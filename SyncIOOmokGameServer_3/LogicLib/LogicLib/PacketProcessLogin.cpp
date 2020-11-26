#include <tuple>
#include <iostream>
#include "User.h"
#include "UserManager.h"
#include "PacketProcess.h"
#include "PacketDef.h"

namespace ChatServerLib
{
	
	ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
	{
		auto reqPkt = (NCommon::PktLogInReq*)packetInfo.pRefData;
		NCommon::PktLogInRes resPkt;

  		auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);
		auto userInfo = m_pRefUserMgr->GetUser(packetInfo.SessionIndex);
		
		if (addRet != ERROR_CODE::NONE) 
		{
			resPkt.SetError(addRet);
			m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);
			return addRet;
		}

		userInfo.second->SetLogin();
		resPkt.ErrorCode = (short)addRet;
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)NCommon::PACKET_ID::LOGIN_IN_RES, sizeof(NCommon::PktLogInRes), (char*)&resPkt);

		return ERROR_CODE::NONE;
	}
	
}