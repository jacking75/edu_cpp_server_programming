#pragma once

#include <memory>
#include "MockTcpNetwork.h"
#include "MockUser.h"
#include "MockLobby.h"


class Helper
{
public:
	Helper() {}
	~Helper() {}

	static std::shared_ptr<MockUser> CreateUser(short index)
	{
		auto pUser = std::make_shared<MockUser>();
		pUser->Init(index);
		return pUser;
	}

	static std::shared_ptr<MockLobby> CreateLobby(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		auto pLobby = std::make_shared<MockLobby>();
		pLobby->Init(lobbyIndex, maxLobbyUserCount, maxRoomCountByLobby, maxRoomUserCount);
		return pLobby;
	}

	static std::vector<std::unique_ptr<MockUser>> EnterUserInLobby(MockLobby* pLobby, int userCount)
	{
		std::vector<std::unique_ptr<MockUser>> userList;

		for (int i = 0; i < userCount; ++i)
		{
			auto userID = std::string("test_") + std::to_string(i);

			auto pUser = std::make_unique<MockUser>();
			pUser->Init(i);
			pUser->Set(1+i, userID.c_str());
						
			pLobby->EnterUser(pUser.get());

			userList.push_back(std::move(pUser));
		}

		return userList;
	}
};