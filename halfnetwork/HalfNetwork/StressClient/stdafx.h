// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
//#include <tchar.h>

#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "HalfNetworkDefine.h"
#include "Logger.h"

typedef std::basic_string<ACE_TCHAR> tstring;

typedef ACE_Singleton<HalfNetwork::Logger, ACE_Thread_Mutex> HalfLoggerSingleton;
#define ConsoleLogger HalfLoggerSingleton::instance()