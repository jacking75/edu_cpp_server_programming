#ifndef __reactoracceptor_h__
#define __reactoracceptor_h__

#pragma once
#include <ace/SOCK_Acceptor.h>
#include <ace/Event_Handler.h>
#include "AbstractAcceptor.h"

namespace HalfNetwork
{

	/////////////////////////////////////////
	// Description:
	//   Custom Sock Acceptor
	/////////////////////////////////////////
	class CustomSockAcceptor : public ACE_Event_Handler
	{
	public:
		CustomSockAcceptor(uint16 service_port, uint8 queue_id);
		~CustomSockAcceptor();

	public:
		bool	Open();
		void	Close();

	public:
		bool	_StartListen();
		////////////////////////////////////////////////////
		// Description:
		//   Get this handler's I/O handle.
		////////////////////////////////////////////////////
		ACE_HANDLE get_handle (void) const;

		////////////////////////////////////////////////////
		// Description:
		//   Called when a connection is ready to accept.
		////////////////////////////////////////////////////
		int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);

		////////////////////////////////////////////////////////////
		// Description:
		//   Called when this handler is removed from the ACE_Reactor.
		////////////////////////////////////////////////////////////
		int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	public:
		void	SetAcceptIP(const ACE_TCHAR* ip);
		void	Suspend();
		void	Resume();
		void	GetInfo(AcceptorInfo& info);
		void	ReceiveBufferSize(uint32 size);

	private:
		ACE_SOCK_Acceptor	_acceptor;
		uint16				_service_port;
		uint8				_queue_id;
		uint32			_receive_buffer_size;
		ACE_TCHAR*				_accept_ip;
		ACE_Atomic_Op<ACE_Thread_Mutex, long>	_suspend_accept;
	};

	/////////////////////////////////////////
	// Description:
	//   Acceptor for reactor
	/////////////////////////////////////////
	class ReactorAcceptor : public AbstractAcceptor
	{
	public:
		ReactorAcceptor(uint16 service_port, uint8 queue_id);
		virtual ~ReactorAcceptor();

	public:
		bool	Open();
		void	Close();

	public:
		virtual uint16	ServicePort();
		virtual void	Suspend();
		virtual void	Resume();
		virtual void	SetAcceptIP(const ACE_TCHAR* ip);
		virtual void	GetInfo(AcceptorInfo& info);
		virtual void	ReceiveBufferSize(uint32 size);
		virtual void	InitialAcceptCount(uint32 count);

	private:
		uint16				_service_port;
		CustomSockAcceptor	_custom_acceptor;
	};

} // namespace HalfNetwork

#endif // __reactoracceptor_h__