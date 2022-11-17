#ifndef __memoryblockpool_h__
#define __memoryblockpool_h__

#pragma once

//#include <ace/Singleton.h>
#include <ace/Malloc_T.h>
#include <ace/Thread_Mutex.h>
//#include <ace/TSS_T.h>
#include <ace/Containers.h>

namespace HalfNetwork
{
	//////////////////////////
	// Description:
	//   
	//////////////////////////
	class MemoryBlockPool
	{
	public:
		MemoryBlockPool(size_t size, uint32 chunkCount);
		~MemoryBlockPool();

	public:
		void*		Allocate(const size_t bufferSize);
		void		Deallocate(void* p);
		void		Dump();
		uint32	AvailableChunkCount();
		uint32	MaxChunkCount();
		uint32	ChunkSize();

	private:
		typedef ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex> CachedAllocator;
		CachedAllocator		_chunkAllocator;

	private:
		size_t				_maxBufferSize;
		uint32				_maxChunkCount;
	};

} // namespace HalfNetwork


#endif // __memoryblockpool_h__