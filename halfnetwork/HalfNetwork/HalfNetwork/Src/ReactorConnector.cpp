#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ReactorConnector.h"
#include "ace/INET_Addr.h"

namespace HalfNetwork
{
	ReactorConnector::ReactorConnector() : _queue_id(0)
	{
	}

	bool ReactorConnector::Open()
	{
		return true;
	}

	void ReactorConnector::Close()
	{
		if (ACE_INVALID_HANDLE == _connector.get_handle())
			return;

		_connector.close();
	}

	bool ReactorConnector::Connect(const ACE_TCHAR* ip, uint16 port, uint8 queue_id)
	{
		return Connect(ip, port, queue_id, 0);
	}

	bool ReactorConnector::Connect(const ACE_TCHAR* ip, 
                                 uint16 port, 
                                 uint8 queue_id, 
                                 uint32 receiveBufferSize)
	{
		_queue_id = queue_id;
		ACE_INET_Addr connect_addr(port, ip);
		ReactorService* service = new ReactorService();
		service->QueueID(_queue_id);
		if (0 != receiveBufferSize)
			service->ReceiveBufferSize(receiveBufferSize);
		if (-1 == _connector.connect(service, connect_addr))
			return false;
		return true;
	}

	bool ReactorConnector::AsynchConnect(const ACE_TCHAR* ip, uint16 port, uint8 queue_id)
	{
		return Connect(ip, port, queue_id, 0);
	}

	bool ReactorConnector::AsynchConnect(const ACE_TCHAR* ip, 
																			 uint16 port, 
																			 uint8 queue_id, 
																			 uint32 receiveBufferSize )
	{
		return Connect(ip, port, queue_id, receiveBufferSize);
	}
} // namespace HalfNetwork
