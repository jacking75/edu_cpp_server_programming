
#pragma once

#ifndef __IOCPMODEULE_H__
#define __IOCPMODEULE_H__

#include "../Common/Common.h"
#include "../Common/SessionData.h"

class CIOCPModule
{
public:
	CIOCPModule();
	~CIOCPModule();
private:
	HANDLE m_CompletionPort;
public:
	const bool			StartIOCP();
	const bool			RegisterClient(SOCKET clientSocket, CSessionData* pplayer);	// 템플릿 가능할까?
	inline const HANDLE	GetCompletionPort() {
		return m_CompletionPort;
	}
};

#endif //  __IOCPMODEULE_H__
