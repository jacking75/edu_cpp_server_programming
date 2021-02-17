#include "stdafx.h"
#include "SessionManager.h"
#include "Session.h"
#include "ServerBase.h"
#include "VirtualMemoryPool.hpp"
#include "Singleton.hpp"
#include "Define.h"
#include "ServerTaskSendAll.h"
#include "CacheManager.hpp"


CSession* CSessionManager::CreateSession(SOCKET const& socket)
{
	/// listen Thread에서는 컨테이너에 접근하지 않고 객체만 생성.
	CSession* newSession;
	CSession::_StInit param;
	param.socket = socket;
	param.server = m_pServer;

	if (!GetPoolObject(newSession, &param))
	{
		// 로그
		return NULL;
	}
	
	// 디버그 로그
	return newSession;
}


SESSION_ID CSessionManager::RegistSession(CSession*& __out session_, SESSION_ID sessionid_, std::string const& id_, std::string const& pass_)
{
	SESSION_ID sessionId = INVALID_SESSION_ID;

	/// recv Thread에서 컨테이너 접근 
	if (session_->GetState() != CSession::STATE_INIT)
	{
		// 로그
		return INVALID_SESSION_ID;
	}

	SESSION_MAP::accessor accessor;
	
	CERTIFICATION_MAP::iterator it_certiry = m_mapCertify.find(id_);
	if (it_certiry != m_mapCertify.end())
	{
		sessionId = (*it_certiry).second;
		if (m_mapSession.find(accessor, sessionId))
		{
			/// 캐시 된 데이터 내에서 예전에 방문했던적이 있는 유저 검색
			CSession* prevSession = static_cast<CSession*>(FindCacheObject(sessionId));
			if (prevSession != NULL)
			{
				ExechangeSession(session_, prevSession);
			}
			else
			{
				/// 캐시가 아닌 세션맵의 데이터 참조
				prevSession = accessor->second;
				if (prevSession)
				{
					if (prevSession->IsConnected())
					{
						/// 이미 접속중이라 캐시 중인 데이터에 세션 정보가 없는 경우라면
						ExechangeSession(session_, prevSession);
					}
					else
					{
						// 로그
						/// 세션 맵에는 등록되어 있는데 캐시에 등록되지 않고 접속도 이루어 지지 않은 경우
						BackPoolObject(session_);
						return INVALID_SESSION_ID;
					}
				}
			}
		}
		else
		{
			/// 등록된 세션 아이디가 캐시 타임이 지나 세션 맵에 존재하지 않을 경우
			sessionId = GenerateSerialID();
			(*it_certiry).second = sessionId;
		}
	}
	else 
	{
		/// 서버가 띄워진 이후 최초 접속
		sessionId = GenerateSerialID();
		m_mapCertify.insert(std::pair<std::string, SESSION_ID>(id_, sessionId));
	}

	
	if (!m_mapSession.insert(accessor, sessionId))
	{
		/// 캐싱되어 있던 데이터가 이전에 접속한 유저가 맞는지 비교
		if ((*accessor).second->GetPassword() != pass_)
		{
			return INVALID_SESSION_ID;
		}
	}

	(*accessor).second = session_;
	
	session_->SetSessionID(sessionId);
	session_->SetPassword(pass_);
	session_->SetState(CSession::STATE_REGISTED);
	return sessionId;
}


void CSessionManager::ExechangeSession(CSession*& __out lhs_, CSession* rhs_)
{
	lhs_->MakeClone(rhs_);
	rhs_->CloseSocket();
	BackPoolObject(rhs_);
}


CSession* CSessionManager::FindSession(SESSION_ID sessionID_)
{
	SESSION_MAP::accessor accessor;
	if (m_mapSession.find(accessor, sessionID_))
	{
		// 로그
		return accessor->second;
	}
	return NULL;
}


void CSessionManager::DestroySession(SESSION_ID sessionID_)
{
	SESSION_MAP::accessor accessor;
	if (!m_mapSession.find(accessor, sessionID_))
	{
		// 로그
		return;
	}
	BackPoolObject(accessor->second);
	m_mapSession.erase(accessor);
}


void CSessionManager::SendAll(packetdef::PacketID id_, protobuf::Message* message_)
{
	CServerTaskSendAll* task = ALLOCATE_POOL_MALLOC(CServerTaskSendAll, sizeof(CServerTaskSendAll));
	new(task) CServerTaskSendAll(m_pServer->GetRecvIOPortHandler().size(), id_, message_);
	m_pServer->PushTask(task);
}


void CSessionManager::ReleaseCache(CCacheObject* obj_)
{
	CSession* session = static_cast<CSession*>(obj_);
	obj_->SetCacheWaitState(false);

	DestroySession(session->GetSessionID());
}


CSessionManager::CSessionManager()
{
}

CSessionManager::CSessionManager(int poolSize_, int poolInterval_, CServerBase* server_) :
	CVirtualMemmoryPool<CSession>(poolSize_, poolInterval_),
	m_pServer(server_),
	m_serialGen(100000)
{
}

CSessionManager::~CSessionManager()
{
}
