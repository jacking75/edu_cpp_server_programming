
#include "ServiceManager.h"
#include "../../Common/Protocol.h"

using namespace NETWORKLIB;

const BOOL ServiceManager::RecvCS_AUTH_LOGIN_ACK(LIB_SESSIONDATA* pSession)
{
	ZeroMemory(pSession->m_SocketCtx.sendContext, MAX_BUFFER);

	SYSTEMTIME st;
	GetSystemTime(&st);

	TCHAR	szTime[MAX_STRING] = _T("");

	wsprintf(szTime, "%d/%d/%ds/%dms", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	memcpy(&pSession->m_SocketCtx.sendContext->Buffer, szTime, strlen(szTime));

	m_Lock.Lock();
	{
		m_SendCtx = pSession;
		SetEvent(m_hSendEvent);
	}
	m_Lock.UnLock();

	return 1;
}