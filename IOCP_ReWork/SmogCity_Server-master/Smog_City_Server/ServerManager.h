
#pragma once

#ifndef __SERVERMANAGER_H__
#define __SERVERMANAGER_H__

#include "../Common/Common.h"
#include "../Common/SessionPool.h"
#include "../Common/Thread.h"
#include "../Common/Lock.h"
#include "../Common/Timer.h"
#include "../Network/IOCPModule.h"
#include "../Network/DB.h"

class CServerManager
{
public :
	CServerManager();
	~CServerManager();
public :	// 서버 실행 및 종료
	const bool	StartServer();
	const bool	StopServer();
private :
	const bool	InitialAllData();
	const bool	InitialNetwork();
	const bool	InitialThread();
	const bool	InitialTimer();
	const bool	InitialDB();
private :
	const bool	ReleaseAllData();
private :
	CMultiThread		m_WorkerThread;
	CSingleThread		m_AcceptThread;
	CSingleThread		m_TimerThread;
	//CSingleThread		m_SendThread;
	CIOCPModule			m_IOCP;
	CSessionPool		m_SessionPool;
	CCriticalLock		m_Lock;
	CTimer				m_Timer;
	CDB					m_DB;
	//HANDLE				m_hSendEvent;
private :
	static const UINT _AcceptThread(LPVOID lpParam);
	static const UINT _WorkerThread(LPVOID lpParam);
	static const UINT _SendThread(LPVOID lpParam);
	static const UINT _TimerThread(LPVOID lpParam);
	const bool	AcceptThread();
	const bool	WorkerThread();
	const bool	SendThread();
	const bool	TimerThread();
private :	// 패킷 처리 함수
	const bool	PacketProcess(CSessionData* psession);

	const bool	DisconnectPlayer(CSessionData* psession);
public:
	const bool	SendSC_Game_Session_Info(CSessionData* psession, bool bbroadcast);
	const bool	SendSC_Game_Info(CGameSession* pgameSession, CSessionData* psession, bool isbroadcast = true);
	const bool	SendSC_Remove_Player(CSessionData *psession);
	const bool  SendSC_Put_Player(CSessionData *psession);
	const bool  SendSC_Die_Player(WORD wid, WORD wgameSessionNum);
	const bool  SendSC_Respawn_Player(WORD wid, WORD wgameSessionNum);
	const bool	SendSC_Put_Object(GameObjectInfo *pobjectInfo, WORD wgameSessionNum);
	const bool	SendSC_Put_Object_To(GameObjectInfo* pobjectInfo, CSessionData* psession);
	const bool	SendSC_Update(CSessionData* psession);
	const bool	SendSC_Update_Life(CSessionData* psession);
	const bool  SendSC_Character_Info(CSessionData* psession);
	const bool  SendSC_Swaped_Weapon(CSessionData* psession);
	const bool	SendSC_Damaged(CSessionData* psession);
	const bool	SendSC_Healed(CSessionData* psession);
	const bool	SendSC_Ammo_Update(CSessionData* psession);
	const bool	SendSC_InGame_Log(TCHAR* from, INT logtype, TCHAR* target, WORD wgameSessionNum);
	const bool	SendSC_Smog_OnOff(CSessionData* psession, BYTE x, BYTE y, BYTE z, bool isbroadcast = true);
private:
	const bool  RecvCS_Login(CSessionData* psession);
	const bool  RecvCS_Signup(CSessionData* psession);
	const bool	RecvCS_Join(CSessionData* psession);
	const bool	RecvCS_Access_Game_Ok(CSessionData* psession);		// 게임화면 접속완료
	const bool	RecvCS_Update(CSessionData* psession);
	const bool	RecvCS_Shoot(CSessionData* psession);
	const bool	RecvCS_Collision(CSessionData* psession);
	const bool  RecvCS_Logout(CSessionData* psession);
	const bool  RecvCS_Back_To_Lobby(CSessionData* psession);
	const bool  RecvCS_Swap_Weapon(CSessionData* psession);
	const bool  RecvCS_Reload(CSessionData* psession);
	const bool	RecvCS_Back_To_MainMenu(CSessionData* psession);
};

#endif	// __SERVERMANAGER_H__