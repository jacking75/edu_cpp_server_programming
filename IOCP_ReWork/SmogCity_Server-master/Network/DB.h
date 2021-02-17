#pragma once

#ifndef __DB_H__
#define __DB_H__

#include "../Common/Common.h"
#include "../Common/SessionData.h"
#include <sqlext.h> 

class CDB
{
public:
	CDB();
	~CDB() {};
private:
	SQLHENV m_hEnv;
	SQLHDBC m_hDbc;
	SQLHSTMT m_hStmt;

	bool m_bIsInitalize;
	bool m_bIsConnected;
public:
	bool InitalizeDB();
	bool ConnectDB(SQLWCHAR* dbName, SQLWCHAR* id, SQLWCHAR* pw);
	void ClearDB();
	bool DisconnectDB();
	bool ExecDirect(SQLWCHAR* wsqlStr);
	bool LoginToDB(TCHAR* pwname, TCHAR* pwpw, CSessionData* pclient);
	bool SignupToDB(TCHAR* pwname, TCHAR* pwpw);
	bool SaveCharacterInfoToDB(CSessionData* pclient);
};

static void display_error_DB(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];
	if (RetCode == SQL_INVALID_HANDLE)
	{
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage, (SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{ // Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5))
		{
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

#endif // !__DB_H__
