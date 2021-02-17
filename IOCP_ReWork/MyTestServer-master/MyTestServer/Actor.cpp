#include "stdafx.h"
#include "Actor.h"
#include "Zone.h"


CActor::CActor(E_ACTOR type_)
	: m_actorType(type_),
	m_actionState(E_ACTOR_STATE::ACTOR_STAND),
	m_speed(100),
	m_targetActorID(INVALID_SERIAL_ID),
	m_zone(NULL),
	m_mesh(NULL),
	m_bIsCache(false)
{
}

CActor::~CActor()
{

}


void CActor::SetPosition(Vec3 const& pos_)
{
	m_pos = pos_;
}


void CActor::SetPosition(packets::Vector3 const& pos_)
{
	m_pos.x = pos_.x();
	m_pos.y = pos_.y();
	m_pos.z = pos_.z();
}


void CActor::SetPosition(float x_, float y_, float z_)
{
	// 일단
	m_pos.x = x_;
	m_pos.y = y_;
	m_pos.z = z_;
}


void CActor::SetSpawnPosition(float x_, float y_, float z_)
{
	// 일단
	m_spawnPos.x = x_;
	m_spawnPos.y = y_;
	m_spawnPos.z = z_;
	SetPosition(x_, y_, z_);
}

void CActor::SetActorID(SERIAL_ID id_)
{
	m_actorID = id_;
}


void CActor::SetSpeed(int speed_)
{
	m_speed = speed_;
}


void CActor::SetZone(CZone* zone_)
{
	m_zone = zone_;
}


void CActor::SetHP(int hp_)
{

}


void CActor::SetTargetID(SERIAL_ID actorId_)
{
	m_targetActorID = actorId_;
}


void CActor::SetActionState(E_ACTOR_STATE state_)
{
	if (m_actionState != state_)
	{
		m_actionState = state_;
	}
}

void CActor::SetCacheState(bool state_)
{
	m_bIsCache = state_;
}


packets::Vector3* CActor::GetPos()
{
	packets::Vector3* pos = new packets::Vector3();
	pos->set_x(m_pos.x);
	pos->set_y(m_pos.y);
	pos->set_z(m_pos.z);
	return pos;
}


int CActor::GetSpeed() const
{
	switch (GetActionState())
	{
	case E_ACTOR_STATE::ACTOR_RUN:
		return 5;

	case E_ACTOR_STATE::ACTOR_WALK:
		return 2;

	case E_ACTOR_STATE::ACTOR_RETURN:
		return 10;

	default:
		return 0;
	}
}


ZONE_ID CActor::GetZoneID() const
{
	if (m_zone)
	{
		return m_zone->GetZoneID();
	}
	return INIT_ZONE_ID;
}


void CActor::Update()
{
	// Don't call this
}


void CActor::SetAIPlayState(bool bIs_)
{
	// Dont't call this
}

bool CActor::IsValid() const
{
	// Don't call this
	return false;
}


void CActor::SetNavmesh(dtNavMesh* mesh_)
{
	m_mesh = mesh_;
	m_navMesh.InitOwner(this);
	m_navMesh.InitMesh(mesh_);
}


UnitNavi* CActor::GetNaviMesh() 
{ 
	return &m_navMesh; 
}
