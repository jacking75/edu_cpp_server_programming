#pragma once
#include "stdafx.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "GeometryDef.h"
#include <string>


using namespace TypeVector;

class CActor;

///////////////////////////////////////////////////////////
// 길찾기 수행 클래스
///////////////////////////////////////////////////////////
class UnitNavi
{
public:
	static const int MAX_POLYS = 256;
	static const int MAX_SMOOTH = 2048;

	struct S_PathNodeData {
		Vec3 From;
		Vec3 To;
		float Yaw;
		float Distance;
		void SetDist() {
			Distance = From.Distance(To);
			//Distance = From.Distance2(To);
		}
		float GetDist() {
			return Distance;
		}

		Vec3 Lerp(float dist_) {
			return Vec3::Lerp(From, To, dist_ / Distance);
		}
	} m_PathNode[MAX_POLYS];


private:
	const dtNavMesh* m_navMesh;      ///< 네비매쉬
	dtNavMeshQuery* m_navQuery;
	dtQueryFilter m_filter;
	S_PathNodeData m_currentPath;     ///< 현재 세팅된 길찾기 정보 

	int m_straightPathOptions;

	dtPolyRef m_startRef;
	dtPolyRef m_endRef;
	dtPolyRef m_polys[MAX_POLYS];
	//dtPolyRef m_parent[MAX_POLYS];
	int m_npolys;

	unsigned char m_straightPathFlags[MAX_POLYS];
	dtPolyRef m_straightPathPolys[MAX_POLYS];
	int m_nstraightPath;
	float m_polyPickExt[3];

	float m_spos[3];
	float m_epos[3];

	double m_MoveStartTime;           ///< 길찾기 시작 시간
	float m_TotalDistance;            ///< 현재 세팅된 길찾기 노드의 총 길이
	CActor* m_Owner;                  ///< 길찾기를 수행할 actor


private:
	/// 길찾기 처리
	bool Pathfinding(Vec3 startPos_, Vec3 endPos_);

public:
	/// 길찾기를 수행할 actor 세팅
	void InitOwner(CActor* actor_);
	
	/// 초기화 함수
	void InitMesh(const dtNavMesh* navMesh_);
	
	/// 길찾기 정보 리셋
	void Reset();

	/// 업데이트
	void Update();

	/// 길찾기 정보 세팅
	bool CommandMove(S_PathNodeData const& pos_);

	/// 특정 좌표에 갈 수 있는지 체크(실제 갈 수 있는 곳 반환) 
	bool getNearPos(Vec3 const& start_, Vec3& pos_);

	/// 업데이트 중인 현재 좌표를 가져온다(속도, 현재좌표 반환, 방향 반환, 목적지 도착 유무 반환)
	bool GetCurPos(float speedPerSec_, Vec3& curPos_, float& direction_, bool& goal_);

	/// 해당 filepath의 메쉬 파일을 로딩 후 메쉬정보 반환
	static dtNavMesh* LoadMeshFromFile(const std::string& filename);

public:
	UnitNavi();
	virtual ~UnitNavi();
};
