#include <iostream>
#include <thread>
#include "../../LogicLib/LogicLib/OmokServer.h"
#include "../../LogicLib/LogicLib/Config.h"

int main()
{
	OmokServerLib::OmokServer omokServer;

	if (omokServer.Init() != OmokServerLib::ERROR_CODE::NONE)
	{
		return 0;
	}

	if (omokServer.Run() != OmokServerLib::ERROR_CODE::NONE)
	{
		return 0;
	}

	return 0;
}

