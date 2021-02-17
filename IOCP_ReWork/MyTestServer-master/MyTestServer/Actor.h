#pragma once
#include "ActorNavi.h"
#include "Define.h"
#include "types.pb.h"

enum E_ACTOR
{
	NONE,
	PC,
	NPC,
	MONSTER
};

enum E_ACTOR_STATE
{
	ACTOR_STAND,
	ACTOR_WALK,
	ACTOR_RUN,
	ACTOR_KNOCKBACK,
	ACTOR_ATTACK,
	ACTOR_DEAD,
	ACTOR_RETURN,
};

class CZone;

///////////////////////////////////////////////////////////
// Actor 클래스
// (actor의 타입)
///////////////////////////////////////////////////////////
class CActor
{
protected:
	Vec3 m_pos;                      ///< actor의 위치
	Vec3 m_spawnPos;                 ///< actor의 스폰 위치
	SERIAL_ID m_actorID;             ///< actor의 serialID
	SERIAL_ID m_targetActorID;       ///< actor의 target의 serialID
	int m_speed;                     ///< actor의 속도
	int m_hp;                        ///< actor의 hp
	bool m_bIsCache;                 ///< actor가 캐시 상태인지의 유무
	E_ACTOR m_actorType;             ///< actor 타입
	E_ACTOR_STATE m_actionState;     ///< actor의 액션 상태
	dtNavMesh* m_mesh;               ///< naviMesh 객체 포인터
	UnitNavi  m_navMesh;             ///< 길찾기 수행 클래스
	CZone*    m_zone;                ///< zone


public:
	/// naviMesh 세팅
	void SetNavmesh(dtNavMesh* mesh_);

	/// 세팅된 naviMesh 가져오기
	UnitNavi* GetNaviMesh();

	/// actor 위치 세팅
	void SetPosition(Vec3 const& pos_);

	/// actor 위치 세팅(packet용...)
	void SetPosition(packets::Vector3 const& pos);

	/// actor 위치 세팅
	void SetPosition(float x_, float y_, float z_);

	/// actor의 스폰 위치 세팅
	void SetSpawnPosition(float x_, float y_, float z_);

	/// actor의 serialID 세팅
	void SetActorID(SERIAL_ID id_);

	/// actor의 속도 세팅
	void SetSpeed(int speed_);

	/// actor가 속해있는 zone 세팅
	void SetZone(CZone* zone);

	/// actor의 hp세팅
	void SetHP(int hp_);

	/// actor의 target에 대한 serialID 세팅
	void SetTargetID(SERIAL_ID actorId_);

	/// actor의 actionState 세팅
	void SetActionState(E_ACTOR_STATE state_);

	/// actor의 캐시 상태 유무 세팅
	void SetCacheState(bool state_);

	/// actor의 x좌표
	inline float GetPosX() const { return m_pos.x; }

	/// actor의 y좌표
	inline float GetPosY() const { return m_pos.y; }

	/// actor의 z좌표
	inline float GetPosZ() const { return m_pos.z; }

	/// actor의 좌표값에 대한 레퍼런스
	inline Vec3& GetPosRef() { return m_pos; }

	/// actor의 좌표
	inline Vec3 const& GetPosCRef() const { return m_pos; }

	/// actor의 packet vector 생성
	packets::Vector3* GetPos();
	
	/// actor의 스폰 x좌표
	inline float GetSpawnPosX() const { return m_spawnPos.x; }

	/// actor의 스폰 y좌표
	inline float GetSpawnPosY() const { return m_spawnPos.y; }

	/// actor의 스폰 z좌표
	inline float GetSpawnPosZ() const { return m_spawnPos.z; }

	/// actor의 스폰 좌표 레퍼런스
	inline Vec3& GetSpawnPosRef() { return m_spawnPos; }

	/// actor의 속도 가져오기
	int GetSpeed() const;

	/// actor의 zone 가져오기
	inline CZone* GetZone() const { return m_zone; }

	/// actor의 zoneID 가져오기
	ZONE_ID GetZoneID() const;

	/// actor의 hp 가져오기
	inline double GetHP() const { return m_hp; }

	/// actor의 타입 가져오기
	inline E_ACTOR GetActorType() const { return m_actorType; }

	/// actor의 target의 serialID 가져오기
	inline SERIAL_ID GetTargetID() const { return m_targetActorID; }

	/// actor의 serialID 가져오기
	inline SERIAL_ID GetActorID() { return m_actorID; }

	/// actor의 액션상태 가져오기
	inline E_ACTOR_STATE GetActionState() const { return m_actionState; }

	/// actor가 캐시큐에 들어갔는가
	inline bool IsCacheState() const { return m_bIsCache; }

	/// 업데이트
	virtual void Update();

	/// 현재 AI상태에 대한 동작 유무
	virtual void SetAIPlayState(bool bIs_);

	/// 유효한 actor인가
	virtual bool IsValid() const;


public:
	CActor(E_ACTOR type_);
	virtual ~CActor();
};