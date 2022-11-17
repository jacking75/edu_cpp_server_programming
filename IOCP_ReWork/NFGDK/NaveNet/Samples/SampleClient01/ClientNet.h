#pragma once

#include "../../Client/NaveNet.h"
#include "../../Client/NFNetClientS.h"

#include "Packet.h"

class ClientNet : public NaveClientNetLib::NFNetClientS
{
public:
	ClientNet(void);
	~ClientNet(void);

public:
	//////////////////////////////////////////////////////////////////////
	// Socket -> Client로 패킷을 전달하기 위해 존재하는 Listener이다.
	//////////////////////////////////////////////////////////////////////
	class IEventListener
	{
	public:
		virtual ~IEventListener() {}
		virtual void EventIRC(char* strCmd, char* strMsg) = 0;
		virtual void EventTEST(char* strMsg) = 0;
		virtual void EventConnect(bool bConnect) = 0;
		virtual void EventSocketClose() = 0;
	};

	void	SetEventListener(IEventListener* event)
	{
		m_event = event;
	}

public:
	virtual void	Update();

	//////////////////////////////////////////////////////////////////////
	// ToServer에 정의된 패킷사용
	// 전달받은 패킷이 처리될 패킷함수 형식은 Recv_패킷이름(CHAR* Packet, int Len) 이된다.
	//////////////////////////////////////////////////////////////////////
	void	Send_IRC(const char* strCommand, const  char* strMsg);

	void	Send_TEST(const  char* strMsg);

	void	Disconnect();

private:
	//////////////////////////////////////////////////////////////////////
	// CNetTest에서 사용되는 변수모음
	//////////////////////////////////////////////////////////////////////
	//	Nave::Sync::CSync					m_Cs;

	// Send를 하기위해 패킷을 만들때 사용된다.
	NaveClientNetLib::NFPacket					m_SendPacket;

	IEventListener*						m_event;

private:
	void	(ClientNet::*OnMsgMap[MaxPacket])(char* Packet, int Len);

	// 소켓 부분에서 떨어지는 이벤트를 처리할때
	void	OnSocketEvent(DWORD dID, DWORD dEvent);

	//////////////////////////////////////////////////////////////////////
	// FromServer에 정의된 패킷사용 (클라이언트는 상관없다.)
	// 전달받은 패킷이 처리될 패킷함수 형식은 Recv_패킷이름(CHAR* Packet, int Len) 이된다.
	//////////////////////////////////////////////////////////////////////

	// IRC Packet 처리함수
	void	Parsed_IRC(char* Packet, int Len);
	void	Parsed_TEST(char* Packet, int Len);

	void	ProcessPacket(NaveClientNetLib::NFPacket* pPacket, int PacketLen);

};