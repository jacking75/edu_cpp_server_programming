
//////////////////////////////////////////////////////////////////////////
template<class PoolType, class ChunkType>
FixedSizePoolT<PoolType, ChunkType>::FixedSizePoolT(uint32 chunkSize)
	: _activePool(NULL),
	_chunkSize(chunkSize),
	_nextChunkCount(StartChuckCount)
{
	MakePool();
}

template<class PoolType, class ChunkType>
FixedSizePoolT<PoolType, ChunkType>::~FixedSizePoolT()
{
	PoolType* p = NULL;
	while ((p = _poolList.delete_head()) != 0)
		SAFE_DELETE(p);
	//SAFE_DELETE(_activePool);
}

template<class PoolType, class ChunkType>
ChunkType*	FixedSizePoolT<PoolType, ChunkType>::Allocate(const size_t bufferSize)
{
	ACE_Guard<ACE_Thread_Mutex> guard(_lock);
	ChunkType* resultBlock = _activePool->Allocate(bufferSize);
	if (NULL != resultBlock)
		return resultBlock;

	return AllocateFromAnotherPool(bufferSize);
}

template<class PoolType, class ChunkType>
void	FixedSizePoolT<PoolType, ChunkType>::Deallocate(ChunkType* p)
{
	_activePool->Deallocate(p);
}

template<class PoolType, class ChunkType>
void	FixedSizePoolT<PoolType, ChunkType>::Dump()
{
	//HALF_DEBUG("InactivePool.\n", 0);
	PoolIterator iter(_poolList);
	while(0 == iter.done())
	{
		//iter.next()->Dump();
		++iter;
	}
	//HALF_DEBUG("CurrentPool.\n", 0);
	_activePool->Dump();
}

// For MessageBlockPool
template<>
inline ACE_Message_Block*	FixedSizePoolT<MessageBlockPool, ACE_Message_Block>::AllocateFromAnotherPool(const size_t bufferSize)
{
	ACE_Message_Block* resultBlock = NULL;
	PoolIterator iter(_poolList);
	while(0 == iter.done())
	{
		resultBlock = iter.next()->Allocate(bufferSize);
		if (NULL != resultBlock)
		{
			_activePool = iter.next();
			//HALF_DEBUG("ChangePool Size(%d) MaxChunk(%d) RemainChunk(%d).\n",
			//	bufferSize, _activePool->MaxChunkCount(), _activePool->AvailableChunkCount());
			return resultBlock;
		}
		++iter;
	}
	MakePool();
	return _activePool->Allocate(bufferSize);
}

// For MemoryBlockPool
template<>
inline void*	FixedSizePoolT<MemoryBlockPool, void>::AllocateFromAnotherPool(const size_t bufferSize)
{
	MakePool();
	return _activePool->Allocate(bufferSize);
}

template<class PoolType, class ChunkType>
void	FixedSizePoolT<PoolType, ChunkType>::MakePool()
{
	//HALF_DEBUG("MakePool Size(%d) ChunkCount(%d).\n", _chunkSize, _nextChunkCount);
	PoolType* newPool = new PoolType(_chunkSize, _nextChunkCount);
	_poolList.insert_tail(newPool);
	_activePool = newPool;
	_nextChunkCount *= 2;
}

template<class PoolType, class ChunkType>
size_t FixedSizePoolT<PoolType, ChunkType>::GetPoolCount() const
{
	return _poolList.size();
}

//////////////////////////////////////////////////////////////////////////
template<class PoolType, class ChunkType>
FlexibleSizePoolT<PoolType, ChunkType>::FlexibleSizePoolT()
{
	for (uint8 i=0;i<MessageBlockPoolCount;++i)
		_pool[i] = NULL;
}

template<class PoolType, class ChunkType>
FlexibleSizePoolT<PoolType, ChunkType>::~FlexibleSizePoolT()
{
	for (uint8 i=0;i<MessageBlockPoolCount;++i)
		SAFE_DELETE(_pool[i]);
}

template<class PoolType, class ChunkType>
ChunkType*	FlexibleSizePoolT<PoolType, ChunkType>::Allocate(const size_t bufferSize)
{
	uint32 chunkSize = 0;
	uint8 poolIndex = 0;
	bool result = GetProperSize(bufferSize, chunkSize, poolIndex);
	if (false == result)
		return NULL;
	FixedSizePoolT<PoolType, ChunkType>* properPool = _pool[poolIndex];
	//if (properPool == NULL)
	//	return NULL;

	if (properPool == NULL)
		properPool = AddBlockPool(bufferSize);

	ACE_ASSERT(NULL != properPool);
	return properPool->Allocate(bufferSize);
}

template<class PoolType, class ChunkType>
void	FlexibleSizePoolT<PoolType, ChunkType>::Deallocate(ChunkType* p, size_t size)
{
	uint32 chunkSize = 0;
	uint8 poolIndex = 0;
	bool result = GetProperSize(size, chunkSize, poolIndex);
	ACE_ASSERT(result);
	FixedSizePoolT<PoolType, ChunkType>* properPool = _pool[poolIndex];
	ACE_ASSERT(NULL != properPool);
	properPool->Deallocate(p);
}

template<class PoolType, class ChunkType>
FixedSizePoolT<PoolType, ChunkType>* FlexibleSizePoolT<PoolType, ChunkType>::AddBlockPool(const size_t bufferSize)
{
	uint32 chunkSize = 0;
	uint8 poolIndex = 0;
	bool result = GetProperSize(bufferSize, chunkSize, poolIndex);
	ACE_ASSERT(result);
	ACE_ASSERT(poolIndex < MessageBlockPoolCount);
	ACE_ASSERT(NULL == _pool[poolIndex]);
	_pool[poolIndex] = new FixedSizePoolT<PoolType, ChunkType>(chunkSize);
	return _pool[poolIndex];
}

template<class PoolType, class ChunkType>
void FlexibleSizePoolT<PoolType, ChunkType>::Dump()
{
	for (uint8 i=0;i<MessageBlockPoolCount;++i)
	{
		if (NULL != _pool[i])
			_pool[i]->Dump();
	}
}

template<class PoolType, class ChunkType>
bool FlexibleSizePoolT<PoolType, ChunkType>::GetProperSize( const size_t bufferSize, uint32& chunkSize, uint8& poolIndex )
{
	const uint8	StartChunkSize = 8;
	chunkSize = StartChunkSize;
	for (;poolIndex<MessageBlockPoolCount;++poolIndex)
	{
		if (bufferSize <= chunkSize)
			break;
		chunkSize *= 2;
	}
	return (poolIndex < MessageBlockPoolCount);
}



