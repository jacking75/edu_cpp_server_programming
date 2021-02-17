
#pragma once

#ifndef __SESSIONPOOL_H__
#define __SESSIONPOOL_H__

#include "Common.h"
#include "SessionData.h"
#include "Lock.h"
#include "Protocol.h"
#include "Timer.h"

constexpr int GAMESESSION_NUM	= SESSION_NUM / MAX_PLAYER;
constexpr int MAX_GAMEOBJECT	= 10000;

static WORD g_wObjectID = BASIC_OBJECT_NUM;

using std::array;

typedef struct stGameObject
{
	stGameObject() {
		m_bIsUse = false;
		ZeroMemory(&m_GameObjectInfo, sizeof(GameObjectInfo));
	}
	volatile bool	m_bIsUse;
	GameObjectInfo	m_GameObjectInfo;
} GAMEOBJECT, GameObject;

typedef concurrency::concurrent_unordered_map<DWORD, GameObject*> MAPOBJECT;

//class CServerManager;

// 게임별로 세션을 관리하자
class CGameSession
{
public:
	CGameSession();
	~CGameSession();
private:
	volatile bool	m_bIsRun;
	bool			m_bIsJoin;

	int				m_nID;
	//CMutex			m_Lock;
public:
	CCriticalLock	m_UpdateLock;
	CCriticalLock	m_SessionLock;
	CMutex	m_ObjectLock;

	DWORD									m_dwLastTickTime;

	array<CSessionData*, MAX_PLAYER>		m_Sessions;
	array<GameObject*, MAX_GAMEOBJECT>		m_GameObject;
	MAPOBJECT								m_mapGameObjects;

	GameInfo								m_GameInfo;
public:
	const bool		StartGame();
	const bool		ClearGame();
	const bool		RoundEnd();

	const bool		JoinSession(CSessionData *psession, WORD wNum);
	const bool		LeaveSession(CSessionData *psession);
	const bool		LeaveSession(WORD pid);

	const bool		BroadcastSendInGame(void *ppacket);

	const WORD		PutObjectInGame(GameObjectInfo* pobjectInfo);
	const bool		RemoveObjectInGame(WORD wid);
	GameObjectInfo*	RegenObject(WORD wobjectID);

	VOID			UpdateGamebyTimer(TimerEvent* ptimerEvent);
	VOID			UpdateGame(CTimer* ptimer);
public: 
	const INT			GetPlayerNum();
	CSessionData*		GetPlayerbyID(WORD wid);
	inline const bool	IsRun() {
		return m_bIsRun;
	}
	inline VOID			SetID(int nid) {
		m_nID = nid;
	}
};

typedef concurrency::concurrent_unordered_map<DWORD, CSessionData*> MAPPLAYER;

class CSessionPool
{
public :
	CSessionPool();
	~CSessionPool();
private :
	CCriticalLock	m_Lock;
	// 세션를 정해진 갯수만큼 미리 생성
	array<CSessionData*, SESSION_NUM> m_SessionPool;
	array<CGameSession, GAMESESSION_NUM> m_GameSession;
public:
	// 실질적으로 사용하는 세션
	MAPPLAYER		m_mapPlayers;
public :
	CSessionData		*CreateSession();
	const CSessionData*	FindSessionbyID(WORD wid);
	const CSessionData* FindSessionbyName(TCHAR* pname);
	inline CGameSession	*GetGameSessionByNum(WORD wnum) {
		return &m_GameSession[wnum];
	}

	const bool			InsertSession(CSessionData *psession);
	const bool			JoinSession(CSessionData *psession);
	const bool			JoinSession(CSessionData* psession, WORD wgameSessionNum);

	const WORD			GetSessionSize();
	VOID				DeleteSession(CSessionData *psession);
	const bool			LeaveSession(CSessionData *psession);
	const WORD			GetSessionbyID(CSessionData *psession);

	const bool			BroadcastSend(void *ppacket);
	const bool			BroadcastSendInGame(WORD wgameNum, void *ppacket);

	const bool			UpdatePlayersInfo();
	VOID				UpdateGamebyTimer(WORD wgameSessionNum, TimerEvent* ptimerEvent);
	VOID				UpdateGame(WORD wgameSessionNum);
};

#endif	// __SESSIONPOOL_H__