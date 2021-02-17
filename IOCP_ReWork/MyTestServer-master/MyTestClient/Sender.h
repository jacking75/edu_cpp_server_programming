#pragma once
#include <thread>
#include <winsock2.h>
#include "Common/PacketHeder.h"
#include "Common/PoolObjectStruct.h"
#include "Common/VirtualMemoryPool.h"
#include "tbb/concurrent_queue.h"
#include "define.h"

class CSession;

///////////////////////////////////////////////////////////
// CSender 클래스
///////////////////////////////////////////////////////////
class CSender : 
	public CVirtualMemmoryPool<SPacketTransInfo>
{
public:
	typedef tbb::concurrent_queue<SPacketTransInfo*> PACKET_QUEUE;

private:
	PACKET_QUEUE m_queue;            ///< Task Thread용 taskQueue
	HANDLE       m_waitingEvent;     ///< taskThread의 다음 작업을 기다리기 위한 핸들

	CSession*     m_pSession;        ///< connector의 세션 객체 포인터
	volatile bool m_bRunning;        ///< send 쓰레드의 동작 유무
	std::thread   m_hThread;         ///< 쓰레드 객체 핸들

public:
	/// send 쓰레드 시작
	bool Start(CSession* session_);

	/// 전송할 패킷을 큐에 집어 넣는다
	bool Push(packetdef::PacketID id_, protobuf::Message* message_);

	/// 해당 객체를 종료한다
	void Close();

	/// 쓰레드로 등록되는 함수
	unsigned CALLBACK SendProc();

public:
	CSender();
	~CSender();
};
