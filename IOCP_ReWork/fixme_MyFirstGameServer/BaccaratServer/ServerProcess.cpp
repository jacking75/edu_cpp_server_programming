// Process.cpp: implementation of the CProcess class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include <windows.h>
#include <process.h>
#include "server.h"
#include "protocol_baccarat.h"
#include "serverutil.h"
#include "sock.h"
#include "channel.h"
#include "room.h"
#include "database.h"
#include "game.h"
#include "packetcoder.h"
#include "serverprocess.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcess::CProcess()
{

}

CProcess::~CProcess()
{
	SAFE_DELETE_ARRAY( m_pn );
	CloseHandle( m_hGameTEvent );
	CloseHandle( m_hGameTProc );
}

int InitProcessLayer()
{
	ServerContext.ps		= new CProcess;
	ServerContext.gameproc	= new CGame;
	
	if( ServerContext.ps == NULL || ServerContext.gameproc == NULL ) return 0;

	ServerContext.ps->InitProcess( );

	OnTransFunc[ REQUEST_LOGOUT ].proc			= OnRequestLogout;
	OnTransFunc[ REQUEST_CHANNEL_LIST ].proc	= OnRequestChannelList;
	OnTransFunc[ REQUEST_FIND_FRIEND ].proc		= OnRequestFindFriend;
	OnTransFunc[ REQUEST_LIFECOUNT ].proc		= OnRequestLifeCount;

#ifdef _LOGPROCESS_
	ServerUtil.ConsoleOutput( "InitProcess\n" );
#endif

	return 1;
}

void CProcess::InitProcess()
{
	int		dummy;
	int		iMaxUserNum = ServerContext.iMaxUserNum;
	
	m_pn = new OBJECTNODE[ iMaxUserNum ];
	for(int i=0; i < iMaxUserNum; ++i )
	{
		m_pn[ i ].next = NOTLINKED;
		m_pn[ i ].prev = NOTLINKED;
	}

	m_iUserBegin		= NOTLINKED;
	m_iUserEnd			= NOTLINKED;
	m_iIndex			= 0;
	m_iBegin			= 0;
	m_iEnd				= 0;
	m_iRestCount		= 0;
	m_iBaseChannelIndex = 0;
	
	time( &ServerContext.tMeasureLifeTime );
	time( &ServerContext.tMeasureRoomTime );

	m_hGameTEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hGameTProc  = (HANDLE)_beginthreadex( NULL, 0, GameTProc, this, 0, (unsigned int*)&dummy );
	if( m_hGameTProc == NULL ) return;
}



unsigned int __stdcall GameTProc(LPVOID lpParam)
{
	CProcess			*pProcess = (CProcess*)lpParam;
	DWORD				dwResult;
	LPSOCKETCONTEXT		lpSockContext;	
	char				*cpPacket;
	int					iPacketSize;
	short				sType;
	time_t				tCurTime;

	while( TRUE )
	{
		dwResult = WaitForSingleObject( pProcess->m_hGameTEvent, 100 );
		if( dwResult == WAIT_OBJECT_0 || ServerContext.bThreadStop == TRUE ) break;

		time( &tCurTime );

		// 연결을 잘라야 될 유저는 잘라버린다.
		ServerContext.ps->CheckKickUserList();

		// 클라이언트의 생존 유/무를 확인.
		if( tCurTime >= ServerContext.tMeasureLifeTime + ServerContext.iCheckLifeTime )
			ServerContext.ps->UserLifeCheck( tCurTime );

		// 방의 상태를 확인.
		if( tCurTime >= ServerContext.tMeasureRoomTime + 1 )
			CheckRoomState( tCurTime );

		while( TRUE )
		{
			pProcess->GameBufDequeue( &lpSockContext );
			if( lpSockContext == GAMEBUFEMPTY ) break;

			RecvTcpBufDequeue( lpSockContext, &cpPacket, &iPacketSize );
			CopyMemory( &sType, cpPacket + sizeof(short), sizeof(short) );

			// 패킷에서 정의한 타입 보다 큰 타입 이라면 무시한다. 
			if( sType < 0 || sType >= FINAL_PACKETDEFINE ) continue;

			OnTransFunc[ sType ].proc( lpSockContext, cpPacket );
		}
	}
	return 1;
}



