#pragma once
#include "stdafx.h"
#include "Define.h"

class CSession;

///////////////////////////////////////////////////////////////////////////
// Server의 attach module 기본 베이스
// (모듈 타입)
///////////////////////////////////////////////////////////////////////////
class CAttachModule
{
public:
	enum ModulerType 
	{
		Module_ServerMessage,
	};

private:
	ModulerType m_moduleType;  ///< 모듈의 타입

public:
	/// 세션이 종료 되었을 때 처리
	virtual void SessionClose(CSession* session_, int reason_);

	/// 세션이 접속 되었을 때 처리
	virtual void Accept(SESSION_ID id_, CSession* session_);

	/// 세션이 메시지를 받았을 때 처리
	virtual void Recv(SESSION_ID id_, CSession* session_, int dwBytes_);

	/// 세션이 메시지를 보낼 때 처리
	virtual void Send(SESSION_ID id_, CSession* session_, int dwBytes_);

public:
	CAttachModule(ModulerType module_);
	virtual ~CAttachModule();
};