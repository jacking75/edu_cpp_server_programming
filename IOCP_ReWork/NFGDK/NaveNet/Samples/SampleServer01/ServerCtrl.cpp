
#include <iostream>

#include "../../Server/NaveServer.h"
#include "../../Server/NFPacket.h"

#include "Config.h"
#include "ServerCtrl.h" 

ServerCtrl::ServerCtrl(void)
{
	m_pLogin = NULL;					
}

ServerCtrl::~ServerCtrl(void) { }

void ServerCtrl::SetConfig(Config* pConfig)
{
	m_pConfig = pConfig;
}

void ServerCtrl::StartServer()
{
	m_iMaxPacket = 0;
	//m_iPrevTick = timeGetTime();

	auto Port = m_pConfig->GetPort();
	auto MaxConn = m_pConfig->GetMaxConn();

	std::cout << "[StartServer()]Port : " << Port << ",  MaxConn: " << MaxConn << std::endl;

	Start(Port, MaxConn);
}

void ServerCtrl::EndServer()
{
	Stop();
}

void ServerCtrl::SendMsg(char* strParam)
{
	NaveNetLib::NFPacket packet;

	packet.SetCommand(IRC);
	packet.SetSize(sizeof(PKIRC));

	LPPKIRC lpIRC = (LPPKIRC)packet.m_Packet;

	strcpy_s(lpIRC->Key, 64, "/SVR");
	strcpy_s(lpIRC->Message, 512, strParam);

	m_NFConnectionManager.SendPostAll(packet);
}

void ServerCtrl::UpdateInfo()
{
	//char	szTime[32] = { 0, };
	//char	szDate[32] = { 0, };

	//_tzset();
	//_strdate(szDate);
	//_strtime(szTime);

	//char strInfo[512];
	//_stprintf(strInfo, L"UpdateTime : %s-%s", szDate, szTime);
	//UpdateInfo(strInfo);
}

// 사용자 오브젝트를 삭제합니다. (메인루프가 끊난후(EndProcess호출후) 호출)
//void ServerCtrl::ReleaseObject()
//{
//	Stop();
//}

void ServerCtrl::ShowServerInfo()
{
	// Join 시켰으니 Leave시켜야한다.
	char	szDate[32] = { 0, }, szTime[32] = { 0, };
	_tzset();
	_strdate_s(szDate);
	_strtime_s(szTime);

	std::cout << "------------------------------------------------" << std::endl;

	char logMsg[1024] = { 0, };
	_snprintf_s(logMsg, _countof(logMsg), _TRUNCATE, "%s initialized at %s, %s", m_pConfig->GetTitle(), szDate, szTime);
	std::cout << logMsg << std::endl;
	
	//////////////////////////////////////////////////////////////////////
	//							Server start							//
	//////////////////////////////////////////////////////////////////////
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "|                 SERVER START                 |" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	std::string Ip;
	GetLocalIP(Ip);
	_snprintf_s(logMsg, _countof(logMsg), _TRUNCATE, "IP(%s), Port(%d), MaxLogin(%d)", Ip.c_str(), m_iPort, m_iMaxConn);
	std::cout << logMsg << std::endl;
	
	std::cout << "시작" << std::endl;
}

bool ServerCtrl::CreateSession(unsigned long long sckListener)
{
	if (m_PacketPool.Create(m_iMaxConn) == NULL) {
		return false;
	}

	// pClientArray(Client Controler..)
	if ((m_pLogin = new TestConnection[m_iMaxConn]) == NULL) {
		return false;		// Create MAXUSER(1000) m_pLogin
	}

							// Initialize pClientArray
	for (int nCnt = 0; nCnt < m_iMaxConn; nCnt++)
	{
		m_pLogin[nCnt].Init(&m_NFConnectionManager);

		if (m_pLogin[nCnt].Create(nCnt,
									m_hIOCP,
									sckListener,
									&m_PacketPool,
									NaveNetLib::DEF_PACKETSIZE) == 0) 
		{
			return false;
		}
			
	}

	m_NFConnectionManager.Init(m_iMaxConn);

	return true;
}

bool ServerCtrl::ReleaseSession()
{
	// Close All User Sockets
	if (m_pLogin)
	{
		for (int nCnt = 0; nCnt < m_iMaxConn; nCnt++)
		{
			m_pLogin[nCnt].Disconnect();
		}
		delete[] m_pLogin;
	}
	m_pLogin = NULL;

	
	// [01] Shutdown the Packet Pool
	m_PacketPool.Release();
	
	std::cout <<"shutdown packet pool.." << std::endl;

	return true;
}

// Process Update...
//void ServerCtrl::Update()
//{
//	NaveNetLib::NFServerCtrl::Update();
//}