#ifndef __poolstategy_h__
#define __poolstategy_h__

#pragma once

//#include <ace/Singleton.h>
#include <ace/Malloc_T.h>
#include <ace/Thread_Mutex.h>
//#include <ace/TSS_T.h>
#include <ace/Containers.h>
#include "MemoryBlockPool.h"
#include "MessageBlockPool.h"

namespace HalfNetwork
{
	const uint32	StartChuckCount = 64;
	//////////////////////////////////////////////////////////////////////////
	template<class PoolType, class ChunkType>
	class FixedSizePoolT
	{
	public:
		typedef ACE_DLList<PoolType>					PoolList;
		typedef ACE_DLList_Iterator<PoolType>	PoolIterator;

		FixedSizePoolT(uint32 chunkSize);
		~FixedSizePoolT();

		ChunkType*	Allocate(const size_t bufferSize);
		void	Deallocate(ChunkType* p);
		void	Dump();
		size_t GetPoolCount() const;

	protected:
		ChunkType*	AllocateFromAnotherPool(const size_t bufferSize)
		{
			return NULL;
		}
		void	MakePool();

	private:
		PoolType*						_activePool;
		PoolList						_poolList;
		uint32							_chunkSize;
		uint32							_nextChunkCount;
		ACE_Thread_Mutex		_lock;
	};


	//////////////////////////////////////////////////////////////////////////
	const uint8	MessageBlockPoolCount = 10;
	template<class PoolType, class ChunkType>
	class FlexibleSizePoolT
	{
	public:
		FlexibleSizePoolT();
		~FlexibleSizePoolT();
		ChunkType*	Allocate(const size_t bufferSize);
		void	Deallocate(ChunkType* p, size_t size);
		void	Dump();
		FixedSizePoolT<PoolType, ChunkType>*			AddBlockPool(const size_t bufferSize);

	protected:
		bool	GetProperSize( const size_t bufferSize, uint32& chunkSize, uint8& poolIndex );

	private:
		///////////////////////////////////////////////////
		// Description:
		// Thread Local Storage(Thread Specific Storage) 
		///////////////////////////////////////////////////
		//ACE_TSS<FixedSizePool*> _pool;
		FixedSizePoolT<PoolType, ChunkType>* _pool[MessageBlockPoolCount];
	};

	#include "PoolStrategy.inl"

} // namespace HalfNetwork

#endif // __poolstategy_h__