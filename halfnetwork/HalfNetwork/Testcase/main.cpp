// Testcase.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <time.h>
#include <ace/ACE.h>
#include <ace/OS_NS_unistd.h>
#include "NetworkFacade.h"
#include "ProactorFactory.h"
#include "ReactorFactory.h"
#include "TimerUtil.h"
#include "MessageBlockUtil.h"
#include "SimpleConfig.h"
#include "EchoBackServer.h"
#include "InterlockedValue.h"
#include "TsidMap.h"
#include "TsQueueWithEvent.h"
#include "SyncSocket.h"

class CTestFunctionPrint
{
public:
	CTestFunctionPrint(const char* functionname) : m_functionname(functionname)
	{
		printf("Begin %s\n", m_functionname);
	}

	~CTestFunctionPrint()
	{
		printf("End.. %s\n", m_functionname);
	}

private:
	const char* m_functionname;
};

using namespace HalfNetwork;

template<class EventModelT>
class NetworkTestcase
{
protected:
	void DoEchoTest(ESendMode mode, uint32 port, uint8 queueID)
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		// startup
		NetworkInstance->Create<EventModelT>();
		NetworkInstance->SetSendMode(mode);
		ACE_TEST_ASSERT(NetworkInstance->AddAcceptor(ACE_TEXT(""), port, queueID));
		ACE_TEST_ASSERT(NetworkInstance->Open());

		// verify information setting
		NetworkFacadeInfo info;
		NetworkInstance->GetInformation(info);
		ACE_TEST_ASSERT(mode == info.send_mode);
		ACE_TEST_ASSERT(GetProperWorkerThreadCount() == info.worker_thread_count);
		ACE_TEST_ASSERT(0 ==ACE_OS::strcmp(GetEventModelName(), info.event_dispatch_model));

		// same port is not accepted
		ACE_TEST_ASSERT(false == NetworkInstance->AddAcceptor(NULL, port, queueID+1));
		// same Queue_ID is not accepted
		ACE_TEST_ASSERT(false == NetworkInstance->AddAcceptor(NULL, port+1, queueID));

		// get acceptor information
		ACE_Unbounded_Queue<AcceptorInfo>::ITERATOR acceptor_iter(info.acceptor_que);
		AcceptorInfo* acceptor_info = NULL;
		while(false == acceptor_iter.done())
		{
			acceptor_iter.next(acceptor_info);
			if (port == acceptor_info->port)
				break;
			acceptor_iter.advance();
		}

		// verify acceptor information
		ACE_TEST_ASSERT(port == acceptor_info->port);
		ACE_TEST_ASSERT(false == acceptor_info->suspend);
		ACE_TEST_ASSERT(queueID == acceptor_info->queue_id);
		//EXPECT_EQ(0, acceptor_info->ip);

		// echo server startup
		EchoBackServer echoServer(NetworkInstance);
		echoServer.RecvQueueID(queueID);
		echoServer.activate();

		// Echo client(send -> receive -> verify buffer)
		DoEchoClientTest(ACE_TEXT("127.0.0.1"), port);

		// cleanup
		echoServer.Terminate();
		NetworkInstance->Close();
		NetworkInstance->Destroy();
	}

	void DoEchoClientTest(const ACE_TCHAR* ip, uint32 port)
	{
		SyncSocket socket;
		// Connect Test
		const uint32 WaitTime = 1000;
		ACE_TEST_ASSERT(socket.Connect(ip, port, WaitTime));

		// echo test
		const int send_recv_test_count = 64;
		for (int j=0; j<send_recv_test_count; ++j)
		{
			const size_t buffer_size = 1024;
			char send_buffer[buffer_size] = {0,};
			char recv_buffer[buffer_size] = {0,};
			GenerateRandomBuffer((unsigned char*)send_buffer, buffer_size);
			ACE_TEST_ASSERT(socket.Send(send_buffer, buffer_size));
			ACE_TEST_ASSERT(buffer_size == socket.Receive(recv_buffer, buffer_size, WaitTime));
			// check same buffer
			ACE_TEST_ASSERT(0 == memcmp(send_buffer, recv_buffer, buffer_size));
			//printf("DoEchoClientTest %d\n", j);
		}
		socket.Disconnect();
	}

	uint32 GetProperWorkerThreadCount()
	{
		return 0;
	}

	const ACE_TCHAR* GetEventModelName()
	{
		return ACE_TEXT("");
	}

	void NetworkTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		const int server_port = 35892;
		const int Recv_Queue_ID = 10;
		DoEchoTest(eSM_Interval, server_port, Recv_Queue_ID);
		DoEchoTest(eSM_Direct, server_port, Recv_Queue_ID+1);
	}
	
