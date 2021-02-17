
#include "SessionPool.h"

CGameSession::CGameSession()
{
	this->m_SessionLock.Lock();
	{
		//for (int i = 0; i < MAX_PLAYER; ++i) 
		//	m_Sessions[i] = nullptr;

		for (int i = 0; i < MAX_GAMEOBJECT; ++i)
			m_GameObject[i] = new GameObject;

		this->ClearGame();

		for (int x = 0; x < SMOG_MAX_X_INDEX; ++x)
			for (int y = 0; y < SMOG_MAX_Y_INDEX; ++y)
				for (int z = 0; z < SMOG_MAX_Z_INDEX; ++z) {
					if (z < SMOG_MAX_Z_INDEX / 2)
						m_GameInfo.m_IsSmog[x][y][z] = true;
					else
						m_GameInfo.m_IsSmog[x][y][z] = false;
				}
	}
	this->m_SessionLock.UnLock();
}

CGameSession::~CGameSession()
{
	this->m_SessionLock.Lock();
	{
		m_bIsRun = false;

		for (int i = 0; i < MAX_GAMEOBJECT; ++i)
			delete m_GameObject[i];

		m_mapGameObjects.clear();
	}
	this->m_SessionLock.UnLock();
}

const bool CGameSession::StartGame()
{
	if (m_bIsRun == true)
		return false;

	m_SessionLock.Lock();
	{
		m_bIsRun = true;
		
		for (int i = 0; i < BASIC_OBJECT_NUM; ++i)
			RegenObject(i);
	}
	m_SessionLock.UnLock();

	return true;
}

const bool CGameSession::ClearGame()
{
	this->m_SessionLock.Lock();
	{
		m_bIsRun = false;
		m_bIsJoin = true;

		m_dwLastTickTime = 0;

		m_GameInfo.m_nRound = 0;

		m_GameInfo.m_nUpWinCount = 0;
		m_GameInfo.m_nDownWinCount = 0;

		m_GameInfo.m_nUpPlayerCount = 0;
		m_GameInfo.m_nDownPlayerCount = 0;

		m_GameInfo.m_bIsEngaging = false;
		m_GameInfo.m_bIsMBDownOccupied = false;
		m_GameInfo.m_bIsMBDownOccupying = false;
		m_GameInfo.m_bIsMBUpOccupied = false;
		m_GameInfo.m_bIsMBUpOccupying = false;
		m_GameInfo.m_fDownTeamPercent = 50.f;
		m_GameInfo.m_fMBDownTime = 0.f;
		m_GameInfo.m_fMBUpTime = 0.f;
		m_GameInfo.m_fUpTeamPercent = 50.f;

		for (int i = 0; i < MAX_PLAYER; ++i)
			m_Sessions[i] = nullptr;

		for (int i = 0; i < MAX_GAMEOBJECT; ++i)
			m_GameObject[i]->m_bIsUse = false;

		m_mapGameObjects.clear();

		for (int x = 0; x < SMOG_MAX_X_INDEX; ++x)
			for (int y = 0; y < SMOG_MAX_Y_INDEX; ++y)
				for (int z = 0; z < SMOG_MAX_Z_INDEX; ++z) {
					if (z < SMOG_MAX_Z_INDEX / 2)
						m_GameInfo.m_IsSmog[x][y][z] = true;
					else
						m_GameInfo.m_IsSmog[x][y][z] = false;
				}
	}
	this->m_SessionLock.UnLock();

	return true;
}

const bool CGameSession::RoundEnd()
{
	this->m_SessionLock.Lock();
	{
		//for (int i = 0; i < MAX_PLAYER; ++i) {
		//	if (m_Sessions[i] == nullptr) continue;
		//	m_Sessions[i]->SetPlayerInfo(i);
		//}

		m_GameInfo.m_nDownPlayerCount = 0;
		m_GameInfo.m_nUpPlayerCount = 0;

		m_GameInfo.m_bIsEngaging = false;
		m_GameInfo.m_bIsMBDownOccupied = false;
		m_GameInfo.m_bIsMBDownOccupying = false;
		m_GameInfo.m_bIsMBUpOccupied = false;
		m_GameInfo.m_bIsMBUpOccupying = false;
		m_GameInfo.m_fDownTeamPercent = 50.f;
		m_GameInfo.m_fMBDownTime = 0.f;
		m_GameInfo.m_fMBUpTime = 0.f;
		m_GameInfo.m_fUpTeamPercent = 50.f;

		for (int i = 0; i < MAX_GAMEOBJECT; ++i)
			m_GameObject[i]->m_bIsUse = false;

		m_mapGameObjects.clear();

		for (int i = 0; i < BASIC_OBJECT_NUM; ++i)
			RegenObject(i);
	}
	this->m_SessionLock.UnLock();

	return true;
}

