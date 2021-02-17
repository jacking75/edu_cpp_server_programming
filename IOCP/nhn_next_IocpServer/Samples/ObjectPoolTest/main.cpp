#include <iostream>
#include <vector>

#include "ClientSession.h"


int main()
{
	std::vector<ClientSession*> sessions;

	for (int i = 0; i < 3; ++i)
	{
		auto session = new ClientSession();
		sessions.push_back(session);
	}

	for (int i = 0; i < 3; ++i)
	{
		delete sessions[i];
	}
	sessions.clear();

	for (int i = 0; i < 3; ++i)
	{
		auto session = new ClientSession();
		sessions.push_back(session);
	}
	return 0;
}