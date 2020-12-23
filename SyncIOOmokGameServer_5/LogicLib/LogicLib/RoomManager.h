#pragma once

#include <functional>
#include <optional>
#include <vector>
#include <chrono>
namespace NServerNetLib
{
	class TcpNetwork;
}

namespace OmokServerLib
{	
	class Room;

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

		std::chrono::system_clock::time_point m_LatestRoomCheckTime = std::chrono::system_clock::now();

		int m_LatestRoomCheckIndex = -1;

		const int checkIndexTime = 100;

		const int checkIndexRangeCount = 50;

	};
}


