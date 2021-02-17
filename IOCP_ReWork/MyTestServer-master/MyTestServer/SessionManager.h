#pragma once

#include <winsock2.h>
#include <map>
#include <string>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_hash_map.h>
#include "CacheManager.h"
#include "Session.h"
#include "VirtualMemoryPool.h"
#include "Singleton.h"
#include "SerialGenerator.h"
#include "ServerCore/SessionManagerBase.h"


class CSreverBase;

///////////////////////////////////////////////////////////////////////////
// Session Manager Class
// (session에 대한 시작 메모리풀 사이즈, 풀이 늘어나는 사이즈, 종속 서버)
///////////////////////////////////////////////////////////////////////////
class CSessionManager : 
	public CSessionManagerBase,
	public CVirtualMemmoryPool<CSession>,
	public CCacheManager<CSession>
{
public:

	typedef tbb::concurrent_hash_map<SESSION_ID, CSession*> SESSION_MAP;          
	typedef tbb::concurrent_unordered_map<std::string, SESSION_ID> CERTIFICATION_MAP;
	
private:
	SESSION_MAP m_mapSession;       ///< 세션 맵
	CERTIFICATION_MAP m_mapCertify; ///< 인증관련 맵

	CServerBase* m_pServer;      ///< 종속하는 서버
	CSerialGenerator m_serialGen;    ///< 시리얼 생성기

private:
	/// 세션 정보를 바꿔치기 한다(캐시된 세션과 방금 접속한 세션의 정보)
	void ExechangeSession(CSession*& __out lhs_, CSession* rhs_);

public:
	inline SESSION_ID GenerateSerialID() { return m_serialGen.GenerateSerialID(); }

	/// 세션을 만든다
	virtual CSession* CreateSession(SOCKET const& socket_);  

	/// 세션을 매니저에 등록한다
	/// (recv Thread에서 수행해야 session 접근이 안전해진다)
	SESSION_ID RegistSession(CSession*& __out session_, SESSION_ID sessionid_, std::string const& id_, std::string const& pass_);

	/// sessionID에 맞는 세션을 찾는다
	CSession* FindSession(SESSION_ID sessionID_);

	/// 세션을 파괴한다
	void DestroySession(SESSION_ID sessionID_);

	/// 모든 세션에게 메시지를 보낸다
	void SendAll(packetdef::PacketID id_, protobuf::Message* message_);

	/// 캐시 상태에서 삭제한다
	virtual void ReleaseCache(CCacheObject* obj_) override;

public:
	CSessionManager();
	CSessionManager(int poolSize_, int poolInterval_, CServerBase* server_);
	virtual ~CSessionManager();
};