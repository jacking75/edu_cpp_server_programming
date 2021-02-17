
#pragma once

#ifndef __SESSIONDATA_H__
#define __SESSIONDATA_H__

#include "Common.h"
#include "Protocol.h"
#include "Lock.h"
#include "Struct.h"

enum e_Client_State {
	e_Login_State,
	e_Lobby_State,
	e_Game_State
};

class CSessionData
{
public :
	CSessionData(WORD wsessionID);
	~CSessionData();
private :
	SOCKADDR_IN			m_ClientAddrInfo;
	WORD				m_wID;
	// 사용되는 중인지
	bool				m_bIsReference;
public :
	CCriticalLock		m_UpdateLock;

	SOCKET				m_Socket;
	IoContext			*m_IoContext;
	INT					m_nPrevSize;
	CHAR				m_RecvBuffer[MAX_BUFFER];			// 패킷 조립을 위한 버퍼

	// 클라이언트별 데이터
	TCHAR				m_cName[MAX_NAME_LEN];
	//INT					m_nRoundAmount;
	CharacterInfo		m_CharacterInfo;
public:	
	WORD						m_wGameSessionNum;
	volatile e_Client_State		m_eClientState;
	GamePlayerInfo				m_GamePlayerInfo;
public :
	VOID				SetClientAddr(SOCKADDR_IN* pclientAddr);
	VOID				ClearSession();
	VOID				ClearIoContext();
	inline const WORD	GetID() {
		return m_wID;
	}
	inline const bool	GetReference() {
		return m_bIsReference;
	}
	inline VOID			SetReference() {
		m_bIsReference = true;
	}

	const bool			Send(void *ppacket);

	const bool			SetPlayerInfo(WORD wid, bool isrespawn);
	const bool			UpdatePlayerInfo();
};

#endif	// __SESSIONDATA_H__