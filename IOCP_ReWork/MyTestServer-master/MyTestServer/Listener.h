#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>

#include "string"
#include "list"
#include "SessionManager.h"
#include "ThreadRegister.h"


class CAttachModule;

///////////////////////////////////////////////////////////
// Listner Class
// (session manager)
///////////////////////////////////////////////////////////
class CListener : 
	public CIOCPThreadRegister<CListener>
{
private:
	typedef std::list<CAttachModule*> ATTACH_MODULE_LIST;

	ATTACH_MODULE_LIST m_moduleList;   ///< 모듈들을 관리하는 리스트
	CSessionManagerBase* m_sessionMgr; ///< 해당 serverBase에 등록되어 있는 sessionManager
	SOCKET m_listenSocket;             ///< Listen 소켓

public: 
	/// Listner Thread 등록 및 시작
	SOCKET Start(HANDLE hCompletionPort_, u_short nPortNum_);

	/// 접속, 세션종료, 보내기, 받기 등이 일어났을 때에 대한 모듈을 등록
	bool AttachModule(CAttachModule* module_);

	/// Thread 종료
	void Close();

	/// Accept가 일어났을 때에 대한 프로시져 
	static unsigned CALLBACK AcceptProc(void* data_);
	
public:
	CListener(CSessionManager* sessionMgr);
	virtual ~CListener();
};
