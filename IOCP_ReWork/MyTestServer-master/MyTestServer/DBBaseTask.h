#pragma once
#include "stdafx.h"
#include "Define.h"
#include "msado15.tlh"

struct SConnectionInfo
{
	_ConnectionPtr connection_ptr;
	_RecordsetPtr record_ptr;
	_CommandPtr command_ptr;
};

///////////////////////////////////////////////////////////
// DB Task처리에 대한 Base 클래스
///////////////////////////////////////////////////////////
class CDBBaseTask
{
protected:
	_ConnectionPtr m_connection_ptr;
	_RecordsetPtr m_record_ptr;
	_CommandPtr m_command_ptr;

	USER_ID m_userID;                   ///< userID
	THREAD_REGION m_threadRegion;       ///< DBTask가 수행되는 Thread의 지역 번호

public:
	/// 초기화 함수
	virtual void Init(SConnectionInfo* connInfo_);

	/// 수행
	virtual bool Execute();

	/// 로그기록
	virtual void WriteLog();

	/// 해당 task를 호출한 userID를 가지고 온다
	inline USER_ID GetCallerUserID() const { return m_userID; }

	/// DBTask가 수행되는 Thread의 지역번호를 가지고 온다
	inline THREAD_REGION GetThreadRegion() const { return m_threadRegion; }

	/// task를 호출하는 User의 userID를 세팅
	inline void SetCallerUserID(USER_ID userID_) { m_userID = userID_; }

	/// DBTask가 수행될 Thread의 지역번호 세팅
	inline void SetThreadRegion(THREAD_REGION region_) { m_threadRegion = region_; }

public:
	CDBBaseTask();
	virtual ~CDBBaseTask();
};