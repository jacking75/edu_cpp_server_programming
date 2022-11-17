#ifndef __proactorconnector_h__
#define __proactorconnector_h__

#pragma once
#include <ace/Asynch_Connector.h>
#include "ProactorService.h"
#include "AbstractConnector.h"

namespace HalfNetwork
{

	/////////////////////////////////////////////////////
	// Description:
	// Asynch_Connector
	/////////////////////////////////////////////////////
	class ProactorAsynchConnector : public ACE_Asynch_Connector<ProactorService>
	{
	public:
		ProactorAsynchConnector();
		virtual ~ProactorAsynchConnector() {}

	public:
		ProactorService*	make_handler();
		void QueueID(uint8 id);
		void ReceiveBufferSize(uint32 size);

	private:
		uint8	_queueId;
		uint32 _receive_buffer_size;
	};

	/////////////////////////////////////////////////////
	// Description:
	// Synch_Connector
	/////////////////////////////////////////////////////
	class ProactorSynchConnector 
	{
	public:
		ProactorSynchConnector();
		virtual ~ProactorSynchConnector() {}

	public:
		bool Connect(const ACE_TCHAR* ip, 
                 uint16 port, 
                 uint8 queue_id, 
                 uint32 receiveBufferSize);

	private:
		uint8	_queueId;
		uint32 _receive_buffer_size;
	};

	/////////////////////////////////////////
	// Description:
	// Connector Interface for proactor
	/////////////////////////////////////////
	class ProactorConnector : public AbstractConnector
	{
	public:
		ProactorConnector();
		virtual ~ProactorConnector() {}

	public:
		bool	Open();
		void	Close();

	public:
		virtual	bool Connect(const ACE_TCHAR* ip, 
                         uint16 port, 
                         uint8 queue_id);

		virtual	bool Connect(const ACE_TCHAR* ip, 
                         uint16 port, 
                         uint8 queue_id, 
                         uint32 receiveBufferSize);

		virtual	bool AsynchConnect(const ACE_TCHAR* ip, 
                               uint16 port, 
                               uint8 queue_id);

		virtual	bool AsynchConnect(const ACE_TCHAR* ip,
                               uint16 port, 
                               uint8 queue_id, 
                               uint32 receiveBufferSize);



	private:
		ProactorAsynchConnector	m_connector;
		ProactorSynchConnector m_synchConnector;
		ACE_INET_Addr	m_connectAddr;
	};

} // namespace HalfNetwork

#endif // __proactorconnector_h__