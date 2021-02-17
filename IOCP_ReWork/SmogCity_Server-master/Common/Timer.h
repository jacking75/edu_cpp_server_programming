#pragma once

#ifndef __TIMER_H__
#define __TIMER_H__

#include "Common.h"
#include "Protocol.h"
#include "SessionData.h"
#include "Lock.h"

#define NOW GetTickCount64() 

constexpr UINT ONE_SECOND					= 1000;
constexpr UINT TIMER_UPDATE_GAME			= 100;
constexpr UINT TIMER_RESPAWN_PLAYER			= 5000;
constexpr UINT TIMER_RESPAWN_HEALPACK		= 10000;
constexpr UINT TIMER_ROUND_START			= 3000;
constexpr UINT TIMER_SWAP_WEAPON			= 500;
constexpr UINT TIMER_RELOAD_MACHINEGUN		= 1000;
constexpr UINT TIMER_RELOAD_ROCKETGUN		= 2000;
constexpr FLOAT REALTIME_UPDATE_CYCLE		= 0.1f;

// e_CallType에 합쳐서쓰자
//enum e_Timer_Type {
//	e_Timer_Start,
//	e_Timer_Update
//};

typedef struct stTimerEvent {
	ULONGLONG		s_time;
	e_Callbacktype	type;
	CSessionData*	session;
	WORD			targetID;
	WORD			subtargetID;
} TimerEvent, TIMEREVENT;

struct TimeComparison
{
	//bool reverse = false;

	bool operator() (const TimerEvent& lhs, const TimerEvent& rhs) const
	{
		return (lhs.s_time > rhs.s_time);
	}
};

typedef std::priority_queue <TimerEvent, std::vector<TimerEvent>, TimeComparison> TimerQueue;

// 타이머 클래스에서는 시간단위로 이벤트생성만 해주고 처리는 워커스레드에서 하자
class CTimer
{
public:
	CTimer();
	~CTimer();
private:
	HANDLE			m_hIocp;
	// 타이머이벤트들을 관리하는 우선순위 큐
	TimerQueue		m_TimerQueue;
	CMutex			m_Lock;
public:
	const bool		Initialize(HANDLE hiocp);

	// 우선순위큐에 넣는 함수
	const bool		AddTimer(ULONGLONG s_time, e_Callbacktype etimerType, CSessionData* psession, WORD wtargetID = NOTREGISTED, WORD wsubtargetID = NOTREGISTED);
	VOID			ProcessTimer();
};

#endif // !__TIMER_H__