const bool CGameSession::JoinSession(CSessionData *psession, WORD wNum)
{
	bool breturn = false;

	if (m_bIsJoin == false)
		return breturn;

	this->m_SessionLock.Lock();
	{
		//if (m_bIsRun == false) 
		//	this->StartGame();

		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Sessions[i] == nullptr) {
				m_Sessions[i] = psession;
				m_Sessions[i]->SetPlayerInfo(i, false);
				m_Sessions[i]->m_wGameSessionNum = wNum;
				breturn = true;
				break;
			}
		}
	}
	this->m_SessionLock.UnLock();

	return breturn;
}

const bool CGameSession::LeaveSession(CSessionData *psession)
{
	bool breturn = false;

	this->m_SessionLock.Lock();
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (m_Sessions[i] == psession && m_Sessions[i]->GetID() == psession->GetID()) {
				m_Sessions[i]->m_eClientState = e_Client_State::e_Lobby_State;
				m_Sessions[i] = nullptr;
				breturn = true;
				break;
			}
		}
	}
	this->m_SessionLock.UnLock();

	if (this->GetPlayerNum() == 0)
	{
		this->ClearGame();
	}

	return breturn;
}

const bool CGameSession::LeaveSession(WORD pid)
{
	this->m_SessionLock.Lock();
	{
		for (int i = 0; i < MAX_PLAYER; ++i) {
			if (static_cast<WORD>(m_Sessions[i]->GetID()) == pid) {
				m_Sessions[i]->ClearSession();
				m_Sessions[i] = nullptr;
				return true;
			}
		}
	}
	this->m_SessionLock.UnLock();

	return false;
}

const bool CGameSession::BroadcastSendInGame(void *ppacket)
{
	CSessionData *psession = nullptr;

	if (m_bIsRun == false)
		return false;

	this->m_SessionLock.Lock();
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			psession = m_Sessions[i];

			if (psession == nullptr)
				continue;
			if (psession->m_eClientState != e_Client_State::e_Game_State)
				continue;

			psession->Send(ppacket);
		}
	}
	this->m_SessionLock.UnLock();

	return true;
}

const INT CGameSession::GetPlayerNum()
{
	int playerNum = 0;

	this->m_SessionLock.Lock();
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			if (m_Sessions[i] == nullptr)
				continue;

			++playerNum;
		}
	}
	this->m_SessionLock.UnLock();

	return playerNum;
}

CSessionData* CGameSession::GetPlayerbyID(WORD wid)
{
	CSessionData* psession = nullptr;

	//this->m_Lock.Lock();
	{
		for (int i = 0; i < MAX_PLAYER; ++i)
		{
			psession = m_Sessions[i];
			if (psession == nullptr)
				continue;

			if (psession->GetID() == wid)
				break;
		}
	}
	//this->m_Lock.UnLock();

	return psession;
}

const WORD CGameSession::PutObjectInGame(GameObjectInfo* pobjectInfo)
{
	WORD wid = NOTREGISTED;
	GameObject* pgameObject = nullptr;

	this->m_ObjectLock.Lock();
	{
		for (int i = BASIC_OBJECT_NUM; i < MAX_GAMEOBJECT; ++i) {
			pgameObject = m_GameObject[i];
			if (pgameObject->m_bIsUse == false) {
				memcpy(&pgameObject->m_GameObjectInfo, pobjectInfo, sizeof(GameObjectInfo));
				pgameObject->m_bIsUse = true;
				wid = g_wObjectID++;
				//m_mapGameObjects.insert({ wid, m_GameObject[i] });
				break;
			}
		}

		m_mapGameObjects.insert({ wid, pgameObject });
	}
	this->m_ObjectLock.UnLock();

	return wid;
}

