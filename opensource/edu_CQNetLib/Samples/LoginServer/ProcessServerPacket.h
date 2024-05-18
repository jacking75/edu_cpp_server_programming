/*-------------------------------------------------------------------------------------------------*/
//	File			:	cProcessServerPacket.h
//  Date			:	08. 01
//	Description		:	게임 서버와 로긴 디비 서버에서 온 패킷을 처리한다.
/*-------------------------------------------------------------------------------------------------*/
#pragma once

class CQNetLib::Session;
class LoginIocpServer;
class UserManager;

class ProcessServerPacket
{
public:
	static void SetLoginServer(LoginIocpServer* pLoginServer,
		UserManager* pUserManager);
	
	static bool	funcGTL_PACKET_GAMESERVER_BASIC_INFO_RQ(CQNetLib::Session* pConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcSTD_PACKET_LOGININFO_AQ(CQNetLib::Session* pConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcGTL_PACKET_CLIENT_AUTH_CONFIRM_RQ(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcGTL_PACKET_CLIENT_GAME_AUTH_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcLTG_PACKET_LOGOUT_CLIENT_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcGTL_PACKET_CHANNEL_LIST_RQ(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );
	static bool	funcGTL_PACKET_CHANGE_CHANNEL_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );

	static bool	funcGTL_PACKET_LOBBY_CUR_USER_COUNT_MSG(CQNetLib::Session* pkConnection,  DWORD dwSize , char* pRecvedMsg );

private:
	static LoginIocpServer* m_pLoginServer;
	static UserManager* m_pUserManager;
};
