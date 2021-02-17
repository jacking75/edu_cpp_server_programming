#pragma once
#include "AttachModule.h"

class CSession;


///////////////////////////////////////////////////////////////////////////
// Server의 attach module - 메시지 출력
///////////////////////////////////////////////////////////////////////////
class CAttachServerMessage :
	public CAttachModule
{
public:
	/// 세션이 종료 되었을 때 처리
	virtual void SessionClose(CSession* session_, int reason_) override;

	/// 세션이 접속 되었을 때 처리
	virtual void Accept(SESSION_ID id_, CSession* session_) override;

	/// 세션이 메시지를 받았을 때 처리
	virtual void Recv(SESSION_ID id_, CSession* session_, int dwBytes_) override;

	/// 세션이 메시지를 보낼 때 처리
	virtual void Send(SESSION_ID id_, CSession* session_, int dwBytes_) override;

public:
	CAttachServerMessage();
	virtual ~CAttachServerMessage();
};