#ifndef __reactorservice_h__
#define __reactorservice_h__

#pragma once
#include <ace/Event_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Svc_Handler.h>
#include "TsidMap.h"
#include "MemoryObject.h"

namespace HalfNetwork
{
	class InterlockedValue;
	class ServiceImpl;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//   created one ReactorService object per one connection
	//////////////////////////////////////////////////////////////////////////
	class ReactorService : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
	{
	public:
		ReactorService();
		virtual ~ReactorService();

	public:
		//////////////////////////////////////////
		// Description:
		//   called when connection is established
		/////////////////////////////////////////
		virtual int		open();
		virtual int		open(void *);

		////////////////////////////////////////////////////
		// Description:
		//   Called when input is available from the client.
		////////////////////////////////////////////////////
		virtual int		handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

		////////////////////////////////////////////////////
		// Description:
		//   Called when output is possible.
		////////////////////////////////////////////////////
		//virtual int		handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

		/////////////////////////////////////////////////////////////
		// Description:
		//   Called when this handler is removed from the ACE_Reactor.
		/////////////////////////////////////////////////////////////
		virtual int		handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask);

		////////////////////////////////////////////////////
		// Description:
		//   Called when timer expires.  
		////////////////////////////////////////////////////
		virtual int		handle_timeout(const ACE_Time_Value &current_time, const void* act = 0);

	public:
		////////////////////////////////////////////////////
		// Description:
		//   Get/Set this handler's I/O handle.
		////////////////////////////////////////////////////
		ACE_HANDLE	get_handle() const;
		void		set_handle(ACE_HANDLE handle);

		//ACE_SOCK_Stream& peer();

	protected:
		bool	_SmartSend(ACE_Message_Block* block);

	public:
		/////////////////////////////////
		// Description:
		//   For MainThread(not recommend)
		/////////////////////////////////
		void	ActiveClose();
		void	ReceiveClose();
		void	QueueID(uint8 id);
		void	ReceiveBufferSize( uint32 size );

	public:
		bool	IntervalSend(ACE_Message_Block* block);
		bool	DirectSend(ACE_Message_Block* block);

	protected:
		void	_CloseHandle();
		void	_RemoveHandler();
		void	_Register();
		void	_UnRegister();
		void	_CleanUp();
		void	_RegisterTimer();
		void	_Close();

		///////////////////////////////
		// Description:
		//   close connection actively
		//////////////////////////////
		void	_ActiveClose();
		void	ReserveClose();

	protected:
		void	_OnEstablish();
		void	_OnReceive(ACE_Message_Block* block);
		void	_NotifyClose();

	protected:
		///////////////////////////////////////
		// Description:
		//   Send gather_block
		///////////////////////////////////////
		void	_SendQueuedBlock();
		bool	_PushQueue(ACE_Message_Block* block, uint32 tick);
		bool	_PopQueue(ACE_Message_Block** param_block);

	protected:
		bool _IsCloseFlagActivate();
		void _CheckZombieConnection();

	private:
		//ACE_SOCK_Stream											_sock;
		ACE_Message_Queue<ACE_NULL_SYNCH>		_wait_queue;
		InterlockedValue*			_closeLock;

	private:
		uint32					_serial;
		uint8						_queue_id;
		uint32					_receive_buffer_size;
		char*						_peer_ip;
		ServiceImpl*		_serviceImpl;
		bool						_sentCloseMessage;
	};

	const uint32 Max_ReactorService = 1024;
	typedef TsIDMapT<ReactorService, Max_ReactorService> ReactorServiceMapType;
	typedef ACE_Singleton<ReactorServiceMapType, ACE_Thread_Mutex> ReactorServiceMapSingleton;
#define ReactorServiceMap ReactorServiceMapSingleton::instance()

} // namespace HalfNetwork

#endif // __reactorservice_h__