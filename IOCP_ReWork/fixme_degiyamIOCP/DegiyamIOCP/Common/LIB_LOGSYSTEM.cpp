
#include "LIB_LOGSYSTEM.h"

using namespace COMMONLIB;

LIB_LOGSYSTEM::LIB_LOGSYSTEM()
{
	m_bLogSystem = 0;
	m_pFile	= NULL;
	m_hFile = INVALID_HANDLE_VALUE;
	ZeroMemory(&m_szEventLogName, MAX_STRING);
}

LIB_LOGSYSTEM::~LIB_LOGSYSTEM()
{	// 멤버 변수 Release는 이미 ReleaseLog 에서 해주고 있다
}

const BOOL LIB_LOGSYSTEM::InitialLogWrite(const BYTE bLogSystem, const LPCTSTR szFileName, const LPCTSTR szEventLogName)
{
	BOOL	bReturn = FALSE;
	TCHAR	szCreateFile[MAX_STRING] = _T("");

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if ( SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY ) != TRUE )
		return bReturn;

	CreateDirectory(STR_FOLDER_ERR, NULL);
	wsprintf(szCreateFile, "%s\\%s", STR_FOLDER_ERR, szFileName);

	m_Lock.Lock();
	{
		if ( _tcslen(szEventLogName) <= 1 )
			return bReturn;
		else
			_tcscpy(m_szEventLogName, szEventLogName);

		m_bLogSystem = bLogSystem;

		switch ( m_bLogSystem )
		{
		case LOGSYSTEM_FILE		:	bReturn = InitialLogFile(szCreateFile);		break;
		case LOGSYSTEM_HANDLE	:	bReturn = InitialLogHandle(szCreateFile);	break;
		default					:	break;
		}
	}
	m_Lock.UnLock();

	return bReturn;
}

const BOOL LIB_LOGSYSTEM::InitialLogFile(const LPCTSTR szFileName)
{
	m_Lock.Lock();
	{
		m_pFile = fopen(szFileName, "a");

		if ( m_pFile == NULL )
		{
			SAFE_CLOSE_FILE(m_pFile);
			return FALSE;
		}
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::InitialLogHandle(const LPCTSTR szFileName)
{
	m_Lock.Lock();
	{
		m_hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
		SetFilePointer(m_hFile, 0, NULL, FILE_END);

		if ( m_hFile == INVALID_HANDLE_VALUE )
		{
			SAFE_CLOSE_HANDLE(m_hFile);
			return FALSE;
		}
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::ReleaseLogFile()
{
	m_Lock.Lock();
	{
		if ( m_pFile != NULL )
		{
			SAFE_DELETE_POINT(m_pFile);
		}
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::ReleaseLogHandle()
{
	m_Lock.Lock();
	{
		if ( m_hFile != INVALID_HANDLE_VALUE )
		{
			SAFE_CLOSE_HANDLE(m_hFile);
		}
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::ReleaseLogWrite()
{
	BOOL bReturn = 0;

	switch ( m_bLogSystem )
	{
	case LOGSYSTEM_FILE		:	bReturn = ReleaseLogFile();		break;
	case LOGSYSTEM_HANDLE	:	bReturn = ReleaseLogHandle();	break;
	default					:	break;
	}

	return bReturn;
}

const BOOL LIB_LOGSYSTEM::WriteLogFile(const LPCTSTR szWrite)
{
	m_Lock.Lock();
	{
		fwrite(szWrite, sizeof(TCHAR), strlen(szWrite), m_pFile);
		fflush(m_pFile);	// fclose 하기전에 버퍼에 있는 내용을 write 한다. 하지만 이 코드를 수행하는 스레드는 잠시 멈춘다 ㅡㅡ^ 성능 떨어짐.
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::WriteLogHandle(const LPCTSTR szWrite)
{
	DWORD dwWritten = 0;

	m_Lock.Lock();
	{
		if ( WriteFile(m_hFile, szWrite, (DWORD)strlen(szWrite), &dwWritten, NULL) != TRUE )
			return FALSE;
	}
	m_Lock.UnLock();

	return TRUE;
}

const BOOL LIB_LOGSYSTEM::WriteLog(const LPCTSTR szWrite)
{
	BOOL bReturn = FALSE;

	TCHAR	szFileWrite[MAX_STRING] = _T("");

	m_Lock.Lock();
	{
		switch ( m_bLogSystem )
		{
		case LOGSYSTEM_FILE		:	wsprintf(szFileWrite, "%s\n", szWrite);
									bReturn = WriteLogFile(szFileWrite);	break;
		case LOGSYSTEM_HANDLE	:	wsprintf(szFileWrite, "%s\r\n", szWrite);
									bReturn = WriteLogHandle(szFileWrite);	break;
		}
	}
	m_Lock.UnLock();

	return bReturn;
}

const VOID LIB_LOGSYSTEM::EventLog(int nLoop, LPCTSTR pFormat, ...)
{
	TCHAR szDate[MAX_PATH]	= _T("");
	TCHAR szError[MAX_PATH] = _T("");

	LPTSTR  lpszStrings[1];
	lpszStrings[0] = szError;

	SYSTEMTIME st;
	::GetLocalTime(&st);
	wsprintf(szDate, "[%0.2d.%0.2d %0.2d:%0.2d:%0.2d] ", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	va_list pArg;
	va_start(pArg, pFormat);
	_vstprintf(szError, pFormat, pArg);
	strcat(szDate, szError);

	if ( nLoop >= 2 )
	{
		for ( int iter = 0; iter < nLoop - 1; iter++ )
		{
			LPCTSTR lpText = va_arg(pArg, LPCTSTR);
			strcat(szDate, " / ");
			strcat(szDate, lpText);
		}
	}

	va_end(pArg);

#ifdef _DEBUG			// 디버그 모드 : 콘솔 화면에 출력, 파일 로그 작성
	puts(szDate);
	WriteLog(szDate);
#else if _NDEBUG		// 릴리즈 모드 : 이벤트 로그 작성, 파일 로그 작성
	HANDLE hEventSource = RegisterEventSource(NULL, m_szEventLogName);
	if ( hEventSource )
	{
		ReportEvent(hEventSource, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*)&lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
	WriteLog(szDate);
#endif	// _DEBUG & _NDEBUG
}