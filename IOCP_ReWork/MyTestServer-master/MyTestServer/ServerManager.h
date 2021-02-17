#pragma once

#include <map>
#include "ServerBase.h"
#include "Singleton.h"

///////////////////////////////////////////////////////////////////////////
// Server Manager Class (하나의 프로세스에 여러개의 서버 인스턴스가 있을 수 있음)
///////////////////////////////////////////////////////////////////////////
class CServerManager : 
	public CSingleton<CServerManager>
{
public:
	typedef std::map<CServerBase::SERVER_TYPE, CServerBase*> MAP_SERVER;

private:
	MAP_SERVER m_mapServer;      ///< 서버 맵
	WSADATA m_wsaData;           ///< wsa 초기화 관련 구조체
	SYSTEM_INFO m_systemInfo;    ///< 시스템 정보 관련 구조체

public:
	/// 초기화
	void Init();

	/// 서버 생성 및 매니저 등록
	bool CreateServer(CServerBase::_StInitInfo const& serverInfo_);

	/// 서버 삭제
	bool RemoveServer(CServerBase::SERVER_TYPE const type_);

	/// 등록된 모든 서버 실행
	bool Start();

	/// 등록된 모든 서버 종료 및 삭제
	void Close();

public:
	CServerManager();
	virtual ~CServerManager();
};