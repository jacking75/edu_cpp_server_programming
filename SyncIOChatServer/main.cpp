#include <iostream>
#include <thread>
#include "TcpNetwork.h"

int main()
{
	NServerNetLib::TcpNetwork serverNet;
	
	std::thread logicThread([&]() { 		
		serverNet.Run(); }
	);
	
	std::cout << "press any key to exit...";
	getchar();

	serverNet.Stop();
	logicThread.join();

	return 0;
}