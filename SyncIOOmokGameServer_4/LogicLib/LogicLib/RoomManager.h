#pragma once

#include "Room.h"
#include <functional>
namespace OmokServerLib
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

		void CheckRoomGameTime();

	private:

		std::vector<Room*> m_RoomList;
		
	};
}


