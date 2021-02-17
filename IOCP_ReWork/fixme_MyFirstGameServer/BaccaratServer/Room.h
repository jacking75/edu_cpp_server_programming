// Room.h: interface for the CRoom class.
//
//////////////////////////////////////////////////////////////////////

#ifndef	_ROOM_H_
#define _ROOM_H_


#include "ServerUtil.h"

const char strCheckUserLinkInRoom[] = "CheckUserLinkInRoom() - Not UserInRoom";


class CRoom  
{

private:
	
	int		m_iIndex;
	char	m_nLevel;
	char	m_iType;
	char	m_cTitleLen, m_cTitle[32], m_cOwnerLen, m_cOwner[16], m_cPWDLen, m_cPWD[32];

	int		m_iOwnerIndex; 
	
	int		m_iUserBegin, m_iUserEnd;
	char	m_iUserNum;

	char	bUsedOrder[MAXPLAYER];			// 현재 방에 있는 사람이 사용한 오더 번호가 저장.

public:
	char	m_iChannel;						// 이 룸이 속한 채널 번호.
	char	m_RoomNum;						// 채널 속에서의 방 번호.
	char	m_iState;
	time_t	m_tRoomTime;					// 방에 기록된 시간.
	int		m_Basebettingmoney[5];				// 배팅 금액.

	SELECTAREAINFO BankerInfo;					// 뱅커를 선택한 유자 수, 유저인덱스, 카드, 점수
	SELECTAREAINFO PlayerInfo;					// 플레이어를 선택한 유자 수, 유저인덱스, 카드, 점수
	SELECTAREAINFO TieInfo;						// 타이를 선택한 유자 수, 유저인덱스, 카드, 점수

	char	m_nStartCompleteUserNum;			//  게임 준비가 끝난 플레이어의 수.
	
	char	m_privatecard[ TOTALMAXCARDNUM ];	//  게임에서 뒤로 덮혀 있는 카드(딜러 카드).
	int		m_iIndexprivatecard;				//  딜러의 비 공개 카드 인덱스
	BOOL	m_bNoExitprivatecard;				//  TRUE : 딜러의 비 공개 카드가 없음.
	
public:
	
	//! 방의 유저들의 링크가 제대로 되어 있는지 확인 후 문제가 있으면 수정한다.
	void CheckUserLinkInRoom()
	{
		LPSOCKETCONTEXT lpSC;
		int iNext = m_iUserBegin;

		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];
			iNext = ServerContext.pn[ iNext ].next;

			if( lpSC->iRoom != m_iIndex || lpSC->idLen <= 0 )
			{
#ifdef _LOGFILELEVEL3_
				ServerUtil.RoomLog( m_iChannel, m_RoomNum, strCheckUserLinkInRoom );
#endif
				KillUserLink( lpSC->index );
			}
		}
	}
	//! 방의 룸 오더를 초기하 한다.
	void InitRoomOrder()
	{
		for( int i = 0; i < MAXPLAYER; ++i )
		{
			bUsedOrder[ i ] = NOTLINKED;
		}
	}

	//! 새로운 유저가 방에 들어오면 오더 번호를 부여한다.
	char GetOrderInRoom()
	{
		char i;

		for( i = 0; i < MAXPLAYER; ++i )
		{
			if( bUsedOrder[ i ] == NOTLINKED )
			{
				bUsedOrder[ i ] = 1;
				return i;
			}	
		}

		return i;
	}

	//! 방의 오더를 반납한다.
	void DeleteOrderInRoom( char i )
	{
		bUsedOrder[ i ] = NOTLINKED;
	}

	//! DBIndex를 이용하여 클라이언트 고유의 인덱스를 얻어온다.
	int GetDBIndexToSockIndex(int iDBIndex)
	{
		LPSOCKETCONTEXT lpSC;
		int				iNext;

		iNext = m_iUserBegin;
		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];
			if( lpSC->iUserDBIndex == iDBIndex )
			{
				return lpSC->index;
			}
			iNext = ServerContext.pn[ iNext ].next;
		}

		return NOTLINKED;
	}


	int GetUserBegin()
	{
		return m_iUserBegin;
	}


	int GetRoomIndex()
	{
		return m_iIndex;
	}


	char GetRoomType()
	{
		return m_iType;
	}


	char GetCurUserNum()
	{
		return m_iUserNum;
	}


	int GetOwnerIndex()
	{
		return m_iOwnerIndex;
	}


	CRoom();
	virtual ~CRoom();

	void InitRoom( int idx, char nlevel );
	void KillUserLink( int idx );
	void SetUserLink( int idx );
	
	
	friend int OnRequestCreateRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend void AnswerCreateRoom( LPSOCKETCONTEXT lpSockContext, CChannel* pChannel, CRoom* pRoom, ROOMINFO& pRoomInfo );
	friend void NotifyCreateRoom( LPSOCKETCONTEXT lpSockContext, CRoom* pRoom, char cRoomNum, 
										int iBeginUser, int iUsedRoomBegin );
	
	friend int OnRequestExitRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int ExitNotifyRoomDelete( CChannel* pChannel, CRoom* pRoom, char& cPacket );
	friend int ExitNotiftRoomSetInfo( LPSOCKETCONTEXT lpSockContext, CChannel* pChannel, CRoom* pRoom, char& cPacket );
	friend void ExitRoomNotifyUserSetInfo( LPSOCKETCONTEXT lpSockContext, char& cPacket, int iSize, 
											int iUserBegin, int iRoomBegin );
	friend void RemainUserInRoom( CRoom* pRoom, LPSOCKETCONTEXT lpSockContext );
	

	friend int OnRequestJoinRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend BOOL AnswerJoinRoom( LPSOCKETCONTEXT lpSockContext, CRoom* pRoom, char cRoomNum, char* cPWD, char cPWDLen );
	friend void NotifyJoinRoomSetInfo( LPSOCKETCONTEXT lpSockContext, CRoom* pRoom, char cRoomNum, 
										int iUserBegin, int iUsedRoomBegin );
	
	
	friend int OnRequestChangeRoomTitle( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestChangeRoomType( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	
	friend int OnRequestChatInRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	

	friend int OnRequestInvite( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestInvite_Result( LPSOCKETCONTEXT lpSockContext, char* cpPacket );


	friend int OnNotifyRoomList( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnNotifyUserList( LPSOCKETCONTEXT lpSockContext, char* cpPacket );


	friend void CheckChangeStateRoom( CRoom* pRoom );

	// CRoom class의 멤버들을 접근하기 위해 선언된 함수. 정의는 다른 곳에서 하고 있다.
	friend int OnRequestRoomPWD_Result( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int LogOutProcess( LPSOCKETCONTEXT lpSockContext );

	
	void UserInRoom_Regulation(void);
};

int InitRoomLayer();

#endif // !defined(AFX_ROOM_H__3BF27723_7098_4778_A07E_96D3AAF17DB8__INCLUDED_)
