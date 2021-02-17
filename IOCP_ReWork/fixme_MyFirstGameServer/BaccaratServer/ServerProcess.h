// Process.h: interface for the CProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SERVERPROCESS_H_
#define _SERVERPROCESS_H_

#include "Server.h"

#define	GAMEBUFEMPTY		NULL
#define	GAMERINGBUFSIZE		8192


class CProcess  
{
private:
	OBJECTNODE			*m_pn;
	int					m_iIndex, m_iBaseChannelIndex;
	int					m_iUserBegin, m_iUserEnd;
	LPSOCKETCONTEXT		m_iGameRingBuf[ GAMERINGBUFSIZE ]; 
	int					m_iBegin, m_iEnd, m_iRestCount;		// 패킷을 버퍼에서 불러올때 사용
	HANDLE				m_hGameTEvent;
	HANDLE				m_hGameTProc;

public:
	CProcess();
	virtual ~CProcess();

	void InitProcess();
	inline void GameBufDequeue( LPSOCKETCONTEXT *lpSockContext );
	void GameBufEnqueue( LPSOCKETCONTEXT lpSockContext );
	
	friend unsigned int __stdcall GameTProc( LPVOID lpParam );
	
	

	void SetUserLink( int idx );
	BOOL UserFindId( char* cID, char idLen );
	inline void UserLifeCheck( time_t tCurTime_ );
	inline void CheckKickUserList();
    

	friend int OnRequestLogin( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestLogout( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int LogOutProcess( LPSOCKETCONTEXT lpSockContext );

	friend int OnRequestChannelList( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestFindFriend( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestLifeCount( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	friend void CheckRoomState( time_t tCurTime_ );

	// 정의는 다른 곳에 되어 있다.
	friend int OnRequestLogoutDB( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	

private:
	void KillUserLink( int idx );
	
};

int InitProcessLayer();

#endif // !defined(AFX_PROCESS_H__D777DCA8_1598_4AAC_B44D_28680CF09451__INCLUDED_)
