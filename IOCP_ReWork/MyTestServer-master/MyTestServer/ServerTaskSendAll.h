#pragma once
#include "ServerTask.h"
#include "Define.h"
#include "PacketHeder.h"


///////////////////////////////////////////////////////////
// 메시지 전체 보내기 서버 태스크
// (수행되는 쓰레드의 개수, 패킷아이디, 메시지)
///////////////////////////////////////////////////////////
class CServerTaskSendAll
	: public CServerTask
{
private:
	packetdef::PacketID m_packetId;  ///< 패킷 아이디
	protobuf::Message* m_pMessage;   ///< 패킷 메시지

public:
	/// 수행
	virtual bool Execute(ZONE_MAP const& zoneMap) override;

public:
	CServerTaskSendAll(size_t executeCnt_, packetdef::PacketID id_, protobuf::Message* message_);
	virtual ~CServerTaskSendAll();
};
