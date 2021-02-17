#include "DB.h"

using std::cout;

CDB::CDB()
	: m_bIsInitalize(false)
{
	setlocale(LC_ALL, "korean");
}

bool CDB::InitalizeDB()
{
	SQLRETURN retcode;

	// Allocate environment handle  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(m_hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
			}
			else
				return false;
		}
		else
			return false;
	}
	else 
		return false;

	//cout << "Database Initalize OK!\n";
	m_bIsInitalize = true;
	return true;
}

bool CDB::ConnectDB(SQLWCHAR* dbName, SQLWCHAR* id, SQLWCHAR* pw)
{
	if (m_bIsInitalize == false)
		return false;

	SQLRETURN retcode;

	// Connect to data source  
	retcode = SQLConnect(m_hDbc, dbName, SQL_NTS, id, (SQLSMALLINT)wcslen(id), pw, (SQLSMALLINT)wcslen(pw));
	//retcode = SQLConnect(m_hDbc, dbName, SQL_NTS, id, 0, pw, 0);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		m_bIsConnected = true;
		//cout << "Database Connect OK\n";
		//retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);
		return true;
	}
	else
		display_error_DB(m_hDbc, SQL_HANDLE_DBC, retcode);
	
	return false;
}

void CDB::ClearDB()
{
	DisconnectDB();

	SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);

	SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
}

bool CDB::DisconnectDB()
{
	if (m_bIsConnected == false)
		return false;

	//SQLCancel(m_hStmt);
	//SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);

	SQLDisconnect(m_hDbc);

	return true;
}

bool CDB::ExecDirect(SQLWCHAR* wsqlStr)
{
	if (m_bIsConnected == false)
		return false;
	
	SQLRETURN retcode;
	
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt);

	retcode = SQLExecDirect(m_hStmt, wsqlStr, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		return true;
	else
		display_error_DB(m_hStmt, SQL_HANDLE_STMT, retcode);

	return false;
}

bool CDB::LoginToDB(TCHAR* pwname, TCHAR* pwpw, CSessionData* pclient)
{
	SQLRETURN retcode;

	// name
	std::wstring str = L"EXEC login ";
	str += pwname;
	str += L", ";
	str += pwpw;

	TCHAR* execStr = (TCHAR*)str.c_str();

	if (ExecDirect((SQLWCHAR*)execStr) == true)
	{
		SQLINTEGER ugameamount{}, ugamevictory{}, ugamedefeat{};
		SQLLEN cb_ugameamount = 0, cb_ugamevictory = 0, cb_ugamedefeat = 0;
		
		retcode = SQLBindCol(m_hStmt, 1, SQL_INTEGER, &ugameamount, 10, &cb_ugameamount);
		retcode = SQLBindCol(m_hStmt, 2, SQL_INTEGER, &ugamevictory, 10, &cb_ugamevictory);
		retcode = SQLBindCol(m_hStmt, 3, SQL_INTEGER, &ugamedefeat, 10, &cb_ugamedefeat);

		retcode = SQLFetch(m_hStmt);
		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			return false;
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			//wprintf(L"level : %d x : %d y : %d hp : %d mp : %d\n", ulevel, ux, uy, uhp, ump);
			//pclient->m_nRoundAmount = uroundamount;
			pclient->m_CharacterInfo.m_nGameAmount = ugameamount;
			pclient->m_CharacterInfo.m_nGameVictory = ugamevictory;
			pclient->m_CharacterInfo.m_nGameDefeat = ugamedefeat;

			SQLCancel(m_hStmt);
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
		}
		else {
			display_error_DB(m_hStmt, SQL_HANDLE_STMT, retcode);
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}

bool CDB::SignupToDB(TCHAR* pwname, TCHAR* pwpw)
{
	SQLRETURN retcode;

	std::wstring str = L"EXEC signup ";
	str += pwname;
	str += L", ";
	str += pwpw;

	TCHAR* execStr = (TCHAR*)str.c_str();

	if (ExecDirect((SQLWCHAR*)execStr) == true)
	{
		SQLINTEGER urv{};
		SQLLEN cb_urvt = 0;

		retcode = SQLBindCol(m_hStmt, 1, SQL_INTEGER, &urv, 10, &cb_urvt);

		retcode = SQLFetch(m_hStmt);
		if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			return false;
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			SQLCancel(m_hStmt);
			SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);

			if(urv == 0)
				return false;
		}
		else {
			display_error_DB(m_hStmt, SQL_HANDLE_STMT, retcode);
			return false;
		}
		//retcode = SQLFetch(m_hStmt);
	}
	else
		return false;

	return true;
}

bool CDB::SaveCharacterInfoToDB(CSessionData* pclient)
{
	SQLRETURN retcode;

	TCHAR temp[10]{};

	// name
	std::wstring str = L"EXEC update_user ";
	str += pclient->m_cName;
	str += L", ";
	_itow_s(pclient->m_CharacterInfo.m_nGameAmount, temp, 10);
	str += temp;
	str += L", ";
	_itow_s(pclient->m_CharacterInfo.m_nGameVictory, temp, 10);
	str += temp;
	str += L", ";
	_itow_s(pclient->m_CharacterInfo.m_nGameDefeat, temp, 10);
	str += temp;

	TCHAR* execStr = (TCHAR*)str.c_str();

	if (ExecDirect((SQLWCHAR*)execStr) == true)
	{

	}
	else {
		return false;
	}

	return true;
}
