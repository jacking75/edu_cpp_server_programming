
#pragma once

#ifndef __COMMON_H__
#define __COMMON_H__

#pragma comment(lib, "ws2_32")

#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <map>
#include <thread>
#include <string>
#include <queue>
#include <ppl.h>
#include <array>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>

/* DEFINE NUMBER */
constexpr int MAX_THREAD = 100;
constexpr int MAX_BUFFER = 100;
constexpr int NUM_THREAD = 14;		// Worker Thread ÀÇ °³¼ö 
									// 16 - Accept(1) - Timer(1)

#ifdef _DEBUG
constexpr int SESSION_NUM = 100;
#else
constexpr int SESSION_NUM = 10000;
#endif	// _DEBUG

/* Game Info */
constexpr int ROUND_AMOUNT_FOR_WIN				= 2;
constexpr float PLAYER_MAX_LIFE					= 250.f;
constexpr float DAMAGE_ROCKETGUN_BULLET			= PLAYER_MAX_LIFE / 5.f;
constexpr float DAMAGE_MACHINEGUN_BULLET		= PLAYER_MAX_LIFE / 10.f;
constexpr float HEALPACK_L_HEAL_AMOUNT	= PLAYER_MAX_LIFE / 4.f;
constexpr float HEALPACK_M_HEAL_AMOUNT	= PLAYER_MAX_LIFE / 5.f;
constexpr int MAX_MACHINEGUN_AMMO = 30;
constexpr int MAX_ROCKETGUN_AMMO = 5;

constexpr float SMOG_PERCENT_FOR_WIN	= 75.f;

constexpr int HEALPACK_NUM = 10;//25;

constexpr int BASIC_OBJECT_NUM = HEALPACK_NUM;



enum e_Callbacktype {
	e_Recv,
	e_Send,
	e_Timer_Start,
	e_Timer_Update,
	e_Timer_Respawn_Player,
	e_Timer_Respawn_Object,
	e_Timer_Round_Start,
	e_Timer_Swap_Weapon,
	e_Timer_Reload
};

typedef struct stIoContext {
	WSAOVERLAPPED	m_Overlapped;
	WSABUF			m_Wsabuf;
	CHAR			m_cBuf[MAX_BUFFER];
	e_Callbacktype	m_eType;
	INT				m_Target;
	INT				m_SubTarget;
} IoContext, IOCONTEXT;

#endif //  __COMMON_H__