void CProcess::GameBufEnqueue(LPSOCKETCONTEXT lpSockContext)
{
	m_iGameRingBuf[ m_iEnd++ ] = lpSockContext;
	if( m_iEnd >= GAMERINGBUFSIZE ) m_iEnd = 0;
	++m_iRestCount;

}


void CProcess::GameBufDequeue(LPSOCKETCONTEXT *lpSockContext)
{
	if( m_iRestCount <= 0 )
	{
		*lpSockContext = GAMEBUFEMPTY;
		return;
	}

	*lpSockContext = m_iGameRingBuf[ m_iBegin++ ];
	if( m_iBegin >= GAMERINGBUFSIZE ) m_iBegin = 0;
	--m_iRestCount;
}



void CProcess::SetUserLink(int idx)
{
	if( ServerContext.iCurUserNum == 0 )
	{
		m_pn[ idx ].prev = NOTLINKED;
		m_iUserBegin = m_iUserEnd = idx;
	}
	else
	{
		m_pn[ m_iUserEnd ].next = idx;
		m_pn[ idx ].prev		= m_iUserEnd;
		m_iUserEnd				= idx;
	}

	m_pn[ idx ].next = NOTLINKED;
	
	if( ServerContext.iCurUserNum < ServerContext.iMaxUserNum )
		++ServerContext.iCurUserNum;

}



void CProcess::KillUserLink(int idx)
{
	int		prev, next;

	prev = m_pn[ idx ].prev;
	next = m_pn[ idx ].next;

	if( ServerContext.iCurUserNum == 1 )
	{
		m_iUserBegin	= NOTLINKED;
		m_iUserEnd		= NOTLINKED;
	}
	else if( idx == m_iUserBegin )
	{
		m_pn[ next ].prev	= NOTLINKED;
		m_iUserBegin		= next;
	}
	else if( idx == m_iUserEnd )
	{
		m_pn[ prev ].next	= NOTLINKED;
		m_iUserEnd			= prev;
	}
	else
	{
		m_pn[ prev ].next = next;
		m_pn[ next ].prev = prev;
	}

	m_pn[ idx ].prev = NOTLINKED;
	m_pn[ idx ].next = NOTLINKED;

	if( ServerContext.iCurUserNum > 0 )
		--ServerContext.iCurUserNum;
}


int OnRequestLogin( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	//short		dummy = REQUEST_LOGIN_DB;

	//CopyMemory( cpPacket + sizeof( short ), &dummy, sizeof( short ) );

	//ServerContext.db->DataBaseBufEnqueue( lpSockContext, cpPacket );
	//OnRequestLoginDB( lpSockContext, NULL );

	return 1;
}



int OnRequestChannelList( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	CPacketCoder	packetcoder;
	char	cPacket[ 64 ] = { 0, };
	char	iMaxChannelNum = static_cast<char>( ServerContext.iMaxChannelInProcess );
	short   MaxUserNum = static_cast<short>( ServerContext.iMaxUserInRoom * ServerContext.iMaxRoomInChannel );
	int		iSize;

	packetcoder.SetBuf( cPacket );
	packetcoder.PutChar( ServerContext.cChannelLowNum );
	packetcoder.PutChar( ServerContext.cCnannelMiddleNum );
	packetcoder.PutChar( ServerContext.cChannelHighNum );
	packetcoder.PutShort( MaxUserNum );
	
	for( int i=0; i < iMaxChannelNum; ++i )
	{
		packetcoder.PutShort( ServerContext.ch[ i ].GetCurUserNum() );
	}

	// 본인의 돈을 보낸다.
	int iHigh = (int)((lpSockContext->GameState.money >> 32) & (__int64)0x00000000FFFFFFFF);
	int iLow = (int)((lpSockContext->GameState.money) & (__int64)0x00000000FFFFFFFF);
	packetcoder.PutInt( iHigh );
	packetcoder.PutInt( iLow );
	
	iSize = packetcoder.SetHeader( ANSWER_CHANNEL_LIST );
	PostTcpSend( 1, (int*)&lpSockContext, cPacket, iSize );

	return 1;
}


