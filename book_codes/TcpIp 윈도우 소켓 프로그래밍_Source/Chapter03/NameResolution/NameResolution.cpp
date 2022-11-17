#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdio.h>

#include <WS2tcpip.h>

#define TESTNAME "www.example.com"

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

// 도메인 이름 -> IPv4 주소
BOOL GetIPAddr(char* host_name, addrinfo* addr)
{
	struct addrinfo hints;
	char* szRemoteAddress = NULL, * szRemotePort = NULL;
	int rc; memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	rc = getaddrinfo(host_name, szRemotePort, &hints, &addr);
	if (rc == WSANO_DATA) { 
		return FALSE; 
	}

	return TRUE;
}

// IPv4 주소 -> 도메인 이름
BOOL GetDomainName(char* pIp, char* name, int namelen)
{
	struct addrinfo* res = 0;
	getaddrinfo(pIp, 0, 0, &res);

	int ret = getnameinfo(res->ai_addr, (socklen_t)res->ai_addrlen, name, namelen, 0, 0, 0);
	if (ret != 0)
	{
		err_display("gethostbyaddr()");
		return FALSE;
	}

	return TRUE;
}

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("도메인 이름(변환 전) = %s\n", TESTNAME);

	// 도메인 이름 -> IP 주소
	//IN_ADDR addr;
	struct addrinfo addr;
	if (GetIPAddr(TESTNAME, &addr)) {
		// 성공이면 결과 출력
		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(addr.ai_addr), clientIP, 32);

		printf("IP 주소(변환 후) = %s\n", clientIP);

		// IP 주소 -> 도메인 이름
		char name[256];
		if (GetDomainName(clientIP, name, sizeof(name))) {
			// 성공이면 결과 출력
			printf("도메인 이름(다시 변환 후) = %s\n", name);
		}
	}

	WSACleanup();
	return 0;
}



/*
// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

//// 도메인 이름 -> IPv4 주소
BOOL GetIPAddr(char *name, IN_ADDR *addr)
{
	HOSTENT *ptr = gethostbyname(name);
	if(ptr == NULL){
		err_display("gethostbyname()");
		return FALSE;
	}
	if(ptr->h_addrtype != AF_INET)
		return FALSE;
	memcpy(addr, ptr->h_addr, ptr->h_length);
	return TRUE;
}

// IPv4 주소 -> 도메인 이름
BOOL GetDomainName(IN_ADDR addr, char *name, int namelen)
{
	HOSTENT *ptr = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
	if(ptr == NULL){
		err_display("gethostbyaddr()");
		return FALSE;
	}
	if(ptr->h_addrtype != AF_INET)
		return FALSE;
	strncpy(name, ptr->h_name, namelen);
	return TRUE;
}

int main(int argc, char *argv[])
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	printf("도메인 이름(변환 전) = %s\n", TESTNAME);

	// 도메인 이름 -> IP 주소
	IN_ADDR addr;
	if(GetIPAddr(TESTNAME, &addr)){
		// 성공이면 결과 출력
		printf("IP 주소(변환 후) = %s\n", inet_ntoa(addr));
	
		// IP 주소 -> 도메인 이름
		char name[256];
		if(GetDomainName(addr, name, sizeof(name))){
			// 성공이면 결과 출력
			printf("도메인 이름(다시 변환 후) = %s\n", name);
		}
	}

	WSACleanup();
	return 0;
}
*/