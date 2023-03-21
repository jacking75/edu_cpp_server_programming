#pragma once

namespace CQNetLib
{
	class ServerConnectionManager;
}

class LoginIocpServer;
class User;
class UserManager;


class ProcessClientPacket
{
public:
	static void SetRefObject(LoginIocpServer* pLoginServer, 
		UserManager* pUserManager, 
		CQNetLib::ServerConnectionManager* pServerConnectionMgr);


	static bool funcCTL_PACKET_LOGIN_RQ( User* pUser,  DWORD dwSize , char* pRecvedMsg );
	static bool funcCTL_PACKET_LOGIN_QUIT_FORCE_RQ( User* pkUser,  DWORD dwSize , char* pRecvedMsg );
	static bool funcCTL_PACKET_DEV_LOGIN_CLOSE_RQ( User* pkUser,  DWORD dwSize , char* pRecvedMsg );

private:
	static LoginIocpServer* m_pLoginServer;
	static UserManager* m_pUserManager;
	static CQNetLib::ServerConnectionManager* m_pServerConnectionMgr;
};
