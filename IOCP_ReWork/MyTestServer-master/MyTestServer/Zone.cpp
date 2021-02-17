#include "Zone.h"
#include "ActorNavi.h"
#include "NPC.h"
#include "PC.h"
#include "ServerBase.h"
#include "sample.pb.h"
#include "types.pb.h"
#include "fmt\format.h"
#include "VirtualMemoryPool.hpp"
#include <google/protobuf/io/coded_stream.h>

using namespace google;
using namespace packetdef;


bool CZone::Load(std::string const& fileName_)
{
	// 임시
	static ZONE_ID tempZoneID = INIT_ZONE_ID;
	static SERIAL_ID npcSerialID = 1000;

	m_zoneId = tempZoneID++;

	CHAR directory[_MAX_PATH];
	::GetModuleFileNameA(NULL, directory, _MAX_PATH);
	std::string strDir(directory);
	
	fmt::MemoryWriter path;
	path << strDir.substr(0, strDir.find_last_of('\\'));;
	path << fileName_.c_str();
	//path << "\\all_tiles_navmesh.bin";


	m_navMesh = UnitNavi::LoadMeshFromFile(path.str());
	if (m_navMesh == NULL)
	{
		// 로그
		return false;
	}

	CNPC* npc1 = new CNPC;
	npc1->SetNavmesh(m_navMesh);
	npc1->SetSpawnPosition(1364, 1074, 140);
	npc1->SetTargetID(INVALID_SERIAL_ID);
	npc1->SetActorID(npcSerialID++);
	Enter(npc1);
	
	CNPC* npc2 = new CNPC;
	npc2->SetNavmesh(m_navMesh);
	npc2->SetSpawnPosition(2100, 1500, 140);
	npc2->SetTargetID(INVALID_SERIAL_ID);
	npc2->SetActorID(npcSerialID++);
	Enter(npc2);

	return true;
}


bool CZone::MakeSector()
{

	return true;
}


bool CZone::Enter(CActor* actor_)
{
	// 임시
	if (actor_ == NULL)
	{
		// 로그
		return false;
	}

	if (!actor_->IsValid())
	{
		// 로그
		return false;
	}


	switch (actor_->GetActorType())
	{
	case E_ACTOR::PC:
		{
			CPC* pc = static_cast<CPC*>(actor_);

			/// 입장한 유저에게 기존의 pc 정보 전송
			for (auto it : m_mapPC)
			{
				packets::EnterPacket nty;
				nty.set_actorid(it.second->GetActorID());
				nty.set_type(packets::PC);
				nty.set_allocated_position(it.second->GetPos());
				pc->GetSession()->Send(PacketID::EnterPacket, &nty);
			}

			/// 입장한 유저에게 기존의 npc 정보 전송
			for (auto it : m_mapNPC)
			{
				packets::EnterPacket nty;
				nty.set_actorid(it.second->GetActorID());
				nty.set_type(packets::NPC);
				nty.set_allocated_position(it.second->GetPos());
				pc->GetSession()->Send(PacketID::EnterPacket, &nty);
			}

			m_mapPC.insert(std::make_pair(actor_->GetActorID(), pc));
		}
		break;

	case E_ACTOR::NPC:
		{
			m_mapNPC.insert(std::make_pair(actor_->GetActorID(), static_cast<CNPC*>(actor_)));
		}
		break;

	default:
		// 로그
		return false;
	}

	/// 입장한 유저의 정보를 모두에게 전송
	packets::EnterPacket nty;
	nty.set_actorid(actor_->GetActorID());
	nty.set_type((actor_->GetActorType() == E_ACTOR::PC) ? packets::PC : packets::NPC);
	nty.set_allocated_position(actor_->GetPos());
	SendZone(PacketID::EnterPacket, &nty, true);

	actor_->SetZone(this);
	return true;
}


bool CZone::Leave(CActor* actor_)
{
	// 임시
	if (actor_ == NULL)
	{
		// 로그
		return false;
	}

	if (!actor_->IsValid())
	{
		// 로그
		return false;
	}

	SERIAL_ID serialID = actor_->GetActorID();

	switch (actor_->GetActorType())
	{
	case E_ACTOR::PC:
	{	
		m_mapPC.erase(serialID);

		packets::LeavePacket nty;
		nty.set_actorid(serialID);
		SendZone(PacketID::LeavePacket, &nty, true);
		break;
	}

	case E_ACTOR::NPC:
	{
		m_mapNPC.erase(serialID);

		packets::LeavePacket nty;
		nty.set_actorid(serialID);
		SendZone(PacketID::LeavePacket, &nty, true);
		break;
	}

	default:
		// 로그
		return false;
	}

	// 패킷
	actor_->SetZone(NULL);

	return true;
}


CPC* CZone::FindInDistPC(Vec3 const& pos_, float dist_)
{
	for (auto it : m_mapPC)
	{
		CPC* pc = it.second;
		if (pc->GetPosCRef().Distance(pos_) <= dist_)
		{
			return pc;
		}
	}
	return NULL;
}


CNPC* CZone::FindInDistNPC(Vec3 const& pos_, float dist_)
{
	for (auto it : m_mapNPC)
	{
		CNPC* npc = it.second;
		if (npc->GetPosCRef().Distance(pos_) <= dist_)
		{
			return npc;
		}
	}
	return NULL;
}

CPC* CZone::FindPC(SERIAL_ID id_)
{
	auto it = m_mapPC.find(id_);
	if (it != m_mapPC.end())
	{
		return (*it).second;
	}
	return NULL;
}


CNPC* CZone::FindNPC(SERIAL_ID id_)
{
	auto it = m_mapNPC.find(id_);
	if (it != m_mapNPC.end())
	{
		return (*it).second;
	}
	return NULL;
}


void CZone::SendZone(packetdef::PacketID id_, protobuf::Message* message_, bool bIsImmediatly_)
{
	size_t nLength = m_mapPC.size();
	if (nLength == 0)
	{
		return;
	}

	int nSize = sizeof(PacketHeader) + message_->ByteSize();
	if (nSize > PACKET_SIZE)
	{
		// 로그
		return;
	}

	/// 메모리 풀에서 할당
	SPacketTransBroadCastInfo* packetInfo;
	SPacketTransBroadCastInfo::_StInit param(id_, message_->ByteSize(), nLength, this);
	if (!GetPoolObject(packetInfo, &param))
	{
		// 로그
		return;
	}

	protobuf::io::ArrayOutputStream output_array_stream(packetInfo->message, nSize);
	protobuf::io::CodedOutputStream os(&output_array_stream);

	packetdef::PacketHeader packetHeader(id_, message_->ByteSize());
	os.WriteRaw(&packetHeader, sizeof(PacketHeader));
	message_->SerializePartialToCodedStream(&os);

	for (auto it = m_mapPC.begin(); it != m_mapPC.end(); ++it)
	{
		CPC* pc = (*it).second;
		if (pc)
		{
			CSession* session = pc->GetSession();
			if (session)
			{
				session->BroadCast(packetInfo, bIsImmediatly_);
				//session->Send(id_, message_);
			}
		}
	}
}


void CZone::Upate()
{
	for (auto it : m_mapNPC)
	{
		it.second->Update();

		packets::Move nty;
		nty.set_actorid(it.second->GetActorID());
		nty.set_allocated_position(it.second->GetPos());
		SendZone(PacketID::Move, &nty, true);

	}

	for (auto it : m_mapPC)
	{
		it.second->Update();
	}
	
}


CZone::CZone(CServerBase* server_, std::string const fileName_)
	: m_pServer(server_)
{
	Load(fileName_);
}

CZone::~CZone()
{

}