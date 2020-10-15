#pragma once

#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>


#include <vector>
#include <deque>
#include <unordered_map>


namespace NServerNetLib
{
	class TcpNetwork
	{
	public:
		TcpNetwork();
		virtual ~TcpNetwork();

		
		void Run();

		void Stop();
	};
}
