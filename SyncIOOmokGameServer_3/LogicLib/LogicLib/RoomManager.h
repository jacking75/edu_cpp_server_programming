#pragma once

#include "Room.h"
#include <functional>
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

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

	private:

		std::vector<Room*> m_RoomList;
		
	};
}


