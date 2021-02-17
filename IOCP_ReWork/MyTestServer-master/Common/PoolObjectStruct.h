#pragma once
#include "VirtualPoolObject.h"
#include "PacketHeder.h"
#ifdef __TBB_malloc_proxy
#include "tbb/tbbmalloc_proxy.h"
#else
#include "tcmalloc.h"
#endif
#include <WinSock2.h>
#include <tbb/atomic.h>
#include "Define.h"
#include "VirtualMemoryPool.h"


///////////////////////////////////////////////////////////////////////////
// 패킷 이송정보 베이스
///////////////////////////////////////////////////////////////////////////
class CPacketTransInfoBase :
	public OVERLAPPED,
	public CVirtualPoolObject
{
public:
	enum TYPE_OP 
	{
		TYPE_NONE,
		TYPE_SEND,
		TYPE_RECV,
	};

	static const int MAX_PACKET_SIZE = PACKET_SIZE * 2; ///< 버퍼가 PACKET_SIZE 인 상태에서 아무리 많이 들어와도 최대 PACKET_SIZE * 2

	packetdef::PacketHeader header;                    ///< 패킷 해더
	void* message;                                     ///< 패킷 메시지

	virtual bool Leave(void* initParam_) { return false; }
	virtual bool Enter() { return false; }
	virtual bool BackPoolObject() { return false; }
	virtual TYPE_OP GetType() { return TYPE_RECV; }

	CPacketTransInfoBase()
	{
		hEvent = 0;
	}

	virtual ~CPacketTransInfoBase()
	{
	}
};


///////////////////////////////////////////////////////////////////////////
// 패킷 이송정보 클래스
///////////////////////////////////////////////////////////////////////////
class SPacketTransInfo :
	public CPacketTransInfoBase
{
public:
	CVirtualMemmoryPool<SPacketTransInfo>* owner;  ///< 메모리풀 소유자
	struct _StInit
	{
		packetdef::PacketHeader header;
		CVirtualMemmoryPool<SPacketTransInfo>* owner;
		_StInit(packetdef::PacketID id_, protobuf::uint16 size_, CVirtualMemmoryPool<SPacketTransInfo>* owner_)
		{
			header.id = id_;
			header.dataSize = size_;
			owner = owner_;
		}
	};

	/// 메모리 풀에서 가져올 때
	virtual bool Leave(void* initParam_) override
	{
		if (initParam_ == NULL)
		{
			// 로그
			return false;
		}

		_StInit* init = reinterpret_cast<_StInit*>(initParam_);
		if (init->owner == NULL)
		{
			// 로그
			return false;
		}

		header.id = init->header.id;
		header.dataSize = init->header.dataSize;
		owner = init->owner;
		message = ALLOCATE_POOL_MALLOC(void, MAX_PACKET_SIZE);
		return true;
	}

	/// 메모리 풀에 들어갈 때
	virtual bool Enter() override
	{
		SAFE_POOL_FREE(message);
		return true;
	}

	/// owner를 통해 메모리 풀에 반환
	virtual bool BackPoolObject() override
	{
		return owner->BackPoolObject(this);
	}

	/// 해당 패킷유닛의 타입
	virtual TYPE_OP GetType() override
	{
		return TYPE_SEND;
	}

	SPacketTransInfo()
	{
		//message = ALLOCATE_POOL_MALLOC(void, MAX_PACKET_SIZE);
	}

	virtual ~SPacketTransInfo()
	{
		//SAFE_POOL_FREE(message);
	}
};


///////////////////////////////////////////////////////////////////////////
// 패킷 이송정보 클래스 (브로드 케스트용)
///////////////////////////////////////////////////////////////////////////
class SPacketTransBroadCastInfo :
	public CPacketTransInfoBase
{
public:
	tbb::atomic<size_t> expireCount;                        ///< 만료 카운트(0이 되면 삭제)
	CVirtualMemmoryPool<SPacketTransBroadCastInfo>* owner;  ///< 메모리풀 소유자

	struct _StInit
	{
		packetdef::PacketHeader header;
		CVirtualMemmoryPool<SPacketTransBroadCastInfo>* owner;
		size_t expireCount;

		_StInit(packetdef::PacketID id_, protobuf::uint16 size_, size_t expireCount_, CVirtualMemmoryPool<SPacketTransBroadCastInfo>* owner_)
		{
			header.id = id_;
			header.dataSize = size_;
			expireCount = expireCount_;
			owner = owner_;
		}
	};

	/// 메모리 풀에서 가져올 때
	virtual bool Leave(void* initParam_) override
	{
		if (initParam_ == NULL)
		{
			// 로그
			return false;
		}
			
		_StInit* init = reinterpret_cast<_StInit*>(initParam_);
		if (init->owner == NULL)
		{
			// 로그
			return false;
		}

		if (init->expireCount < 1)
		{
			// 로그
			return false;
		}

		header.id = init->header.id;
		header.dataSize = init->header.dataSize;
		expireCount = init->expireCount;
		owner = init->owner;
		message = ALLOCATE_POOL_MALLOC(void, MAX_PACKET_SIZE);
		return true;
	}

	/// 메모리 풀에 들어갈 때
	virtual bool Enter() override
	{
		if (expireCount.fetch_and_decrement() <= 1)
		{
			SAFE_POOL_FREE(message);
			return true;
		}
		
		return false;
	}

	/// owner를 통해 메모리 풀에 반환
	virtual bool BackPoolObject() override
	{
		return owner->BackPoolObject(this);
	}

	/// 해당 패킷유닛의 타입
	virtual TYPE_OP GetType() override
	{
		return TYPE_SEND;
	}


	SPacketTransBroadCastInfo()
	{
		//message = ALLOCATE_POOL_MALLOC(void, MAX_PACKET_SIZE);
	}

	virtual ~SPacketTransBroadCastInfo()
	{
		//SAFE_POOL_FREE(message);
	}

};





