#define _WIN32_WINDOWS 0x0410 // 윈도우 98 이상
#define _WIN32_WINNT   0x0500 // 윈도우 2000 이상

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <af_irda.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

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

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while(left > 0){
		received = recv(s, ptr, left, flags);
		if(received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if(received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	// socket()
	SOCKET sock = socket(AF_IRDA, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// 최대 10개의 IrDA 장치 검색
	struct MyDevList{
		ULONG numDevice;             // IrDA 장치 개수
		IRDA_DEVICE_INFO Device[10]; // IrDA 장치 정보
	} optval;
	optval.numDevice = 0;            // IrDA 장치 개수를 0으로 초기화
	int optlen = sizeof(optval);
	retval = getsockopt(sock, SOL_IRLMP, IRLMP_ENUMDEVICES,
		(char *)&optval, &optlen);
	if(retval == SOCKET_ERROR) err_quit("getsockopt");

	// 장치 검색 결과 출력
	printf("[IrDA 클라이언트] %d개의 적외선 장치 발견!\n", optval.numDevice);
	if(optval.numDevice == 0) exit(1);

	// 첫 번째 IrDA 장치에 접속
	SOCKADDR_IRDA serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.irdaAddressFamily = AF_IRDA;
	memcpy(&serveraddr.irdaDeviceID[0], &optval.Device[0].irdaDeviceID[0], 4);
	strcpy(serveraddr.irdaServiceName, "IrServer");
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len;

	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		ZeroMemory(buf, sizeof(buf));
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
		printf("[IrDA 클라이언트] %d바이트를 보냈습니다.\n", retval);

		// 데이터 받기
		retval = recvn(sock, buf, retval, 0);
		if(retval == SOCKET_ERROR){
			err_display("recv()");
			break;
		}
		else if(retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[IrDA 클라이언트] %d바이트를 받았습니다.\n", retval);
		printf("[받은 데이터] %s\n", buf);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}