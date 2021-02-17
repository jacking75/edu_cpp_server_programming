// Game.cpp: implementation of the CGame class.
//
//////////////////////////////////////////////////////////////////////

#include "server.h"
#include "sock.h"
#include "protocol_baccarat.h"
#include "packetcoder.h"
#include "database.h"
#include "serverprocess.h"
#include "channel.h"
#include "room.h"
#include "serverutil.h"
#include "Game.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGame::CGame()
{
	InitializeGameProc();
	AheadPacketed();
}

CGame::~CGame()
{

}


void InitializeGameProc()
{
	OnTransFunc[ REQUEST_BEGINGAME ].proc		  = OnRequestBeginGame;
	
}


void CGame::AheadPacketed()
{
	short size;
	short ReservateExitRoom = ANSWER_RESERVATE_EXITROOM;
	short NotMoney = ANSWER_EXITROOM;
	char  result;

	// 나가기 예약한 사람은 나가라는 패킷 정의.
	size = HEADERSIZE;
	CopyMemory( ReservateExitRoomPacket, &size, sizeof(short) );
	CopyMemory( ReservateExitRoomPacket + sizeof(short), &ReservateExitRoom, sizeof(short) );

	// 돈이 없으니 방을 나간 후 로그 아웃 하라는 패킷 정의.
	size = 1;
	result = EXITROOM_NOTMONEY;
	CopyMemory( NotMoneyPacket, &size, sizeof(short) );
	CopyMemory( NotMoneyPacket + sizeof(short), &NotMoney, sizeof(short) );
	CopyMemory( NotMoneyPacket + HEADERSIZE, &result, sizeof(char) );
}


void CGame::MixCard(CRoom *pRoom, int MixCount)
{
	int num1, num2;
	char ImsiCard;						// 카드를 섞기위한 임시변수
	
	srand((unsigned)time(NULL));		// 난수생성의 초기화

	
	for(int k = 0 ; k < MixCount ; k++)
	{
		num1 = rand() % TOTALMAXCARDNUM;
		num2 = rand() % TOTALMAXCARDNUM;

		ImsiCard					= pRoom->m_privatecard[num1];
		pRoom->m_privatecard[num1]	= pRoom->m_privatecard[num2];
		pRoom->m_privatecard[num2]	= ImsiCard;
	}
}




void CGame::BeginGameInRoom( CRoom* pRoom )
{
	CPacketCoder	packetcoder;
	char			cPacket[32] = { 0, };
	int				i, iSize, iNext;
	int				nRoomNum = pRoom->m_RoomNum;

	pRoom->m_iState					= GAME_WAIT;

	// 방에 있는 유저의 상태를 대기 상태로 바꾼다.
	i = 0;
	iNext = pRoom->GetUserBegin();
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		// 없는 유저라면 당장 중지한다.
		if( iNext <= NOTLINKED || iNext >= ServerContext.iMaxUserNum )
			break;

		// 도중에 나간 유저가 있으면 루프를 빠져나온다.
		if( ServerContext.sc[ iNext ].iRoom != nRoomNum )
			break;

		ServerContext.sc[ iNext ].cState = GAME_WAIT;
		ServerContext.sc[ iNext ].bReservation = FALSE;
		ServerContext.sc[ iNext ].nSelectArea = NOTLINKED;
	
		iNext = ServerContext.pn[ iNext ].next;

		++i;
	}

	// 유저의 초기화 도중 나간 유저가 있다면
	if( i != pRoom->GetCurUserNum() )
	{
		BackGameInRoomState( pRoom );
		return;
	}

    NotifyRoomState( pRoom->m_iChannel, pRoom->m_RoomNum, GAME_ING );

    // 뱅커, 플레이어, 타이의 카드를 초기화 한다.
	pRoom->BankerInfo.nCurCardNum = 0;
	pRoom->PlayerInfo.nCurCardNum = 0;
	pRoom->TieInfo.nCurCardNum    = 0;

	// 각 카드의 정보를 초기화 한다.
	memset( pRoom->BankerInfo.Cards, NOTUSECARD, BACCARAT_HOLDCARDNUM );
	memset( pRoom->PlayerInfo.Cards, NOTUSECARD, BACCARAT_HOLDCARDNUM );
	memset( pRoom->TieInfo.Cards, NOTUSECARD, BACCARAT_HOLDCARDNUM );
		

	// 뱅커, 플레이어의 영역의 점수를 초기화 한다.
	pRoom->BankerInfo.Score = 0;
	pRoom->PlayerInfo.Score = 0;

	// 카드를 섞고 초기화 한다.
	MixCard( pRoom, TOTALMAXCARDNUM );
	pRoom->m_iIndexprivatecard = NOTLINKED;
	

	// 게임이 시작됨을 통보 한다.
	packetcoder.SetBuf( cPacket );
	iSize = packetcoder.SetHeader( ANSWER_GAMESTART );
	PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

