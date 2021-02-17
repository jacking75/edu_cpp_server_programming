#pragma once

#ifndef __STRUCT_H__
#define __STRUCT_H__

//#include "Protocol.h"
enum e_OBJECT_TYPE;

// 게임에 필요한 게임 진행 데이터
typedef struct stGameInfo {
	float m_fElsapedTime;

	// 몇 라운드
	int m_nRound;

	// 각 몇 라운드 이겼는지
	int m_nUpWinCount;
	int m_nDownWinCount;

	// Main Base Info
	int m_nUpPlayerCount;
	int m_nDownPlayerCount;
	//bool m_bIsOccupied = false;
	float m_fUpTeamPercent;
	bool m_bIsMBUpOccupying = false;
	bool m_bIsMBUpOccupied = false;
	float m_fMBUpTime;
	float m_fDownTeamPercent;
	bool m_bIsMBDownOccupying = false;
	bool m_bIsMBDownOccupied = false;
	float m_fMBDownTime;
	bool m_bIsEngaging = false;

	bool m_IsSmog[SMOG_MAX_X_INDEX][SMOG_MAX_Y_INDEX][SMOG_MAX_Z_INDEX];
} GameInfo, GAMEINFO;

typedef struct stCharacterInfo {
	int		m_nGameAmount;
	int		m_nGameVictory;
	int		m_nGameDefeat;
}CharacterInfo, CHARACTERINFO;

// 게임에 필요한 플레이어 데이터
typedef struct stGamePlayerInfo {
	// 세션별 아이디 정보
	WORD				m_wID = NOTREGISTED;
	// 체력 정보
	FLOAT				m_fLife;
	// 위치 정보
	FLOAT				m_fX = 0.f;
	FLOAT				m_fY = 0.f;
	FLOAT				m_fZ = 0.f;
	// 회전 정보
	FLOAT				m_fYaw = 0.f;
	FLOAT				m_fPitch = 0.f;
	FLOAT				m_fRoll = 0.f;
	// 속도 정보
	FLOAT				m_fVX = 0.f;
	FLOAT				m_fVY = 0.f;
	FLOAT				m_fVZ = 0.f;
	// 팀 정보
	bool				m_bIsDown = true;
	// 생사 정보
	bool				m_bIsLive = true;
	// 애니메이션을 위한 정보
	bool				m_bIsUp;
	bool				m_bIsFly;
	bool				m_bIsShoot;
	bool				m_bIsReloading;
	bool				m_bIsGround;
	bool				m_bIsRun;
	bool				m_bIsFalling;
	bool				m_bIsChangingWeapon;
	float				m_fSpeed;
	float				m_fDirection;
	float				m_fRotatorYaw;
	float				m_fRotatorPitch;
	float				m_fRotatorRoll;

	bool				m_bIsInMainBase;

	BYTE				m_cInGameID;

	BYTE				m_cWeaponNum;

	BYTE				m_cMachinegunAmmo;
	BYTE				m_cRocketgunAmmo;
} GamePlayerInfo, GAMEPLAYERINFO;
typedef std::map<WORD, GamePlayerInfo> mapPlayersInfo;

// 게임에 필요한 오브젝트 데이터
typedef struct stGameObjectInfo {
	WORD				m_wID = NOTREGISTED;
	// 누구 소유인지
	WORD				m_wWhoID = NOTREGISTED;
	// 팀 정보
	bool				m_bIsDown = true;
	// 타입 정보
	e_OBJECT_TYPE		m_eType = e_OBJECT_TYPE::e_Rocketgun_Bullet;
	// 위치 정보
	FLOAT				m_fX = 0.f;
	FLOAT				m_fY = 0.f;
	FLOAT				m_fZ = 0.f;
	// 회전 정보
	FLOAT				m_fYaw = 0.f;
	FLOAT				m_fPitch = 0.f;
	FLOAT				m_fRoll = 0.f;
	// 속도 정보
	FLOAT				m_fVX = 0.f;
	FLOAT				m_fVY = 0.f;
	FLOAT				m_fVZ = 0.f;
} GameObjectInfo, GAMEOBJECTINFO;

typedef std::map<WORD, GameObjectInfo> mapObjectsInfo;

#endif // !__STRUCT_H__