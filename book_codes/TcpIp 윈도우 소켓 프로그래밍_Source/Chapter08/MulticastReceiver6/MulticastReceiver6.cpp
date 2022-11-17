#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define MULTICASTIP "FF12::1:2:3:4"
#define LOCALPORT   9000
#define BUFSIZE     512

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
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET6, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// SO_REUSEADDR 옵션 설정
	BOOL optval = TRUE;
	retval = setsockopt(sock, SOL_SOCKET,
		SO_REUSEADDR, (char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	SOCKADDR_IN6 localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin6_family = AF_INET6;
	localaddr.sin6_addr = in6addr_any;
	localaddr.sin6_port = htons(LOCALPORT);
	retval = bind(sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// 주소 변환(문자열 -> IPv6)
	SOCKADDR_IN6 tmpaddr;
	int addrlen = sizeof(tmpaddr);
	WSAStringToAddress(MULTICASTIP, AF_INET6, NULL,
		(SOCKADDR *)&tmpaddr, &addrlen);

	// 멀티캐스트 그룹 가입
	struct ipv6_mreq mreq;
	mreq.ipv6mr_multiaddr = tmpaddr.sin6_addr;
	mreq.ipv6mr_interface = 0;
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN6 peeraddr;
	char buf[BUFSIZE+1];

	// 멀티캐스트 데이터 받기
	while(1){
		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_display("recvfrom()");
			continue;
		}

		// 주소 변환(IPv6 -> 문자열)
		char ipaddr[50];
		DWORD ipaddrlen = sizeof(ipaddr);
		WSAAddressToString((SOCKADDR *)&peeraddr, sizeof(peeraddr),
			NULL, ipaddr, &ipaddrlen);

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP/%s] %s\n", ipaddr, buf);
	}

	// 멀티캐스트 그룹 탈퇴
	retval = setsockopt(sock, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}