private:
	void GenerateRandomBuffer(unsigned char* buffer, int len)
	{
		srand((unsigned int)time(NULL));
		for (int i=0; i<len; ++i)
			buffer[i] = rand()%255+1;
	}

public:
	void Run()
	{
		NetworkTest();
	}
};

template<>
uint32 NetworkTestcase<ProactorFactory>::GetProperWorkerThreadCount()
{
	return ACE_OS::num_processors_online()*2;
}

template<>
uint32 NetworkTestcase<ReactorFactory>::GetProperWorkerThreadCount()
{
	return 1;
}

template<>
const ACE_TCHAR* NetworkTestcase<ProactorFactory>::GetEventModelName()
{
	return ACE_TEXT("Proactor");
}
template<>
const ACE_TCHAR* NetworkTestcase<ReactorFactory>::GetEventModelName()
{
	return ACE_TEXT("Reactor");
}

struct TestElement
{
	int serial;
};

class UtilityTestcase
{
protected:
	void MakeRandomBuffer( char* buffer, uint32 len )
	{
		for(uint32 i=0; i<len; ++i)
			buffer[i] = rand();
	}

	void BlockTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		const uint32 BlockSize = 32;
		const uint32 BlockCount = 18;
		char buffer[BlockSize] = {0,};

		ACE_Message_Block* blockList = new ACE_Message_Block(BlockSize);
		MakeRandomBuffer(buffer, BlockSize);
		blockList->copy(buffer, BlockSize);	
		for(uint32 i=0; i<BlockCount-1; ++i)
		{
			ACE_Message_Block* tail = FindTailBlock(blockList);
			ACE_TEST_ASSERT(NULL != tail);
			ACE_Message_Block* block = new ACE_Message_Block(BlockSize);
			MakeRandomBuffer(buffer, BlockSize);
			block->copy(buffer, BlockSize);	
			tail->cont(block);
		}

		ACE_TEST_ASSERT(blockList->total_length() == BlockSize*BlockCount);
		ACE_Message_Block* mergedBlock = new ACE_Message_Block(blockList->total_length());
		ACE_TEST_ASSERT(MakeMergedBlock(blockList, mergedBlock));
		ACE_TEST_ASSERT(blockList->total_length() == mergedBlock->length());

		ACE_Message_Block* block = blockList;
		char* mergedBlockPtr = mergedBlock->rd_ptr();
		do 
		{
			char* blockListPtr = block->rd_ptr();		
			for(uint32 i=0; i<block->length(); ++i)
			{
				ACE_TEST_ASSERT(blockListPtr[i] == mergedBlockPtr[i]);
			}
			mergedBlockPtr += block->length();
		} while (NULL != (block = block->cont()));

		ACE_Message_Block* fakeBlock = new ACE_Message_Block(blockList->total_length()-2);
		ACE_TEST_ASSERT(false == MakeMergedBlock(blockList, fakeBlock));

