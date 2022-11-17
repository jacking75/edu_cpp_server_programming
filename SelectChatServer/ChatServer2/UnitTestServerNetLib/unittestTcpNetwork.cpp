#include "stdafx.h"
#include "CppUnitTest.h"

#include "MockTcpNetwork.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestServerNetLib
{		
	TEST_CLASS(unittestTcpNetwork)
	{
	public:
		
		TEST_METHOD(CreateSessionPool)
		{
			int sessionCount = 0;

			MockTcpNetwork network;

			sessionCount = 5;
			network.CreateSessionPool(sessionCount);

			Assert::AreEqual(sessionCount, (int)network.GetClientSession().size());
		}



		
	};
}