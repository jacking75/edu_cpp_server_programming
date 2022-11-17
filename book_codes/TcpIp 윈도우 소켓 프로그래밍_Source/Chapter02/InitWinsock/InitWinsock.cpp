#pragma comment(lib, "ws2_32")
#include <winsock2.h>

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

int main(int argc, char *argv[])
{
	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;
	MessageBox(NULL, "윈속 초기화 성공", "알림", MB_OK);

	// socket()
	SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(tcp_sock == INVALID_SOCKET) err_quit("socket()");
	MessageBox(NULL, "TCP 소켓 생성 성공", "알림", MB_OK);

	// closesocket()
	closesocket(tcp_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}