		fakeBlock->release();
		blockList->release();
		mergedBlock->release();
	}

	void SimpleConfigTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		tstring targetString = ACE_TEXT("serial=10;name=alice;level=41;");
		HalfNetwork::SimpleConfig configObj;
		configObj.Parse(targetString);
		uint32 level = configObj.GetValue<uint32>(ACE_TEXT("level"));
		ACE_TEST_ASSERT(level == 41);
		uint32 serial = configObj.GetValue<uint32>(ACE_TEXT("serial"));
		ACE_TEST_ASSERT(serial == 10);
		tstring name = configObj.GetValue<tstring>(ACE_TEXT("name"));
		ACE_TEST_ASSERT(name == ACE_TEXT("alice"));
	}

	void SimpleConfigAdvancedTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		tstring targetString = ACE_TEXT("serial = 10;name=alice ;level  =41;");
		HalfNetwork::SimpleConfig configObj;
		configObj.Parse(targetString);
		uint32 level = configObj.GetValue<uint32>(ACE_TEXT("level"));
		ACE_TEST_ASSERT(level == 41);
		uint32 serial = configObj.GetValue<uint32>(ACE_TEXT("serial"));
		ACE_TEST_ASSERT(serial == 10);
		tstring name = configObj.GetValue<tstring>(ACE_TEXT("name"));
		ACE_TEST_ASSERT(name == ACE_TEXT("alice"));
	}

	void InterlockedValueTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		InterlockedValue interlockedValue;
		interlockedValue.Exchange(1);
		ACE_TEST_ASSERT(interlockedValue.Compare(1));
		ACE_TEST_ASSERT(interlockedValue.CompareExchange(2, 1));
		ACE_TEST_ASSERT(false == interlockedValue.CompareExchange(2, 1));
		ACE_TEST_ASSERT(interlockedValue.Compare(2));

		ACE_TEST_ASSERT(interlockedValue.CompareExchange(0, 2));
		ACE_TEST_ASSERT(interlockedValue.Compare(0));
		ACE_TEST_ASSERT(interlockedValue.Acquire());
		ACE_TEST_ASSERT(false == interlockedValue.Acquire());
		interlockedValue.Release();
		ACE_TEST_ASSERT(interlockedValue.Acquire());
	}

	void TsMapTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);

		const uint32 MaxElementCount = 12;
		TsIDMapT<TestElement, MaxElementCount> container;
		TestElement elementArray[MaxElementCount];
		for(uint32 i=0; i<MaxElementCount; ++i)
			elementArray[i].serial = i;

		uint32 id0 = container.Register(&elementArray[0]);
		ACE_TEST_ASSERT(id0 != Invalid_ID);
		TestElement* t0 = container.Get(id0);
		ACE_TEST_ASSERT(NULL != t0);
		ACE_TEST_ASSERT(1 == (int)container.Size());
		container.UnRegister(id0);
		ACE_TEST_ASSERT(0 == (int)container.Size());

		for(uint32 i=0; i<MaxElementCount; ++i)
			container.Register(&elementArray[i]);
		ACE_TEST_ASSERT(MaxElementCount == container.Size());
		TestElement tempElement;
		tempElement.serial = 99;
		uint32 id1 = container.Register(&tempElement);
		ACE_TEST_ASSERT(id1 == Invalid_ID);

		for(uint32 i=0; i<MaxElementCount; ++i)
			container.UnRegister(i);
		ACE_TEST_ASSERT(0 == container.Size());
	}

	void TsQueueWithEventTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		TSQueueWithEventT<TestElement> container;
		container.Init();
		TestElement elem0;
		const int waitTime = 200;
		unsigned int tick = HalfNetwork::GetTick();
		ACE_TEST_ASSERT(false == container.Pop(elem0, waitTime));
		unsigned int actualTerm = HalfNetwork::GetTick()-tick;
		ACE_TEST_ASSERT( abs((int)(actualTerm-waitTime)) < 20 );
		TestElement elem1;
		elem1.serial = 99;
		container.Push(elem1);
		ACE_TEST_ASSERT(1 == container.Size());
		tick = HalfNetwork::GetTick();
		ACE_TEST_ASSERT(container.Pop(elem0, waitTime));
		actualTerm = HalfNetwork::GetTick()-tick;
		ACE_TEST_ASSERT( actualTerm < 10 );
	}

