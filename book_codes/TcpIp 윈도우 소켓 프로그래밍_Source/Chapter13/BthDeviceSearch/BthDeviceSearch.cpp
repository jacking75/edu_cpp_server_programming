#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2bth.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa)!=0) return 1;

	// 블루투스 장치 검색 준비
	DWORD qslen = sizeof(WSAQUERYSET);
	WSAQUERYSET *qs = (WSAQUERYSET *)malloc(qslen);
	ZeroMemory(qs, qslen);
	qs->dwSize = qslen;
	qs->dwNameSpace = NS_BTH;
	DWORD flags = LUP_CONTAINERS; /* 필수! */
	flags |= LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;

	// 블루투스 장치 검색 시작
	HANDLE hLookup;
	retval = WSALookupServiceBegin(qs, flags, &hLookup);
	if(retval == SOCKET_ERROR){
		printf("[오류] 발견된 블루투스 장치 없음!\n");
		exit(1);
	}

	// 검색된 블루투스 장치 정보 출력
	SOCKADDR_BTH *sa = NULL;
	bool done = false;
	while(!done){
		retval = WSALookupServiceNext(hLookup, flags, &qslen, qs);
		if(retval == NO_ERROR){
			// 블루투스 장치 정보를 담고 있는 소켓 주소 구조체에 접근
			sa = (SOCKADDR_BTH *)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			// 블루투스 장치 주소를 문자열로 출력
			char addr[40] = {0};
			DWORD addrlen = sizeof(addr);
			WSAAddressToString((SOCKADDR *)sa, sizeof(SOCKADDR_BTH),
				NULL, addr, &addrlen);
			printf("블루투스 장치 발견! %s - %s\n",
				addr, qs->lpszServiceInstanceName);
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

	// 블루투스 장치 검색 종료
	WSALookupServiceEnd(hLookup);
	free(qs);

	// 윈속 종료
	WSACleanup();
	return 0;
}