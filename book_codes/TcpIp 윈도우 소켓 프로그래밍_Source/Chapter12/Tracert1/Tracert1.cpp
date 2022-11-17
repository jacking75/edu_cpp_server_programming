#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 1500

// IP 헤더
typedef struct _IPHEADER
{
	u_char  ip_hl:4;  // header length
	u_char  ip_v:4;   // version
	u_char  ip_tos;   // type of service
	short   ip_len;   // total length
	u_short ip_id;    // identification
	short   ip_off;   // flags & fragment offset field
	u_char  ip_ttl;   // time to live
	u_char  ip_p;     // protocol
	u_short ip_cksum; // checksum
	IN_ADDR ip_src;   // source address
	IN_ADDR ip_dst;   // destination address
} IPHEADER;

// ICMP 메시지
typedef struct _ICMPMESSAGE
{
	u_char  icmp_type;  // type of message
	u_char  icmp_code;  // type sub code
	u_short icmp_cksum; // checksum
	u_short icmp_id;    // identifier
	u_short icmp_seq;   // sequence number
} ICMPMESSAGE;

#define ICMP_ECHOREPLY   0
#define ICMP_DESTUNREACH 3
#define ICMP_ECHOREQUEST 8
#define ICMP_TIMEOUT     11

// ICMP 메시지 분석 함수
BOOL DecodeICMPMessage(char *, int, SOCKADDR_IN *, SOCKADDR_IN *);
// 도메인 이름 -> IPv4 주소 변환 함수
BOOL GetIPAddr(char *name, IN_ADDR *addr);
// 체크섬 계산 함수
u_short checksum(u_short *buffer, int size);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);

int main(int argc, char *argv[])
{
	int retval;

	if(argc < 2){
		printf("Usage: %s <host_name>\n", argv[0]);
		return 1;
	}

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// 소켓 옵션 설정
	int optval = 1000;
	retval = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
		(char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
		(char *)&optval, sizeof(optval));
	if(retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	SOCKADDR_IN destaddr;
	ZeroMemory(&destaddr, sizeof(destaddr));
	destaddr.sin_family = AF_INET;
	IN_ADDR addr;
	if(GetIPAddr(argv[1], &addr))
		destaddr.sin_addr = addr;
	else
		return 1;

	// 데이터 통신에 사용할 변수
	ICMPMESSAGE icmpmsg;
	char buf[BUFSIZE];
	SOCKADDR_IN peeraddr;
	int addrlen;

	for(int ttl=1; ttl<=30; ttl++){
		// TTL 값 변경
		optval = ttl;
		retval = setsockopt(sock, IPPROTO_IP, IP_TTL,
			(char *)&optval, sizeof(optval));
		if(retval == SOCKET_ERROR) err_quit("setsockopt()");

		// ICMP 메시지 초기화
		ZeroMemory(&icmpmsg, sizeof(icmpmsg));
		icmpmsg.icmp_type = ICMP_ECHOREQUEST;
		icmpmsg.icmp_code = 0;
		icmpmsg.icmp_id = (u_short)GetCurrentProcessId();
		icmpmsg.icmp_seq = ttl;
		icmpmsg.icmp_cksum = checksum((u_short *)&icmpmsg, sizeof(icmpmsg));

		// 에코 요청 ICMP 메시지 보내기
		retval = sendto(sock, (char *)&icmpmsg, sizeof(icmpmsg), 0,
			(SOCKADDR *)&destaddr, sizeof(destaddr));
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() == WSAETIMEDOUT){
				printf("[오류] Send timed out!\n");
				continue;
			}
			err_display("sendto()");
			break;
		}

		// ICMP 메시지 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if(retval == SOCKET_ERROR){
			if(WSAGetLastError() == WSAETIMEDOUT){
				printf("[오류] Request timed out!\n");
				continue;
			}
			err_display("recvfrom()");
			break;
		}

		// ICMP 메시지 분석
		if(!DecodeICMPMessage(buf, retval, &peeraddr, &destaddr))
			break;
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// ICMP 메시지 분석 함수
BOOL DecodeICMPMessage(char *buf, int len, SOCKADDR_IN *from, SOCKADDR_IN *to)
{
    IPHEADER *iphdr = (IPHEADER *)buf;
    int iphdrlen = iphdr->ip_hl * 4;
    ICMPMESSAGE *icmpmsg = (ICMPMESSAGE *)(buf + iphdrlen);

	// 오류 체크
	if((len - iphdrlen) < 8){
		printf("[오류] ICMP packet is too short!\n");
		return FALSE;
	}

	switch(icmpmsg->icmp_type){
	case ICMP_TIMEOUT:
	    printf("%s\n", inet_ntoa(from->sin_addr));
		return TRUE;
	case ICMP_ECHOREPLY:
	    printf("%s\n", inet_ntoa(from->sin_addr));
		return FALSE;
	case ICMP_DESTUNREACH:
		printf("[오류] Host unreachable!\n");
		return FALSE;
	default:
		printf("[오류] Unknown icmp packet!\n");
		return TRUE;
	}

    return TRUE;
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

// 체크섬 계산 함수
u_short checksum(u_short *buf, int len)
{
	u_long cksum = 0;
	u_short *ptr = buf;
	int left = len;

	while(left > 1){
		cksum += *ptr++;
		left -= sizeof(u_short);
	}

	if(left == 1)
		cksum += *(u_char *)buf;

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (u_short)(~cksum);
}

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