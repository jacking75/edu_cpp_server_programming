#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")

#include "../CppServerNetLib/CommonDefine.h"
#include "../CppServerNetLib/MiniDump.h"

#include "../GameServerLib/GameServer.h"

int main(void)
{
	/*NetLib::MiniDump::Begin();
	if (NetLib::MiniDump::CreateDirectories() == false)
	{
		printf("Fail MiniDump::CreateDirectories() \r\n");
		return 0;
	}*/

	NetLib::LogFuncPtr = [](const int eLogInfoType, const char* szLogMsg) {
		UNREFERENCED_PARAMETER(eLogInfoType);
		printf("%s\r\n", szLogMsg);
	};

	ChatServer Server;
	Server.Run();

	//NetLib::MiniDump::End();
	return 0;
}
