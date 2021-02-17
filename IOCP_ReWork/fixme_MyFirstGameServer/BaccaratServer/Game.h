// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

// 바카라 게임 룰 http://nellycw.tistory.com/324
#if !defined(AFX_GAME_H__0219C91D_3AA7_4BD5_A031_6AF80F7A5F24__INCLUDED_)
#define AFX_GAME_H__0219C91D_3AA7_4BD5_A031_6AF80F7A5F24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const char strBackGameInRoomState[] = "BackGameInRoomState()";
const char strRequestBeginGame[] = "OnRequestBeginGame()";
const char strAgencyRequestBeginGame[] = "AgencyRequestBeginGame()";
const char strSendStartCards[] = "SendStartCards()";
const char strResetUserState[] = "ResetUserState()";
const char strSendGameOff[] = "SendGameOff";




class CGame  
{
	
public:
	void BeginGameInRoom( CRoom* pRoom );
	void BackGameInRoomState( CRoom* pRoom );

	char GetPrivateCard( CRoom* pRoom );
	
	CGame();
	virtual ~CGame();

private:
	void AheadPacketed();
	char ReservateExitRoomPacket[4];
	char NotMoneyPacket[5];

	void MixCard( CRoom* pRoom, int MixCount );
	void SendStartCards( CRoom* pRoom );
	void SendGameOff( CRoom* pRoom );


public:
	void AgencyRequestBeginGame( CRoom* pRoom );
	void ResetUserState( CRoom* pRoom );
	void NotifyGameResult( CRoom* pRoom );
	
	BOOL CanBankerReceiveThreeCard( char Playerthreecard, char BankerScore );
	BOOL CanPlayerReceiveThreedCard( char PlayerScore );
	
	void NotifyRoomState( int iChannelIndex, char RoomNum, char state );
	
	friend int OnRequestBeginGame( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	
	
};

void InitializeGameProc();
#endif // !defined(AFX_GAME_H__0219C91D_3AA7_4BD5_A031_6AF80F7A5F24__INCLUDED_)
