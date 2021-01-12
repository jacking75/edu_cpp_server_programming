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
		//TODO �����
		// User ����ó�� �ѹ��� �� ���� ���� �߰� ����� üũ �ϵ��� �ϼ���. �ѹ��� ��� �� �� üũ�ϸ� ���ϰ� ���������� �����Ǵ� ������ ���� �� �ֽ��ϴ�.
		//-> �ذ�
		auto curTime = std::chrono::system_clock::now();
		auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - m_LatestRoomCheckTime);

		if (diffTime.count() < checkIndexTime)
		{
			return;
		}
		else
		{
			m_LatestRoomCheckTime = curTime;
		}

		auto curSecTime = std::chrono::system_clock::to_time_t(curTime);

		const auto maxSessionCount = (int)m_RoomList.size();

		if (m_LatestRoomCheckIndex >= maxSessionCount)
		{
			m_LatestRoomCheckIndex = -1;
		}

		++m_LatestRoomCheckIndex;

		auto lastCheckIndex = m_LatestRoomCheckIndex + checkIndexRangeCount;
		if (lastCheckIndex > maxSessionCount)
		{
			lastCheckIndex = maxSessionCount;
		}

		for (; m_LatestRoomCheckIndex < lastCheckIndex; ++m_LatestRoomCheckIndex)
		{
			auto i = m_LatestRoomCheckIndex;
			m_RoomList[i]->CheckTurnTimeOut();
		}

	}
}