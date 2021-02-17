
#include "ServiceManager.h"
#include "../../Common/Protocol.h"

using namespace NETWORKLIB;

const BOOL ServiceManager::RecvCS_AUTH_LOGIN_ACK(LIB_SESSIONDATA* pSession)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	TCHAR	szTime[MAX_STRING] = _T("");
	wsprintf(szTime, "%d/%d/%ds/%dms", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	pSession->PostSend(szTime, strlen(szTime));

	return 1;
}