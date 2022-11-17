#pragma once

#include "../../Server/NFPacket.h"
#include "../../Server/NFConnection.h"

#include "Packet.h"

namespace NaveNetLib { class NFConnectionManager; }

// LoginServer의 UserConnect가 접속속할때 사용되는 Connect..
class TestConnection : public NaveNetLib::NFConnection
{
public:
	TestConnection() = default;
	~TestConnection(void) = default;

public:
	void Init(NaveNetLib::NFConnectionManager* pNFConnectionManager);

	void Clear();

	void DispatchPacket(NaveNetLib::NFPacket& Packet);

	//------------------------ 멤 버 변 수 -------------------------//
protected:
	void  (TestConnection::*OnMsgMap[MaxPacket])(char* Packet, int Len);

	void OnConnect(bool bConnect);
	void OnDisconnect();

public:
	void Parsed_IRC(char* Packet, int Len);
	void Parsed_TEST(char* Packet, int Len);

	void Send_IRC(const char* strCommand, const char* strMsg);


private:
	NaveNetLib::NFConnectionManager* m_pNFConnectionManager;

	NaveNetLib::NFPacket				m_SendPacket;
};