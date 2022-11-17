#include "SyncSocket.h"
#include <ace/SOCK.h>
#include <ace/INET_Addr.h>
#include <ace/SOCK_Connector.h>
#include <ace/Time_Value.h>
#include "TimerUtil.h"

namespace HalfNetwork
{
	SyncSocket::SyncSocket() : m_stream(new ACE_SOCK_Stream())
	{
	}

	SyncSocket::~SyncSocket()
	{
		SAFE_DELETE(m_stream);
	}

	bool SyncSocket::Connect( const ACE_TCHAR* ip, uint16 port, uint32 timeoutMs)
	{
		ACE_INET_Addr connectAddr(port, ip);
		ACE_SOCK_Connector connector;
		ACE_Time_Value waitTime;
		ConvertTimeValue(waitTime, timeoutMs);
		int result = connector.connect(*m_stream, connectAddr, &waitTime);
		if (-1 == result)
			return false;
		return true;
	}

	bool SyncSocket::Send(const char* buffer, uint32 len)
	{
		if (len <= 0)
			return false;
		return (len == (uint32)m_stream->send_n(buffer, len));
	}

	uint32 SyncSocket::Receive(char* buffer, uint32 len, uint32 timeoutMs)
	{
		ACE_Time_Value waitTime;
		ConvertTimeValue(waitTime, timeoutMs);
		return (uint32)m_stream->recv(buffer, len, &waitTime);
	}

	void SyncSocket::Disconnect()
	{
		m_stream->close();
	}

} // namespace HalfNetwork
