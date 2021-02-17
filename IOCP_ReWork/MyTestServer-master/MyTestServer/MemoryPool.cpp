#include "stdafx.h"
#include "MemoryPoolManager.h"
#include "tcmalloc.h"

void* CMemoryPoolManager::GetPoolObject(size_t const size_)
{
	void* obj;
#if __tcmalloc
	obj = tc_malloc(size_);
#else
	OBJECT_POOL_QUEUE* pool;
	POOL_MAP::iterator it = m_mapPool.find(size_);
	if (it == m_mapPool.end())
	{
		OBJECT_POOL_QUEUE* pool = AllocateEmptyObject(size_, m_intervalSize);
		if (pool == NULL)
		{
			// 로그
			return NULL;
		}

		if (!pool->try_pop(obj))
		{
			// 로그
			return NULL;
		}
	}
	else
	{
		OBJECT_POOL_QUEUE* pool = (*it).second;
		if (!pool->try_pop(obj))
		{
			pool = AllocateEmptyObject(size_, m_intervalSize);
			if (pool == NULL)
			{
				// 로그
				return NULL;
			}

			if (!pool->try_pop(obj))
			{
				// 로그
				return NULL;
			}
		}
	}

	std::pair<SIZE_POOL_MAP::iterator, bool> ret = m_mapSizePool.insert(std::make_pair(obj, pool));
	if (!ret.second)
	{
		// 로그
		return NULL;
	}


#endif
	return obj;
}


bool CMemoryPoolManager::BackPoolObject(__in void* obj_)
{
#if __tcmalloc
	tc_free(obj_);
#else
	SIZE_POOL_MAP::iterator it = m_mapSizePool.find(obj_);
	if (it == m_mapSizePool.end())
	{
		// 로그
		return false;
	}

	OBJECT_POOL_QUEUE* pool_queue = (*it).second;
	pool_queue->push(obj_);
#endif	
	return true;
}
	

bool CMemoryPoolManager::SetIntervalSize(const int length_)
{
	if (length_ <= 0)
	{
		// 로그
		return false;
	}
	m_intervalSize = length_;

	return true;
}


CMemoryPoolManager::OBJECT_POOL_QUEUE* CMemoryPoolManager::AllocateEmptyObject(size_t size_, const int length_)
{
	POOL_MAP::iterator it = m_mapPool.find(size_);		
	OBJECT_POOL_QUEUE* pool_queue;
	if (it == m_mapPool.end())
	{
		/// 해당 사이즈가 없을 경우
		pool_queue = new OBJECT_POOL_QUEUE;
		if (pool_queue == NULL)
		{
			// 로그
			return NULL;
		}
			
		std::pair<POOL_MAP::iterator, bool> ret = m_mapPool.insert(std::make_pair(size_, pool_queue));
		if (!ret.second)
		{
			// 로그
			return NULL;
		}
	}
	else
	{
		/// 이미 해당 사이즈가 있을 경우
		pool_queue = (*it).second;
	}

	for (int i = 0; i < length_; ++i)
	{
		void* empty = new char[size_];
		if (empty == NULL)
		{
			// 로그
			break;
		}
		pool_queue->push(empty);
	}
	return pool_queue;
}


CMemoryPoolManager::CMemoryPoolManager() : 
	m_intervalSize(1)
{
}


CMemoryPoolManager::~CMemoryPoolManager()
{
}

