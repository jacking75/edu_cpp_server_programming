#include "RoomManager.h"
#include "Room.h"
#include "User.h"


namespace ChatServerLib
{
	void RoomManager::Init(const int maxRoomNum, NServerNetLib::TcpNetwork* pNetwork)
	{
		for (int i = 0; i < maxRoomNum; ++i)
		{
			m_RoomList.emplace_back(new Room());
			m_RoomList[i]->Init((short)i, maxRoomNum);
			m_RoomList[i]->SetNetwork(pNetwork);
		}
	}

	Room* RoomManager::FindRoom(const int roomIndex)
	{
		if (roomIndex > (m_RoomList.size() - 1))
		{
			return nullptr;
		}

		return m_RoomList[roomIndex];
	}

}