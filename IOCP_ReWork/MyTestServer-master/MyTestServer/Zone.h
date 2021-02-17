#pragma once
#include "Define.h"
#include "GeometryDef.h"
#include "PacketHeder.h"
#include "VirtualMemoryPool.h"
#include "PoolObjectStruct.h"
#include "tbb/scalable_allocator.h"
#include <map>

using namespace TypeVector;

class CPC;
class CNPC;
class CActor;
class dtNavMesh;
class CServerBase;

///////////////////////////////////////////////////////////
// Zone Class
// (zone이 속해있는 서버)
///////////////////////////////////////////////////////////
class CZone : 
	public CVirtualMemmoryPool<SPacketTransBroadCastInfo>
{
public:
	typedef std::map<SERIAL_ID, CPC*, std::less<SERIAL_ID>, tbb::tbb_allocator<std::pair<const SERIAL_ID, CPC>>> MAP_PC;
	typedef std::map<SERIAL_ID, CNPC*, std::less<SERIAL_ID>, tbb::scalable_allocator<std::pair<const SERIAL_ID, CNPC>>> MAP_NPC;

	//typedef std::map<SERIAL_ID, CPC*> MAP_PC;
	//typedef std::map<SERIAL_ID, CNPC*> MAP_NPC;

private:
	ZONE_ID m_zoneId;         ///< zoneID
	MAP_PC  m_mapPC;          ///< 등록된 PC 맵
	MAP_NPC m_mapNPC;         ///< 등록된 NPC 맵
	dtNavMesh* m_navMesh;     ///< 해당 zone의 navmesh
	CServerBase* m_pServer;   ///< zone이 속해있는 서버


public:
	__int32  MaxSectorX;      ///< 섹터의 최대 x 크기
	__int32  MaxSectorY;      ///< 섹터의 최대 y 크기


public:
	/// zoneID를 가지고 온다
	inline ZONE_ID GetZoneID() const { return m_zoneId; }

	/// zone에 속해 있는 특정 거리 안에 있는 PC중 하나를 가지고 온다
	CPC* FindInDistPC(Vec3 const& pos_, float dist_);

	/// zone에 속해 있는 특정 거리 안에 있는 NPC중 하나를 가지고 온다
	CNPC* FindInDistNPC(Vec3 const& pos_, float dist_);

	/// zone에 속해 있는 특정 serialID의 PC를 가지고 온다
	CPC* FindPC(SERIAL_ID id_);

	/// zone에 속해 있는 특정 serialID의 NPC를 가지고 온다
	CNPC* FindNPC(SERIAL_ID id_);

	/// 해당 존에 있는 PC들에게 패킷 메시지를 보낸다.
	void SendZone(packetdef::PacketID id_, protobuf::Message* message_, bool bIsImmediatly_ = true);

	/// zone에 actor 진입
	virtual bool Enter(CActor* actor_);

	/// zone에서 actor 퇴장
	virtual bool Leave(CActor* actor_);

	/// 업데이트
	virtual void Upate();

private:	
	/// 데이터 로딩
	bool Load(std::string const& fileName_);

	/// 섹터 생성
	bool MakeSector();

public:
	CZone(CServerBase* server_, std::string const fileName_);
	virtual ~CZone();
};