const bool CGameSession::RemoveObjectInGame(WORD wid)
{
	bool breturn = true;

	this->m_ObjectLock.Lock();
	{
		if (m_mapGameObjects.find(wid) == m_mapGameObjects.end())
			breturn = false;

		GameObject* pgameObject = m_mapGameObjects[wid];
		if (pgameObject == nullptr) {
			m_mapGameObjects.unsafe_erase(wid);
			this->m_ObjectLock.UnLock();
			return false;
		}

		if (pgameObject->m_bIsUse == false)
			breturn = false;
		
		pgameObject->m_bIsUse = false;
		m_mapGameObjects.unsafe_erase(wid);
	}
	this->m_ObjectLock.UnLock();

	return breturn;
}

GameObjectInfo* CGameSession::RegenObject(WORD wobjectID)
{
	if (wobjectID >= BASIC_OBJECT_NUM)
		return nullptr;

	m_ObjectLock.Lock();
	GameObject* gameObject = m_GameObject[wobjectID];
	if (gameObject->m_bIsUse == true) {
		m_ObjectLock.UnLock();
		return nullptr;
	}
	if (m_mapGameObjects.find(wobjectID) == m_mapGameObjects.end())
		m_mapGameObjects[wobjectID] = gameObject;
	gameObject->m_bIsUse = true;
	m_ObjectLock.UnLock();

	gameObject->m_GameObjectInfo.m_wID = wobjectID;

	switch(wobjectID)
	{
	case 0:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
		gameObject->m_GameObjectInfo.m_fX = 12790.f;
		gameObject->m_GameObjectInfo.m_fY = -7000.f;
		gameObject->m_GameObjectInfo.m_fZ = 24290.f;
	} break;
	case 1:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
		gameObject->m_GameObjectInfo.m_fX = -9420.f;
		gameObject->m_GameObjectInfo.m_fY = -7160.f;
		gameObject->m_GameObjectInfo.m_fZ = 26180.f;
	} break;
	case 2:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
		gameObject->m_GameObjectInfo.m_fX = 7330.f;
		gameObject->m_GameObjectInfo.m_fY = 7610.f;
		gameObject->m_GameObjectInfo.m_fZ = 14150.f;
	} break;
	case 3:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
		gameObject->m_GameObjectInfo.m_fX = 820.f;
		gameObject->m_GameObjectInfo.m_fY = -3010.f;
		gameObject->m_GameObjectInfo.m_fZ = 3090.f;
	} break;
	case 4:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
		gameObject->m_GameObjectInfo.m_fX = 3130.f;
		gameObject->m_GameObjectInfo.m_fY = 7330.f;
		gameObject->m_GameObjectInfo.m_fZ = 5209.f;
	} break;
	// HEALPACK_M
	case 5:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
		gameObject->m_GameObjectInfo.m_fX = 7460.f;
		gameObject->m_GameObjectInfo.m_fY = 3770.f;
		gameObject->m_GameObjectInfo.m_fZ = 23500.f;
	} break;
	case 6:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
		gameObject->m_GameObjectInfo.m_fX = -250.f;
		gameObject->m_GameObjectInfo.m_fY = 9700.f;
		gameObject->m_GameObjectInfo.m_fZ = 23890.f;
	} break;
	case 7:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
		gameObject->m_GameObjectInfo.m_fX = 15090.f;
		gameObject->m_GameObjectInfo.m_fY = 5990.f;
		gameObject->m_GameObjectInfo.m_fZ = 15410.f;
	} break;
	case 8:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
		gameObject->m_GameObjectInfo.m_fX = 12180.f;
		gameObject->m_GameObjectInfo.m_fY = -5840.f;
		gameObject->m_GameObjectInfo.m_fZ = 4390.f;
	} break;
	case 9:
	{
		gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
		gameObject->m_GameObjectInfo.m_fX = 1940.f;
		gameObject->m_GameObjectInfo.m_fY = 10610.f;
		gameObject->m_GameObjectInfo.m_fZ = 1110.f;
	} break;
		// HEALPACK_L
	//case 0:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -17080.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 800.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 26639.3730469f;
	//} break;
	//case 1:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -13470.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -8660.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 27320.1347656f;
	//} break;
	//case 2:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = 4930.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -6520.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 24840.0f;
	//} break;
	//case 3:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -2030.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 3500.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 1650.0f;
	//} break;
	//case 4:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = 2190.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 9040.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 4780.0f;
	//} break;
	//case 5:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = 3420.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 4480.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 3260.0f;
	//} break;
	//case 6:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = 3090.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 2200.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 3140.0f;
	//} break;
	//case 7:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = 4390.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 9390.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 5170.0f;
	//} break;
	//case 8:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -8760.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -5670.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 7140.0;
	//} break;
	//case 9:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -6940.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 3670.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 4320.0f;
	//} break;
	//case 10:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -2290.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 8910.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 4780.0f;
	//} break;
	//case 11:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -4700.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 12360.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 3870.0f;
	//} break;
	//case 12:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -5480.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 9190.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 17410.0f;
	//} break;
	//case 13:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_L;
	//	gameObject->m_GameObjectInfo.m_fX = -2480.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -7910.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 9660.0f;
	//} break;
	//// HEALPACK_M
	//case 14:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -50.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 11040.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 58.5080185f;
	//} break;
	//case 15:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -1340.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 10460.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 520.0f;
	//} break;
	//case 16:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = 1160.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 10460.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 520.0f;
	//} break;
	//case 17:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = 8410.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 2610.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 6150.0f;
	//} break;
	//case 18:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = 5030.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -5070.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 4360.0f;
	//} break;
	//case 19:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -9170.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -5870.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 3540.0f;
	//} break;
	//case 20:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = 5030.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -5070.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 4360.0f;
	//} break;
	//case 21:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -3180.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 3180.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 5060.0f;
	//} break;
	//case 22:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -5490.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 10040.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 3730.0f;
	//} break;
	//case 23:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -4290.0f;
	//	gameObject->m_GameObjectInfo.m_fY = -9450.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 19990.0f;
	//} break;
	//case 24:
	//{
	//	gameObject->m_GameObjectInfo.m_eType = e_OBJECT_TYPE::e_HealPack_M;
	//	gameObject->m_GameObjectInfo.m_fX = -10140.0f;
	//	gameObject->m_GameObjectInfo.m_fY = 10790.0f;
	//	gameObject->m_GameObjectInfo.m_fZ = 15370.0f;
	//} break;
	}

	return &gameObject->m_GameObjectInfo;
}

