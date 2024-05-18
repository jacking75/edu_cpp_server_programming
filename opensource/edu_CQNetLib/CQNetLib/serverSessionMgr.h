#pragma once

#include <vector>
#include <mutex>

#include "singleton.h"
#include "thread.h"
#include "session.h"


namespace CQNetLib
{
#define DEFAULT_SERVERCONN 3

	class Session;
	class IocpServer;
	
	//TODO 이 클래스는 애플리케이션 쪽으로 가야한다.
	class ServerConnectionManager : public Thread, public Singleton< ServerConnectionManager >
	{
	public:
		ServerConnectionManager(void)
		{
			/*for (BYTE i = 0; i < CT_COUNT; ++i)
				m_byIdxConnection[i] = 0;*/
		}

		~ServerConnectionManager(void) {}

		/*bool Start(UINT32 dwTickCount)
		{
			ConnectToServer();
			CreateThread(dwTickCount);
			Run();
			return true;
		}*/

		//Session* GetNextConnection(eConnectionType ConnType) //가져오면서 해당 서버의 인덱스를 증가시킨다.
		//{
		//	std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

		//	size_t nConnCnt = m_vecConnection[ConnType].size();
		//	for (size_t i = 0; i < nConnCnt; ++i)
		//	{
		//		++m_byIdxConnection[ConnType];

		//		if (m_byIdxConnection[ConnType] > nConnCnt)
		//			m_byIdxConnection[ConnType] = 1;

		//		if (m_vecConnection[ConnType][m_byIdxConnection[ConnType] - 1]->GetIsConnect() == true)
		//			return m_vecConnection[ConnType][m_byIdxConnection[ConnType] - 1];
		//	}

		//	return NULL;
		//}
		//
		//Session*	GetCurrentConnection(eConnectionType ConnType) //현재 연결을 가져온다.
		//{
		//	std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

		//	size_t nConnCnt = m_vecConnection[ConnType].size();
		//	for (size_t i = 0; i < nConnCnt; ++i)
		//	{
		//		if (m_vecConnection[ConnType][i]->GetIsConnect() == true)
		//			return m_vecConnection[ConnType][i];
		//	}

		//	return NULL;
		//}
		
		//INT32 GetCurrentConnectionCnt(eConnectionType ConnType) //현재 연결 개수를 가져온다.
		//{
		//	std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

		//	return static_cast<INT32>(m_vecConnection[ConnType].size());
		//}

		//bool CreateConnection(Session* pConnection, BYTE byMaxConnCnt = DEFAULT_SERVERCONN)
		//{
		//	//서버 연결이 아니라면
		//	if (pConnection->GetConnectionType() == CT_CLIENT || pConnection->GetConnectionType() > CT_COUNT)
		//		return false;

		//	//이전에 있던 연결을 삭제
		//	RemoveConnection(pConnection->GetConnectionType());

		//	auto connType = pConnection->GetConnectionType();
		//	std::lock_guard<std::mutex> lock(m_Locks[connType]);

		//	for (BYTE i = 0; i < byMaxConnCnt; ++i)
		//	{
		//		Session* pCloneConnection = pConnection->Clone();
		//		const INITCONFIG& initConfig = pConnection->GetInitConfig();
		//		pCloneConnection->CreateConnection(initConfig);
		//		m_vecConnection[pConnection->GetConnectionType()].push_back(pCloneConnection);
		//	}
		//	return true;
		//}
		
		/*bool RemoveConnection(eConnectionType ConnType)
		{
			std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

			size_t nConnCnt = m_vecConnection[ConnType].size();

			for (size_t i = 0; i < nConnCnt; ++i)
				SAFE_DELETE(m_vecConnection[ConnType][i]);

			m_vecConnection[ConnType].clear();
			return true;
		}*/
		
		/*void RemoveAll()
		{
			for (BYTE i = 1; i < CT_COUNT; ++i)
				RemoveConnection(static_cast<eConnectionType>(i));
		}*/

		//bool ConnectToServer() //모든 서버를 돌면서 접속 시킨다.
		//{
		//	for (BYTE i = 1; i < CT_COUNT; ++i)
		//		ConnectToServer(static_cast<eConnectionType>(i));

		//	return true;
		//}
		
		//bool ConnectToServer(eConnectionType ConnType)	//해당 서버 군만 돌면서 접속 시킨다.
		//{
		//	std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

		//	size_t nConnCnt = m_vecConnection[ConnType].size();

		//	for (size_t i = 0; i < nConnCnt; ++i)
		//	{
		//		const INITCONFIG& initConfig = m_vecConnection[ConnType][i]->GetInitConfig();
		//		if (m_vecConnection[ConnType][i]->GetIsConnect() == true) {
		//			continue;
		//		}

		//		if (m_vecConnection[ConnType][i]->ConnectTo(initConfig.szIP, (UINT16)initConfig.nServerPort) == false)
		//		{
		//			return false;
		//		}
		//	}

		//	return true;
		//}
		
		/*
		bool CloseServer() //모든 서버의 연결을 끊는다.
		{
			for (BYTE i = 1; i < CT_COUNT; ++i)
				CloseServer(static_cast<eConnectionType>(i));

			return true;
		}
		
		bool CloseServer(eConnectionType ConnType) //해당 서버 군만 연결을 끊는다.
		{
			std::lock_guard<std::mutex> lock(m_Locks[ConnType]);

			size_t nConnCnt = m_vecConnection[ConnType].size();

			for (size_t i = 0; i < nConnCnt; ++i)
			{
				if (m_pIocpServer)
					m_pIocpServer->CloseConnection(m_vecConnection[ConnType][i]);

			}
			return true;
		}
		*/

		/*virtual void OnProcess()
		{
			ConnectToServer();
		}*/


		//static inline void			SetIocpServer(IocpServer* pIocpServer) { m_pIocpServer = pIocpServer; }

	private:
		//std::vector<Session*>		m_vecConnection[CT_COUNT];
		//BYTE						m_byIdxConnection[CT_COUNT];
		//std::mutex					m_Locks[CT_COUNT];
	};

}