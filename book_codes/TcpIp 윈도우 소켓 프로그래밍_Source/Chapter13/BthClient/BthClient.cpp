#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <Initguid.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERADDR "00:09:31:00:13:F9"
#define BUFSIZE    512

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

	// 블루투스 장치 검색(생략)
	// - 편의상 SERVERADDR 매크로 상수로 대신한다.

	// 블루투스 서비스 검색 준비
	DWORD qslen = sizeof(WSAQUERYSET);
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen);
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;
	qs->dwNameSpace = NS_BTH;
	qs->lpServiceClassId = (GUID *)&BthServer_Service;
	qs->lpszContext = SERVERADDR;
	DWORD flags = LUP_FLUSHCACHE | LUP_RETURN_ADDR;
	
	// 블루투스 서비스 검색 시작
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup);
	if(retval == SOCKET_ERROR){
		printf("[오류] 발견된 블루투스 장치 없음!\n");
		exit(1);
	}

	// 검색된 블루투스 서비스 정보 확인
	SOCKADDR_BTH *sa = NULL;
	int serverport = 0;
	bool done = false;
	while(!done){
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if(retval == NO_ERROR){
			// 블루투스 장치 정보를 담고 있는 소켓 주소 구조체에 접근
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			// 서버 포트 번호 저장 후 탈출
			serverport = sa->port;
			break;
		}
		else{
			if(WSAGetLastError() == WSAEFAULT){
				free(qs);
				qs = (WSAQUERYSET *)malloc(qslen);
			}
			else{
				done = true;
			}
		}
	}
	if(sa == NULL){
		printf("[오류] 블루투스 장치(%s)에서 실행 중인 서버 없음!\n", SERVERADDR);
		exit(1);
	}

	// 블루투스 서비스 검색 종료
	WSALookupServiceEnd(hLookup);
	free(qs);

	// socket()
	SOCKET sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_BTH serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	int addrlen = sizeof(serveraddr);
	WSAStringToAddress(SERVERADDR, AF_BTH, NULL,
		(SOCKADDR *)&serveraddr, &addrlen);
	serveraddr.port = serverport;
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len;

	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		printf("\n[보낼 데이터] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		// 데이터 보내기
		retval = send(sock, buf, strlen(buf), 0);
		if(retval == SOCKET_ERROR){
			err_display("send()");
			break;
		}
		printf("[블루투스 클라이언트] %d바이트를 보냈습니다.\n", retval);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}