#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <initguid.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

DEFINE_GUID(BthServer_Service, 0x4672de25, 0x588d, 0x48af,
	0x80, 0x73, 0x5f, 0x2b, 0x7b, 0x0, 0x60, 0x1f);

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	// socket()
	SOCKET listen_sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_BTH serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.addressFamily = AF_BTH;
	serveraddr.btAddr = 0;
	serveraddr.port = BT_PORT_ANY;
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// 서버 포트 번호 출력(옵션)
	int addrlen = sizeof(serveraddr);
	retval = getsockname(listen_sock, (SOCKADDR *)&serveraddr, &addrlen);
	if(retval == SOCKET_ERROR) err_quit("bind()");
	printf("[블루투스 서버] 사용 중인 포트 번호 : %d\n", serveraddr.port);

	// 서버 정보 등록(필수)
	CSADDR_INFO addrinfo;
	addrinfo.LocalAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.LocalAddr.iSockaddrLength = sizeof(serveraddr);
	addrinfo.RemoteAddr.lpSockaddr = (SOCKADDR *)&serveraddr;
	addrinfo.RemoteAddr.iSockaddrLength = sizeof(serveraddr);
	addrinfo.iSocketType = SOCK_STREAM;
	addrinfo.iProtocol = BTHPROTO_RFCOMM;

	WSAQUERYSET qset;
	ZeroMemory(&qset, sizeof(qset));
	qset.dwSize = sizeof(qset);
	qset.lpszServiceInstanceName = "Bluetooth Server Test Service";
	qset.lpServiceClassId = (GUID *)&BthServer_Service;
	qset.dwNameSpace = NS_BTH;	
	qset.dwNumberOfCsAddrs = 1;
	qset.lpcsaBuffer = &addrinfo;
	retval = WSASetService(&qset, RNRSERVICE_REGISTER, 0);
	if(retval == SOCKET_ERROR) err_quit("WSASetService()");

	// listen()
	retval = listen(listen_sock, 1);
	if(retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_BTH clientaddr;
	char buf[BUFSIZE+1];

	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			err_display("accept()");
			break;
		}

		printf("\n[블루투스 서버] 클라이언트 접속!\n");

		// 클라이언트와 데이터 통신
		while(1){
			// 데이터 받기
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if(retval == SOCKET_ERROR){
				err_display("recv()");
				break;
			}
			else if(retval == 0)
				break;

			// 받은 데이터 출력
			buf[retval] = '\0';
			printf("[블루투스 서버] %s\n", buf);
		}

		// closesocket()
		closesocket(client_sock);
		printf("[블루투스 서버] 클라이언트 종료!\n");
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}