#ifdef _LOGFILELEVEL3_
	ServerUtil.BeginGameInRoomLog( pRoom->m_iChannel, pRoom->m_RoomNum, pRoom->GetCurUserNum() );
#endif
	
	pRoom->m_nStartCompleteUserNum	= 0;	// 게임 준비가 끝난 사람 숫자 초기화.
	
	time( &pRoom->m_tRoomTime );
	
}



int	OnRequestBeginGame( LPSOCKETCONTEXT lpSockContext, char* cpPacket )
{
	CPacketCoder	packetcoder;
	LPSOCKETCONTEXT lpSC;
	int				roomNum = lpSockContext->iRoom;
	int				iSize, iNext;
	char			BetLevel, Pos, cPacket[64] = { 0,};	

	if( roomNum < 0 || roomNum >= ServerContext.iMaxRoom ) return 0;

	CRoom*		pRoom = &ServerContext.rm[ roomNum ];
	
	if( pRoom->m_iState != GAME_WAIT || lpSockContext->cState != GAME_WAIT ) return 0;
	
	packetcoder.SetBuf( cpPacket );
	packetcoder.GetChar( &BetLevel );
	packetcoder.GetChar( &Pos );

	// 선택 위치를 유효한 곳에 했는지 체크 한다.
	if( Pos < 0 || Pos >= SELECTAREANUM ) return 0;

	// 배팅금액이 이 방의 최소 최대 금액을 넘어서는 것이라면 무시한다. 
	if( BetLevel < MINBETTINGMULTYPLE || BetLevel >= MAXBETTINGMULTYPLE ) return 0;
	
	// 배팅 금액을 계산한다.
	lpSockContext->ibetMoney = pRoom->m_Basebettingmoney[ BetLevel ];
	

	// 어느쪽을 선택 했는가를 체크하여 저장한다.
	lpSockContext->nSelectArea = Pos;

	
	// 배팅을 선택한것을 알려준 유저의 수를 저장한다.
	pRoom->m_nStartCompleteUserNum++;

		
	// 유저의 상태를 게임 중으로 바꾼다.
	lpSockContext->cState = GAME_ING;

	
	// 방의 유저 수만큼 응답이 왔으면 카드 2장을 보낸다.
	if( pRoom->m_nStartCompleteUserNum == pRoom->GetCurUserNum() )
	{
		// 모든 유저에게  유저의 배팅 선택 영역과 금액을 알려준다.
		packetcoder.SetBuf( cPacket );
		packetcoder.PutChar( pRoom->GetCurUserNum() );

		iNext = pRoom->GetUserBegin();
		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{	
			lpSC = &ServerContext.sc[ iNext ];

			packetcoder.PutInt( lpSC->iUserDBIndex );
			packetcoder.PutInt( lpSC->ibetMoney );
			packetcoder.PutChar( lpSC->nSelectArea );

			iNext = ServerContext.pn[ iNext ].next;
		}
		iSize = packetcoder.SetHeader( ANSWER_BEGINGAME );
		PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );
		
		pRoom->m_nStartCompleteUserNum = 0;
		pRoom->m_iState = GAME_ING;

#ifdef _LOGFILELEVEL3_
		ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strRequestBeginGame );
#endif
		
		// 카드를 보낸다.
		ServerContext.gameproc->SendStartCards( pRoom );
	}
	
	return 1;
	
}



void CGame::AgencyRequestBeginGame(CRoom *pRoom)
{
	LPSOCKETCONTEXT lpSC;
	CPacketCoder	packetcoder;
	char			cPacket[64] = {0,};
	int				iSize;
	int				iNext = pRoom->GetUserBegin();


	pRoom->m_nStartCompleteUserNum = 0;
	pRoom->m_iState = GAME_ING;

	// 유저의 선택 사항을 모든 유저에게 통보해준다.
	packetcoder.SetBuf( cPacket );
	packetcoder.PutChar( pRoom->GetCurUserNum() );
		
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		
		// 선택을 알려주지 않은 유저는 서버가 임의로 대신 선택한다.
		if( lpSC->cState == GAME_WAIT )
		{
			lpSC->cState	  = GAME_ING;
			lpSC->nSelectArea = _PLAYER;
			lpSC->ibetMoney   = pRoom->m_Basebettingmoney[0];
		}

		packetcoder.PutInt( lpSC->iUserDBIndex );
		packetcoder.PutInt( lpSC->ibetMoney );
		packetcoder.PutChar( lpSC->nSelectArea );

		iNext = ServerContext.pn[ iNext ].next;
		
	}
	iSize = packetcoder.SetHeader( ANSWER_BEGINGAME );
	PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strAgencyRequestBeginGame );
