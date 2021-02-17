#pragma once

#include "Timer.h"

CTimer::CTimer()
{
	m_hIocp = INVALID_HANDLE_VALUE;
}

CTimer::~CTimer()
{
}

const bool CTimer::Initialize(HANDLE hiocp)
{
	m_hIocp = hiocp;

	return true;
}

const bool CTimer::AddTimer(ULONGLONG s_time, e_Callbacktype etimerType, CSessionData* psession, WORD wtargetID, WORD wsubtargetID)
{
	m_Lock.Lock();
	{
		m_TimerQueue.push(TimerEvent{ s_time, etimerType, psession, wtargetID, wsubtargetID });
	}
	m_Lock.UnLock();

	return true;
}

VOID CTimer::ProcessTimer()
{
	while (true)
	{
		m_Lock.Lock();
		if (m_TimerQueue.empty() == true) {
			m_Lock.UnLock();
			break;
		}

		if (m_TimerQueue.top().s_time >= GetTickCount64()) {
			m_Lock.UnLock();
			break;
		}

		TimerEvent tevent = m_TimerQueue.top();
		m_TimerQueue.pop();
		m_Lock.UnLock();
		IoContext* poverlapped = new IoContext; // 꼭 동적할당해서 보내야함 보내고 사라지면 메모리 크러쉬나니까...

		poverlapped->m_eType = tevent.type;
		poverlapped->m_Target = tevent.targetID;
		poverlapped->m_SubTarget = tevent.subtargetID;

		PostQueuedCompletionStatus(m_hIocp, 1, reinterpret_cast<ULONG_PTR>(tevent.session), &poverlapped->m_Overlapped);
	}
}
