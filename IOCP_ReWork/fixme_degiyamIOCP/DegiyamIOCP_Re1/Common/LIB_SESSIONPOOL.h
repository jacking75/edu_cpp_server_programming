
#pragma once

#ifndef __SESSIONPOOL_H__
#define __SESSIONPOOL_H__

#include "Common.h"
#include "LIB_SESSIONDATA.h"
#include "LIB_LOCK.h"

namespace COMMONLIB
{
	// 템플릿으로 수정할 것
	class LIB_SESSIONPOOL
	{
	public :
		LIB_SESSIONPOOL();
		~LIB_SESSIONPOOL();
	private :
		LIB_CRITICALLOCK	m_Lock;
		LIB_SESSIONDATA*	m_SessionBuffer[SESSION_NUM];
		PLAYERMAP		m_mapPlayer;
	public :
		LIB_SESSIONDATA*	CreateSession();
		/*const LIB_SESSIONDATA*	FindSession(WORD wSession);
		const LIB_SESSIONDATA*	FindSession(LPCTSTR szName);
		const LIB_SESSIONDATA*	FindSessionID(WORD wID);*/
		LIB_SESSIONDATA*	FindSession(const int index);
		const VOID			InsertSession(LIB_SESSIONDATA* pSession);
		const WORD			GetSessionSize();
		const VOID			DeleteSession(LIB_SESSIONDATA* pSession);
		// GETSESSIONID
	};
}

#endif	// __SESSIONPOOL_H__