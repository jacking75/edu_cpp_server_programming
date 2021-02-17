#pragma once
#include <thread>
#include <winsock2.h>

class CMessageContext;
class CSession;


///////////////////////////////////////////////////////////
// CReceiver 클래스
///////////////////////////////////////////////////////////
class CReceiver
{
private:
	CSession* m_pSession;         ///< connector의 세션 객체 포인터
	CMessageContext* m_pContext;  ///< recv 메시지를 처리할 context
	volatile bool m_bRunning;	  ///< recv 쓰레드의 동작 유무
	std::thread m_hThread;        ///< 쓰레드 객체 핸들


public:
	/// recv 쓰레드 시작
	bool Start(CSession* session_);

	/// recv 메시지를 처리할 context 등록
	void SetWorkHandler(CMessageContext* const context_);

	/// 관련 메시지를 처리하는 context를 가지고 온다
	CMessageContext const* GetWorkHandler();

	/// 해당 객체를 종료한다
	void Close();

	/// 쓰레드로 등록되는 함수
	unsigned CALLBACK RecvProc();

public:
	CReceiver();
	~CReceiver();
};
