#pragma once
#include <string>

namespace NServerNetLib
{
	struct ServerConfig
	{
		unsigned short Port;
		int BackLogCount;
		int MaxClientCount;
		int ExtraClientCount;

		short MaxClientRecvBufferSize;
		short MaxClientSendBufferSize;

		int MaxRoomCountByLobby;
		int MaxRoomUserCount;

		std::string RedisAddress;
		int RedisPortNum;

		bool IsLoginCheck;

	};

	const int MAX_IP_LEN = 32; // IP 문자열 최대 길이
	const int MAX_PACKET_BODY_SIZE = 1024; // 최대 패킷 보디 크기

	enum class PACKET_ID : short
	{
		NTF_SYS_CONNECT_SESSION = 2,
		NTF_SYS_CLOSE_SESSION = 3,
		NTF_SYS_RECV_SESSION=4

	};

	struct RecvPacketInfo
	{
		int SessionIndex = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};

#pragma pack(push, 1)
	struct PacketHeader
	{
		short TotalSize;
		short Id;
		unsigned char Reserve;
	};

	const int PACKET_HEADER_SIZE = sizeof(PacketHeader);


	struct PktNtfSysCloseSession : PacketHeader
	{
		int SockFD;
	};
#pragma pack(pop)
}
