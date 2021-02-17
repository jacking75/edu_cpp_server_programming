#include "stdafx.h"
#include "Session.h"
#include "Sender.h"
#include "ServerBase.h"
#include "ATLComTime.h"
#include "VirtualMemoryPool.hpp"
#include "PCManager.h"
#include "ZoneManager.h"
#ifdef __TBB_malloc_proxy
#include "tbb/tbbmalloc_proxy.h"
#else
#include "tcmalloc.h"
#endif
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


using namespace packetdef;
using namespace google;



void CSession::SetSessionID(SESSION_ID sessionId_)
{
	m_sessionID = sessionId_;
}


void CSession::SetPassword(std::string const& pass_)
{
	m_pass = pass_;
}


USER_ID  CSession::GetUserID() const
{
	if (m_pActor && m_pActor->IsValid())
	{
		CPC* pc = static_cast<CPC*>(m_pActor);
		if (pc)
		{
			return pc->GetUserID();
		}
	}

	return INVALID_USER_ID;
}


SERIAL_ID CSession::GetActorID() const
{
	if (m_pActor && m_pActor->IsValid())
	{
		return m_pActor->GetActorID();
	}

	return INVALID_SERIAL_ID;
}


ZONE_ID CSession::GetZoneID() const
{
	if (m_pActor && m_pActor->IsValid())
	{
		return m_pActor->GetZoneID();
	}

	return INIT_ZONE_ID;
}


bool CSession::Send(packetdef::PacketID id_, protobuf::Message* message_)
{
	if (!m_bIsConnected)
		return false;

	if (message_ == NULL)
	{
		// 로그
		return false;
	}

	int nSize = sizeof(PacketHeader) + message_->ByteSize();
	if (nSize > PACKET_SIZE)
	{
		// 로그
		return false;
	}

	/// 메모리 풀에서 할당
	SPacketTransInfo* packetInfo;
	SPacketTransInfo::_StInit param(id_, message_->ByteSize(), this);

	if (!GetPoolObject(packetInfo, &param))
	{
		// 로그
		return false;
	}

	protobuf::io::ArrayOutputStream output_array_stream(packetInfo->message, nSize);
	protobuf::io::CodedOutputStream os(&output_array_stream);

	packetdef::PacketHeader packetHeader(id_, message_->ByteSize());
	os.WriteRaw(&packetHeader, sizeof(PacketHeader));
	message_->SerializePartialToCodedStream(&os);

	SendInfo->queue.push(packetInfo);

	return true;
}


bool CSession::SendImmediately(packetdef::PacketID id_, protobuf::Message* message_)
{
	if (Send(id_, message_))
	{
		FlushAllSend();
		return true;
	}
	return false;
}


bool CSession::BroadCast(SPacketTransBroadCastInfo* packetInfo_, bool bIsImmediatly_)
{
	if (packetInfo_ == NULL)
	{
		// 로그
		return false;
	}

	if (!m_bIsConnected)
	{
		// 로그
		packetInfo_->BackPoolObject();
		return false;
	}

	SendInfo->queue.push(packetInfo_);

	if (bIsImmediatly_)
	{
		FlushAllSend();
	}
	return true;
}


void CSession::FlushAllSend()
{
	BOOL expected = TRUE;
//	if (SendInfo->available.compare_exchange_strong(expected, FALSE))
	{
		//if (!SendInfo->queue.empty())
		//{
		//	printf("!!!\n");
		//}

		 m_pServer->GetSenderRef().Push(this);
	}

	/*
	do
	{
		BOOL expected = SendInfo->available;
		if (expected == TRUE)
		{
			m_pServer->GetSenderRef().Push(this);
			return;
		}
	} while (SendInfo->available.compare_and_swap(FALSE, TRUE) == TRUE);
	*/
}


void CSession::SetThreadRegion(int criteria_)
{
	m_threadRegion = criteria_;
}


void CSession::SetState(E_STATE const state_)
{
	m_state = state_;
	switch (m_state)
	{
	case STATE_INIT:
	case STATE_REGISTED:
		m_bIsConnected = true;
		break;

	default:
		m_bIsConnected = false;

	}
}


bool CSession::Leave(void* initParam_)
{
	if (initParam_ == NULL)
		return false;

	Reset(static_cast<CSession::_StInit*>(initParam_));
	m_pActor = NULL;
	SetState(STATE_INIT);
	m_createTimer = COleDateTime::GetCurrentTime().GetTickCount();
	return true;
}


