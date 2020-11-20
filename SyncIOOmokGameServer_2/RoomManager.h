#pragma once

#include "Room.h"

namespace ChatServerLib
{

	class RoomManager
	{
	public:

		RoomManager() = default;
		~RoomManager() = default;

		void Init(const int maxRoomNum, NServerNetLib::TcpNetwork* pNetwork);

		std::optional <Room*> FindProperRoom();

		std::optional <Room*> FindRoom(const int roomIndex);

	private:

		std::vector<Room*> m_RoomList;
		
	};
}