int OnRequestFindFriend( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	CPacketCoder		packetcoder;
	LPSOCKETCONTEXT		lpSC;
	int					iSize, iNext;
	char				cPacket[ 32 ] = { 0, };
	char				cIdLen, cID[ 16 ] = { 0, };
	char				cResult, cChannelNum, cRoomNum;
	char				cMaxRoomInChannel;

	iSize = cIdLen = 0;
	cRoomNum = cChannelNum = NOTLINKED;
	cMaxRoomInChannel = ServerContext.iMaxRoomInChannel;
	cResult = FIND_FAIL;
		
	packetcoder.SetBuf( cpPacket );
	packetcoder.GetChar( &cIdLen );   if( cIdLen <= 0 || cIdLen >= MAXIDLENGTH ) return 0;
	packetcoder.GetText( cID, cIdLen );

	iNext = ServerContext.ps->m_iUserBegin;
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		if( cIdLen == lpSC->idLen && strncmp( cID, lpSC->cID, cIdLen ) == 0 )
		{
			cResult     = FIND_SUCCESS;
			cChannelNum = static_cast<char>( lpSC->iChannel );
			cRoomNum    = static_cast<char>( lpSC->iRoom - ( cChannelNum*cMaxRoomInChannel ) );
			break;
		}

		iNext = ServerContext.ps->m_pn[ iNext ].next;
	}

	packetcoder.SetBuf( cPacket );
	packetcoder.PutChar( cResult );
	if( cResult == FIND_SUCCESS )
	{
		packetcoder.PutChar( cIdLen );
		packetcoder.PutText( cID, cIdLen );
		packetcoder.PutChar( cChannelNum );
		packetcoder.PutChar( cRoomNum );
	}
	
	iSize = packetcoder.SetHeader( ANSWER_FIND_FRIEND );
	PostTcpSend( 1, (int*)&lpSockContext, cPacket, iSize );
	
	return 1;
}


int OnRequestLifeCount( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	return 1;
}


void CProcess::UserLifeCheck( time_t tCurTime_ )
{
	LPSOCKETCONTEXT	lpSC;
	int				iNext;
	time_t			ElispTime;

	time( &ServerContext.tMeasureLifeTime );

	iNext = ServerContext.ps->m_iUserBegin;
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		if( lpSC->idLen <= 0 ) break;

		iNext = ServerContext.ps->m_pn[ iNext ].next;

		if( lpSC->bAgency == FALSE )
		{
			ElispTime = tCurTime_ - lpSC->RecvTime; 
			if( ElispTime >= LIFETIMECOUNT )
			{
				if( UserKickOrAgency( lpSC ) == TRUE )
				{
#ifdef _LOGPROCESS_
					ServerUtil.ConsoleOutput( "UserLifeCheck : Kick(%s) - CurTime(%d)\n", lpSC->cID, tCurTime_ );
#endif
#ifdef _LOGFILELEVEL3_
					ServerUtil.LogPrint( "UserLifeCheck : Kick(%s) - CurTime(%d)\n", lpSC->cID, tCurTime_ );
#endif
				}
				else
				{
#ifdef _LOGPROCESS_
					ServerUtil.ConsoleOutput( "UserLifeCheck : Agency(%s) - CurTime(%d)\n", lpSC->cID, tCurTime_ );
#endif
#ifdef _LOGFILELEVEL3_
					ServerUtil.LogPrint( "UserLifeCheck : Agency(%s) - CurTime(%d)\n", lpSC->cID, tCurTime_ );
#endif
				}
			}
		}
	}
}

