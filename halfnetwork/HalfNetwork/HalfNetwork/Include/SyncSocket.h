#ifndef __syncsocket_h__
#define __syncsocket_h__

#pragma once
#include <string>
#include "HalfNetworkType.h"

class ACE_SOCK_Stream;

namespace HalfNetwork
{
	/////////////////////////////////////////////////////
	// Description:
	//   Connect / Send / Recv through sync operation
	/////////////////////////////////////////////////////
	class SyncSocket
	{
	public:
		SyncSocket();
		virtual ~SyncSocket();
		bool Connect(const ACE_TCHAR* ip, uint16 port, uint32 timeoutMs);
		bool Send(const char* buffer, uint32 len);
		uint32 Receive(char* buffer, uint32 len, uint32 timeoutMs);
		void Disconnect();
	private:
		ACE_SOCK_Stream* m_stream;
	};
} // namespace HalfNetwork

#endif // __syncsocket_h__