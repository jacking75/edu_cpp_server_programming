#pragma once

// 멀티바이트 집합 사용시 define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>
//#include "CommonClass.h"

using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000
#define MAX_CLIENTS		1000

// IOCP 소켓 구조체
struct scSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			wsabuf;
	SOCKET			socket;
	char			buf[MAX_BUFFER];
	int				recvbytes;
	int				sendbytes;
};

// 패킷 처리 함수 포인터
struct FuncProcess
{
	void(*funcProcessPacket)(stringstream & RecvStream, scSOCKETINFO * pSocket);
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};

class IocpBase
{
public:
	IocpBase();
	virtual ~IocpBase();

	// 소켓 등록 및 서버 정보 설정
	bool Initialize();
	// 서버 시작
	virtual void StartServer();
	// 작업 스레드 생성
	virtual bool CreateWorkerThread();	
	// 작업 스레드
	virtual void WorkerThread();
	// 클라이언트에게 송신
	virtual void Send(scSOCKETINFO * pSocket);
	// 클라이언트 수신 대기
	virtual void Recv(scSOCKETINFO * pSocket);		

protected:
	scSOCKETINFO	*SocketInfo;	// 소켓 정보
	SOCKET			ListenSocket;	// 서버 리슨 소켓
	HANDLE			hIOCP;			// IOCP 객체 핸들
	bool			bAccept;		// 요청 동작 플래그
	bool			bWorkerThread;	// 작업 스레드 동작 플래그
	HANDLE *		hWorkerHandle;	// 작업 스레드 핸들		
	int				nThreadCnt;	
};