VOID CGameSession::UpdateGamebyTimer(TimerEvent* ptimerEvent)
{
	this->m_UpdateLock.Lock();
	{
		if (m_bIsRun == true)
		{

		}
	}
	this->m_UpdateLock.UnLock();
}

VOID CGameSession::UpdateGame(CTimer* ptimer)
{
	if (m_bIsJoin == false)
		return;
	this->m_UpdateLock.Lock();
	{
		if (m_bIsRun == true)
		{
			// Smog Percent
			{
				if (m_GameInfo.m_bIsMBDownOccupied == true)
				{
					if (m_dwLastTickTime + TIMER_UPDATE_GAME * 50.f <= NOW)
					{
						m_GameInfo.m_fDownTeamPercent += 1.f;
						m_GameInfo.m_fUpTeamPercent -= 1.f;

						if (m_GameInfo.m_fDownTeamPercent >= SMOG_PERCENT_FOR_WIN) {
							
							m_GameInfo.m_nDownWinCount += 1;
							m_GameInfo.m_nRound += 1;
							if (m_GameInfo.m_nDownWinCount == ROUND_AMOUNT_FOR_WIN &&
								m_GameInfo.m_bIsEngaging == false) {
								SC_PACKET_GAME_RESULT packet2;
								packet2.size = sizeof(SC_PACKET_GAME_RESULT);
								for (int i = 0; i < MAX_PLAYER; ++i) {
									if (m_Sessions[i] == nullptr) continue;
									if (m_Sessions[i]->m_GamePlayerInfo.m_bIsDown) {
										m_Sessions[i]->m_CharacterInfo.m_nGameAmount += 1;
										m_Sessions[i]->m_CharacterInfo.m_nGameVictory += 1;
										packet2.type = e_SC_PACKET_TYPE::e_Game_Victory;
										m_Sessions[i]->Send(&packet2);
									}
									else {
										m_Sessions[i]->m_CharacterInfo.m_nGameAmount += 1;
										m_Sessions[i]->m_CharacterInfo.m_nGameDefeat += 1;
										packet2.type = e_SC_PACKET_TYPE::e_Game_Defeat;
										m_Sessions[i]->Send(&packet2);
									}
									this->m_bIsJoin = false;
								}
							}
							else {
								SC_PACKET_ROUND_RESULT packet;
								packet.size = sizeof(SC_PACKET_ROUND_RESULT);
								for (int i = 0; i < MAX_PLAYER; ++i) {
									if (m_Sessions[i] == nullptr) continue;
									if (m_Sessions[i]->m_GamePlayerInfo.m_bIsDown) {
										packet.type = e_SC_PACKET_TYPE::e_Round_Win;
										m_Sessions[i]->Send(&packet);
									}
									else {
										packet.type = e_SC_PACKET_TYPE::e_Round_Lose;
										m_Sessions[i]->Send(&packet);
									}
								}
								ptimer->AddTimer(NOW + TIMER_ROUND_START, e_Callbacktype::e_Timer_Round_Start, nullptr, m_nID);
								// 여기에 Timer에 라운드 재시작 추가
							}
						}

						m_dwLastTickTime = (DWORD)NOW;
					}
				}
				else
				{
					if (m_GameInfo.m_bIsMBUpOccupied == true)
					{
						if (m_dwLastTickTime + TIMER_UPDATE_GAME * 50.f <= NOW)
						{
							m_GameInfo.m_fUpTeamPercent += 1.f;
							m_GameInfo.m_fDownTeamPercent -= 1.f;

							if (m_GameInfo.m_fUpTeamPercent >= SMOG_PERCENT_FOR_WIN) {
								
								m_GameInfo.m_nUpWinCount += 1;
								m_GameInfo.m_nRound += 1;
								if (m_GameInfo.m_nUpWinCount == ROUND_AMOUNT_FOR_WIN) {
									SC_PACKET_GAME_RESULT packet2;
									packet2.size = sizeof(SC_PACKET_GAME_RESULT);
									for (int i = 0; i < MAX_PLAYER; ++i) {
										if (m_Sessions[i] == nullptr) continue;
										if (m_Sessions[i]->m_GamePlayerInfo.m_bIsDown) {
											m_Sessions[i]->m_CharacterInfo.m_nGameAmount += 1;
											m_Sessions[i]->m_CharacterInfo.m_nGameDefeat += 1;
											packet2.type = e_SC_PACKET_TYPE::e_Game_Defeat;
											m_Sessions[i]->Send(&packet2);
										}
										else {
											m_Sessions[i]->m_CharacterInfo.m_nGameAmount += 1;
											m_Sessions[i]->m_CharacterInfo.m_nGameVictory += 1;
											packet2.type = e_SC_PACKET_TYPE::e_Game_Victory;
											m_Sessions[i]->Send(&packet2);
										}
										this->m_bIsJoin = false;
									}
								}
								else {
									SC_PACKET_ROUND_RESULT packet;
									packet.size = sizeof(SC_PACKET_ROUND_RESULT);
									for (int i = 0; i < MAX_PLAYER; ++i) {
										if (m_Sessions[i] == nullptr) continue;
										if (m_Sessions[i]->m_GamePlayerInfo.m_bIsDown) {
											packet.type = e_SC_PACKET_TYPE::e_Round_Lose;
											m_Sessions[i]->Send(&packet);
										}
										else {
											packet.type = e_SC_PACKET_TYPE::e_Round_Win;
											m_Sessions[i]->Send(&packet);
										}
									}
									// 여기에 Timer에 라운드 재시작 추가
									ptimer->AddTimer(NOW + TIMER_ROUND_START, e_Callbacktype::e_Timer_Round_Start, nullptr, m_nID);
								}
							}

							m_dwLastTickTime = (DWORD)NOW;
						}
					}
					else
					{

					}
				}
			}

			// Main Base
			{
				if (m_GameInfo.m_bIsMBUpOccupied == true)
				{
					if (m_GameInfo.m_nUpPlayerCount == 0
						&& m_GameInfo.m_nDownPlayerCount == 0
						&& true)
					{
						m_GameInfo.m_bIsMBUpOccupying = false;
						//m_GameInfo.m_bIsMBDownOccupying = false;
						m_GameInfo.m_bIsEngaging = false;
						if (m_GameInfo.m_fMBDownTime > 0.f)
							m_GameInfo.m_fMBDownTime -= REALTIME_UPDATE_CYCLE * 2.f;
						else {
							m_GameInfo.m_bIsMBDownOccupying = false;
							m_GameInfo.m_fMBDownTime = 0;
						}
					}
					else
					{
						if (m_GameInfo.m_nUpPlayerCount > 0
							&& m_GameInfo.m_nDownPlayerCount > 0
							&& true)
						{
							m_GameInfo.m_bIsEngaging = true;
						}
						else
						{
							if (m_GameInfo.m_nUpPlayerCount > 0)
							{
								m_GameInfo.m_bIsMBUpOccupying = false;
								m_GameInfo.m_bIsEngaging = false;
								m_GameInfo.m_fMBDownTime = 0.f;
							}
							else
							{
								if (m_GameInfo.m_nDownPlayerCount > 0)
								{
									m_GameInfo.m_bIsMBDownOccupying = true;
									m_GameInfo.m_bIsEngaging = false;
									m_GameInfo.m_fMBUpTime = 0.f;

									if (m_GameInfo.m_fMBDownTime >= 10.f)
									{
										m_GameInfo.m_bIsMBDownOccupied = true;
										m_GameInfo.m_bIsMBDownOccupying = false;
										m_GameInfo.m_bIsMBUpOccupied = false;
									}
									else
										m_GameInfo.m_fMBDownTime += REALTIME_UPDATE_CYCLE;
								}
							}
						}
					}
				}
				else
				{
					if (m_GameInfo.m_bIsMBDownOccupied == true)
					{
						if (m_GameInfo.m_nUpPlayerCount == 0
							&& m_GameInfo.m_nDownPlayerCount == 0
							&& true)
						{
							//m_GameInfo.m_bIsMBUpOccupying = false;
							m_GameInfo.m_bIsMBDownOccupying = false;
							m_GameInfo.m_bIsEngaging = false;
							if (m_GameInfo.m_fMBUpTime > 0.f)
								m_GameInfo.m_fMBUpTime -= REALTIME_UPDATE_CYCLE * 2.f;
							else {
								m_GameInfo.m_fMBUpTime = 0.f;
								m_GameInfo.m_bIsMBUpOccupying = false;
							}
						}
						else
						{
							if (m_GameInfo.m_nUpPlayerCount > 0
								&& m_GameInfo.m_nDownPlayerCount > 0
								&& true)
							{
								m_GameInfo.m_bIsEngaging = true;
							}
							else
							{
								if (m_GameInfo.m_nUpPlayerCount > 0)
								{
									m_GameInfo.m_bIsMBUpOccupying = true;
									m_GameInfo.m_fMBDownTime = 0.f;
									m_GameInfo.m_bIsEngaging = false;

									if (m_GameInfo.m_fMBUpTime >= 10.f)
									{
										m_GameInfo.m_bIsMBUpOccupied = true;
										m_GameInfo.m_bIsMBUpOccupying = false;
										m_GameInfo.m_bIsMBDownOccupied = false;
									}
									else
										m_GameInfo.m_fMBUpTime += REALTIME_UPDATE_CYCLE;
								}
								else
								{
									if (m_GameInfo.m_nDownPlayerCount > 0)
									{
										m_GameInfo.m_bIsMBDownOccupying = false;
										m_GameInfo.m_bIsEngaging = false;
										m_GameInfo.m_fMBUpTime = 0.f;
									}
								}
							}
						}
					}
					else
					{
						if (m_GameInfo.m_nUpPlayerCount == 0
							&& m_GameInfo.m_nDownPlayerCount == 0
							&& true)
						{
							//m_GameInfo.m_bIsMBUpOccupying = false;
							//m_GameInfo.m_bIsMBDownOccupying = false;
							m_GameInfo.m_bIsEngaging = false;
							if (m_GameInfo.m_fMBUpTime > 0.f)
								m_GameInfo.m_fMBUpTime -= REALTIME_UPDATE_CYCLE * 2.f;
							else {
								m_GameInfo.m_bIsMBUpOccupying = false;
								m_GameInfo.m_fMBUpTime = 0.f;
							}
							if (m_GameInfo.m_fMBDownTime > 0.f)
								m_GameInfo.m_fMBDownTime -= REALTIME_UPDATE_CYCLE * 2.f;
							else {
								m_GameInfo.m_bIsMBDownOccupying = false;
								m_GameInfo.m_fMBDownTime = 0.f;
							}
						}
						else
						{
							if (m_GameInfo.m_nUpPlayerCount > 0
								&& m_GameInfo.m_nDownPlayerCount > 0
								&& true)
							{
								m_GameInfo.m_bIsEngaging = true;
							}
							else
							{
								if (m_GameInfo.m_nUpPlayerCount > 0)
								{
									m_GameInfo.m_bIsMBUpOccupying = true;
									m_GameInfo.m_fMBDownTime = 0.f;
									m_GameInfo.m_bIsEngaging = false;

									if (m_GameInfo.m_fMBUpTime >= 10.f)
									{
										m_GameInfo.m_bIsMBUpOccupied = true;
										m_GameInfo.m_bIsMBUpOccupying = false;
										m_GameInfo.m_bIsMBDownOccupied = false;
									}
									else
										m_GameInfo.m_fMBUpTime += REALTIME_UPDATE_CYCLE;
								}
								else
								{
									if (m_GameInfo.m_nDownPlayerCount > 0)
									{
										m_GameInfo.m_bIsMBDownOccupying = true;
										m_GameInfo.m_fMBUpTime = 0.f;
										m_GameInfo.m_bIsEngaging = false;

										if (m_GameInfo.m_fMBDownTime >= 10.f)
										{
											m_GameInfo.m_bIsMBDownOccupied = true;
											m_GameInfo.m_bIsMBDownOccupying = false;
											m_GameInfo.m_bIsMBUpOccupied = false;
										}
										else
											m_GameInfo.m_fMBDownTime += REALTIME_UPDATE_CYCLE;
									}
								}
							}
						}
					}
				}
			}
		}

	}
	this->m_UpdateLock.UnLock();
}

