#pragma once
#include <winsock2.h>
#include <tbb/concurrent_queue.h>
#include <tbb/atomic.h>
#include "VirtualPoolObject.h"
#include "VirtualMemoryPool.h"
#include "PacketHeder.h"
#include "Define.h"
#include "PoolObjectStruct.h"
#include "Actor.h"
#include "CacheObject.h"
#include "ServerCore/SessionBase.h"
#include "ATLComTime.h"
#include <atomic>


typedef tbb::concurrent_queue<CPacketTransInfoBase*> PACKET_QUEUE;

struct SSendInfo
{
	PACKET_QUEUE   queue;
	std::atomic<BOOL> available;
};

struct SRecvInfo
{
	_WSABUF        wsa;
	_WSABUF        remain;           ///< recv처리를 하지 못하고 남은 버퍼
	CPacketTransInfoBase*  overlapped;
	PACKET_QUEUE   queue;
	std::atomic<BOOL> available;
};


class CPC;
class CServerBase;

///////////////////////////////////////////////////////////
// Session Class
///////////////////////////////////////////////////////////
class CSession : 
	public CSessionBase,
	public CVirtualPoolObject, 
	public CVirtualMemmoryPool<SPacketTransInfo>,
	public CCacheObject
{
public:
	enum E_STATE
	{
		STATE_INIT,
		STATE_REGISTED,
		STATE_DISCONNECTED,
		STATE_DESTROY,
	};

	struct _StInit
	{
		SOCKET socket;
		CServerBase* server;
		ULONG  addressNumber;
	};

private:
	SESSION_ID m_sessionID;          ///< sessionID
	SOCKET     m_socket;             ///< Socket
	DATE       m_createTimer;        ///< 세션이 만들어진 시간
	THREAD_REGION m_threadRegion;    ///< 세션이 방문하는 recv thread의 지역 번호
	CServerBase*  m_pServer;         ///< 종속하는 서버
	ULONG         m_addressNumber;   ///< IP address의 DWORD값
	E_STATE       m_state;           ///< 세션의 상태
	bool          m_bIsConnected;    ///< 접속 여부
	CActor*       m_pActor;          ///< 종속된 액터
	std::string   m_pass;            ///< 세션 캐시를 위한 패스워드

public:
	SSendInfo*      SendInfo;        ///< Send의 정보
	SRecvInfo*      RecvInfo;        ///< Recv의 정보
	
public:
	/// 세션 아이디를 가지고 온다
	inline SESSION_ID GetSessionID() const { return m_sessionID; }

	/// 세션 아이디를 세팅한다.
	void SetSessionID(SESSION_ID sessionId_);

	/// 소캣을 가지고 온다
	inline SOCKET     GetSocket() { return m_socket; }

	/// 세션이 속해 있는 recv thread의 지역 아이디를 가지고 온다
	inline THREAD_REGION GetThreadRegion() { return m_threadRegion; }

	/// IP의 DWORD 형태의 주소값을 가지고 온다
	inline ULONG      GetAddressNumber() const { return m_addressNumber; }

	/// 세션의 상태를 가지고 온다
	inline E_STATE    GetState() const { return m_state; }

	/// 접속 여부를 가지고 온다
	inline bool       IsConnected() const { return m_bIsConnected; }

	/// 패스워드를 가지고 온다
	inline std::string GetPassword() const { return m_pass; }

	/// 패스워드를 세팅한다.
	void SetPassword(std::string const& pass_);

	/// 유저 아이디를 가지고 온다
	USER_ID    GetUserID() const;

	/// 액터가 속한 zoneID를 가지고 온다
	ZONE_ID    GetZoneID() const;

	/// 패킷을 sendQueue에 넣는다 (recv thread 내에서 사용해야 한다)
	bool       Send(packetdef::PacketID id_, protobuf::Message* message_);

	/// 패킷을 곧바로 전송한다 (recv thread 내에서 사용해야 한다)
	bool       SendImmediately(packetdef::PacketID id_, protobuf::Message* message_);

	/// 패킷을 broadcast하기 위해 sendQueue에 넣는다 (recv thread 내에서 사용해야 한다)
	bool	   BroadCast(SPacketTransBroadCastInfo* packetInfo_, bool bIsImmediatly_);

	/// sendQueue에 쌓여있는 패킷을 flush한다.
	void       FlushAllSend();

	/// 세션이 방문하게 될 thread의 지역 아이디를 세팅한다.
	void       SetThreadRegion(int criteria_);

	/// 세션의 상태를 세팅한다
	void       SetState(E_STATE const state_);


	/// 세션의 정보들을 리셋한다
	void       Reset(_StInit* initParam_);

	/// 자신을 입력된 세션의 복사본화 한다
	void	   MakeClone(CSession const* session_);

	/// 소켓을 닫음
	void       CloseSocket();

	/// Recv 쓰레드의 메시지 해석 가능 여부
	virtual void  SetTranslateMessageState(bool state_);

///////////////////////////////////////////////////////////////////////////////////

	/// 액터의 시리얼 아이디를 가지고 온다
	SERIAL_ID  GetActorID() const;

	/// 세션의 액터를 세팅한다
	void       SetActor(CActor const* actor_);

	/// PC를 만든다 
	CPC*       CreatePlayer(SERIAL_ID serialId_);

	/// PC를 가지고 온다
	CPC*       GetPlayer();


private:
	/// 메모리 풀에서 세션을 꺼내온다
	virtual bool Leave(void* initParam_) override;

	/// 메모리 풀에 세션을 집어넣는다
	virtual bool Enter() override;

	/// 캐시 상태를 세팅한다
	virtual void SetCacheWaitState(bool state_) override;

	/// 캐시에서 삭제할 수 있는 상태인지 체크한다
	virtual bool CheckEnableReleaseCache() const override;

public:
	CSession();
	virtual ~CSession();
};