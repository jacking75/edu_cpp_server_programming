#include "chatServer.h"


ChatServer::ChatServer()
{

}

ChatServer::~ChatServer()
{

}

void ChatServer::Init(CQNetLib::INITCONFIG& initConfig, UINT32 dwMaxConnection)
{
	m_ConnectionMgr.CreateConnection(initConfig, 10, this);
}

bool ChatServer::OnAccept(CQNetLib::Session* lpConnection) 
{
	return true;
}

//client에서 packet이 도착하여 순서 성 있는 패킷을 처리할 때 호출되는 함수
bool ChatServer::OnRecv(CQNetLib::Session* lpConnection, UINT32 dwSize, char* pRecvedMsg)
{
	return true;
}

//client에서 packet이 도착하여 순서 성 없는 패킷을 처리할 때 호출되는 함수
bool ChatServer::OnRecvImmediately(CQNetLib::Session* lpConnection, UINT32 dwSize, char* pRecvedMsg)
{
	return true;
}

//client 접속 종료시 호출되는 함수
void ChatServer::OnClose(CQNetLib::Session* lpConnection)
{

}

//서버에서 ProcessThread가 아닌 다른 쓰레드에서 발생시킨 
//메시지가 순서 성있게 처리되야 한다면 이 함수를 사용.
bool ChatServer::OnSystemMsg(void* pOwnerKey, UINT32 dwMsgType, LPARAM lParam)
{
	return true;
}