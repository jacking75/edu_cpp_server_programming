#ifndef __messageblockpool_h__
#define __messageblockpool_h__

#pragma once

#include <ace/Malloc_T.h>
#include <ace/Message_Block.h>
#include <ace/Thread_Mutex.h>
//#include <ace/TSS_T.h>
#include <ace/Containers.h>

namespace HalfNetwork
{

	//////////////////////////
	// Description:
	//   Allocate Message_Block
	//////////////////////////
	class MessageBlockPool
	{
	public:
		MessageBlockPool(size_t size, uint32 chunkCount);
		~MessageBlockPool();

	public:
		ACE_Message_Block*	Allocate(const size_t bufferSize);
		void								Dump();
		uint32							AvailableChunkCount();
		uint32							MaxChunkCount();
		uint32							ChunkSize();

	private:
		typedef ACE_Dynamic_Cached_Allocator<ACE_Thread_Mutex> CachedAllocator;

		/////////////////////////////////////////////////////
		// Description:
		// Allocators for Message_Block
		// To make one Message_Block, 3 allocators required
		/////////////////////////////////////////////////////
		CachedAllocator		_strategyAllocator;			// for Buffer
		CachedAllocator 	_dataBlockAllocator;		// for DataBlock
		CachedAllocator 	_messageBlockAllocator;		// for MessageBlock

	private:
		size_t				_maxBufferSize;
		uint32				_maxChunkCount;
	};

} // namespace HalfNetwork

#endif // __messageblockpool_h__