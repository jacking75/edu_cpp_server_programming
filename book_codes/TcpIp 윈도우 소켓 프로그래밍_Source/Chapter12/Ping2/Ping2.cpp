#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

// 구조체 선언
typedef struct {
	unsigned char Ttl;             // Time to live
	unsigned char Tos;             // Type of service
	unsigned char Flags;           // IP header flags
	unsigned char OptionsSize;     // Size in bytes of options data
	unsigned char *OptionsData;    // Pointer to options data
} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;

typedef struct {
	DWORD Address;                 // Replying address
	unsigned long  Status;         // Reply status
	unsigned long  RoundTripTime;  // RTT in milliseconds
	unsigned short DataSize;       // Echo data size
	unsigned short Reserved;       // Reserved for system use
	void *Data;                    // Pointer to the echo data
	IP_OPTION_INFORMATION Options; // Reply options
} ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;

// 함수 포인터 타입 선언
typedef HANDLE (WINAPI *pIcmpCreateFile)(void);
typedef DWORD (WINAPI *pIcmpSendEcho)(
	HANDLE IcmpHandle, ULONG DestinationAddress,
	LPVOID RequestData, WORD RequestSize,
	PIP_OPTION_INFORMATION RequestOptions,
	LPVOID ReplyBuffer, DWORD ReplySize, DWORD Timeout);
typedef BOOL (WINAPI *pIcmpCloseHandle)(HANDLE IcmpHandle);

// 도메인 이름 -> IPv4 주소 변환 함수
BOOL GetIPAddr(char *name, IN_ADDR *addr);
// 소켓 함수 오류 출력
void err_display(char *msg);

int main(int argc, char *argv[])
{
	if(argc < 2){
		printf("Usage: %s <host_name>\n", argv[0]);
		return 1;
	}

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	// argv[1]을 32비트 IP 주소로 변환
	IN_ADDR addr;
	if(!GetIPAddr(argv[1], &addr)) return 1;

	// ICMP.DLL 로드
	HINSTANCE hDLL = LoadLibrary("ICMP.DLL");
	if(hDLL == NULL){
		printf("[오류] ICMP.DLL을 찾을 수 없습니다!\n");
		return 1;
	}

	// Icmp*() 함수 주소 얻기
	pIcmpCreateFile IcmpCreateFile = (pIcmpCreateFile)
		GetProcAddress(hDLL, "IcmpCreateFile");
	pIcmpSendEcho IcmpSendEcho = (pIcmpSendEcho)
		GetProcAddress(hDLL, "IcmpSendEcho");
	pIcmpCloseHandle IcmpCloseHandle = (pIcmpCloseHandle)
		GetProcAddress(hDLL, "IcmpCloseHandle");
	if(IcmpCreateFile == NULL || IcmpSendEcho == NULL
		|| IcmpCloseHandle == NULL)
	{
		printf("[오류] Icmp*() 함수를 찾을 수 없습니다!\n");
		return 1;
	}

	// IcmpCreateFile()
	HANDLE hIcmp = IcmpCreateFile();
	if(hIcmp == INVALID_HANDLE_VALUE) return 1;

	// 버퍼 정의
	char request[32];
	int replysize = sizeof(ICMP_ECHO_REPLY) + sizeof(request) + 1500;
	ICMP_ECHO_REPLY *reply = (ICMP_ECHO_REPLY *)malloc(replysize);

	for(int i=0; i<4; i++){
		// Ping 패킷 보내기
		DWORD retval = IcmpSendEcho(hIcmp, addr.s_addr, request,
			sizeof(request), NULL, reply, replysize, 1000);
		if(retval != 0){
			printf("Reply from %s: ", inet_ntoa((IN_ADDR)addr));
			printf("time = %dms, ", reply->RoundTripTime);
			printf("TTL = %d\n", reply->Options.Ttl);
		}
		else{
			printf("[오류] 응답이 없거나 오류가 발생했습니다!\n");
		}

		Sleep(1000);
	}

	// 동적 메모리와 DLL 제거
	free(reply);
	FreeLibrary(hDLL);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 도메인 이름 -> IPv4 주소 변환 함수
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