#pragma once
#include <memory>

#include "Packet.h"
#include "ErrorCode.h"



namespace ChatServerLib
{

	class ChatServer
	{
	public:
		ChatServer();
		~ChatServer();

		void Run();

		void Stop();


	private:
		bool m_IsRun = false;

		
	};
}
