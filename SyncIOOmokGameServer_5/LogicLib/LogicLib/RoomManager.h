#pragma once

//TODO 최흥배
// 전반 선언으로 해주세요
//-> 엄청난 에러들 발생... 다시고민
#include <functional>
#include "../../ServerNetLib/ServerNetLib/TcpNetwork.h"
#include "Room.h"
namespace OmokServerLib
{

	class RoomManager
	{
	public:

		RoomManager() = default;
		~RoomManager() = default;

		//TODO 최흥배
		// 아마 다른 헤더 파일에서 TcpNetwork 관련 헤더 파일을 include 해서 이 파일에서 include 하지 않아도 빌드 에러가 나지 않습니다.
		// 그러나 이런 경우 다른 파일에서 헤더 포함을 수정하면 여기에서 빌드 에러가 나올 수 있습니다.
		// 전방 선언을 하고 cpp에서 TcpNetwork 헤더 파일을 include 하세요
		//-> 엄청난 에러 발생... 다시고민
		void Init(const int maxRoomNum, NServerNetLib::TcpNetwork* pNetwork);

		std::optional <Room*> FindProperRoom();

		std::optional <Room*> FindRoom(const int roomIndex);

		std::function<void(const int, const short, const short, char*)> SendPacketFunc;

		void CheckRoomGameTime();

	private:

		std::vector<Room*> m_RoomList;
	};
}


