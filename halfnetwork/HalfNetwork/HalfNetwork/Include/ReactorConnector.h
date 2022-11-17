#ifndef __reactorconnector_h__
#define __reactorconnector_h__

#pragma once
#include <ace/Event_Handler.h>
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Connector.h"
#include "ReactorService.h"
#include "AbstractConnector.h"

namespace HalfNetwork
{

	/////////////////////////////////////////
	// Description:
	// Connector Interface for reactor
	/////////////////////////////////////////
	class ReactorConnector : public AbstractConnector
	{
	public:
		ReactorConnector();
		virtual ~ReactorConnector() {}

	public:
		bool Open();
		void Close();

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
		ACE_Connector<ReactorService, ACE_SOCK_CONNECTOR> _connector;
		uint8	_queue_id;
	};

} // namespace HalfNetwork


#endif // __reactorconnector_h__