/////////////////////////////////////////////////////////////////////////////////////////////

CSessionPool::CSessionPool()
	: m_SessionPool()
{
	this->m_Lock.Lock();
	{ 
		// 개수만큼 미리 생성해둔다
		for (WORD i = 0; i < SESSION_NUM; ++i) 
			m_SessionPool[i] = new CSessionData(i);

		for (int i = 0; i < GAMESESSION_NUM; ++i)
			m_GameSession[i].SetID(i);
	}
	this->m_Lock.UnLock();
}

CSessionPool::~CSessionPool()
{
	this->m_Lock.Lock();
	{
		for ( WORD i = 0; i < SESSION_NUM; i++ )
			delete m_SessionPool[i];

		m_mapPlayers.clear();
	}
	this->m_Lock.UnLock();
}

CSessionData *CSessionPool::CreateSession()
{
	CSessionData *pplayer = nullptr;

	this->m_Lock.Lock();
	{
		for ( int i = 0; i < SESSION_NUM; i++ )
		{
			pplayer = m_SessionPool[i];
			if (pplayer->GetReference() != true )	// 사용되지 않고 있는 session 이면
			{
				pplayer->SetReference();				// session 사용중으로 바꾼다.
				//pplayer = m_SessionPool[i];
				break;
			}
		}
	}
	this->m_Lock.UnLock();

	return pplayer;
}

