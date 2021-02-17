
#include "LIB_PROFILE.h"

using namespace COMMONLIB;

LIB_PROFILE::LIB_PROFILE()
{
	ZeroMemory(m_FilePath, MAX_STRING);
}

LIB_PROFILE::~LIB_PROFILE()
{
}

const VOID LIB_PROFILE::SetFilePath(LPCTSTR strPath)
{
	strcpy(m_FilePath, strPath);
}

const VOID LIB_PROFILE::WriteToINI(LPCTSTR strSection, LPCTSTR strKey, int nValue)
{
	TCHAR	strValue[MAX_STRING] = "";
	wsprintf(strValue, "%d", nValue);
	WritePrivateProfileString(strSection, strKey, strValue, m_FilePath);
}

const VOID LIB_PROFILE::WriteToINI(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue)
{
	WritePrivateProfileString(strSection, strKey, strValue, m_FilePath);
}

const VOID LIB_PROFILE::GetStringFromINI(LPCTSTR strSection, LPCTSTR strKey, LPSTR strResult)
{
	GetPrivateProfileString(strSection, strKey, "", strResult, MAX_STRING, m_FilePath);
}

const DWORD LIB_PROFILE::GetDWORDFromINI(LPCTSTR strSection, LPCTSTR strKey)
{
	return GetPrivateProfileInt(strSection, strKey, 0, m_FilePath);
}

const BOOL LIB_PROFILE::DeleteINI()
{
	return ::DeleteFile(m_FilePath);
}