void CProcess::CheckKickUserList()
{
	LPSOCKETCONTEXT lpSC;
	deque<int>::iterator index;

	EnterCriticalSection( &ServerContext.csKickUserIndexList );

	if( ServerContext.dKickUserIndexList.size() > 0 )
	{
		for( index = ServerContext.dKickUserIndexList.begin();
			index != ServerContext.dKickUserIndexList.end();
			++index )
		{
			lpSC = &ServerContext.sc[ *index ];

			if( lpSC->cState == GAME_OFF )
				EnqueueClose( lpSC );
		}

		ServerContext.dKickUserIndexList.clear();
	}

	LeaveCriticalSection( &ServerContext.csKickUserIndexList );
}


// 방의 상태는 GAME_OFF -> GAME_READY -> GAME_WAIT -> GAME_ING 순으로 변화한다.
void CheckRoomState( time_t tCurTime_ )
{
	CChannel*	pChannel = NULL;
	CRoom*		pRoom	 = NULL;
	int			i, iNext;
	int			iMaxChannelNum = ServerContext.iMaxChannel;
	time_t		Elisptime;

	time( &ServerContext.tMeasureRoomTime );

	for( i = 0; i < iMaxChannelNum; ++i )
	{
		pChannel = &ServerContext.ch[i];
		
		iNext = pChannel->GetBeginRoom();
		while( iNext > NOTLINKED && iNext < ServerContext.iMaxRoom )
		{
			pRoom = &ServerContext.rm[ iNext ];
			iNext = ServerContext.rn[ iNext ].next;
						
			if( pRoom->m_iState != GAME_OFF && pRoom->m_iState != GAME_SCORECALCULATE )
			{
				Elisptime = tCurTime_ - pRoom->m_tRoomTime;
				
				if( pRoom->m_iState == GAME_READY && Elisptime >= ServerContext.iGameReadyTime )
				{
					ServerContext.gameproc->BeginGameInRoom( pRoom );
				}
				else if( pRoom->m_iState == GAME_WAIT && Elisptime >= ServerContext.iThinkTime )
				{
					ServerContext.gameproc->AgencyRequestBeginGame( pRoom );
				}
			}
		}
	}
}


BOOL CProcess::UserFindId(char *cID, char idLen)
{
	LPSOCKETCONTEXT lpSC;
	int iNext = ServerContext.ps->m_iUserBegin;
	
	if( ServerContext.iCurUserNum == 0 ) return FALSE;

	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		if( idLen == lpSC->idLen && strncmp( cID, lpSC->cID, idLen ) == 0 )
		{
			return TRUE;
		}

		iNext = ServerContext.ps->m_pn[ iNext ].next;
	}

	return FALSE;
}


int OnRequestLogout( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	CPacketCoder	packetcoder;
	int				DBIndex;
	

	lpSockContext->cState = GAME_OFF;

	packetcoder.SetBuf( cpPacket );
	packetcoder.GetInt( &DBIndex );

	if( DBIndex != lpSockContext->iUserDBIndex ) return 0;

	EnqueueClose( lpSockContext );

	return 1;
}




