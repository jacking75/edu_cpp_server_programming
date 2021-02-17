
#pragma once

#ifndef __LIB_PROFILE_H__
#define __LIB_PROFILE_H__

#include "Common.h"

namespace COMMONLIB
{
	/*
	 *	Make a person	: 이재득
	 *	Make a date		: 2006.10.07
	 *	Class name		: LIB_PROFILE
	 *	Class describe	: INI 파일로부터 텍스트를 READ 하거나 WRITE 를 가능하게하는 클래스
	*/

	class LIB_PROFILE
	{
	public :
		LIB_PROFILE();
		~LIB_PROFILE();
	private :
		TCHAR	m_FilePath[MAX_STRING];
	public :
		const VOID	SetFilePath(LPCTSTR strPath);
		const VOID	WriteToINI(LPCTSTR strSection, LPCTSTR strKey, int nValue);
		const VOID	WriteToINI(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue);
		const VOID	GetStringFromINI(LPCTSTR strSection, LPCTSTR strKey, LPSTR strResult);
		const DWORD	GetDWORDFromINI(LPCTSTR strSection, LPCTSTR strKey);
		const BOOL	DeleteINI();
	};
}

#endif	// __LIB_PROFILE_H__