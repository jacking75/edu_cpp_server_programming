#pragma once

#include <Windows.h>

#include <unordered_map>

#include "../../CQNetLib/session.h"

#include "Protocol.h"


class ConnectionManager
{	
public:
	ConnectionManager(void) {}

	~ConnectionManager(void)
	{
		delete[] m_pConnection;
	}

	typedef std::pair<CQNetLib::Session*, UINT64>  CONN_PAIR;
	typedef std::unordered_map<CQNetLib::Session*, UINT64>   CONN_MAP;
	//typedef CONN_MAP::iterator	 CONN_IT;	

	bool CreateConnection(CQNetLib::INITCONFIG& initConfig, UINT32 dwMaxConnection, CQNetLib::IocpServer* pServer)
	{
		CQNetLib::SpinLockGuard lock(m_Lock);

		m_pConnection = new CQNetLib::Session[dwMaxConnection];
		
		for (int i = 0; i < (int)dwMaxConnection; i++)
		{
			initConfig.nIndex = i;
			if (auto ret = m_pConnection[i].CreateConnection(initConfig); ret != CQNetLib::ERROR_CODE::none)
			{
				return false;
			}

			m_pConnection[i].SetDelegate(SA::delegate<void(CQNetLib::Session*)>::create<CQNetLib::IocpServer, &CQNetLib::IocpServer::OnClose>(pServer),
								SA::delegate<bool(CQNetLib::Session*, bool)>::create<CQNetLib::IocpServer, &CQNetLib::IocpServer::CloseConnection>(pServer));
		}

		return true;
	}

	bool AddConnection(CQNetLib::Session* pConnection)
	{
		CQNetLib::SpinLockGuard lock(m_Lock);

		auto conn_it = m_mapConnection.find(pConnection);
		
		//이미 접속되어 있는 연결이라면
		if (conn_it != m_mapConnection.end())
		{
			//LOG(LOG_INFO_NORMAL, "SYSTEM | ConnectionManager::AddConnection() | index[%d]는 이미 m_mapConnection에 있습니다.", pConnection->GetIndex());
			return false;
		}

		m_mapConnection.insert(CONN_PAIR(pConnection, GetTickCount64()));
		return true;
	}

	bool RemoveConnection(CQNetLib::Session* pConnection)
	{
		CQNetLib::SpinLockGuard lock(m_Lock);

		auto conn_it = m_mapConnection.find(pConnection);
		
		//접속되어 있는 연결이 없는경우
		if (conn_it == m_mapConnection.end())
		{
			//LOG(LOG_INFO_NORMAL, "SYSTEM | ConnectionManager::RemoveConnection() | index[%d]는 m_mapConnection에 없습니다.", pConnection->GetIndex());
			return false;
		}

		m_mapConnection.erase(pConnection);
		return true;
	}

	inline int __fastcall	GetConnectionCnt() { return (int)m_mapConnection.size(); }
	
	void BroadCast_Chat(char* szIP, char* szChatMsg)
	{
		CQNetLib::SpinLockGuard lock(m_Lock);

		for (auto conn_it = m_mapConnection.begin(); conn_it != m_mapConnection.end(); conn_it++)
		{
			auto pConnection = (CQNetLib::Session*)conn_it->first;
			auto pChat = (Packet_Chat*)pConnection->PrepareSendPacket(sizeof(Packet_Chat));

			if (nullptr == pChat)
			{
				continue;
			}

			pChat->s_sType = PACKET_CHAT;
			strncpy_s(pChat->s_szChatMsg, szChatMsg, MAX_CHATMSG);
			strncpy_s(pChat->s_szIP, szIP, MAX_IP);
			pConnection->SendPost(pChat->s_nLength);
		}
	}

protected:
	CONN_MAP		m_mapConnection;
	CQNetLib::Session*	m_pConnection = nullptr;
	CQNetLib::SpinLock m_Lock;
};