int LogOutProcess( LPSOCKETCONTEXT lpSockContext )
{
	CChannel		*pChannel = &ServerContext.ch[ lpSockContext->iChannel ];
	CRoom			*pRoom, *pTmp;
	LPSOCKETCONTEXT lpSC;
	CPacketCoder	packetcoder;
	char			cPacket[ 256 ] = { 0, };
	char			cPacketOnlyRoom[20] = {0,};
	int				iSizeOnlyRoom;
	int				iSize, iNext, iNextNum;
	BOOL			bRemain = FALSE;

	iNextNum = ServerContext.pn[ lpSockContext->index ].next;
	
	if( lpSockContext->iRoom == NOTLINKED )
	{
		pChannel->KillUserLink( lpSockContext->index );
		lpSockContext->iChannel = NOTLINKED;
		lpSockContext->cState	= GAME_OFF;

		//유저가 채널에서 나감을 통보한다.
		packetcoder.SetBuf( cPacket );
		packetcoder.PutInt( lpSockContext->iUserDBIndex );
		iSize = packetcoder.SetHeader( ANSWER_NOTIFY_USERDELETE );
	}
	else
	{
		pRoom = &ServerContext.rm[ lpSockContext->iRoom ];
		pRoom->KillUserLink( lpSockContext->index );
		lpSockContext->cState	= GAME_OFF;
		lpSockContext->cPosition = WH_LOBBY;
		lpSockContext->iRoom = NOTLINKED;

		CheckChangeStateRoom( pRoom );

		if( pRoom->m_iUserNum <= 0 )
		{
			// < iSize = ExitNotifyRoomDelete( pChannel, pRoom, *cPacket ); > 항후 대체 고려
			// 방이 삭제됨을 통보
			pChannel->KillRoomLink( pRoom->m_iIndex );

			packetcoder.SetBuf( cPacket );
			packetcoder.PutChar( static_cast<char>(pRoom->m_iIndex - pChannel->m_iBaseRoomIndex) );
			iSize = packetcoder.SetHeader( ANSWER_NOTIFY_ROOMDELETE );
		}
		else
		{
			// < iSize = ExitNotiftRoomSetInfo( lpSockContext, pChannel, pRoom, *cPacket ); > 항후 대체 고려
			// 방 정보가 변경됨을 알린다.
			packetcoder.SetBuf( cPacket );
			packetcoder.PutChar( static_cast<char>(pRoom->m_iIndex - pChannel->m_iBaseRoomIndex) );
			packetcoder.PutChar( pRoom->m_iUserNum );

			if( pRoom->m_iOwnerIndex == lpSockContext->index )
			{
				lpSC = &ServerContext.sc[ pRoom->m_iUserBegin ];
				CopyMemory( pRoom->m_cOwner, lpSC->cID, lpSC->idLen );
				pRoom->m_cOwnerLen = lpSC->idLen;
				pRoom->m_iOwnerIndex = lpSC->index;

				packetcoder.PutInt( lpSC->iUserDBIndex );
			}
			else
			{
				packetcoder.PutInt( 0 );
			}
	
			iSize = packetcoder.SetHeader( ANSWER_NOTIFY_ROOMSETINFO );
			bRemain = TRUE;
		}

		// 유저가 룸에서 나감을 룸의 남은 사람들에게 통보
		if( bRemain )
		{
			//< RemainUserInRoom( pRoom, lpSockContext ); > 항후 대체 가능하면 아래 루틴을 이 함수로 대체한다.
			packetcoder.SetBuf( cPacketOnlyRoom );
			packetcoder.PutInt( lpSockContext->iUserDBIndex );
			packetcoder.PutChar( lpSockContext->iOrderInRoom );
			iSizeOnlyRoom = packetcoder.SetHeader( ANSWER_NOTIFY_USEREXITROOM );
			PostTcpSend( pRoom->GetUserBegin(), cPacketOnlyRoom, iSizeOnlyRoom );

			// 방의 순번을 반납한다.
			pRoom->DeleteOrderInRoom( lpSockContext->iOrderInRoom );
		}


		// 유저 정보 갱신 통보
		packetcoder.SetBuf( cPacket + iSize );
		packetcoder.PutInt( lpSockContext->iUserDBIndex );
		packetcoder.PutChar( WH_LOBBY );
		iSize += packetcoder.SetHeader( ANSWER_NOTIFY_USERSETINFO );

		// 유저가 채널에서 나감을 통보
		packetcoder.SetBuf( cPacket + iSize );
		packetcoder.PutInt( lpSockContext->iUserDBIndex );
		iSize += packetcoder.SetHeader( ANSWER_NOTIFY_USERDELETE );
	}

	// 모든 패킷을 한번에 보내버린다.
	PostTcpSend( pChannel->m_iUserBegin, cPacket, iSize );

	iNext = pChannel->m_iUsedRoomBegin;
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxRoom )
	{
		pTmp = &ServerContext.rm[ iNext ];
		PostTcpSend( pTmp->m_iUserBegin, cPacket, iSize );
		iNext = ServerContext.rn[ iNext ].next;
	}

	lpSockContext->cPosition = WH_CHANNELSELECT;

	return 1;
}