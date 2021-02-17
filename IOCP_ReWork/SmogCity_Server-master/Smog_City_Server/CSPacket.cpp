#include "ServerManager.h"

const bool CServerManager::RecvCS_Login(CSessionData* psession)
{
	CS_PACKET_LOGIN* ppacket = reinterpret_cast<CS_PACKET_LOGIN*>(psession->m_RecvBuffer);
	TCHAR* id = ppacket->id;			// 아이디 패스워드가 제대로 출력이 안된다. 고치자
	TCHAR* pw = ppacket->pw;

	//_tprintf(_T("id : %s\n"), id);
	std::wcout << _T("ID : ") << id << "\n";

	// Login 처리
	// DB적용시 여기서 하자.
	bool isLoginOK = false;
	if (m_SessionPool.FindSessionbyName(id) == nullptr) {
		isLoginOK = m_DB.LoginToDB(id, pw, psession);
	}

	if (isLoginOK == true) {
		SC_PACKET_LOGIN_OK packet;
		packet.size = sizeof(SC_PACKET_LOGIN_OK);
		packet.type = e_SC_PACKET_TYPE::e_Login_Ok;
		packet.id = (psession->GetID());

		if (psession->Send(&packet) == false)
			return false;

		lstrcpy(psession->m_cName, id);

		psession->m_eClientState = e_Client_State::e_Lobby_State;

		// 로비정보들을 보내주자
		if (this->SendSC_Game_Session_Info(psession, false) == false) return false;
		if (this->SendSC_Character_Info(psession) == false)	return false;
	}
	else {
		SC_PACKET_LOGIN_FAIL packet;
		packet.id = (psession->GetID());
		packet.size = sizeof(SC_PACKET_LOGIN_FAIL);
		packet.type = e_SC_PACKET_TYPE::e_Login_Fail;

		if (psession->Send(&packet) == false)
			return false;
	}

	return true;
}

const bool CServerManager::RecvCS_Signup(CSessionData* psession)
{
	CS_PACKET_SIGNUP* ppacket = reinterpret_cast<CS_PACKET_SIGNUP*>(psession->m_RecvBuffer);
	TCHAR* id = ppacket->id;			// 아이디 패스워드가 제대로 출력이 안된다. 고치자
	TCHAR* pw = ppacket->pw;

	std::wcout << _T("ID : ") << id << "\n";

	UINT size = 0;

	// Signup 처리
	// DB적용시 여기서 하자.
	bool isSignupOK = true;
	isSignupOK = m_DB.SignupToDB(id, pw);
	if (isSignupOK == true) {
		SC_PACKET_SIGNUP_OK packet;
		packet.size = sizeof(SC_PACKET_SIGNUP_OK);
		packet.type = e_SC_PACKET_TYPE::e_Signup_Ok;
		packet.id = (psession->GetID());

		if (psession->Send(&packet) == false)
			return false;
	}
	else {
		SC_PACKET_SIGNUP_FAIL packet;
		packet.id = (psession->GetID());
		packet.size = sizeof(SC_PACKET_SIGNUP_FAIL);
		packet.type = e_SC_PACKET_TYPE::e_Sighup_Fail;

		if (psession->Send(&packet) == false)
			return false;
	}

	return true;
}

