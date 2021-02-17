#pragma once
#include "Define.h"
#include <map>


class CZone;

///////////////////////////////////////////////////////////
// Session Class
// (Session이 속해있는 Zone)
///////////////////////////////////////////////////////////
class CSession
{
public:
	enum E_DIR
	{
		LEFT,
		LEFTUP,
		UP,
		RIGHTUP,
		RIGHT,
		RIGHTDOWN,
		DOWN,
		LEFTDOWN,
		MAX_DIR
	};

	typedef std::map<E_DIR, CSession*> MAP_SESSION;

private:
	SECTOR_ID m_sectorId;        ///< 현재의 섹터 아이디
	__int32   m_indexX;          ///< 현재의 섹터 x 인덱스
	__int32   m_indexY;          ///< 현재의 섹터 y 인덱스
	
	MAP_SESSION m_mapAdjSector;  ///< 주변에 링크되어 있는 섹터
	CZone* m_zone;               ///< 현재 섹터가 종속되어 있는 존

public:
	bool MakeSector();

public:
	CSession(CZone* zone_);
	~CSession();
};



