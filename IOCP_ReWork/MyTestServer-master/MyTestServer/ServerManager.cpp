#include "ServerManager.h"
#include "WrappingServer.h"

void CServerManager::Init()
{
	GetSystemInfo(&m_systemInfo);

	if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
	{
		// WSAStarup 실패
		return;
	}
}


bool CServerManager::CreateServer(CServerBase::_StInitInfo const& serverInfo_)
{
	CWrappingServer* server = new CWrappingServer;
	if (server == NULL)
	{
		// 로그
		return false;
	}

	server->Init(serverInfo_);
	std::pair<MAP_SERVER::iterator, bool> ret = m_mapServer.insert(std::make_pair(serverInfo_.serverType, server));
	if (!ret.second)
	{
		// 로그
		return false;
	}
	return true;
}


bool CServerManager::RemoveServer(CServerBase::SERVER_TYPE const type_)
{
	if (m_mapServer.find(type_) == m_mapServer.end())
	{
		// 로그
		return false;
	}

	m_mapServer.erase(type_);
	return true;
}


bool CServerManager::Start()
{
	for (MAP_SERVER::iterator it = m_mapServer.begin(); it != m_mapServer.end(); ++it)
	{
		if (!(*it).second->Start())
		{
			// 로그
			Close();
			return false;
		}
	}

	for (MAP_SERVER::iterator it = m_mapServer.begin(); it != m_mapServer.end(); ++it)
	{
		(*it).second->Join();
	}

	return true;
}

void CServerManager::Close()
{
	for (MAP_SERVER::iterator it = m_mapServer.begin(); it != m_mapServer.end(); ++it)
	{
		CServerBase* server = (*it).second;
		server->Close();
		SAFE_DELETE(server);
	}

	if (m_mapServer.size() > 0)
	{
		m_mapServer.clear();
	}

	::WSACleanup();
}

CServerManager::CServerManager()
{
}

CServerManager::~CServerManager()
{
	Close();
}