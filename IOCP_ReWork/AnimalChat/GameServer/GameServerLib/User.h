#pragma once


enum E_USER_STATE : BYTE
{
	E_USER_STATE_NONE = 0,
	E_USER_STATE_CONNECT,
	E_USER_STATE_LOGIN,
	E_USER_STATE_ROOM,
};

struct User
{
	E_USER_STATE UserState = E_USER_STATE_NONE;
	std::string UserID;
	int RoomNumber = NetLib::INVALID_VALUE;

	void Clear(void)
	{
		UserState = E_USER_STATE_NONE;
		UserID = "";
		RoomNumber = NetLib::INVALID_VALUE;
	}
};