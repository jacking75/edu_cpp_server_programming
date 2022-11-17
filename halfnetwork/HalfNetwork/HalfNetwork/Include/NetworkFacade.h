#ifndef __networkfacade_h__
#define __networkfacade_h__

#pragma once

#include "AbstractFactory.h"
#include <ace/Containers.h>
#include <ace/Message_Block.h>
#include <ace/Task.h>
#include "PoolStrategy.h"
#include <ace/Singleton.h>
#include "HalfNetworkDefine.h"

class ACE_Handler;

namespace HalfNetwork
{
	typedef ACE_Unbounded_Queue<AcceptorInfo> AcceptorQueue;
	struct NetworkFacadeInfo 
	{
		uint8			worker_thread_count;
		ACE_TCHAR			event_dispatch_model[FACTORY_NAME_SIZE];
		AcceptorQueue	acceptor_que;
		ESendMode		send_mode;
		uint32			receive_buffer_len;
		uint32			interval_send_term;
		uint32			service_count;
	};

	const uint8 TimerQueueID = 11;
	
	class MessageQueueRepository;

	class NetworkFacade
	{
	public:
		typedef ACE_DLList<AbstractAcceptor>			AcceptorList;
		typedef ACE_DLList_Iterator<AbstractAcceptor>	AcceptorIter;

	public:
		NetworkFacade();
		virtual ~NetworkFacade();

	public:
		///////////////////////////////////////////////////////
		// Description:
		//   Start listen, start worker thread and so on
		//   Before Open you must Create
		// See Also: Create
		// Arguments:
		//   config - all about config values
		// Return:
		//   success or not
		///////////////////////////////////////////////////////
		bool	Open(SystemConfig* const config = NULL);
		void	Close();

	public:
		///////////////////////////////////////////////////////
		// Description:
		//   Configure Event model
		// Arguments:
		//   factory - EventModel factory
		// Return:
		//   success or not
		///////////////////////////////////////////////////////
		template <class EventModel>
		bool	Create()
		{
			return Create(new EventModel);
		}
		void	Destroy();

	public:
		/////////////////////////////////////////////////////////////////////////
		// Arguments:
		//   acceptIp - allowed ip. if you don't know much, just put NULL or ""
		//   port - accept port
		//   queueId - Related queue id
		// Return:
		//   success or not
		/////////////////////////////////////////////////////////////////////////
		bool	AddAcceptor(uint16 port, uint8 queueId);

		bool	AddAcceptor(const ACE_TCHAR* acceptIp, 
											uint16 port, 
											uint8 queueId);

		bool	AddAcceptor(const ACE_TCHAR* acceptIp, 
											uint16 port, 
											uint8 queueId, 
											uint32 receiveBufferSize);

		bool	AddAcceptor(const ACE_TCHAR* acceptIp, 
											uint16 port, 
											uint8 queueId, 
											uint32 receiveBufferSize, 
											uint32 initialAcceptCount);

		bool AddTimer(uint32 timerID, uint32 interval, uint32 start = 0);

		/////////////////////////////////////////////////////////////////////////
		// Description:
		//   Connect right now. Result will be informed Message through queue 
		//   No accepting, No message.
		//   If you want to check "Are there any accepting?", use TryConnect()
		// Arguments:
		//   queueId - Related queue id
		/////////////////////////////////////////////////////////////////////////
		bool	Connect(const ACE_TCHAR* ip, uint16 port, uint8 queueId);
		bool	Connect(const ACE_TCHAR* ip, 
                  uint16 port, 
                  uint8 queueId, 
                  uint32 receiveBufferSize);

		bool	AsynchConnect(const ACE_TCHAR* ip, 
                        uint16 port, 
                        uint8 queueId);
		bool	AsynchConnect(const ACE_TCHAR* ip, 
                        uint16 port, 
                        uint8 queueId, 
                        uint32 receiveBufferSize);

		/////////////////////////////////////////////////////////////////////////
		// Description:
		//   Just check the ip and port are accepting for now.
		//	 If you want send/recv packet, you must use Connect() method
		//////////////////////////////////////////////////////////////////////////
		bool	TryConnect(const ACE_TCHAR* ip, 
										 uint16 port, 
										 uint32 waitMs) const;

	public:
		////////////////////////////////////////////////////////////
		// Description:
		//   Get Message from specified MessageQueue(queue_id)
		// Arguments:
		//   timeout : wait time(millisecond) / -1 means infinite
		///////////////////////////////////////////////////////////
		bool	PopMessage(uint8 queueId, ACE_Message_Block** block, int timeout);
		bool	PopAllMessage(uint8 queueId, ACE_Message_Block** block, int timeout);

