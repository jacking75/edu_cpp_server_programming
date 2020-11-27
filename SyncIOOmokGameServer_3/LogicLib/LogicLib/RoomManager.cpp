#include "RoomManager.h"
#include <optional>
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"

namespace ChatServerLib
{
	void RoomManager::Init(const int maxRoomNum, NServerNetLib::TcpNetwork* pNetwork)
	{
		for (int i = 0; i < maxRoomNum; ++i)
		{
			auto newRoom = new Room();
			m_RoomList.push_back(newRoom);
			m_RoomList[i]->Init((short)i, 2, pNetwork);
			m_RoomList[i]->SendPacketFunc = SendPacketFunc;
		}
	}

	std::optional <Room*> RoomManager::FindRoom(const int roomIndex)
	{
		if (roomIndex < 0 || roomIndex >= m_RoomList.size())
		{
			return std::nullopt;
		}

		return m_RoomList[roomIndex];
	}

	std::optional <Room*> RoomManager::FindProperRoom()
	{
		for (auto room : m_RoomList)
		{
			if (room->m_UserList.size() == 1)
			{
				auto roomIndex = room->GetIndex();
				return m_RoomList[roomIndex];
			}
		}
		return std::nullopt;
	}
}