#include "ServerManager.h"

const bool CServerManager::SendSC_Game_Session_Info(CSessionData* psession, bool bboardcast)
{
	SC_PACKET_GAME_SESSION_INFO packet;
	packet.size = sizeof(SC_PACKET_GAME_SESSION_INFO);
	packet.type = e_SC_PACKET_TYPE::e_Game_Session_Info;
	CGameSession* pgameSession = nullptr;

	for (int i = 0; i < GAMESESSION_NUM; ++i)
	{
		pgameSession = m_SessionPool.GetGameSessionByNum(i);

		packet.roomnum = i;
		packet.isstart = pgameSession->IsRun();
		packet.currentnum = pgameSession->GetPlayerNum();

		if (bboardcast == true) {
			m_SessionPool.BroadcastSend(&packet);
		}
		else {
			psession->Send(&packet);
		}
	}

	return true;
}

const bool CServerManager::SendSC_Game_Info(CGameSession* pgameSession, CSessionData* psession, bool isbroadcast)
{
	SC_PACKET_GAME_INFO packet;
	packet.size = sizeof(SC_PACKET_GAME_INFO);
	packet.type = e_SC_PACKET_TYPE::e_Game_Info;
	//CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(psession->m_wGameSessionNum);
	GameInfo* pgameSessionInfo = &(pgameSession->m_GameInfo);

	packet.mbupoccupying = pgameSessionInfo->m_bIsMBUpOccupying;
	packet.mbdownoccupying = pgameSessionInfo->m_bIsMBDownOccupying;

	packet.mbuptime = pgameSessionInfo->m_fMBUpTime;
	packet.mbdowntime = pgameSessionInfo->m_fMBDownTime;

	packet.mbupoccupied = pgameSessionInfo->m_bIsMBUpOccupied;
	packet.mbdownoccupied = pgameSessionInfo->m_bIsMBDownOccupied;

	packet.upteampercent = (char)pgameSessionInfo->m_fUpTeamPercent;
	packet.downteampercent = (char)pgameSessionInfo->m_fDownTeamPercent;

	packet.isengaing = pgameSessionInfo->m_bIsEngaging;

	packet.downwincount = pgameSessionInfo->m_nDownWinCount;
	packet.upwincount = pgameSessionInfo->m_nUpWinCount;

	if (isbroadcast == true) {
		if (pgameSession->BroadcastSendInGame(&packet) == false)
			return false;
	}
	else {
		if (psession->Send(&packet) == false)
			return false;
	}

	return true;
}

