#ifndef __abstractconnector_h__
#define __abstractconnector_h__

#pragma once

namespace HalfNetwork
{

	/////////////////////////////////////////////
	// Description:
	//   Connector Interface
	/////////////////////////////////////////////
	class AbstractConnector
	{
	public:
		AbstractConnector() {}
		virtual ~AbstractConnector() {}

	public:
		virtual	bool Open() = 0;
		virtual	void Close() = 0;

	public:
		virtual	bool AsynchConnect(const ACE_TCHAR* ip, 
                               uint16 port, 
                               uint8 queue_id) = 0;

		virtual	bool AsynchConnect(const ACE_TCHAR* ip, 
                               uint16 port, 
                               uint8 queue_id, 
                               uint32 receiveBufferSize) = 0;

		virtual	bool Connect(const ACE_TCHAR* ip, 
                         uint16 port, 
                         uint8 queue_id) = 0;

		virtual	bool Connect(const ACE_TCHAR* ip, 
                         uint16 port, 
                         uint8 queue_id, 
                         uint32 receiveBufferSize) = 0;
	};

} // namespace HalfNetwork

#endif // __abstractconnector_h__