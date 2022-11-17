#ifndef __proactoracceptor_h__
#define __proactoracceptor_h__

#pragma once
#include <ace/Asynch_Acceptor.h>
#include "ProactorService.h"
#include "AbstractAcceptor.h"
#include <ace/Task.h>

namespace HalfNetwork
{

	////////////////////////////////////////////////////
	// Description:
	// Custom Asynch_Acceptor
	////////////////////////////////////////////////////
	class CustomAsynchAcceptor : public ACE_Asynch_Acceptor<ProactorService>
	{
	public:
		CustomAsynchAcceptor();

	public:
		ProactorService*	make_handler();
		int validate_connection (const ACE_Asynch_Accept::Result& result,
														 const ACE_INET_Addr &remote,
														 const ACE_INET_Addr& local);
	public:
		void QueueID(uint8 id);
		void Suspend();
		void Resume();
		bool IsSuspend();
		void ReceiveBufferSize(uint32 size);

	private:
		uint8 _queueId;
		uint32 _receive_buffer_size;
		ACE_Atomic_Op<ACE_Thread_Mutex, long>	_suspendAccept;
	};

	/////////////////////////////////////////
	// Description:
	// Acceptor for proactor
	/////////////////////////////////////////
	class ProactorAcceptor : public AbstractAcceptor
	{
	public:
		ProactorAcceptor(uint16 service_port, uint8 queue_id);
		virtual ~ProactorAcceptor();

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

	protected:
		bool StartListen();

	private:
		CustomAsynchAcceptor _acceptor;
		uint16 _servicePort;
		uint8	_queueId;
		ACE_TCHAR* _acceptIp;
		uint32 _receiveBufferSize;
		uint32 _initialAcceptcount;
	};

} // namespace HalfNetwork

#endif // __proactoracceptor_h__