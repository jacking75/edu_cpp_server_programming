#pragma once



namespace NServerNetLib
{
	struct ServerConfig
	{
		unsigned short Port;
		int BackLogCount;

		int MaxClientCount;
		int ExtraClientCount; // 가능하면 로그인에서 짜르도록 MaxClientCount + 여유분을 준비한다.

		short MaxClientSockOptRecvBufferSize;
		short MaxClientSockOptSendBufferSize;
		short MaxClientRecvBufferSize;
		short MaxClientSendBufferSize;

		bool IsLoginCheck;	// 연결 후 특정 시간 이내에 로그인 완료 여부 조사

		int MaxLobbyCount;
		int MaxLobbyUserCount;
		int MaxRoomCountByLobby;
		int MaxRoomUserCount;
	};

	const int MAX_IP_LEN = 32; // IP 문자열 최대 길이
	const int MAX_PACKET_BODY_SIZE = 1024; // 최대 패킷 보디 크기
}