const CSessionData* CSessionPool::FindSessionbyID(WORD wid)
{
	CSessionData* pfindSession = nullptr;

	//this->m_Lock.Lock();
	{
		pfindSession = m_mapPlayers.find(wid)->second;
	}
	//this->m_Lock.UnLock();

	return pfindSession;
}

const CSessionData* CSessionPool::FindSessionbyName(TCHAR* pname)
{
	CSessionData* pfindSession = nullptr;

	this->m_Lock.Lock();
	auto sessions = m_mapPlayers;
	this->m_Lock.UnLock();

	for (auto session : sessions)
	{
		if (lstrcmp(session.second->m_cName, pname) == 0)
			pfindSession = session.second;
	}

	return pfindSession;
}

const bool CSessionPool::InsertSession(CSessionData* psession)
{
	//this->m_Lock.Lock();
	{
		m_mapPlayers.insert({ psession->GetID(), psession });
	}
	//this->m_Lock.UnLock();

	return true;
}
// 랜덤한방 조인
const bool CSessionPool::JoinSession(CSessionData* psession)
{
	//this->m_GameSessionLock.Lock();				// Lock 걸면 데드락 생김
	{
		for (int i = 0; i < GAMESESSION_NUM; ++i) {
			if (m_GameSession[i].JoinSession(psession, i) == true) {
				return true;
			}
		}
	}
	//this->m_GameSessionLock.UnLock();

	return false;
}

