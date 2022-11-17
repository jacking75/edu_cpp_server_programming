#include <string>
#include "HalfNetworkType.h"
#include "MemoryBlockPool.h"

namespace HalfNetwork
{

	//////////////////////////////////////////////////////////////////////////
	MemoryBlockPool::MemoryBlockPool(size_t size, uint32 chunkCount) : 
		_chunkAllocator(chunkCount, size), 
		_maxBufferSize(size),
		_maxChunkCount(chunkCount)
	{
	}

	MemoryBlockPool::~MemoryBlockPool()
	{
	}

	void* MemoryBlockPool::Allocate(const size_t bufferSize)
	{
		//HALF_DEBUG("Memory Allocate RequestSize(%d) MaxSize(%d), MaxChunk(%d), RemainChunk(%d)\n", 
		//	bufferSize, _maxBufferSize, _maxChunkCount, AvailableChunkCount());
		return _chunkAllocator.malloc(bufferSize);
	}

	void MemoryBlockPool::Dump()
	{
		//HALF_DEBUG ("MemoryBlockPool Size(%d) MaxChunk(%d) RemainChunk(%d).\n", 
		//	_maxBufferSize, _maxChunkCount, _chunkAllocator.pool_depth());
	}

	uint32 MemoryBlockPool::AvailableChunkCount()
	{
		return (uint32)_chunkAllocator.pool_depth();
	}

	uint32 MemoryBlockPool::MaxChunkCount()
	{
		return _maxChunkCount;
	}

	void MemoryBlockPool::Deallocate( void* p )
	{
		_chunkAllocator.free(p);
	}

	uint32 MemoryBlockPool::ChunkSize()
	{
		return (uint32)_maxBufferSize;
	}
} // namespace HalfNetwork