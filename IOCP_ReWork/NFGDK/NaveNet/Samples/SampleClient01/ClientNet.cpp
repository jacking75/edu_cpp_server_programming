#include "ClientNet.h"


CNetTest::CNetTest(void) : m_event(0)
{
	// 함수를 Map으로 선언한다.
	OnMsgMap[IRC] = &CNetTest::Parsed_IRC;
	OnMsgMap[TEST] = &CNetTest::Parsed_TEST;
}

CNetTest::~CNetTest(void)
{
	Disconnect();
}

void CNetTest::Disconnect()
{
	NFNetClientS::Disconnect();

	m_SendPacket.Init();
}

void CNetTest::OnSocketEvent(DWORD dID, DWORD dEvent)
{
	// 소켓에서 일어난 이벤트를 처리하기 위한것.
	if (dID == WM_CONNECT)			// 접속 이벤트
	{
		if (m_event)
			m_event->EventConnect(dEvent);
	}
	else if (dID == WM_SOCK_CLOSE)	// 소켓 종료 이벤트
	{
		if (m_event)
			m_event->EventSocketClose();
	}
	else if (dID == WM_RECV_MSG)		// 패킷 한개 받은 이벤트
	{
		// Test할때만 사용된다 나중에 정식으로 들어가면 빠진다.
		//			::PostMessage(m_hWnd, dID, dEvent, NULL); // 완전한 Packet Data return
	}
}

void CNetTest::Update()
{
	// Socket Update;
	NFNetClientS::Update();


	// 기타처리
}

void CNetTest::ProcessPacket(NFPacket* pPacket, int PacketLen)
{
	/*	if(pPacket->GetCommand() == 0)
	Parsed_IRC(pPacket->m_Packet, pPacket->GetSize());
	else
	Parsed_TEST(pPacket->m_Packet, pPacket->GetSize());
	*/
	// 함수 호출
	(this->*OnMsgMap[pPacket->GetCommand()])(pPacket->m_Packet, pPacket->GetSize());
}

void CNetTest::Parsed_IRC(CHAR* Packet, int Len)
{
	LPPKIRC pkIRC = (LPPKIRC)Packet;

	if (m_event)
		m_event->EventIRC(pkIRC->Key, pkIRC->Message);
}

void CNetTest::Parsed_TEST(CHAR* Packet, int Len)
{
	LPPKTEST pkTest = (LPPKTEST)Packet;

	if (m_event)
		m_event->EventTEST(pkTest->Message);
}

void CNetTest::Send_IRC(const char* strCommand, const char* strMsg)
{
	//	Sync::CLive CL(&m_Cs);

	m_SendPacket.SetCommand(IRC);
	m_SendPacket.SetSize(sizeof(PKIRC));

	LPPKIRC lpIRC = (LPPKIRC)m_SendPacket.m_Packet;

	strcpy(lpIRC->Key, strCommand);
	strcpy(lpIRC->Message, strMsg);

	SendPost(m_SendPacket);
}

void CNetTest::Send_TEST(const  char* strMsg)
{
	//	Sync::CLive CL(&m_Cs);

	m_SendPacket.SetCommand(TEST);
	m_SendPacket.SetSize(sizeof(PKTEST));

	LPPKTEST lpTest = (LPPKTEST)m_SendPacket.m_Packet;

	lpTest->TestKey = timeGetTime();
	sprintf(lpTest->Message, "%d : %s", lpTest->TestKey, strMsg);

	SendPost(m_SendPacket);
}