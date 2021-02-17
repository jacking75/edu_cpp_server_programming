#pragma once

#include "tbb/atomic.h"

///////////////////////////////////////////////////////////////////////////////
// 상속용 특정 타입의 오브젝트 베이스
///////////////////////////////////////////////////////////////////////////////
class CVirtualPoolObject
{
private:
	//tbb::atomic<bool> m_bUsed;   ///< 매니저를 여러 쓰레드에서 쓰게 된다면 고려할 부분...
	bool m_bUsed;                  ///< 사용 유무(오브젝트 풀에서 꺼내왔는가의 유무)

public:
	/// 오브젝트가 풀에서 빠져나와 사용중이라 이용할 수 없음으로 세팅
	virtual bool SetUnAvailableState();

	/// 오브젝트가 풀에 있어서 이용할 수 있는 상태로 세팅
	virtual bool SetAvailableState();

	/// 오브젝트의 사용 유무
	virtual bool IsUsed() const;

	/// 오브젝트 풀에서 빠져 나올 때 수행
	virtual bool Leave(void* initParam_);

	/// 오브젝트 풀에 입장할 때 수행
	virtual bool Enter();

public:
	CVirtualPoolObject();
	virtual ~CVirtualPoolObject();
};
