
#include "IOCPModule.h"

CIOCPModule::CIOCPModule()
{
	m_CompletionPort = nullptr;
}

CIOCPModule::~CIOCPModule()
{
	CloseHandle(m_CompletionPort);
	m_CompletionPort = INVALID_HANDLE_VALUE;
}

// iocp 생성
const bool CIOCPModule::StartIOCP()
{
	m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);		// 마지막 인자에 0을 넣으면 자동으로 cpu개수와 같은 스레드 실행

	if (m_CompletionPort == nullptr) {
		std::cout << "[CreateIoCompletionPort() Error!] in StartIOCP\n";
		return false;
	}

	return true;
}

// accept한 클라이언트를 iocp에 연결
const bool CIOCPModule::RegisterClient(SOCKET clientSocket, CSessionData* pplayer)
{
	// pplayer를 넘겨준다. 아이디를 넘겨주는게 나을까 생각해보자
	 HANDLE hresult = CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_CompletionPort, reinterpret_cast<ULONG_PTR>(pplayer), 0);

	if (hresult == INVALID_HANDLE_VALUE) {
		std::cout << "[CreateIoCompletionPort() Error!] in RegisterClient\n";
		return false;
	}

	return true;
}