		/////////////////////////////////////////////////////
		// Description:
		//   Get Message from whole MessageQueue(make chain)
		/////////////////////////////////////////////////////
		bool	PopAllMessage(ACE_Message_Block** block, int timeout);

		/////////////////////////////////////////////////////
		// Description:
		//   Push user custom message block
		/////////////////////////////////////////////////////
		bool	PushCustomMessage(uint8 queId, ACE_Message_Block* block);
		bool	PushTimerMessage(uint32 timerID);

		/////////////////////////////////////////////////////
		// Description:
		//   Wake up thread which wait PopMessage
		/////////////////////////////////////////////////////
		void	Pulse();
		void	Pulse(uint8 queueId);

	public:
		///////////////////////////////////////////////////////////////////////////////////
		// Description:
		//   Send block or buffer. 
		//   Directly(eSM_Direct) or after a while(eSM_Interval) depend on mode(ESendMode)
		///////////////////////////////////////////////////////////////////////////////////
		bool	SendRequest(uint32 streamId, 
											ACE_Message_Block* block, 
											bool copy_block = false);
		bool	SendRequest(uint32 streamId, const char* buffer, uint32 length);

		///////////////////////////////////////////////////////////////////////////////////
		// Description:
		//   Send buffer delayed time
		// Arguments:
		//   delay : delayed time(millisecond)
		///////////////////////////////////////////////////////////////////////////////////
		bool	SendReserve(uint32 streamId, const char* buffer, uint32 length, uint32 delay);

		/////////////////////////////////////////////////
		// Description:
		//   Close connection
		/////////////////////////////////////////////////
		void	CloseStream(uint32 streamID);
		void	CloseReceiveStream(uint32 streamID);

	public:
		/////////////////////////////////////////////////////
		// Description:
		//   Setup options
		/////////////////////////////////////////////////////
		void	SetWorkerThreadCount(uint8 count);
		void	SetSendMode(ESendMode mode);
		void	SetReceiveBufferLen(uint32 length);
		void	SetIntervalSendTerm(uint32 ms);

	public:
		void	GetInformation(NetworkFacadeInfo& info);

		/////////////////////////////////////////////////////
		// Description:
		//   Show current status
		/////////////////////////////////////////////////////
		void	Dump();

		/////////////////////////////////////////////////////
		// Description:
		//   Allocate a MessageBlock from pool
		/////////////////////////////////////////////////////
		ACE_Message_Block*	AllocateBlock(size_t bufferSize);

		/////////////////////////////////////////////////////
		// Description:
		//   Allocate MessageBlocks and save in the pool
		//   In other words, Allocate blocks in advance
		/////////////////////////////////////////////////////
		void	PrepareMessageBlock(size_t bufferSize, uint32 count);

		/////////////////////////////////////////////////////
		// Description:
		//   Allocate MemoryBlocks and save in the pool
		/////////////////////////////////////////////////////
		void	PrepareMemoryBlock(size_t bufferSize, uint32 count);

	public:
		/////////////////////////////////////////////////////
		// Description:
		//   Stop accepting
		/////////////////////////////////////////////////////
		bool	SuspendAcceptor(uint16 port);
		bool	ResumeAcceptor(uint16 port);

	protected:
		//////////////////////////////////////////////////////////////////////////////////
		// Description:
		//   Push event message. For example
		//   Accept(Connect) new connection / Receive packets / Close connection
		//////////////////////////////////////////////////////////////////////////////////
		bool	PushMessage(uint8 queId, 
							char command, 
							uint32 serial, 
							ACE_Message_Block* block);

	protected:
		bool	Create(AbstractFactory* const factory);
		bool	StartListen();
		void	StartTimer();
		void	ClearAcceptor();
		bool	CheckDuplicatedServicePort(uint16 port);
		void	CloseService();
		void	ClearMessageQueue();

		/////////////////////////////////////////////////////
		// Description:
		//   Setup default value of config
		/////////////////////////////////////////////////////
		void	SetupDefaultConfig();

	private:
		AbstractFactory*			_factory;
		AbstractConnector*			_connector;
		AbstractEventPool*			_eventPool;
		AbstractServiceAccessor*	_serviceAccessor;
		AbstractTimer* _timer;

	private:
		MessageQueueRepository*		_queueRepository;
		FlexibleSizePoolT<MessageBlockPool, ACE_Message_Block>*	_blockPool;

	private:
		AcceptorList _acceptorList;		// acceptor repository

	private:
		ACE_Atomic_Op<ACE_Thread_Mutex, int32>	_suspend;

	friend class ServiceImpl;

	};
	typedef ACE_Singleton<NetworkFacade, ACE_Thread_Mutex> NetworkFacadeSingleton;
} // namespace HalfNetwork

#define NetworkInstance HalfNetwork::NetworkFacadeSingleton::instance()

#endif // __networkfacade_h__