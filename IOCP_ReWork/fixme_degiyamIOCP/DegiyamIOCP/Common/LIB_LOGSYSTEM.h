
#pragma once

#ifndef __LIB_LOGSYSTEM_H__
#define __LIB_LOGSYSTEM_H__

#include "Common.h"
#include "LIB_LOCK.h"

namespace COMMONLIB
{
	enum ENUM_LOGSYSTEM
	{
		LOGSYSTEM_FILE		= 1,
		LOGSYSTEM_HANDLE	= 2,
	};

	/*
     *	Make a person	: 이재득
	 *	Make a date		: 2007.02.09
	 *	Class name		: LIB_LOGSYSTEM
	 *	Class describe	: 파일에 로그를 남긴다.
	*/

	class LIB_LOGSYSTEM
	{
	public :
		LIB_LOGSYSTEM();
		~LIB_LOGSYSTEM();
	private :
		LIB_CRITICALLOCK	m_Lock;
		FILE*			m_pFile;
		HANDLE			m_hFile;
		BYTE			m_bLogSystem;
		TCHAR			m_szEventLogName[MAX_STRING];
	private :
		const BOOL	InitialLogFile(const LPCTSTR szFileName);
		const BOOL	InitialLogHandle(const LPCTSTR szFileName);
		const BOOL	ReleaseLogFile();
		const BOOL	ReleaseLogHandle();
		const BOOL	WriteLogFile(const LPCTSTR szWrite);
		const BOOL	WriteLogHandle(const LPCTSTR szWrite);
		const BOOL	WriteLog(const LPCTSTR szWrite);
	public :
		const BOOL	InitialLogWrite(const BYTE bLogSystem, const LPCTSTR szFileName, const LPCTSTR szEventLogName);
		const BOOL	ReleaseLogWrite();
		const VOID	EventLog(int nLoop, LPCTSTR pFormat, ...);
	};
}

#endif //	__LIB_LOGSYSTEM_H__