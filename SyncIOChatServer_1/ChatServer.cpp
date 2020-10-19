#include <thread>
#include <chrono>

#include "ErrorCode.h"
#include "ChatServer.h"


namespace ChatServerLib
{
	ChatServer::ChatServer()
	{
	}

	ChatServer::~ChatServer()
	{
	}

	
	void ChatServer::Stop()
	{
		m_IsRun = false;
	}

	void ChatServer::Run()
	{
		
	}

		
}