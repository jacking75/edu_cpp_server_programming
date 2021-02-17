#include "stdafx.h"
#include "IocpBase.h"
#include <process.h>
#include <sstream>
#include <algorithm>
#include <string>

IocpBase::IocpBase()
{
	// 멤버 변수 초기화
	bWorkerThread = true;
	bAccept = true;
}


IocpBase::~IocpBase()
{
	// winsock 의 사용을 끝낸다
	WSACleanup();
	// 다 사용한 객체를 삭제
	if (SocketInfo)
	{
		delete[] SocketInfo;
		SocketInfo = NULL;
	}

	if (hWorkerHandle)
	{
		delete[] hWorkerHandle;
		hWorkerHandle = NULL;
	}
}

bool IocpBase::Initialize()
{
	int nResult;
	// winsock 2.2 버전으로 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("[ERROR] winsock 초기화 실패\n");
		return false;
	}

	// 소켓 생성
	ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (ListenSocket == INVALID_SOCKET)
	{
		printf("[ERROR] 소켓 생성 실패\n");
		return false;
	}

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 소켓 설정
	// boost bind 와 구별짓기 위해 ::bind 사용
	nResult = ::bind(ListenSocket, (SOCKADDR *)&serverAddr, sizeof(SOCKADDR_IN));
	
	if (nResult == SOCKET_ERROR)
	{
		printf("[ERROR] bind 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	// 수신 대기열 생성
	nResult = listen(ListenSocket, SOMAXCONN);
	if (nResult == SOCKET_ERROR)
	{
		printf("[ERROR] listen 실패\n");
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

void IocpBase::StartServer()
{
	int nResult;
	// 클라이언트 정보
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	DWORD recvBytes;

	// Completion Port 객체 생성
	hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Worker Thread 생성
	if (!CreateWorkerThread()) return;	

	printf("[INFO] 서버 시작...\n");

	// 클라이언트 접속을 받음
	while (bAccept)
	{
		clientSocket = WSAAccept(ListenSocket, (SOCKADDR *)&clientAddr, &addrLen, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			printf("[ERROR] Accept 실패\n");
			return;
		}

		SocketInfo = new scSOCKETINFO();
		SocketInfo->socket = clientSocket;
		SocketInfo->recvbytes = SocketInfo->sendbytes = 0;
		SocketInfo->wsabuf.len = MAX_BUFFER;
		SocketInfo->wsabuf.buf = SocketInfo->buf;

		printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);

		DWORD flags = 0;
		// 중첩 소켓을 지정하고 완료시 실행될 함수를 넘겨줌
		nResult = WSARecv(
			SocketInfo->socket,
			&SocketInfo->wsabuf,
			1,
			&recvBytes,
			&flags,
			&(SocketInfo->overlapped),
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("[ERROR] IO Pending 실패 : %d", WSAGetLastError());
			return;
		}
	}

}

bool IocpBase::CreateWorkerThread()
{
	return false;
}

void IocpBase::Send(scSOCKETINFO * pSocket)
{
	int nResult;
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	nResult = WSASend(
		pSocket->socket,
		&(pSocket->wsabuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSASend 실패 : ", WSAGetLastError());
	}


}

void IocpBase::Recv(scSOCKETINFO * pSocket)
{
	int nResult;
	// DWORD	sendBytes;
	DWORD	dwFlags = 0;

	// stSOCKETINFO 데이터 초기화
	ZeroMemory(&(pSocket->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocket->buf, MAX_BUFFER);
	pSocket->wsabuf.len = MAX_BUFFER;
	pSocket->wsabuf.buf = pSocket->buf;
	pSocket->recvbytes = 0;
	pSocket->sendbytes = 0;

	dwFlags = 0;

	// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
	nResult = WSARecv(
		pSocket->socket,
		&(pSocket->wsabuf),
		1,
		(LPDWORD)&pSocket,
		&dwFlags,
		(LPWSAOVERLAPPED)&(pSocket->overlapped),
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("[ERROR] WSARecv 실패 : ", WSAGetLastError());
	}
}

void IocpBase::WorkerThread()
{
	
}