#include "stdafx.h"
#include "VirtualPoolObject.h"
#include "VirtualMemoryPool.h"
#include "Define.h"
#ifdef __TBB_malloc_proxy
#include "tbb/tbbmalloc_proxy.h"
#else
#include "tcmalloc.h"
#endif


template<typename T>
bool CVirtualMemmoryPool<T>::GetPoolObject(__out T*& out_, void* initParam_)
{
	CVirtualPoolObject* obj;
	if (m_pool.empty() || !m_pool.try_pop(obj)) {
		if (AllocateEmptyObject(m_intervalSize) > 0)
		{
			if (!m_pool.try_pop(obj))
			{
				// 로그
				return false;
			}
		}
		else
		{
			// 로그
			return false;
		}
	}

	if (!obj->SetUnAvailableState())
	{
		// 로그
		return false;
	}

	if (!obj->Leave(initParam_))
	{
		return false;
	}

	out_ = static_cast<T*>(obj);

	return true;
}


template<typename T>
bool CVirtualMemmoryPool<T>::BackPoolObject(__in T* obj_)
{
	CVirtualPoolObject* obj = static_cast<CVirtualPoolObject*>(obj_);
	if (!obj->SetAvailableState())
	{
		// 로그
		return false;
	}

	if (!obj->Enter())
	{
		return false;
	}

	m_pool.push(obj);
	
	return true;
}


template<typename T>
int CVirtualMemmoryPool<T>::AllocateEmptyObject(const int length_)
{
	for (int i = 0; i < length_; ++i)
	{		
		T* empty = ALLOCATE_POOL_MALLOC(T, sizeof(T));
		if (empty == NULL)
		{
			//로그
			return i;
		}
		new(empty) T();
		m_pool.push(empty);
		++m_totalPoolSize;
		
	}
	return length_;
}


template<typename T>
bool CVirtualMemmoryPool<T>::SetIntervalSize(const int length_)
{
	if (length_ <= 0)
	{
		// 로그
		return false;
	}
	m_intervalSize = length_;
	return true;
}


template<typename T>
void CVirtualMemmoryPool<T>::ReleasePool()
{
	while (!m_pool.empty())
	{
		CVirtualPoolObject* obj;
		m_pool.try_pop(obj);

		obj->~CVirtualPoolObject();
		SAFE_POOL_FREE(obj);

	}
}


template<typename T>
CVirtualMemmoryPool<T>::CVirtualMemmoryPool()
	: m_intervalSize(1),
	m_totalPoolSize(0)
{
}


template<typename T>
CVirtualMemmoryPool<T>::CVirtualMemmoryPool(const int length_, const int intervalSize_)
{
	if (length_ != AllocateEmptyObject(length_))
	{
		//로그
	}
	m_intervalSize = intervalSize_;
}


template<typename T>
CVirtualMemmoryPool<T>::~CVirtualMemmoryPool()
{
	ReleasePool();
}

