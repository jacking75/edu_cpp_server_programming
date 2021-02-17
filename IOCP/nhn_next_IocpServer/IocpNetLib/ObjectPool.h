#pragma once


#include "Exception.h"
#include "FastSpinlock.h"

template <class TOBJECT, int ALLOC_COUNT = 100>
class ObjectPool : public ClassTypeLock<TOBJECT>
{	
public:
// 	ObjectPool()
// 	{
// 		static_assert(std::is_polymorphic<TOBJECT>::value == false, "NO VIRTUAL TOBJECT!");
// 	}

	static void* operator new(size_t objSize)
	{
		using uint8_t = unsigned char;

		LockGuard criticalSection;

		if (!mFreeList)
		{
			mFreeList = new unsigned char[sizeof(TOBJECT)*ALLOC_COUNT];

			auto* pNext = mFreeList;
			auto ppCurr = reinterpret_cast<unsigned char**>(mFreeList);

			for (int i = 0; i < ALLOC_COUNT - 1; ++i)
			{
				/// OBJECT의 크기가 반드시 포인터 크기보다 커야 한다
				pNext += sizeof(TOBJECT);
				*ppCurr = pNext;
				ppCurr = reinterpret_cast<unsigned char**>(pNext);
			}

			*ppCurr = 0; ///< 마지막은 0으로 표시
			mTotalAllocCount += ALLOC_COUNT;
		}

		auto* pAvailable = mFreeList;
		mFreeList = *reinterpret_cast<unsigned char**>(pAvailable);
		++mCurrentUseCount;

		return pAvailable;
	}

	static void	operator delete(void* obj)
	{
		LockGuard criticalSection;

		CRASH_ASSERT(mCurrentUseCount > 0);

		--mCurrentUseCount;

		*reinterpret_cast<unsigned char**>(obj) = mFreeList;
		mFreeList = static_cast<unsigned char*>(obj);
	}


private:
	static unsigned char*	mFreeList;
	static int		mTotalAllocCount; ///< for tracing
	static int		mCurrentUseCount; ///< for tracing

};


template <class TOBJECT, int ALLOC_COUNT>
unsigned char* ObjectPool<TOBJECT, ALLOC_COUNT>::mFreeList = nullptr;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mTotalAllocCount = 0;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mCurrentUseCount = 0;


