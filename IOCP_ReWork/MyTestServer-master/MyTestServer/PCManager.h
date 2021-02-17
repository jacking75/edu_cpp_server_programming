#pragma once

#include <tbb/concurrent_hash_map.h>
#include "Define.h"
#include "PC.h"
#include "Singleton.h"
#include "VirtualMemoryPool.h"

class CSession;

///////////////////////////////////////////////////////////
// PC 관리 클래스
///////////////////////////////////////////////////////////
class CPCManager :
	public CSingleton<CPCManager>,
	public CVirtualMemmoryPool<CPC>	
{
public:
	typedef tbb::concurrent_hash_map<SERIAL_ID, CPC*> MAP_PC;

private:
	MAP_PC m_mapPC;  ///< PC 맵

public:
	// 1. 가져온 데이터가 같은 쓰레드 내에서 처리되면 안전함. 
	// 2. 이것을 외부에서 포인터로 가지고 있는 변수는 없어야 함(예: npc target을 해당 포인터로 두는 경우)
	// 3. 2번의 경우에는 shared_ptr로 dangle pointer를 일정 부분 막을 수 있으나, 성능상 보류
	/// PC 객체를 생성한다
	CPC* CreatePC(SERIAL_ID serialId_);

	/// 해당 serialID에 대한 PC객체를 찾는다
	CPC* FindPC(SERIAL_ID actorid_);  

	/// 해당 PC 삭제
	bool DestroyPC(CPC const* pc_);

	/// 해당 PC 삭제
	bool DestroyPC(SERIAL_ID actorid_);

public:
	CPCManager();
	~CPCManager();
};
