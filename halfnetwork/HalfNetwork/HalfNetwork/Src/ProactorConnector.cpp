#include <string>
#include <ace/Singleton.h>
#include <ace/SOCK_Connector.h>
#include "HalfNetworkType.h"
#include "ProactorConnector.h"
#include "TimerUtil.h"

namespace HalfNetwork
{
	///////////////////////
	// ProactorAsynchConnector
	///////////////////////
	ProactorAsynchConnector::ProactorAsynchConnector() 
		: _queueId(0)
		, _receive_buffer_size(0)
	{
	}

	ProactorService* ProactorAsynchConnector::make_handler()
	{
		ProactorService* handler = new ProactorService();
		handler->QueueID(_queueId);
		if (0 != _receive_buffer_size)
			handler->ReceiveBufferSize(_receive_buffer_size);
		return handler;
	}

	void ProactorAsynchConnector::QueueID(uint8 id)
	{
		_queueId = id;
	}

	void ProactorAsynchConnector::ReceiveBufferSize( uint32 size )
	{
		_receive_buffer_size = size;
	}

	//////////////////////////////////////////////////////////////////////////

	ProactorSynchConnector::ProactorSynchConnector()
	{
	}

	bool ProactorSynchConnector::Connect( const ACE_TCHAR* ip, 
                                        uint16 port, 
                                        uint8 queue_id, 
                                        uint32 receiveBufferSize )
	{
		ACE_SOCK_Stream* stream = new ACE_SOCK_Stream();
		ACE_INET_Addr connectAddr(port, ip);
		ACE_SOCK_Connector connector;
		int result = connector.connect(*stream, connectAddr);
		if (-1 == result)
			return false;

		ProactorService* handler = new ProactorService();
		handler->QueueID(queue_id);
		if (0 != receiveBufferSize)
			handler->ReceiveBufferSize(receiveBufferSize);

		ACE_Message_Block mb;
		handler->open(stream->get_handle(), mb);
		delete stream;
		stream = NULL;
		return true;
	}

	//////////////////////////////////////////////////////////////////////////


	///////////////////////
	// ProactorConnector
	///////////////////////
	ProactorConnector::ProactorConnector()
	{
	}

	bool ProactorConnector::Open()
	{
		m_connector.open();
		return true;
	}

	void ProactorConnector::Close()
	{
	}

	bool ProactorConnector::Connect(const ACE_TCHAR* ip, uint16 port, uint8 queue_id)
	{
		return Connect(ip, port, queue_id, 0);
	}

	bool ProactorConnector::Connect(const ACE_TCHAR* ip, 
                                  uint16 port, 
                                  uint8 queue_id, 
                                  uint32 receiveBufferSize)
	{
		return m_synchConnector.Connect(ip, port, queue_id, receiveBufferSize);
	}

	bool ProactorConnector::AsynchConnect(const ACE_TCHAR* ip, uint16 port, uint8 queue_id)
	{
		return AsynchConnect(ip, port, queue_id, 0);
	}

	bool ProactorConnector::AsynchConnect(const ACE_TCHAR* ip, 
                                        uint16 port, 
                                        uint8 queue_id, 
                                        uint32 receiveBufferSize )
	{
		m_connector.QueueID(queue_id);
		m_connectAddr.set(port, ip);
		if (0 != receiveBufferSize)
			m_connector.ReceiveBufferSize(receiveBufferSize);
		if (-1 == m_connector.connect(m_connectAddr))
			return false;

		return true;
	}
} // namespace HalfNetwork