#endif

	ServerContext.gameproc->SendStartCards( pRoom );
	
}


void CGame::SendStartCards( CRoom* pRoom )
{
	CPacketCoder	packetcoder;
	char			BankerScore, PlayerScore, cPacket[ 64 ] = { 0, };
	int				iSize;
	
	// 뱅커와 플레이어에게 카드를 2장식 할당한다.
	pRoom->BankerInfo.Cards[0] = GetPrivateCard( pRoom );
	pRoom->PlayerInfo.Cards[0] = GetPrivateCard( pRoom );

	pRoom->BankerInfo.Cards[1] = GetPrivateCard( pRoom );
	pRoom->PlayerInfo.Cards[1] = GetPrivateCard( pRoom );
	
	pRoom->BankerInfo.nCurCardNum = 2;
	pRoom->PlayerInfo.nCurCardNum = 2;

	// 뱅커가 받은 카드의 점수를 계산한다.
	pRoom->BankerInfo.Score += CardToNumber[ pRoom->BankerInfo.Cards[0] ];
	pRoom->BankerInfo.Score += CardToNumber[ pRoom->BankerInfo.Cards[1] ];
	if( pRoom->BankerInfo.Score >= 10 ) pRoom->BankerInfo.Score %= 10;
	

	// 플레이어가 받은 카드의 점수를 계산한다.
	pRoom->PlayerInfo.Score += CardToNumber[ pRoom->PlayerInfo.Cards[0] ];
	pRoom->PlayerInfo.Score += CardToNumber[ pRoom->PlayerInfo.Cards[1] ];
	if( pRoom->PlayerInfo.Score >= 10 ) pRoom->PlayerInfo.Score %= 10;
	

	// 뱅커와 플레이어의 카드와 점수를 알려준다. 
	packetcoder.SetBuf( cPacket );
	packetcoder.PutText( pRoom->BankerInfo.Cards, 2 );
	packetcoder.PutChar( pRoom->BankerInfo.Score );
	packetcoder.PutText( pRoom->PlayerInfo.Cards, 2 );
	packetcoder.PutChar( pRoom->PlayerInfo.Score );
	iSize = packetcoder.SetHeader( ANSWER_STARTCARDS );
	PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strSendStartCards );
#endif

	BankerScore = pRoom->BankerInfo.Score;
	PlayerScore = pRoom->PlayerInfo.Score;

	// 세번째 카드를 줄지 아니면 점수를 바로 보낼지 결정하여 보내준다.
	if( BankerScore >= EIGHTCARD || PlayerScore >= EIGHTCARD )
	{
		NotifyGameResult( pRoom );
	}
	else	// 플레이어나 뱅커가 세번째 카드를 받을 수 있다면
	{
		char	P_threecard = NOTUSECARD;	// 플레이어의 세번째 카드
		char	B_threecard = NOTUSECARD;	// 뱅커의 세번째 카드

		packetcoder.SetBuf( cPacket );

		// 플레이어는 세번째 카드를 받을 수 있나
		if( ServerContext.gameproc->CanPlayerReceiveThreedCard( PlayerScore ) == TRUE )
		{
			P_threecard = ServerContext.gameproc->GetPrivateCard( pRoom );
			pRoom->PlayerInfo.Cards[2] = P_threecard;
			pRoom->PlayerInfo.Score += CardToNumber[ P_threecard ];
			if( pRoom->PlayerInfo.Score >= 10 ) pRoom->PlayerInfo.Score %= 10;
		}
        packetcoder.PutChar( P_threecard );
	

		// 뱅커는 세번째 카드를 받을 수 있나 ?
		if( ServerContext.gameproc->CanBankerReceiveThreeCard( P_threecard, BankerScore ) == TRUE )
		{
			B_threecard = ServerContext.gameproc->GetPrivateCard( pRoom );
			pRoom->BankerInfo.Cards[2] = B_threecard;
			pRoom->BankerInfo.Score += CardToNumber[ B_threecard ];
			if( pRoom->BankerInfo.Score >= 10 ) pRoom->BankerInfo.Score %= 10;
		}
        packetcoder.PutChar( B_threecard );
		

		// 세번째 카드를 보낸다.
		iSize = packetcoder.SetHeader( ANSWER_SENDTHREECARD );
		PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

		// 결과를 통보 한다.
		NotifyGameResult( pRoom );
	}

}


