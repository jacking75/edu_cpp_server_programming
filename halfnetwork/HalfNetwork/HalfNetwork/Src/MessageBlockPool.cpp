#include <string>
#include "HalfNetworkType.h"
#include "MessageBlockPool.h"

namespace HalfNetwork
{

	MessageBlockPool::MessageBlockPool(size_t size, uint32 chunkCount) : 
		_strategyAllocator(chunkCount, size), 
		_dataBlockAllocator(chunkCount, sizeof(ACE_Data_Block)), 
		_messageBlockAllocator(chunkCount, sizeof(ACE_Message_Block)),
		_maxBufferSize(size),
		_maxChunkCount(chunkCount)
	{
	}

	MessageBlockPool::~MessageBlockPool()
	{
	}

	ACE_Message_Block* MessageBlockPool::Allocate(const size_t bufferSize)
	{
		//HALF_DEBUG("Message Allocate RequestSize(%d) MaxSize(%d), MaxChunk(%d), RemainChunk(%d)\n", 
		//	bufferSize, _maxBufferSize, _maxChunkCount, AvailableChunkCount());
		//if (bufferSize > _maxBufferSize)
		//	return NULL;
		//return new ACE_Message_Block(bufferSize);
		if (_messageBlockAllocator.pool_depth() == 0)
			return NULL;
		if (_dataBlockAllocator.pool_depth() == 0)
			return NULL;
		if (_strategyAllocator.pool_depth() == 0)
			return NULL;

		ACE_Message_Block* block = NULL;
		char* newBlock = (char*)_messageBlockAllocator.malloc(sizeof(ACE_Message_Block));
		if (NULL != newBlock)
		{
			block = new (newBlock)ACE_Message_Block(
										bufferSize,
										ACE_Message_Block::MB_DATA,
										0,
										0,
										&_strategyAllocator,
										0,
										ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
										ACE_Time_Value::zero,
										ACE_Time_Value::max_time,
										&_dataBlockAllocator,
										&_messageBlockAllocator);
		}
		else
		{
			//block = new ACE_Message_Block(bufferSize);
			return NULL;
		}

		ACE_ASSERT(NULL != block->data_block());
		return block;
	}

	void MessageBlockPool::Dump()
	{
		//HALF_DEBUG("MessageBlockPool size(%d) max(%d) available(%d).\n", 
		//		_maxBufferSize, _maxChunkCount, _messageBlockAllocator.pool_depth());
	}

	uint32 MessageBlockPool::AvailableChunkCount()
	{
		return (uint32)_messageBlockAllocator.pool_depth();
	}

	uint32 MessageBlockPool::MaxChunkCount()
	{
		return _maxChunkCount;
	}

	uint32 MessageBlockPool::ChunkSize()
	{
		return (uint32)_maxBufferSize;
	}
} // namespace HalfNetwork