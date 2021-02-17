// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_ 
#endif

#define __TBB_malloc_proxy
#ifdef __TBB_malloc_proxy
#ifdef _DEBUG
#pragma comment (lib, "tbbmalloc_proxy_debug.lib") 
#else
#pragma comment (lib, "tbbmalloc_proxy.lib") 
#endif
#pragma comment (linker, "/include:__TBB_malloc_proxy") 
#include "tbb/tbbmalloc_proxy.h"

#else
#ifdef _DEBUG
#pragma comment (lib, "libtcmalloc_minimal-debug.lib") 
#else
#pragma comment (lib, "libtcmalloc_minimal.lib") 
#endif
#pragma comment (linker, "/include:__tcmalloc") 
#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