const bool CSessionPool::JoinSession(CSessionData* psession, WORD wgameSessionNum)
{
	bool breturn = m_GameSession[wgameSessionNum].JoinSession(psession, wgameSessionNum);

	return breturn;
}

const WORD CSessionPool::GetSessionSize()
{
	WORD wSessionSize = 0;

	//this->m_Lock.Lock();
	{
		wSessionSize = (WORD)(m_mapPlayers.size());
	}
	//this->m_Lock.UnLock();

	return wSessionSize;
}

VOID CSessionPool::DeleteSession(CSessionData* psession)
{
	MAPPLAYER::iterator iter;

	this->LeaveSession(psession);

	//this->m_Lock.Lock();			// Lock 걸어줘야 할까?
	{
		for ( iter = m_mapPlayers.begin(); iter != m_mapPlayers.end(); ++iter )
		{
			CSessionData* pfindSession = iter->second;
			if (psession)
			{
				if ( (pfindSession == psession) && (pfindSession->GetID() == psession->GetID()) )
				{
					pfindSession->ClearSession();
					//m_mapPlayers.unsafe_erase(iter);
					//m_mapPlayers.erase(iter);				// 지우지말고 다음 m_bIsReference를 false로 만들어주고 초기화 시켜주고 다른 클라이언트가 채우게 하자
															// 맵에 있는 동적할당된 데이터는 마지막 소멸자에서 모두 delete한다.	// 아니다 지워주자
					break;
				}
			}
		}
	}
	//this->m_Lock.UnLock();

	this->m_Lock.Lock();
	{
		m_mapPlayers.unsafe_erase(iter);
	}
	this->m_Lock.UnLock();
}

