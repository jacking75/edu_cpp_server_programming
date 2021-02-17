#include "ServerTaskSendAll.h"
#include "Zone.h"


bool CServerTaskSendAll::Execute(ZONE_MAP const& zoneMap)
{
	for (auto it : zoneMap)
	{
		CZone* zone = it.second;
		zone->SendZone(m_packetId, m_pMessage);
	}
	return true;
}


CServerTaskSendAll::CServerTaskSendAll(size_t executeCnt_, packetdef::PacketID id_, protobuf::Message* message_)
	: CServerTask(E_TYPE::WORLD_TASK_TYPE, executeCnt_),
	m_packetId(id_)
{
	m_pMessage->CopyFrom(*message_);
}


CServerTaskSendAll::~CServerTaskSendAll()
{
	m_pMessage->Clear();
}
