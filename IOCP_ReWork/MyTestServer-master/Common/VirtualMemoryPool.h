#pragma once

#include <tbb/concurrent_queue.h>

class CVirtualPoolObject;

///////////////////////////////////////////////////////////////////////////////
// 상속용 특정 타입의 오브젝트 풀
// <오브젝트 타입>
// (최소 생성 길이, 늘어나는 간격)
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class CVirtualMemmoryPool 
{
public:
	typedef tbb::concurrent_queue<CVirtualPoolObject*> OBJECT_POOL_QUEUE;

private:
	OBJECT_POOL_QUEUE m_pool;       ///< 오브젝트 풀 큐
	int m_intervalSize;             ///< 오브젝트 큐가 가득 찼을 경우 새로 생성되는 사이즈
	int m_totalPoolSize;            ///< 생성된 풀 총 사이즈

public:
	/// 오브젝트 풀에서 객체를 획득
	bool GetPoolObject(__out T*& out_, void* initParam_ = NULL);

	/// 오브젝트 풀에서 객체를 반환
	bool BackPoolObject(__in T* obj_);

	/// 큐가 가득 찼을 경우 새로 생성되는 사이즈 세팅
	bool SetIntervalSize(const int length_);

	/// 비어있는 오브젝트들을 해당 크기만큼 오브젝트 풀에 할당
	int AllocateEmptyObject(const int length_);

	/// 풀에 있는 메모리 해제
	void ReleasePool();

public:
	CVirtualMemmoryPool();
	CVirtualMemmoryPool(const int length_, const int intervalSize_);
	virtual ~CVirtualMemmoryPool();
};
