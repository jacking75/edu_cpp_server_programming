#pragma once
#include <string>
#include "DBBaseTask.h"
#include "ThreadRegister.h"

///////////////////////////////////////////////////////////
// DB처리 클래스
// <Derive class, DBTask>
///////////////////////////////////////////////////////////
class CAdoOleDB :
	public CTaskThreadRegister<CAdoOleDB, CDBBaseTask>
{
public:
	enum DB_TYPE
	{
		ACCOUNT_DB,
		GAME_DB,
		LOG_DB,
	};

	struct _StInitInfo
	{
		tstring userID;             ///< userID
		tstring pass;			    ///< password
		tstring ipAddress;          ///< 접속주소
		tstring dbName;             ///< DB 스키마 이름
		DWORD port;                 ///< 포트
		DB_TYPE dbType;             ///< DB 타입
		int nSessionThreadCount;    ///< 해당 주소에 대한 접속 세션 수
	};
	typedef std::vector<SConnectionInfo*> VEC_CONNECTION_INFO;

private:
	VEC_CONNECTION_INFO m_vecConnInfo;                                 ///< DB접속 정보 백터
	_StInitInfo m_dbInfo;                                              ///< DB정보
	bool m_bInit;                                                      ///< 초기화 유무

public:
	/// 초기화 함수
	void Init(_StInitInfo const& info_);

	/// theread 시작
	bool Start();

	/// 접속 정보 해제
	void ReleaseDB();

	/// DBTask 추가
	void PushTask(CDBBaseTask* task_);

	/// 모든 DBTask에 대한 flush 처리
	virtual void FlushAllMessage() override;

	/// 메시지 처리 쓰레드
	static unsigned CALLBACK MessageProc(void* data_);

public:
	CAdoOleDB();
	virtual ~CAdoOleDB();
};