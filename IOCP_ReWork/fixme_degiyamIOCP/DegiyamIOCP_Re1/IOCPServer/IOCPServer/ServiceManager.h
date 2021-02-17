
#pragma once

#ifndef __SERVICEMAIN_H__
#define __SERVICEMAIN_H__

#include "../../Common/Common.h"
#include "../../Common/LIB_SESSIONPOOL.h"
#include "../../Common/LIB_THREAD.h"
#include "../../Common/LIB_LOGSYSTEM.h"
#include "../../Common/LIB_SERVERSYSTEM.h"
#include "../../Common/LIB_LOCK.h"
#include "../../NetworkLib/LIB_IOCPModule.h"

using namespace COMMONLIB;
using namespace NETWORKLIB;

class ServiceManager
{
public :	// Constructor & Destructor
	ServiceManager();
	~ServiceManager();
public :	// Server On and Off
	const BOOL	StartServer(LPTSTR* argv);
	const BOOL	StopServer();
private :	// Interface of Initialization
	const BOOL	InitialAllData();
	const BOOL	InitialProfile();
	const BOOL	InitialNetwork();
	const BOOL	InitialThread();
	const BOOL	InitialLog();
private :	// Interface of Release
	const BOOL	ReleaseAllData();
	const BOOL	ReleaseLog();
public :	// Log System
	LIB_LOGSYSTEM	m_Log;
private :	// Member Variables of ServiceMain
	LIB_MULTITHREAD		m_WorkerThread;
	LIB_SINGLETHREAD	m_AcceptThread;
	LIB_SINGLETHREAD	m_ControlThread;
	LIB_SINGLETHREAD	m_SendThread;
	LIB_IOCPModule		m_IOCP;
	LIB_SERVERINFO		m_ServerInfo;
	LIB_SESSIONPOOL		m_SessionPool;
	LIB_CRITICALLOCK	m_Lock; // 삭제 예정
	
	HANDLE				m_hStopEvent;
	
private :	// Function of Thread
	static const UINT _AcceptThread(LPVOID lpParam);
	static const UINT _ControlThread(LPVOID lpParam);
	static const UINT _WorkerThread(LPVOID lpParam);
	static const UINT _SendThread(LPVOID lpParam);
	const BOOL	AcceptThread();
	const BOOL	ControlThread();
	const BOOL	WorkerThread();
	const BOOL	SendThread();
private :	// REQ PACKET
	const BOOL	PacketProcess(LIB_SESSIONDATA* pSession);
	const BOOL	RecvCS_AUTH_LOGIN_REQ(LIB_SESSIONDATA* pSession);
private :	// ACK PACKET
	const BOOL	RecvCS_AUTH_LOGIN_ACK(LIB_SESSIONDATA* pSession);
	
};

#endif	// __SERVICEMAIN_H__