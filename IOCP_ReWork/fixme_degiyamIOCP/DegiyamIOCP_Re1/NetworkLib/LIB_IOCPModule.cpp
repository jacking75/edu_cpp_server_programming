
#include "LIB_IOCPModule.h"

using namespace NETWORKLIB;

LIB_IOCPModule::LIB_IOCPModule()
{
	m_CompletionPort = NULL;
}

LIB_IOCPModule::~LIB_IOCPModule()
{
	SAFE_CLOSE_HANDLE(m_CompletionPort);
}

const BOOL LIB_IOCPModule::START_IOCP()
{
	m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if ( m_CompletionPort == NULL )
		return FALSE;

	return TRUE;
}

const BOOL LIB_IOCPModule::REGISTER_CLIENT(SOCKET ClientSocket, COMMONLIB::LIB_SESSIONDATA* Player)
{
	HANDLE hResult = INVALID_HANDLE_VALUE;
	hResult = CreateIoCompletionPort((HANDLE)ClientSocket, m_CompletionPort, (ULONG_PTR)Player, 0);

	if ( hResult == INVALID_HANDLE_VALUE )
		return FALSE;

	return TRUE;
}

const HANDLE LIB_IOCPModule::GetCompletionPort()
{
	return m_CompletionPort;
}