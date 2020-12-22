#include "RoomManager.h"
#include "Room.h"
#include <optional>
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
namespace OmokServerLib
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

	void RoomManager::CheckRoomGameTime()
	{
		//TODO 최흥배
		// User 조사처럼 한번에 다 하지 말고 잘게 나누어서 체크 하도록 하세요. 한번에 모든 방 다 체크하면 부하가 순간적으로 증가되는 현상이 생길 수 있습니다.
		for (auto room : m_RoomList)
		{
			room->CheckTurnTimeOut();
		}
	}
}