const bool CServerManager::RecvCS_Join(CSessionData* psession)
{
	CS_PACKET_JOIN* ppacket = reinterpret_cast<CS_PACKET_JOIN*>(psession->m_RecvBuffer);
	WORD wgameSessionNum = (ppacket->roomnum);

	if (wgameSessionNum < 0 || wgameSessionNum > GAMESESSION_NUM - 1)
		return false;

	bool isJoinOK = m_SessionPool.JoinSession(psession, wgameSessionNum);

	if (isJoinOK == true) {
		SC_PACKET_JOIN_OK packet;
		packet.size = sizeof(SC_PACKET_JOIN_OK);
		packet.type = e_SC_PACKET_TYPE::e_Join_Ok;

		if (psession->Send(&packet) == false)
			return false;

		CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(wgameSessionNum);

		if (pgameSession->StartGame() == true)
			m_Timer.AddTimer(NOW + ONE_SECOND * 2, e_Callbacktype::e_Timer_Update, nullptr, wgameSessionNum);

		SC_PACKET_GAME_SESSION_INFO sessionPacket;
		sessionPacket.size = sizeof(SC_PACKET_GAME_SESSION_INFO);
		sessionPacket.type = e_SC_PACKET_TYPE::e_Game_Session_Info;
		sessionPacket.roomnum = (wgameSessionNum);
		sessionPacket.isstart = true;
		sessionPacket.currentnum = pgameSession->GetPlayerNum();

		if (m_SessionPool.BroadcastSend(&sessionPacket) == false)
			return false;
	}
	else {
		SC_PACKET_JOIN_FAIL packet;
		packet.size = sizeof(SC_PACKET_JOIN_FAIL);
		packet.type = e_SC_PACKET_TYPE::e_Join_Fail;

		if (psession->Send(&packet) == false)
			return false;
	}

	return true;
}

