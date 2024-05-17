#include <iostream>
#include <thread>

#include "ServerCtrl.h"
#include "Config.h"

int main()
{
	ServerCtrl Server;
		
	Config config;		
	Server.SetConfig(&config);

	Server.StartServer();
	
	std::cout << "Press any key to continue..." << std::endl;
	getchar();
	
	Server.EndServer();
	
	return 0;
}