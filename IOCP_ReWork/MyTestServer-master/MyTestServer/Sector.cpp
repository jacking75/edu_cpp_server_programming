#include "Sector.h"
#include "Zone.h"


CSession::CSession(CZone* zone_)
	: m_zone(zone_)
{
}


CSession::~CSession()
{
}


bool CSession::MakeSector()
{

	for (int y = 0; y < m_zone->MaxSectorY; ++y)
	{
		for (int x = 0; x < m_zone->MaxSectorX; ++x)
		{

		}
	}
	m_mapAdjSector[LEFT] 

	


	return true;
}
