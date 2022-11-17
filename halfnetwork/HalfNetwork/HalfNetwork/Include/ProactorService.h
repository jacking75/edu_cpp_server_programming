#ifndef __proactorservice_h__
#define __proactorservice_h__

#pragma once

#include <ace/Asynch_Acceptor.h>
#include "TsidMap.h"
#include "MemoryObject.h"

namespace HalfNetwork
{

	class InterlockedValue;
	class ServiceImpl;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//   
	//////////////////////////////////////////////////////////////////////////
	class DeleteHandler : public ACE_Handler
	{
	public:
		DeleteHandler(ACE_Service_Handler* p);
		virtual ~DeleteHandler();
		virtual void handle_time_out(const ACE_Time_Value& tv, const void* arg);

	private:
		ACE_Service_Handler*	_parentHandle;
		InterlockedValue*			_timerLock;
	};

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//   created one ProactorService object per one connection
	//////////////////////////////////////////////////////////////////////////
	class ProactorService : public ACE_Service_Handler, public MemoryObject
	{
	public:
		ProactorService();
		virtual ~ProactorService();

	public:
		//////////////////////////////////////////
		// Description:
		//   called when connection is established
		/////////////////////////////////////////
		virtual void	open(ACE_HANDLE handle, ACE_Message_Block&);

		//////////////////////////////////////////////////////
		// Description:
		//   called when read(receive) operation is completed
		/////////////////////////////////////////////////////
		virtual void	handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);

		////////////////////////////////////////////////////
		// Description:
		//   called when write(send) operation is completed
		////////////////////////////////////////////////////
		virtual void	handle_write_stream(const ACE_Asynch_Write_Stream::Result &result);


		////////////////////////////////////////////////////
		// Description:
		//   Called when timer expires.  
		////////////////////////////////////////////////////
		virtual void	handle_time_out(const ACE_Time_Value& tv, const void* arg);

		//////////////////////////////////////////////////
		// Description:
		//   notify address of peer
		/////////////////////////////////////////////////
		virtual void	addresses(const ACE_INET_Addr& remote, const ACE_INET_Addr& local);

	public:
		/////////////////////////////////////////////////////////
		// Description:
		//   set queue id(use communicate with Application layer)
		/////////////////////////////////////////////////////////
		void	QueueID(uint8 id);
		void	ReceiveBufferSize(uint32 size);

		/////////////////////////////////////////
		// Description:
		//   Close Stream
		/////////////////////////////////////////
		void	ActiveClose();
		void	ReserveClose();
		void	ReceiveClose();

	public:
		bool	IntervalSend(ACE_Message_Block* block);
		bool	DirectSend(ACE_Message_Block* block);
		bool	ReserveSend(ACE_Message_Block* block, uint32 delay);

	protected:
		//////////////////////////////////////////////////////////////////////
		// Description:
		//   Write(Send)
		//   1. when completed previous send operation : directly send
		//   2. when incompleted previous send operation : insert gather_block
		//////////////////////////////////////////////////////////////////////
		bool	_SmartSend(ACE_Message_Block* block);
		bool	_SmartSendImpl(ACE_Message_Block* block);

	protected:
		///////////////////////////////
		// Description:
		//   closesocket
		//////////////////////////////
		void	_CloseHandle();
		void	_CancelTimer();
		void	_Close();

		///////////////////////////////
		// Description:
		//   close connection actively
		//////////////////////////////
		void	_ActiveClose();
		void	_ReceiveClose();

	protected:
		void	_OnEstablish();
		void	_OnReceive(ACE_Message_Block& block);

	protected:
		//////////////////////////////////
		// Description:
		//   Register to ProactorServiceMap
		/////////////////////////////////
		void	_Register();

		/////////////////////////////////////
		// Description:
		//   UnRegister from ProactorServiceMap
		/////////////////////////////////////
		void	_UnRegister();
		void	_RegisterTimer();

	protected:
		///////////////////////////////
		// Description:
		//   Start Reading(Receiving)
		///////////////////////////////
		void	_InitializeRead();

		///////////////////////////////
		// Description:
		//   Log Error
		///////////////////////////////
		void	_LogError(uint32 error_code);

	protected:
		//////////////////////////
		// Description:
		// 
		//////////////////////////
		bool	_PushQueue(ACE_Message_Block* block, uint32 tick);
		bool	_PopQueue(ACE_Message_Block** block);
		void	_SendQueuedBlock();

	protected:
		bool	_IsCloseFlagActivate();
		void _CheckZombieConnection();

	private:
		ACE_Asynch_Read_Stream	m_reader;
		ACE_Asynch_Write_Stream	m_writer;

	private:
		uint32				_serial;
		uint8					_queue_id;
		uint32				_receive_buffer_size;
		char*					_peer_ip;
		DeleteHandler*	_deleteHandler;

	private:
		ServiceImpl*	_serviceImpl;
	};

	const uint32 Max_ProactorService = 1024*4;
	typedef TsIDMapT<ProactorService, Max_ProactorService> ProactorServiceMapType;
	typedef ACE_Singleton<ProactorServiceMapType, ACE_Thread_Mutex> ProactorServiceMapSingleton;
	#define ProactorServiceMap ProactorServiceMapSingleton::instance()

} // namespace HalfNetwork


#endif // __proactorservice_h__