const bool CSessionPool::LeaveSession(CSessionData *psession)
{
	WORD gameSessionNum = psession->m_wGameSessionNum;
	if (gameSessionNum == NOTREGISTED)
		return false;

	if (m_GameSession[gameSessionNum].LeaveSession(psession) == true)
		return true;

	return false;
}

const WORD CSessionPool::GetSessionbyID(CSessionData* psession)
{
	CSessionData *pfindSession = nullptr;

	this->m_Lock.Lock();
	{
		for (auto &iter : m_mapPlayers)
		{
			if (iter.second->GetID() == psession->GetID()) {
				pfindSession = iter.second;
				break;
			}
		}
	}
	this->m_Lock.UnLock();

	return pfindSession->GetID();
}

const bool	CSessionPool::BroadcastSend(void *ppacket)
{
	CSessionData *psession = nullptr;

	//this->m_Lock.Lock();
	{
		for (auto &iter : m_mapPlayers)
		{
			psession = iter.second;

			if (psession->m_eClientState != e_Client_State::e_Lobby_State)
				continue;

			psession->Send(ppacket);
		}
	}
	//this->m_Lock.UnLock();

	return true;
}

const bool CSessionPool::BroadcastSendInGame(WORD wgameNum, void *ppacket)
{
	if (wgameNum == NOTREGISTED)
		return false;

	CGameSession *pgameSession = &m_GameSession[wgameNum];
	if (pgameSession == nullptr)
		return false;

	pgameSession->BroadcastSendInGame(ppacket);

	return true;
}

const bool	CSessionPool::UpdatePlayersInfo()
{
	CSessionData *psession = nullptr;

	m_Lock.Lock();
	auto players = m_mapPlayers;
	m_Lock.UnLock();

	//this->m_Lock.Lock();
	{
		for (auto& iter : players)
		{
			psession = iter.second;

			if (psession->m_eClientState == e_Client_State::e_Login_State)
				continue;

			psession->UpdatePlayerInfo();
		}
	}
	//this->m_Lock.UnLock();

	return true;
}

VOID CSessionPool::UpdateGamebyTimer(WORD wgameSessionNum, TimerEvent* ptimerEvent)
{
	m_GameSession[wgameSessionNum].UpdateGamebyTimer(ptimerEvent);
}

VOID CSessionPool::UpdateGame(WORD wgameSessionNum)
{
	//m_GameSession[wgameSessionNum].UpdateGame();
}