public:
	void Run()
	{
		BlockTest();
		SimpleConfigTest();
		SimpleConfigAdvancedTest();
		InterlockedValueTest();
		TsMapTest();
		TsQueueWithEventTest();
	}
};

template<class PoolType, class ChunkType>
class BlockPoolTestcase
{
protected:
	void AllocateTest()
	{
		CTestFunctionPrint autoPrint(__FUNCTION__);
		const int TestCount = 8;
		for (int i=0;i<TestCount; ++i)
		{
			uint32 chunkSize = rand()%2040 + 6;
			FixedSizePoolT<PoolType, ChunkType>	pool(chunkSize);
			uint32 chunkCount = StartChuckCount;
			for(uint32 i=0;i<10; ++i)
			{
				chunkCount *= 2;
				ACE_TEST_ASSERT(WorkingWithCorrectPoolCount(pool, chunkCount+3, chunkSize));
			}
		}
	}

	bool WorkingWithCorrectPoolCount(FixedSizePoolT<PoolType, ChunkType>& pool, uint32 testChunkCount, uint32 chunkSize)
	{
		std::vector<ChunkType*> blockVector;
		for(uint32 i=0; i<testChunkCount; ++i)
		{
			ChunkType* b0 = pool.Allocate(chunkSize);
			ACE_TEST_ASSERT(NULL != b0);
			SizeCheck(b0, chunkSize);
			blockVector.push_back(b0);
		}
		size_t poolCount0 = pool.GetPoolCount();
		for(uint32 i=0; i<testChunkCount; ++i)
			ChunkRelease(pool, blockVector[i]);

		for(uint32 i=0; i<testChunkCount; ++i)
		{
			ChunkType* b0 = pool.Allocate(chunkSize);
			ACE_TEST_ASSERT(NULL != b0);
			SizeCheck(b0, chunkSize);
			blockVector.push_back(b0);
		}
		size_t poolCount1 = pool.GetPoolCount();
		return (poolCount0 == poolCount1);
	}

	void SizeCheck(ChunkType* p, uint32 chunkSize)
	{
		ACE_TEST_ASSERT(false);
	}

	void ChunkRelease(FixedSizePoolT<PoolType, ChunkType>& pool, ChunkType* p)
	{
		ACE_TEST_ASSERT(false);
	}

public:
	void Run()
	{
		AllocateTest();
	}
};

template<>
inline void BlockPoolTestcase<MessageBlockPool, ACE_Message_Block>::SizeCheck(ACE_Message_Block* p, uint32 chunkSize)
{
	ACE_TEST_ASSERT(p->size() >= chunkSize);
}

template<>
inline void BlockPoolTestcase<MemoryBlockPool, void>::SizeCheck(void* p, uint32 chunkSize)
{
}

template<>
inline void BlockPoolTestcase<MessageBlockPool, ACE_Message_Block>::ChunkRelease(FixedSizePoolT<MessageBlockPool, ACE_Message_Block>& pool, ACE_Message_Block* p)
{
	p->release();
}

template<>
inline void BlockPoolTestcase<MemoryBlockPool, void>::ChunkRelease(FixedSizePoolT<MemoryBlockPool, void>& pool, void* p)
{
	pool.Deallocate(p);
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
	srand((unsigned int)time(NULL));
#ifdef WIN32
	NetworkTestcase<ProactorFactory> proactorTestcase;
	proactorTestcase.Run();
#endif
	NetworkTestcase<ReactorFactory> reactorTestcase;
	reactorTestcase.Run();
	UtilityTestcase utilTestcase;
	utilTestcase.Run();
	BlockPoolTestcase<MessageBlockPool, ACE_Message_Block> messageBlockTestcase;
	messageBlockTestcase.Run();
	BlockPoolTestcase<MemoryBlockPool, void> memoryBlockTestcase;
	memoryBlockTestcase.Run();

	printf("HalfNetwork test success.\n");
	getchar();
	return 0;
}

