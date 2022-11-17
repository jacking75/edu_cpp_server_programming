#pragma once

#include "../LogicLib/Lobby.h"

using namespace NLogicLib;

class MockLobby : public Lobby
{
public:
	MockLobby() {}
	

	ERROR_CODE AddUser(User* pUser) { return __super::AddUser(pUser); }

	void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1)
	{
		__super::SendToAllUser(packetId, dataSize, pData, passUserindex);
	}

};