const bool CServerManager::SendSC_Remove_Player(CSessionData* psession)
{
	SC_PACKET_REMOVE_PLAYER packet;
	packet.size = sizeof(SC_PACKET_REMOVE_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Remove_Player;
	packet.id = (psession->GetID());

	/*if (m_SessionPool.BroadcastSend(&packet) == false)
		return false;*/

	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Put_Player(CSessionData* psession)
{
	SC_PACKET_PUT_PLAYER packet;
	packet.size = sizeof(SC_PACKET_PUT_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Put_Player;

	CSessionData* pfindsession = nullptr;
	for (auto& iter : m_SessionPool.m_mapPlayers) {
		pfindsession = iter.second;
		packet.id = (pfindsession->GetID());

		packet.x = pfindsession->m_GamePlayerInfo.m_fX;
		packet.y = pfindsession->m_GamePlayerInfo.m_fY;
		packet.z = pfindsession->m_GamePlayerInfo.m_fZ;

		//packet.islive

		packet.weaponnum = pfindsession->m_GamePlayerInfo.m_cWeaponNum;
		//packet.isdown = pfindsession->m_GamePlayerInfo.m_bIsDown;
		//packet.islive = pfindsession->m_GamePlayerInfo.m_bIsLive;

		psession->Send(&packet);
	}

	return true;
}

const bool CServerManager::SendSC_Die_Player(WORD wid, WORD wgameSessionNum)
{
	SC_PACKET_DIE_PLAYER packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.id = (wid);
	packet.size = sizeof(SC_PACKET_DIE_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Die_Player;

	if (m_SessionPool.GetGameSessionByNum(wgameSessionNum)->BroadcastSendInGame(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Respawn_Player(WORD wid, WORD wgameSessionNum)
{
	SC_PACKET_RESPAWN_PLAYER packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.id = (wid);
	packet.size = sizeof(SC_PACKET_RESPAWN_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Respawn_Player;

	if (m_SessionPool.GetGameSessionByNum(wgameSessionNum)->BroadcastSendInGame(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Put_Object(GameObjectInfo* pobjectInfo, WORD wgameSessionNum)
{
	SC_PACKET_PUT_OBJECT packet;
	int size = sizeof(SC_PACKET_PUT_OBJECT);
	ZeroMemory(&packet, size);

	packet.etype = pobjectInfo->m_eType;
	packet.id = (pobjectInfo->m_wID);
	packet.whoid = (pobjectInfo->m_wWhoID);
	packet.isdown = pobjectInfo->m_bIsDown;
	packet.size = size;
	packet.type = e_SC_PACKET_TYPE::e_Put_Object;

	packet.x = pobjectInfo->m_fX;
	packet.y = pobjectInfo->m_fY;
	packet.z = pobjectInfo->m_fZ;

	packet.vx = pobjectInfo->m_fVX;
	packet.vy = pobjectInfo->m_fVY;
	packet.vz = pobjectInfo->m_fVZ;

	packet.yaw = pobjectInfo->m_fYaw;
	packet.pitch = pobjectInfo->m_fPitch;
	packet.roll = pobjectInfo->m_fRoll;

	if (m_SessionPool.BroadcastSendInGame(wgameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Put_Object_To(GameObjectInfo* pobjectInfo, CSessionData* psession)
{
	SC_PACKET_PUT_OBJECT packet;
	int size = sizeof(SC_PACKET_PUT_OBJECT);
	ZeroMemory(&packet, size);

	packet.etype = pobjectInfo->m_eType;
	packet.id = (pobjectInfo->m_wID);
	packet.whoid = (pobjectInfo->m_wWhoID);
	packet.isdown = pobjectInfo->m_bIsDown;
	packet.size = size;
	packet.type = e_SC_PACKET_TYPE::e_Put_Object;

	packet.x = pobjectInfo->m_fX;
	packet.y = pobjectInfo->m_fY;
	packet.z = pobjectInfo->m_fZ;

	packet.vx = pobjectInfo->m_fVX;
	packet.vy = pobjectInfo->m_fVY;
	packet.vz = pobjectInfo->m_fVZ;

	packet.yaw = pobjectInfo->m_fYaw;
	packet.pitch = pobjectInfo->m_fPitch;
	packet.roll = pobjectInfo->m_fRoll;

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Update(CSessionData* psession)
{
	SC_PACKET_UPDATE_PLAYER packet;
	packet.size = sizeof(SC_PACKET_UPDATE_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Update_Player;

	packet.id = (psession->GetID());

	GamePlayerInfo* pplayerInfo = &psession->m_GamePlayerInfo;

	packet.life = pplayerInfo->m_fLife;
	packet.x = pplayerInfo->m_fX;
	packet.y = pplayerInfo->m_fY;
	packet.z = pplayerInfo->m_fZ;
	packet.vx = pplayerInfo->m_fVX;
	packet.vy = pplayerInfo->m_fVY;
	packet.vz = pplayerInfo->m_fVZ;
	packet.yaw = pplayerInfo->m_fYaw;
	packet.pitch = pplayerInfo->m_fPitch;
	packet.roll = pplayerInfo->m_fRoll;

	packet.isup = pplayerInfo->m_bIsUp;
	packet.isfly = pplayerInfo->m_bIsFly;
	packet.isshoot = pplayerInfo->m_bIsShoot;
	packet.isground = pplayerInfo->m_bIsGround;
	packet.isrun = pplayerInfo->m_bIsRun;
	packet.isfalling = pplayerInfo->m_bIsFalling;
	packet.ischangingweapon = pplayerInfo->m_bIsChangingWeapon;
	packet.speed = pplayerInfo->m_fSpeed;
	packet.direction = pplayerInfo->m_fDirection;
	packet.rotatoryaw = pplayerInfo->m_fRotatorYaw;
	packet.rotatorpitch = pplayerInfo->m_fRotatorPitch;
	packet.rotatorroll = pplayerInfo->m_fRotatorRoll;

	//packet.weaponnum = pplayerInfo->m_cWeaponNum;

	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Update_Life(CSessionData* psession)
{
	SC_PACKET_UPDATE_PLAYER_LIFE packet;
	packet.size = sizeof(SC_PACKET_UPDATE_PLAYER_LIFE);
	packet.type = e_SC_PACKET_TYPE::e_Update_Player_Life;
	packet.id = (psession->GetID());
	packet.life = psession->m_GamePlayerInfo.m_fLife;

	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Character_Info(CSessionData* psession)
{
	SC_PACKET_CHARACTER_INFO packet;
	packet.size = sizeof(SC_PACKET_CHARACTER_INFO);
	packet.type = e_SC_PACKET_TYPE::e_Character_Info;
	packet.gameamount = psession->m_CharacterInfo.m_nGameAmount;
	packet.gamevictory = psession->m_CharacterInfo.m_nGameVictory;
	packet.gamedefeat = psession->m_CharacterInfo.m_nGameDefeat;

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Swaped_Weapon(CSessionData* psession)
{
	SC_PACKET_SWAPED_WEAPON packet;
	packet.size = sizeof(SC_PACKET_SWAPED_WEAPON);
	packet.type = e_SC_PACKET_TYPE::e_Swaped_Weapon;
	packet.id = psession->GetID();
	packet.weaponnum = psession->m_GamePlayerInfo.m_cWeaponNum;

	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Damaged(CSessionData* psession)
{
	SC_PACKET_DAMAGED packet;
	packet.size = sizeof(SC_PACKET_DAMAGED);
	packet.type = e_SC_PACKET_TYPE::e_Damaged;
	packet.life = psession->m_GamePlayerInfo.m_fLife;

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Healed(CSessionData* psession)
{
	SC_PACKET_HEALED packet;
	packet.size = sizeof(SC_PACKET_HEALED);
	packet.type = e_SC_PACKET_TYPE::e_Healed;
	packet.life = psession->m_GamePlayerInfo.m_fLife;

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Ammo_Update(CSessionData* psession)
{
	SC_PACKET_AMMO_UPDATE packet;
	packet.size = sizeof(SC_PACKET_AMMO_UPDATE);
	packet.type = e_SC_PACKET_TYPE::e_Ammo_Update;
	switch (psession->m_GamePlayerInfo.m_cWeaponNum)
	{
	case WEAPON_MACHINEGUN:
	{
		packet.max = MAX_MACHINEGUN_AMMO;
		packet.now = psession->m_GamePlayerInfo.m_cMachinegunAmmo;
	} break;
	case WEAPON_ROCKETGUN:
	{
		packet.max = MAX_ROCKETGUN_AMMO;
		packet.now = psession->m_GamePlayerInfo.m_cRocketgunAmmo;
	} break;
	}

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_InGame_Log(TCHAR* from, INT logtype, TCHAR* target, WORD wgameSessionNum)
{
	if (from == nullptr)
		return false;

	SC_PACKET_INGAME_LOG packet;
	packet.size = sizeof(SC_PACKET_INGAME_LOG);
	packet.type = e_SC_PACKET_TYPE::e_InGame_Log;
	
	lstrcpy(packet.from, from);
	packet.logtype = logtype;
	if(target != nullptr)
		lstrcpy(packet.target, target);

	if (m_SessionPool.BroadcastSendInGame(wgameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::SendSC_Smog_OnOff(CSessionData* psession, BYTE x, BYTE y, BYTE z, bool isbroadcast)
{
	SC_PACKET_SMOG_ONOFF packet;
	packet.size = sizeof(SC_PACKET_SMOG_ONOFF);
	packet.type = e_SC_PACKET_TYPE::e_Smog_OnOff;
	CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(psession->m_wGameSessionNum);
	GameInfo* pgameSessionInfo = &(pgameSession->m_GameInfo);

	bool issmog = pgameSessionInfo->m_IsSmog[x][y][z];
	packet.xIndex = x;
	packet.yIndex = y;
	packet.zIndex = z;
	packet.issmog = issmog;

	if (isbroadcast == true) {
		if (pgameSession->BroadcastSendInGame(&packet) == false)
			return false;
	}
	else {
		if (psession->Send(&packet) == false)
			return false;
	}

	return true;
}
