#pragma once

#include <deque>
#include "ErrorCode.h"
#include "Room.h"

namespace ChatServerLib
{
	class User;

	class RoomManager
	{
	public:
		RoomManager() = default;
		~RoomManager() = default;


	private:
		std::vector<Room*> m_RoomList;


	public:

		void Init(const int maxRoomNum, NServerNetLib::TcpNetwork* pNetwork);
		Room* FindRoom(const int roomIndex);
		void Release();
	};
}


