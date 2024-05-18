#pragma once

#include "../../CQNetLib/IocpServer.h"

#include "sessionManager.h"

class ChatServer : public CQNetLib::IocpServer
{
public:
	ChatServer();
	~ChatServer();

	void Init(CQNetLib::INITCONFIG& initConfig, UINT32 dwMaxConnection);
	
	virtual	bool OnAccept(CQNetLib::Session* lpConnection) override;
	

	//client에서 packet이 도착하여 순서 성 있는 패킷을 처리할 때 호출되는 함수
	virtual	bool OnRecv(CQNetLib::Session* lpConnection, UINT32 dwSize, char* pRecvedMsg) override;
	
	//client에서 packet이 도착하여 순서 성 없는 패킷을 처리할 때 호출되는 함수
	virtual	bool OnRecvImmediately(CQNetLib::Session* lpConnection, UINT32 dwSize, char* pRecvedMsg) override;
	
	//client 접속 종료시 호출되는 함수
	virtual	void OnClose(CQNetLib::Session* lpConnection) override;
	
	//서버에서 ProcessThread가 아닌 다른 쓰레드에서 발생시킨 
	//메시지가 순서 성있게 처리되야 한다면 이 함수를 사용.
	virtual bool OnSystemMsg(void* pOwnerKey, UINT32 dwMsgType, LPARAM lParam) override;
	

private:
	ConnectionManager m_ConnectionMgr;

};