bool CSession::Enter()
{
	if (GetState() != STATE_DISCONNECTED)
	{
		// CloseSocket은 반드시 이전에 호출되어야 한다.
		// 로그
		return false;
	}

	// sendQueue가 남아 있으면 send thread에서 session에 접근하게 되므로 비워질 때까지 세션을 파괴해서는 안됨.
	if (!SendInfo->queue.empty())
	{
		// 로그
		return false;
	}

	if (m_pActor)
	{
		CPC *pc = static_cast<CPC*>(m_pActor);
		if (pc && pc->IsUsed())
		{
			CPCManager::GetInstance().DestroyPC(pc);
		}
		m_pActor = NULL;
	}

	SetState(STATE_DESTROY);
	ReleasePool();

	return true;
}


inline void CSession::SetActor(CActor const* actor_)
{
	m_pActor = const_cast<CActor*>(actor_);
}


CPC* CSession::CreatePlayer(SERIAL_ID serialId_)
{
	if (m_pActor == NULL)
	{
		m_pActor = CPCManager::GetInstance().CreatePC(serialId_);
		if (m_pActor == NULL)
		{
			// 로그 
			return NULL;
		}
	}

	CPC* pc = static_cast<CPC*>(m_pActor);
	pc->SetSession(this);

	return pc;
}


inline CPC* CSession::GetPlayer()
{
	if (m_pActor == NULL)
	{ 
		// 로그
		return false;
	}

	CPC *pc = static_cast<CPC*>(m_pActor);
	if (!pc->IsUsed())
	{
		// 로그
		return false;
	}
	
	return pc;
}


void CSession::Reset(_StInit* initParam_)
{
	CloseSocket();

	_StInit *init = reinterpret_cast<_StInit*>(initParam_);
	m_socket = init->socket;
	m_pServer = init->server;
	m_addressNumber = init->addressNumber;
	RecvInfo->wsa.len = PACKET_SIZE;
	RecvInfo->remain.len = 0;
	m_threadRegion = INIT_REGION_ID;
	RecvInfo->available = true;
	SendInfo->available = true;
}


void CSession::MakeClone(CSession const* session_)
{
	CSession* session = const_cast<CSession*>(session_);
	m_threadRegion = session->GetThreadRegion();
	while (!session->SendInfo->queue.empty())
	{
		CPacketTransInfoBase* info;
		session->SendInfo->queue.try_pop(info);
		SendInfo->queue.push(info);
	}
	m_pActor = session->GetPlayer();
	session->SetActor(NULL);
}


void CSession::CloseSocket()
{
	if (m_bIsConnected)
	{
		/// closeSocket이 확정된 상황에서 send 패킷 데이터는 무의미함.
		/// broadcast했던 데이터의 expireCount를 0으로 만들려면 이 시점에서 호출 필요.
		while (!SendInfo->queue.empty())
		{
			CPacketTransInfoBase *info;
			if (SendInfo->queue.try_pop(info) && info)
			{
				info->BackPoolObject();
			}
		}

		SetState(STATE_DISCONNECTED);
		::closesocket(m_socket);
	}
}


void CSession::SetTranslateMessageState(bool state_)
{
	RecvInfo->available = state_;
}


inline void CSession::SetCacheWaitState(bool state_)
{
	__super::SetCacheWaitState(state_);
	if (m_pActor)
	{
		m_pActor->SetCacheState(state_);
	}
}


inline bool CSession::CheckEnableReleaseCache() const
{
	return SendInfo->queue.empty();
}


CSession::CSession() 
	: CCacheObject(),
	CVirtualMemmoryPool(10,10),
	m_sessionID(INVALID_SERIAL_ID)
{
	RecvInfo = ALLOCATE_POOL_NEW(RecvInfo, SRecvInfo);
	SendInfo = ALLOCATE_POOL_NEW(SendInfo, SSendInfo);
	
	RecvInfo->overlapped = ALLOCATE_POOL_NEW(RecvInfo->overlapped, CPacketTransInfoBase);
	::memset(static_cast<LPOVERLAPPED>(RecvInfo->overlapped), 0, sizeof(OVERLAPPED));
	RecvInfo->wsa.buf = ALLOCATE_POOL_MALLOC(char, PACKET_SIZE);
	RecvInfo->remain.buf = ALLOCATE_POOL_MALLOC(char, PACKET_SIZE);
	RecvInfo->remain.len = 0;
	RecvInfo->available = false;
	SendInfo->available = false;
	m_bIsConnected = false;
}


CSession::~CSession()
{
	Enter();
	SAFE_POOL_FREE(RecvInfo->wsa.buf);
	SAFE_POOL_FREE(RecvInfo->remain.buf);
	SAFE_POOL_DELETE(RecvInfo->overlapped, CPacketTransInfoBase);
	SAFE_POOL_FREE(RecvInfo);
	SAFE_POOL_FREE(SendInfo);
}