#include <iostream>
#include <thread>

#include "ServerCtrl.h"
#include "Config.h"

int main()
{
	ServerCtrl Server;
		
	Config config;
	config.Load();
		
	Server.SetConfig(&config);

	Server.StartServer();
	
	std::cout << "Press any key to continue..." << std::endl;
	getchar();
	
	Server.EndServer();
	// 콘솔에서 exit 누르면 빠져나오도록한다
	/*while (true)
	{
		std::this_thread::yield();
	}*/

	return 0;
}