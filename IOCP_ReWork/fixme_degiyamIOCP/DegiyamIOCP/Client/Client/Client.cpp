// Client.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <winsock2.h>

#define BUFSIZE		4096
#define	PORT_SERVER	23452

#define CG_TEST_LOGIN 50000 + 4000
#define PACKET_SIZE		2
#define PROTOCOL_SIZE	2
#define HEADER_SIZE		PACKET_SIZE + PROTOCOL_SIZE

enum CS_AUTH_PROTOCOL
{
	CS_AUTH_START		= 0x1000,

	CS_AUTH_PLAYERKEY	= CS_AUTH_START + 1,

	CS_AUTH_LOGIN_REQ	= CS_AUTH_START + 2,
	CS_AUTH_LOGIN_ACK	= CS_AUTH_START + 3,
};

int MK_GetPacket_CS_AUTH_LOGIN_REQ(char *send_buf, const int id, const int pw);

int _tmain(int argc, _TCHAR* argv[])
{
	UINT nCount = 0;

	while ( 1 )
	{
		WSADATA		wsaData;
		SOCKET		hSocket;
		SOCKADDR_IN	servAddr;

		if ( WSAStartup(MAKEWORD(2, 2), &wsaData) != 0 )
			return 0;

		hSocket = socket(PF_INET, SOCK_STREAM, 0);
		if ( hSocket == INVALID_SOCKET)
			return 0;

		memset(&servAddr, 0, sizeof(servAddr));
		servAddr.sin_family			= AF_INET;
		servAddr.sin_addr.s_addr	= inet_addr("127.0.0.1");
		servAddr.sin_port			= htons(PORT_SERVER);

		if ( inet_ntoa(servAddr.sin_addr) == NULL )
			return 0;

		if ( connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR )	// 서버로 연결 요청
			return 0;

		char SendBuffer[BUFSIZE] = "";
		char RecvBuffer[BUFSIZE] = "";

		int nSendSize = MK_GetPacket_CS_AUTH_LOGIN_REQ(SendBuffer, 999, 555);

		const int MAX_REPEAT_COUNT = 5;
		int repeatCount = 0;
		while (repeatCount < MAX_REPEAT_COUNT)
		{
			int nSend = send(hSocket, (char*)&SendBuffer, nSendSize, 0);
			int nRecv = recv(hSocket, (char*)&RecvBuffer, BUFSIZE, 0);
			
			printf("Recv Data : %s\n", RecvBuffer);

			ZeroMemory(RecvBuffer, BUFSIZE);

			++repeatCount;
			Sleep(2000);
		}

		closesocket(hSocket);
		WSACleanup();

		printf("연결, 해제 횟수 : %d\n", ++nCount);

//		Sleep(atoi(argv[1]));
		Sleep(100);
	}

	return 0;
}

struct	header_special
{
	unsigned	short	body_size;
	unsigned	short	command;
};

struct	body_CS_AUTH_LOGIN_REQ
{
	int	id;
	int	pw;
};

int MK_GetPacket_CS_AUTH_LOGIN_REQ(char *send_buf, const int id, const int pw)
{
	struct	header_special			*header	= (struct header_special *)send_buf;
	struct	body_CS_AUTH_LOGIN_REQ	*body	= (struct body_CS_AUTH_LOGIN_REQ *)(send_buf + HEADER_SIZE);

	header->body_size	= sizeof(struct body_CS_AUTH_LOGIN_REQ);
	header->command		= CS_AUTH_LOGIN_REQ;

	body->id	= id;
	body->pw	= pw;

	return	(HEADER_SIZE + header->body_size);
}