// Channel.h: interface for the CChannel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "define.h"
#include "room.h"

class CChannel  
{
private:
	/*OBJECTNODE	*m_pn;
	int			m_iTUserBegin, m_iTUserEnd, m_iTUserNum;*/

	int			m_iIndex, m_iBaseRoomIndex;
	int			m_iUserBegin, m_iUserEnd, m_iUserNum;
	int			m_iUsedRoomBegin, m_iUsedRoomEnd, m_iUsedRoomNum;
	int			m_iUnusedRoomBegin, m_iUnusedRoomEnd, m_iUnusedRoomNum;
	char		m_nLevel;

private:
	inline void DeleteUnusedRoomList( int idx );
	inline void AddUsedRommList( int idx );
	inline void DeleteRoomLink( int idx );

public:
	
	int GetBeginRoom()
	{
		return m_iUsedRoomBegin;
	}
	
	int GetBeginUser()
	{
		return m_iUserBegin;
	}
	
	inline void InitChannel( int idx );
	void KillUserLink( int idx );
	void SetRoomLink( int idx );
	void KillRoomLink( int idx );
	void SetUserLink( int idx );
	
	
	//! DBIndex를 이용하여 클라이언트의 인덱스 값을 얻어온다.
	int GetDBIndexToSockIndex( int iDBIndex )
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

	//! 아이디를 이용하여 클라이언트의 인덱스 값을 얻어온다.
	int GetIdToSockIndex(char cIdLen, char *cId)
	{
		LPSOCKETCONTEXT lpSC;
		int				iNext;

		iNext = m_iUserBegin;
		while( iNext > NOTLINKED && iNext < ServerContext.iMaxUserNum )
		{
			lpSC = &ServerContext.sc[ iNext ];
			if( cIdLen == lpSC->idLen)
			{
				if( strncmp( cId, lpSC->cID, cIdLen ) == 0 )
				{
					return lpSC->index;
				}
			}

			iNext = ServerContext.pn[ iNext ].next;
		}

		return NOTLINKED;
	}


	short GetCurUserNum()
	{
		CRoom			*pRoom;
		int				iRoomNext;
		int				iRoomInUserNum = 0;

		iRoomNext = m_iUsedRoomBegin;
		while( iRoomNext > NOTLINKED && iRoomNext < ServerContext.iMaxRoom )
		{
			pRoom = &ServerContext.rm[ iRoomNext ];
			iRoomInUserNum += pRoom->GetCurUserNum();
			iRoomNext = ServerContext.rn[ iRoomNext ].next;
		}

		return static_cast<short>( m_iUserNum + iRoomInUserNum );
	}

	char GetLevel()
	{
		return m_nLevel;
	}
	
	
	friend int OnRequestJoinChannel( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestJoinChannelInt( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestChangeChannel( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	friend int OnRequestAllInviteReject( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestAllChatReject( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	friend int OnRequestChatInLobby( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestWhisper( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	
	friend int OnRequestRoomPWD( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestRoomPWD_Result( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	friend int OnRequest1ON1Chat_Demand( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequest1ON1Chat_Result( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequest1ON1Chat( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequest1ON1Chat_Close( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	
	friend int OnNotifyRoomList( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnNotifyUserList( LPSOCKETCONTEXT lpSockContext, char* cpPacket );

	friend int OnRequestReLoadItem( LPSOCKETCONTEXT lpSockContext, char* cpPacket );	


	// CChannel class의 멤버들을 접근하기 위해 선언된 함수. 정의는 다른 곳에서 하고 있다.
	friend int OnRequestCreateRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend void AnswerCreateRoom( LPSOCKETCONTEXT lpSockContext, CChannel* pChannel, CRoom* pRoom, ROOMINFO& pRoomInfo );
	friend int OnRequestJoinRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestExitRoom( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int ExitNotifyRoomDelete( CChannel* pChannel, CRoom* pRoom, char& cPacket );
	friend int ExitNotiftRoomSetInfo( LPSOCKETCONTEXT lpSockContext, CChannel* pChannel, CRoom* pRoom, char& cPacket );
	friend int LogOutProcess( LPSOCKETCONTEXT lpSockContext );

	CChannel();
	virtual ~CChannel();
};

int InitChannelLayer();

#endif // !defined(AFX_CHANNEL_H__A8D34EB0_F17E_47D7_8DCA_9C675D801BEF__INCLUDED_)
