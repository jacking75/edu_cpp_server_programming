#pragma once
#include <string>
#include <WinSock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

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
	const int PACKET_DATA_BUFFER_SIZE = 8096;

	enum class PACKET_ID : short
	{
		NTF_SYS_CONNECT_SESSION = 2,
		NTF_SYS_CLOSE_SESSION = 3,
		NTF_SYS_RECV_SESSION=4

	};

	struct RecvPacketInfo
	{
		UINT32 SessionIndex = 0;
		UINT16 PacketId = 0;
		UINT16 PacketBodySize = 0;
		char* pRefData = 0;
	};

	const UINT32 MAX_SOCK_RECVBUF = 256;	// 소켓 버퍼의 크기
	const UINT32 MAX_SOCK_SENDBUF = 4096;	// 소켓 버퍼의 크기
	const UINT64 RE_USE_SESSION_WAIT_TIMESEC = 3;


	enum class IOOperation
	{
		ACCEPT,
		RECV,
		SEND
	};

	//WSAOVERLAPPED구조체를 확장 시켜서 필요한 정보를 더 넣었다.
	struct stOverlappedEx
	{
		WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
		WSABUF		m_wsaBuf;				//Overlapped I/O작업 버퍼
		IOOperation m_eOperation;			//작업 동작 종류
		UINT32 SessionIndex = 0;
	};


}
