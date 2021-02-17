//#include "stdafx.h"
#include "ActorNavi.h"
#include "Actor.h"
#include "ATLComTime.h"


UnitNavi::UnitNavi() :
	m_navMesh(0),
	m_navQuery(0),
	m_straightPathOptions(0),
	m_startRef(0),
	m_endRef(0),
	m_npolys(0),
	m_nstraightPath(0)
{
	m_filter.setIncludeFlags(65535);
	m_filter.setExcludeFlags(0);
	m_polyPickExt[0] = 1;
	m_polyPickExt[1] = 100;
	m_polyPickExt[2] = 1;
}


UnitNavi::~UnitNavi()
{
}


void UnitNavi::InitMesh(const dtNavMesh* navMesh)
{
	m_navMesh = navMesh;

	if (m_navQuery != nullptr) {
		dtFreeNavMeshQuery(m_navQuery);
		m_navQuery = nullptr;
	}

	if (m_navQuery == nullptr) {
		m_navQuery = dtAllocNavMeshQuery();
		m_navQuery->init(navMesh, 2048);
	}
	Reset();
}


void UnitNavi::Reset()
{
	m_startRef = 0;
	m_endRef = 0;
	m_npolys = 0;
	m_nstraightPath = 0;

	::memset(m_spos, 0, sizeof(m_spos));
	::memset(m_epos, 0, sizeof(m_epos));

	::memset(m_PathNode, 0x00, sizeof(S_PathNodeData)*MAX_POLYS);

	m_MoveStartTime = 0;
	m_TotalDistance = 0;
}


void UnitNavi::InitOwner(CActor* actor_) 
{
	m_Owner = actor_;
};


void UnitNavi::Update()
{
	float speed = static_cast<float>(m_Owner->GetSpeed());
	Vec3 pos;
	float yaw;

	bool bGoal = false;	
	if (GetCurPos(speed, pos, yaw, bGoal)) {
		if (m_Owner)
		{
			m_Owner->SetPosition(pos.x, pos.z, pos.y);
		}

		if (bGoal == true) {
			Reset();
			if (m_Owner)
				m_Owner->SetAIPlayState(false);
		}
	}
}


bool UnitNavi::CommandMove(S_PathNodeData const& pos_)
{
	Reset();
	double curTime = ::GetTickCount();

	if (Pathfinding(pos_.From, pos_.To)) {
		if (m_nstraightPath <= 1)	// Node가 2개이상이 나와야 pathfinding이 된것
			return false;

		m_MoveStartTime = curTime;
		m_currentPath = pos_;
		return true;
	}

	return false;
}


bool UnitNavi::getNearPos(Vec3 const& start, Vec3 &pos)
{
	if (DT_SUCCESS == m_navQuery->findNearestPoly(start, m_polyPickExt, &m_filter, &m_startRef, pos))
		if (m_startRef != 0)
			return true;
	return false;
}


bool UnitNavi::Pathfinding(Vec3 startPos, Vec3 endPos) {
	if (!m_navMesh || !m_navQuery)
		return false;

	if (DT_SUCCESS != m_navQuery->findNearestPoly(startPos, m_polyPickExt, &m_filter, &m_startRef, m_spos))
		return false;

	if (DT_SUCCESS != m_navQuery->findNearestPoly(endPos, m_polyPickExt, &m_filter, &m_endRef, m_epos))
		return false;

	//m_pathFindStatus = DT_FAILURE;

	//m_toolMode = TOOLMODE_PATHFIND_STRAIGHT;	//이걸로 고정

	if (m_startRef && m_endRef) {
		m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
		m_nstraightPath = 0;
		if (m_npolys)
		{
			float epos[3];
			dtVcopy(epos, m_epos);
			if (m_polys[m_npolys - 1] != m_endRef)
				m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, 0);

			float fPath[MAX_POLYS * 3];
			if (DT_SUCCESS == m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
				fPath, m_straightPathFlags,
				m_straightPathPolys, &m_nstraightPath, MAX_POLYS, m_straightPathOptions)) {

				if (m_nstraightPath < 1)	// find node poly에 경우 2개 부터 
					return false;

				m_TotalDistance = 0;
				for (int n = 0; n < m_nstraightPath - 1; ++n) {
					m_PathNode[n].From = Vec3(fPath[n * 3], fPath[n * 3 + 1], fPath[n * 3 + 2]);
					m_PathNode[n].To = Vec3(fPath[(n + 1) * 3], fPath[(n + 1) * 3 + 1], fPath[(n + 1) * 3 + 2]);

					m_PathNode[n].SetDist();
					m_TotalDistance += m_PathNode[n].GetDist();
				}
			}
		}
		else
		{
			m_npolys = 0;
			m_nstraightPath = 0;
		}
	}
	return true;
}

bool UnitNavi::GetCurPos(float speedPerSec, Vec3 &curPos, float &direction, bool &goal)
{
	double curTime = ::GetTickCount();
	curPos = Vec3(0, 0, 0);
	direction = 0.f;
	goal = false;

	if (m_currentPath.From == m_currentPath.To) {
		curPos = m_currentPath.To;
		//direction = m_Move.Head;
		goal = true;
		return false;
	}
	if (m_MoveStartTime == 0) return false;
	

	float distance = static_cast<float>(speedPerSec * max(0.f, curTime - m_MoveStartTime) / 10);

	if (distance >= m_TotalDistance) {
		curPos = m_currentPath.To;
		//direction = m_Move.Head;
		goal = true;
		return true;
	}

	S_PathNodeData* node = nullptr;

	for (int n = 0; n < m_nstraightPath - 1; n++) {
		node = &m_PathNode[n];
		if (distance <= node->Distance) {
			curPos = node->Lerp(distance);
			direction = node->Distance;
			break;
		}
		distance -= node->Distance;
	}

	return true;
}


dtNavMesh* UnitNavi::LoadMeshFromFile(const std::string& filename)
{
	FILE* fp;
	fopen_s(&fp, filename.c_str(), "rb");
	if (!fp) return nullptr;

	static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
	static const int NAVMESHSET_VERSION = 1;

	struct NavMeshSetHeader {
		int magic;
		int version;
		int numTiles;
		dtNavMeshParams params;
	};

	struct NavMeshTileHeader {
		dtTileRef tileRef;
		int dataSize;
	};

	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1) {
		fclose(fp);
		return nullptr;
	}
	if (header.magic != NAVMESHSET_MAGIC) {
		fclose(fp);
		return nullptr;
	}
	if (header.version != NAVMESHSET_VERSION) {
		fclose(fp);
		return nullptr;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh) {
		fclose(fp);
		return nullptr;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status)) {
		fclose(fp);
		return nullptr;
	}

	for (int i = 0; i < header.numTiles; ++i) {
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(NavMeshTileHeader), 1, fp);
		if (readLen != 1) {
			fclose(fp);
			return nullptr;
		}
		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1) {
			fclose(fp);
			return nullptr;
		}
		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}
	fclose(fp);
	return mesh;
}