BOOL CGame::CanPlayerReceiveThreedCard( char PlayerScore )
{
	if( PlayerScore <= 5 ) 
		return TRUE;
	else
		return FALSE;
}



BOOL CGame::CanBankerReceiveThreeCard( char Playerthreecard, char BankerScore )
{
	if( BankerScore >= 0 && BankerScore <= 2 )
	{
		return TRUE;
	}
	else
	{
		if( Playerthreecard == NOTUSECARD ) return FALSE;

		switch ( BankerScore )
		{
		case 3:
			if( Playerthreecard != 8 ) return TRUE;
			break;
		case 4:
			if( Playerthreecard >= 2 && Playerthreecard <= 7 ) return TRUE;
			break;
		case 5:
			if( Playerthreecard >= 4 && Playerthreecard <= 7 ) return TRUE;
			break;
		case 6:
			if( Playerthreecard == 6 || Playerthreecard == 7 ) return TRUE;
			break;
		}
	}

	return FALSE;
}


void CGame::NotifyGameResult(CRoom *pRoom)
{
	CPacketCoder	packetcoder;
	LPSOCKETCONTEXT lpSC;
	char			cPacket[64] = {0,};
	char			BankerScore = pRoom->BankerInfo.Score;
	char			PlayerScore = pRoom->PlayerInfo.Score;
	char			area;
	char			nUserNum = pRoom->GetCurUserNum();
	int				iSize;
	int				iNext = pRoom->GetUserBegin();


	pRoom->m_iState	= GAME_SCORECALCULATE;
	time( &pRoom->m_tRoomTime );


	packetcoder.SetBuf( cPacket );
	packetcoder.PutChar( NOTLINKED );
	packetcoder.PutChar( BankerScore );
	packetcoder.PutChar( PlayerScore );
	packetcoder.PutChar( nUserNum );


	if( BankerScore == PlayerScore )	// 뱅커아 플레이어의 점수가 같다. 즉 타이가 이김.
	{	
		area = _TIE;
		CopyMemory( cPacket + HEADERSIZE, &area, sizeof(char) );


		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];

			if( lpSC->nSelectArea == _TIE )		// 유저가 타이를 선택했다면
			{
                lpSC->ibetMoney *= TIEWIN_MULTIPLE;
				ServerContext.db->SaveUserData( lpSC, FALSE );

				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}
			else	// 유저가 타이를 선택하지 않았다면
			{
				lpSC->ibetMoney = -( lpSC->ibetMoney ); 

				ServerContext.db->SaveUserData( lpSC, FALSE );
				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}

			iNext = ServerContext.pn[ iNext ].next;
		}
	}
	else if( BankerScore > PlayerScore )	// 뱅커가 플레이어를 이긴 경우.
	{
		area = _BANKER;
		CopyMemory( cPacket + HEADERSIZE, &area, sizeof(char) );

		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];

			if( lpSC->nSelectArea == _BANKER )	// 유저는 뱅커를 선택했다.
			{
				lpSC->ibetMoney -= static_cast<int>( ( lpSC->ibetMoney * COMMISSION ) );
				ServerContext.db->SaveUserData( lpSC, FALSE );
				
				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}
			else								// 유저는 뱅커를 선택하지 않았다.		
			{
				lpSC->ibetMoney = -( lpSC->ibetMoney );
				ServerContext.db->SaveUserData( lpSC, FALSE );
				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}
			iNext = ServerContext.pn[ iNext ].next;
		}
	}
	else								// 플레이어가 뱅커를 이긴 경우.
	{
		area = _PLAYER;
		CopyMemory( cPacket + HEADERSIZE, &area, sizeof(char) );

		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];

			if( lpSC->nSelectArea == _PLAYER )		//  유저는 플레이어쪽을 선택했다.
			{
				ServerContext.db->SaveUserData( lpSC, FALSE );
				
				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}
			else									// 유저는 플레이어쪽을 선택하지 않았다.
			{
				lpSC->ibetMoney = -( lpSC->ibetMoney ); 
				ServerContext.db->SaveUserData( lpSC, FALSE );
				packetcoder.PutInt( lpSC->iUserDBIndex );
				packetcoder.PutInt( lpSC->ibetMoney );
			}
			iNext = ServerContext.pn[ iNext ].next;
		}
	}

	iSize = packetcoder.SetHeader( ANSWER_RESULT );
	PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameResultLog( pRoom->m_iChannel, pRoom->m_RoomNum );
