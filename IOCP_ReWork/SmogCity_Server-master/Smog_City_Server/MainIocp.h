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
//#include "DBConnector.h"
#include "IocpBase.h"
//#include "Monster.h"

using namespace std;

// DB 정보
#define DB_ADDRESS		"localhost"
#define	DB_PORT			3306
#define DB_ID			"root"
#define DB_PW			"anfrhrl"
#define DB_SCHEMA		"sungminworld"

class MainIocp : public IocpBase
{
public:
	MainIocp();
	virtual ~MainIocp();
	
	virtual void StartServer() override;
	// 작업 스레드 생성
	virtual bool CreateWorkerThread() override;
	// 작업 스레드
	virtual void WorkerThread() override;
	// 클라이언트에게 송신
	static void Send(scSOCKETINFO * pSocket);	

private:
	static map<int, SOCKET> SessionSocket;	// 세션별 소켓 저장

	FuncProcess				fnProcess[100];	// 패킷 처리 구조체

	// 브로드캐스트 함수
	static void Broadcast(stringstream & SendStream);
};