const bool CServerManager::RecvCS_Access_Game_Ok(CSessionData* psession)
{
	//if (m_SessionPool.JoinSession(psession) == false)
	//	return false;
	// 서버 업데이트 시작하자

	CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(psession->m_wGameSessionNum);
	//pgameSession->m_Lock.Lock();

	//if (pgameSession->StartGame() == true)
	//	m_Timer.AddTimer(NOW + ONE_SECOND, e_Callbacktype::e_Timer_Update, psession);

	psession->m_eClientState = e_Client_State::e_Game_State;

	SC_PACKET_PUT_PLAYER packet;
	ZeroMemory(&packet, sizeof(packet));
	packet.size = sizeof(SC_PACKET_PUT_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Put_Player;

	GamePlayerInfo* pplayerInfo = &psession->m_GamePlayerInfo;

	packet.isdown = pplayerInfo->m_bIsDown;
	packet.islive = pplayerInfo->m_bIsLive;
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

	packet.weaponnum = pplayerInfo->m_cWeaponNum;

	packet.id = psession->GetID();

	// 접속한 캐릭터 정보를 뿌린다.
	/*if (m_SessionPool.BroadcastSend(&packet) == false)
		return false;*/

	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	// 접속해 있는 다른 클라이언트의 캐릭터의 정보을 보내준다.
	/*for (auto &iter : m_SessionPool.m_mapPlayers) {
		CSessionData *pplayer = iter.second;

		if (pplayer->m_eClientState == e_Client_State::e_Login_State)
			continue;

		if (pplayer->GetID() == psession->GetID())
			continue;

		SC_PACKET_PUT_PLAYER packet2;
		packet2.size = sizeof(SC_PACKET_PUT_PLAYER);
		packet2.type = e_SC_PACKET_TYPE::e_Put_Player;
		packet2.isdown = pplayer->m_GamePlayerInfo.m_bIsDown;
		packet2.islive = pplayer->m_GamePlayerInfo.m_bIsLive;
		packet2.x = pplayer->m_GamePlayerInfo.m_fX;
		packet2.y = pplayer->m_GamePlayerInfo.m_fY;
		packet2.z = pplayer->m_GamePlayerInfo.m_fZ;
		packet2.vx = pplayer->m_GamePlayerInfo.m_fVX;
		packet2.vy = pplayer->m_GamePlayerInfo.m_fVY;
		packet2.vz = pplayer->m_GamePlayerInfo.m_fVZ;
		packet2.yaw = pplayer->m_GamePlayerInfo.m_fYaw;
		packet2.pitch = pplayer->m_GamePlayerInfo.m_fPitch;
		packet2.roll = pplayer->m_GamePlayerInfo.m_fRoll;

		packet2.id = pplayer->GetID();

		if (psession->Send(&packet2) == false)
	}*/
	// 같은 방에 있는 캐릭터들의 정보를 보내준다.
	for (int i = 0; i < MAX_PLAYER; ++i) {
		CSessionData* pplayer = pgameSession->m_Sessions[i];
		if (pplayer == nullptr)
			continue;
		if (pplayer->m_eClientState != e_Client_State::e_Game_State)
			continue;
		if (pplayer->GetID() == psession->GetID())
			continue;

		SC_PACKET_PUT_PLAYER packet2;
		packet2.size = sizeof(SC_PACKET_PUT_PLAYER);
		packet2.type = e_SC_PACKET_TYPE::e_Put_Player;

		pplayerInfo = &pplayer->m_GamePlayerInfo;

		packet2.isdown = pplayerInfo->m_bIsDown;
		packet2.islive = pplayerInfo->m_bIsLive;
		packet2.life = pplayerInfo->m_fLife;
		packet2.x = pplayerInfo->m_fX;
		packet2.y = pplayerInfo->m_fY;
		packet2.z = pplayerInfo->m_fZ;
		packet2.vx = pplayerInfo->m_fVX;
		packet2.vy = pplayerInfo->m_fVY;
		packet2.vz = pplayerInfo->m_fVZ;
		packet2.yaw = pplayerInfo->m_fYaw;
		packet2.pitch = pplayerInfo->m_fPitch;
		packet2.roll = pplayerInfo->m_fRoll;

		packet2.id = pplayer->GetID();

		packet2.weaponnum = pplayerInfo->m_cWeaponNum;

		if (psession->Send(&packet2) == false)
			return false;
	}

	auto objects = pgameSession->m_mapGameObjects;
	for (auto object : objects) {
		if (object.second->m_bIsUse == false) continue;
		SendSC_Put_Object_To(&object.second->m_GameObjectInfo, psession);
	}

	// 게임 정보를 보내준다.
	if (SendSC_Game_Info(pgameSession, psession, false) == false)
		return false;

	if (SendSC_Ammo_Update(psession) == false)
		return false;

	if (SendSC_InGame_Log(psession->m_cName, INGAME_LOG_JOIN, nullptr, psession->m_wGameSessionNum) == false)
		return false;
	//if (SendSC_InGame_Log(psession, INGAME_LOG_KILL, psession, psession->m_wGameSessionNum) == false)
	//	return false;
	//if (SendSC_InGame_Log(psession, INGAME_LOG_LEAVE, nullptr, psession->m_wGameSessionNum) == false)
	//	return false;

	return true;
}

const bool CServerManager::RecvCS_Update(CSessionData* psession)
{
	CS_PACKET_UPDATE* ppacket = reinterpret_cast<CS_PACKET_UPDATE*>(psession->m_RecvBuffer);
	UINT size = 0;

	psession->m_UpdateLock.Lock();

	GamePlayerInfo* pplayerInfo = &psession->m_GamePlayerInfo;
	// 받은 정보를 서버에 저장
	pplayerInfo->m_fX = ppacket->x;
	pplayerInfo->m_fY = ppacket->y;
	pplayerInfo->m_fZ = ppacket->z;

	pplayerInfo->m_fVX = ppacket->vx;
	pplayerInfo->m_fVY = ppacket->vy;
	pplayerInfo->m_fVZ = ppacket->vz;

	pplayerInfo->m_fYaw = ppacket->yaw;
	pplayerInfo->m_fPitch = ppacket->pitch;
	pplayerInfo->m_fRoll = ppacket->roll;

	pplayerInfo->m_bIsUp = ppacket->isup;
	pplayerInfo->m_bIsFly = ppacket->isfly;
	pplayerInfo->m_bIsShoot = ppacket->isshoot;
	//pplayerInfo->m_bIsReloading = ppacket->isreloading;
	pplayerInfo->m_bIsGround = ppacket->isground;
	pplayerInfo->m_bIsRun = ppacket->isrun;
	pplayerInfo->m_bIsFalling = ppacket->isfalling;
	pplayerInfo->m_fSpeed = ppacket->speed;
	pplayerInfo->m_fDirection = ppacket->direction;
	pplayerInfo->m_fRotatorYaw = ppacket->rotatoryaw;
	pplayerInfo->m_fRotatorPitch = ppacket->rotatorpitch;
	pplayerInfo->m_fRotatorRoll = ppacket->rotatorroll;

	psession->m_UpdateLock.UnLock();

	// 받은 정보를 뿌린다.
	SC_PACKET_UPDATE_PLAYER packet;
	packet.size = sizeof(SC_PACKET_UPDATE_PLAYER);
	packet.type = e_SC_PACKET_TYPE::e_Update_Player;
	packet.id = psession->GetID();
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
	packet.isreloading = pplayerInfo->m_bIsReloading;
	packet.isground = pplayerInfo->m_bIsGround;
	packet.isrun = pplayerInfo->m_bIsRun;
	packet.isfalling = pplayerInfo->m_bIsFalling;
	packet.speed = pplayerInfo->m_fSpeed;
	packet.direction = pplayerInfo->m_fDirection;
	packet.rotatoryaw = pplayerInfo->m_fRotatorYaw;
	packet.rotatorpitch = pplayerInfo->m_fRotatorPitch;
	packet.rotatorroll = pplayerInfo->m_fRotatorRoll;

	packet.ischangingweapon = pplayerInfo->m_bIsChangingWeapon;

	/*if (m_SessionPool.BroadcastSend(&packet) == false)
		return false;*/
	if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
		return false;

	return true;
}

const bool CServerManager::RecvCS_Shoot(CSessionData* psession)
{
	CS_PACKET_SHOOT* ppacket = reinterpret_cast<CS_PACKET_SHOOT*>(psession->m_RecvBuffer);

	if (psession->m_GamePlayerInfo.m_bIsChangingWeapon == true)
		return false;

	GameObjectInfo objectInfo;
	ZeroMemory(&objectInfo, sizeof(GameObjectInfo));

	if (psession->m_GamePlayerInfo.m_cWeaponNum == WEAPON_MACHINEGUN) {
		if (psession->m_GamePlayerInfo.m_cMachinegunAmmo == 0)
			return false;
		objectInfo.m_eType = e_Machinegun_Bullet;
		psession->m_GamePlayerInfo.m_cMachinegunAmmo -= 1;
	}
	else if (psession->m_GamePlayerInfo.m_cWeaponNum == WEAPON_ROCKETGUN) {
		if (psession->m_GamePlayerInfo.m_cRocketgunAmmo == 0)
			return false;
		objectInfo.m_eType = e_Rocketgun_Bullet;
		psession->m_GamePlayerInfo.m_cRocketgunAmmo -= 1;
	}

	if (SendSC_Ammo_Update(psession) == false)
		return false;

	objectInfo.m_wWhoID = (ppacket->id);
	objectInfo.m_bIsDown = psession->m_GamePlayerInfo.m_bIsDown;

	objectInfo.m_fX = ppacket->x;
	objectInfo.m_fY = ppacket->y;
	objectInfo.m_fZ = ppacket->z;

	objectInfo.m_fVX = ppacket->vx;
	objectInfo.m_fVY = ppacket->vy;
	objectInfo.m_fVZ = ppacket->vz;

	objectInfo.m_fYaw = ppacket->yaw;
	objectInfo.m_fPitch = ppacket->pitch;
	objectInfo.m_fRoll = ppacket->roll;

	WORD wid = m_SessionPool.GetGameSessionByNum(psession->m_wGameSessionNum)->PutObjectInGame(&objectInfo);
	if (wid == NOTREGISTED)
		return false;

	objectInfo.m_wID = wid;

	this->SendSC_Put_Object(&objectInfo, psession->m_wGameSessionNum);

	return true;
}

const bool CServerManager::RecvCS_Collision(CSessionData* psession)
{
	CS_PACKET_COLLISION* ppacket = reinterpret_cast<CS_PACKET_COLLISION*>(psession->m_RecvBuffer);

	WORD oid = ppacket->oid;
	WORD cid = ppacket->cid;

	CGameSession* pgameSession = m_SessionPool.GetGameSessionByNum(psession->m_wGameSessionNum);

	switch (ppacket->etype)
	{
	case e_Collsion_Type::e_Rocketgun_Bullet_To_Other:
	{
		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		GameObject* pobject = pgameSession->m_mapGameObjects[oid];

		pgameSession->RemoveObjectInGame(oid);
	} break;
	case e_Collsion_Type::e_Rocketgun_Bullet_To_Character:
	{
		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		GameObject* pobject = pgameSession->m_mapGameObjects[oid];
		if (pobject == nullptr) {
			pgameSession->RemoveObjectInGame(oid);
			break;
		}
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;

		if (pobject->m_GameObjectInfo.m_bIsDown != pcharacter->m_GamePlayerInfo.m_bIsDown) {
			if (pcharacter->m_GamePlayerInfo.m_fLife > 0) {
				pcharacter->m_GamePlayerInfo.m_fLife -= DAMAGE_ROCKETGUN_BULLET;
				//SendSC_Update_Life(pcharacter);

				if (pcharacter->m_GamePlayerInfo.m_fLife <= 0) {
					if (pcharacter->m_GamePlayerInfo.m_bIsInMainBase) {
						pcharacter->m_GamePlayerInfo.m_bIsInMainBase = false;
						if (pcharacter->m_GamePlayerInfo.m_bIsDown)
							pgameSession->m_GameInfo.m_nDownPlayerCount -= 1;
						else
							pgameSession->m_GameInfo.m_nUpPlayerCount -= 1;
					}
					pcharacter->m_GamePlayerInfo.m_fLife = 0;
					pcharacter->m_GamePlayerInfo.m_bIsLive = false;

					SendSC_Die_Player(cid, psession->m_wGameSessionNum);
					m_Timer.AddTimer(NOW + TIMER_RESPAWN_PLAYER, e_Callbacktype::e_Timer_Respawn_Player, pcharacter);

					SendSC_InGame_Log(pgameSession->GetPlayerbyID(pobject->m_GameObjectInfo.m_wWhoID)->m_cName, INGAME_LOG_KILL, pcharacter->m_cName, pcharacter->m_wGameSessionNum);
				}
				SendSC_Damaged(pcharacter);
			}
		}

		pgameSession->RemoveObjectInGame(oid);
	} break;
	case e_Collsion_Type::e_Machinegun_Bullet_To_Other:
	{
		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		GameObject* pobject = pgameSession->m_mapGameObjects[oid];

		pgameSession->RemoveObjectInGame(oid);
	} break;
	case e_Collsion_Type::e_Machinegun_Bullet_To_Character:
	{
		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		GameObject* pobject = pgameSession->m_mapGameObjects[oid];
		if (pobject == nullptr) {
			pgameSession->RemoveObjectInGame(oid);
			break;
		}
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;

		if (pobject->m_GameObjectInfo.m_bIsDown != pcharacter->m_GamePlayerInfo.m_bIsDown) {
			if (pcharacter->m_GamePlayerInfo.m_fLife > 0) {
				pcharacter->m_GamePlayerInfo.m_fLife -= DAMAGE_MACHINEGUN_BULLET;
				//SendSC_Update_Life(pcharacter);

				if (pcharacter->m_GamePlayerInfo.m_fLife <= 0) {
					if (pcharacter->m_GamePlayerInfo.m_bIsInMainBase) {
						pcharacter->m_GamePlayerInfo.m_bIsInMainBase = false;
						if (pcharacter->m_GamePlayerInfo.m_bIsDown)
							pgameSession->m_GameInfo.m_nDownPlayerCount -= 1;
						else
							pgameSession->m_GameInfo.m_nUpPlayerCount -= 1;
					}
					pcharacter->m_GamePlayerInfo.m_fLife = 0;
					pcharacter->m_GamePlayerInfo.m_bIsLive = false;

					SendSC_Die_Player(cid, psession->m_wGameSessionNum);
					m_Timer.AddTimer(NOW + TIMER_RESPAWN_PLAYER, e_Callbacktype::e_Timer_Respawn_Player, pcharacter);

					SendSC_InGame_Log(pgameSession->GetPlayerbyID(pobject->m_GameObjectInfo.m_wWhoID)->m_cName, INGAME_LOG_KILL, pcharacter->m_cName, pcharacter->m_wGameSessionNum);
				}

				SendSC_Damaged(pcharacter);
			}
		}

		pgameSession->RemoveObjectInGame(oid);
	} break;
	case e_Collsion_Type::e_Character_To_Mainbase:
	{
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;
		if (pcharacter->m_GamePlayerInfo.m_bIsLive == false)
			break;

		if (pcharacter->m_GamePlayerInfo.m_bIsDown == false)
			pgameSession->m_GameInfo.m_nUpPlayerCount += 1;
		else
			pgameSession->m_GameInfo.m_nDownPlayerCount += 1;

		pcharacter->m_GamePlayerInfo.m_bIsInMainBase = true;
	} break;
	case e_Collsion_Type::e_Character_To_Mainbase_Out:
	{
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;
		if (pcharacter->m_GamePlayerInfo.m_bIsLive == false)
			break;

		if (pcharacter->m_GamePlayerInfo.m_bIsInMainBase == true) {
			if (pcharacter->m_GamePlayerInfo.m_bIsDown == false)
				pgameSession->m_GameInfo.m_nUpPlayerCount -= 1;
			else
				pgameSession->m_GameInfo.m_nDownPlayerCount -= 1;
		}

		pcharacter->m_GamePlayerInfo.m_bIsInMainBase = false;
	} break;
	case e_Collsion_Type::e_Character_To_HealPack_L:
	{
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;
		if (pcharacter->m_GamePlayerInfo.m_bIsLive == false)
			break;
		if (pcharacter->m_GamePlayerInfo.m_fLife == PLAYER_MAX_LIFE)
			break;
		GameObject* pobject = pgameSession->m_mapGameObjects[oid];
		pgameSession->RemoveObjectInGame(oid);
		
		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		float hp = pcharacter->m_GamePlayerInfo.m_fLife + HEALPACK_L_HEAL_AMOUNT;
		if (hp > PLAYER_MAX_LIFE)
			hp = PLAYER_MAX_LIFE;
		pcharacter->m_GamePlayerInfo.m_fLife = hp;

		//SendSC_Update_Life(pcharacter);

		SendSC_Healed(pcharacter);

		m_Timer.AddTimer(NOW + TIMER_RESPAWN_HEALPACK, e_Callbacktype::e_Timer_Respawn_Object, nullptr, psession->m_wGameSessionNum, oid);
	} break;
	case e_Collsion_Type::e_Character_To_HealPack_M:
	{
		CSessionData* pcharacter = pgameSession->GetPlayerbyID(cid);
		if (pcharacter == nullptr)
			break;
		if (pcharacter->m_GamePlayerInfo.m_bIsLive == false)
			break;
		if (pcharacter->m_GamePlayerInfo.m_fLife == PLAYER_MAX_LIFE)
			break;
		GameObject* pobject = pgameSession->m_mapGameObjects[oid];
		
		if (pgameSession->RemoveObjectInGame(oid) == false)
			break;

		SC_PACKET_REMOVE_OBJECT packet;
		packet.id = (oid);
		packet.size = sizeof(SC_PACKET_REMOVE_OBJECT);
		packet.type = e_SC_PACKET_TYPE::e_Remove_Object;

		if (m_SessionPool.BroadcastSendInGame(psession->m_wGameSessionNum, &packet) == false)
			break;

		float hp = pcharacter->m_GamePlayerInfo.m_fLife + HEALPACK_M_HEAL_AMOUNT;
		if (hp > PLAYER_MAX_LIFE)
			hp = PLAYER_MAX_LIFE;
		pcharacter->m_GamePlayerInfo.m_fLife = hp;

		SendSC_Healed(pcharacter);

		m_Timer.AddTimer(NOW + TIMER_RESPAWN_HEALPACK, e_Callbacktype::e_Timer_Respawn_Object, nullptr, psession->m_wGameSessionNum, oid);
	} break;
	default: break;
	}

	return true;
}

const bool CServerManager::RecvCS_Logout(CSessionData* psession)
{
	CS_PACKET_LOGOUT* ppacket = reinterpret_cast<CS_PACKET_LOGOUT*>(psession->m_RecvBuffer);
	WORD id = ppacket->id;
	UINT size = 0;

	SC_PACKET_LOGOUT_OK packet;
	packet.size = sizeof(SC_PACKET_LOGOUT_OK);
	packet.type = e_SC_PACKET_TYPE::e_Logout_Ok;
	packet.id = psession->GetID();

	if (psession->Send(&packet) == false)
		return false;

	return true;
}

const bool CServerManager::RecvCS_Back_To_Lobby(CSessionData* psession)
{
	int gamesessionnum = psession->m_wGameSessionNum;

	m_SessionPool.GetGameSessionByNum(gamesessionnum)->LeaveSession(psession);

	SendSC_InGame_Log(psession->m_cName, INGAME_LOG_LEAVE, nullptr, gamesessionnum);

	if (SendSC_Game_Session_Info(psession, true) == false)
		return false;

	if (SendSC_Character_Info(psession) == false)
		return false;
	
	return true;
}

const bool CServerManager::RecvCS_Swap_Weapon(CSessionData* psession)
{
	if (psession->m_GamePlayerInfo.m_bIsChangingWeapon == true)
		return false;
	if (psession->m_GamePlayerInfo.m_bIsReloading == true)
		return false;
	if (psession->m_GamePlayerInfo.m_bIsLive == false)
		return false;

	CS_PACKET_SWAP_WEAPON* ppacket = reinterpret_cast<CS_PACKET_SWAP_WEAPON*>(psession->m_RecvBuffer);
	
	char weaponnum = ppacket->weaponnum;
	char playerweaponnum = psession->m_GamePlayerInfo.m_cWeaponNum;

	if (weaponnum == playerweaponnum)
		return true;

	m_Timer.AddTimer(NOW + TIMER_SWAP_WEAPON, e_Callbacktype::e_Timer_Swap_Weapon, psession, weaponnum);

	//psession->m_GamePlayerInfo.m_cWeaponNum = weaponnum;
	psession->m_GamePlayerInfo.m_bIsChangingWeapon = true;

	//if (SendSC_Update(psession) == false)
	//	return false;

	return true;
}

const bool CServerManager::RecvCS_Reload(CSessionData* psession)
{
	if (psession->m_GamePlayerInfo.m_bIsChangingWeapon == true)
		return false;
	if (psession->m_GamePlayerInfo.m_bIsReloading == true)
		return false;
	if (psession->m_GamePlayerInfo.m_bIsLive == false)
		return false;

	switch (psession->m_GamePlayerInfo.m_cWeaponNum)
	{
	case WEAPON_MACHINEGUN:
	{
		if (psession->m_GamePlayerInfo.m_cMachinegunAmmo == MAX_MACHINEGUN_AMMO)
			return false;
		psession->m_GamePlayerInfo.m_bIsReloading = true;
		m_Timer.AddTimer(NOW + TIMER_RELOAD_MACHINEGUN, e_Callbacktype::e_Timer_Reload, psession);
	} break;
	case WEAPON_ROCKETGUN:
	{
		if (psession->m_GamePlayerInfo.m_cRocketgunAmmo == MAX_ROCKETGUN_AMMO)
			return false;
		psession->m_GamePlayerInfo.m_bIsReloading = true;
		m_Timer.AddTimer(NOW + TIMER_RELOAD_ROCKETGUN, e_Callbacktype::e_Timer_Reload, psession);
	} break;
	}
	
	return true;
}

const bool CServerManager::RecvCS_Back_To_MainMenu(CSessionData* psession)
{
	psession->m_eClientState = e_Client_State::e_Login_State;

	m_DB.SaveCharacterInfoToDB(psession);

	ZeroMemory(psession->m_cName, MAX_NAME_LEN);

	return true;
}