#endif

	// 비 정상 종료 유저는 종료 시키고, 나가기 예약 유저는 방을 나가게 한다.
	ResetUserState( pRoom );

	// 다시 게임을 할 수 있는 상태로 만든다.
	SendGameOff( pRoom );
}



void CGame::ResetUserState(CRoom *pRoom)
{
	LPSOCKETCONTEXT lpSC;
	CPacketCoder	packetcoder;
	int				iNext = pRoom->GetUserBegin();
	
	pRoom->m_iState = GAME_OFF;

	while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		iNext = ServerContext.pn[ iNext ].next;

		if( lpSC->bAgency == FALSE && lpSC->cState != GAME_NOTMONEY )
			lpSC->cState = GAME_OFF;

		// 비 정상 종료자는  쫒아 낸다.
		if( lpSC->bAgency == TRUE )	
		{
			EnqueueClose( lpSC );
		}
		else // 돈이 없는 사람은 나가라고 유저에게 통보한다.
		if( lpSC->cState == GAME_NOTMONEY )
		{
			lpSC->cState = GAME_OFF;
			OnRequestExitRoom( lpSC, NotMoneyPacket );
		}
		else // 나가기 예약이라면
		if( lpSC->bReservation == TRUE )		
		{
			lpSC->bReservation = FALSE;
			PostTcpSend( 1, (int*)&lpSC, ReservateExitRoomPacket, HEADERSIZE );
		}
	}

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strResetUserState );
#endif

}



void CGame::SendGameOff( CRoom* pRoom )
{
	NotifyRoomState( pRoom->m_iChannel, pRoom->m_RoomNum, GAME_OFF );


	if( pRoom->GetCurUserNum() >= MINPLAYER )
	{
		pRoom->m_iState = GAME_READY;
		time( &pRoom->m_tRoomTime );
	}

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strSendGameOff );
#endif
}



void CGame::NotifyRoomState( int iChannelIndex, char RoomNum, char state )
{
	
	CPacketCoder	packetcoder;
	CChannel*	pChannel = &ServerContext.ch[ iChannelIndex ];
	CRoom*		pRoom	= NULL;
	char		cPacket[16] = { 0, };
	int			iSize, iNext;


	// 로비에 있는 유저에게 방의 상태를 알려준다.
	packetcoder.SetBuf( cPacket );
	packetcoder.PutChar( RoomNum );
	packetcoder.PutChar( state );
	iSize = packetcoder.SetHeader( ANSWER_NOTIFYROOMSTATE );
	PostTcpSend( pChannel->GetBeginUser(), cPacket, iSize );

	// 채널의 룸에 있는 유저에게 알려준다.
	iNext = pChannel->GetBeginRoom();
	while( iNext > NOTLINKED && iNext < ServerContext.iMaxRoom )
	{
		pRoom = &ServerContext.rm[ iNext ];

		if( pRoom->m_RoomNum != RoomNum )
            PostTcpSend( pRoom->GetUserBegin(), cPacket, iSize );

		iNext = ServerContext.rn[ iNext ].next;
	}

#ifdef _LOGFILELEVEL3_
	ServerUtil.NotifyRoomStateLog( pRoom->m_iChannel, pRoom->m_RoomNum, pRoom->m_iState );
#endif
}


char CGame::GetPrivateCard( CRoom* pRoom )
{
	char card;
	 
	++pRoom->m_iIndexprivatecard;
	
	card = pRoom->m_privatecard[ pRoom->m_iIndexprivatecard ];

	return card;
}

void CGame::BackGameInRoomState( CRoom* pRoom )
{
	LPSOCKETCONTEXT lpSC; 
	int iNext = pRoom->GetUserBegin();

	pRoom->m_iState = GAME_OFF;

	while( iNext >= 0 && iNext < ServerContext.iMaxUserNum )
	{
		lpSC = &ServerContext.sc[ iNext ];
		if( lpSC->idLen == 0 ) break;

		iNext = ServerContext.pn[ iNext ].next;

		if( lpSC->cState != GAME_OFF ) lpSC->cState = GAME_OFF;
	}

#ifdef _LOGFILELEVEL3_
	ServerUtil.GameLog( pRoom->m_iChannel, pRoom->m_RoomNum, strBackGameInRoomState );
#endif

	CheckChangeStateRoom( pRoom );
}