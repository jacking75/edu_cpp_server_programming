// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include "targetver.h"

#ifdef UNICODE 
#define tstring std::wstring
#define stringWriter fmt::WMemoryWriter
#else
#define tstring std::string
#define stringWriter fmt::MemoryWriter
#endif

#include <stdio.h>
#include <tchar.h>

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_  //< winsock2.h 와 msado15.tlh(ws2def.h) 의 재정의 방지
#endif

//#define _USE_ADO_DB_MYSQL 1
//#define _USE_ADO_DB_MSSQL 1

// TODO: reference additional headers your program requires here
