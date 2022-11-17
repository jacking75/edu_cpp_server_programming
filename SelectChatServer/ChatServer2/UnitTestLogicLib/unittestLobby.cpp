#include "stdafx.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../../Common/ErrorCode.h"
#include "../../Common/PacketID.h"
#include "../../Common/Packet.h"
#include "../LogicLib/Room.h"

#include "Helper.h"

using ERROR_CODE = NCommon::ERROR_CODE;


namespace UnitTestLogicLib
{		
	TEST_CLASS(UnitTestLobby)
	{
	public:
		
		TEST_METHOD(Init)
		{
			MockLobby lobby;

			//[실행]
			lobby.Init(10, 100, 20, 5);

			//[검증]
			Assert::AreEqual(10, (int)lobby.GetIndex());
			Assert::AreEqual(100, (int)lobby.MaxUserCount());
			Assert::AreEqual(20, (int)lobby.MaxRoomCount());

			auto pRoom = lobby.GetRoom(0);
			Assert::AreEqual(5, (int)pRoom->MaxUserCount());
		}

		TEST_METHOD(AddUser_success)
		{
			auto pLobby = Helper::CreateLobby(10, 50, 20, 5);

			auto userIndex = (short)16;
			auto pUser = Helper::CreateUser(userIndex);
			
			
			auto result = pLobby->AddUser(pUser.get());

			Assert::AreEqual((short)ERROR_CODE::NONE, (short)result);
		}

		TEST_METHOD(AddUser_fail)
		{
			auto pLobby = Helper::CreateLobby(10, 0, 20, 5);

			auto userIndex = (short)2;
			auto pUser = Helper::CreateUser(userIndex);


			auto result = pLobby->AddUser(pUser.get());

			Assert::AreEqual((short)ERROR_CODE::LOBBY_ENTER_EMPTY_USER_LIST, (short)result);
		}


		TEST_METHOD(SendToAllUser)
		{
			MockTcpNetwork network;
			network.Init(10);

			auto pLobby = Helper::CreateLobby(10, 10, 5, 5);
			pLobby->SetNetwork(&network, nullptr);

			auto userList = Helper::EnterUserInLobby(pLobby.get(), 10);
			Assert::AreEqual((size_t)10, userList.size());
			Assert::AreEqual((short)10, pLobby->GetUserCount());

			pLobby->SendToAllUser((short)NCommon::PACKET_ID::LOBBY_LIST_REQ, 0, nullptr);


			Assert::AreEqual((size_t)10, network.m_PacketList.size());
		}
	};
}