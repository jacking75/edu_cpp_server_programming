
#pragma once

#ifndef _COMMON_H_
#define _COMMON_H_

#include <WinSock2.h>
#include <Windows.h>
#include <map>
#include <assert.h>
#include <tchar.h>

/* COLOR INFO */
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define BACKGROUND_WHITE (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE)

/* SAFE RELEASE */
#define SAFE_DELETE_POINT(P)	{ if(P) { delete P; P = NULL; } }
#define SAFE_CLOSE_SOCKET(P)	{ if(P) { closesocket(P); P = INVALID_SOCKET; } }
#define SAFE_CLOSE_HANDLE(P)	{ if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }
#define SAFE_CLOSE_FILE(P)		{ if(P)	{ fclose(P); P = NULL;	} }

/* DEFINE NUMBER */
#define MAX_THREAD		100
#define MAX_BUFFER		4096
#define MAX_STRING		128
#define MIN_STRING		64
#define VALUE_THREAD	4		// RecvWork Thread ÀÇ °³¼ö

/* DEFINE STRING */
#define STR_FOLDER_ERR	"ErrorLog"

#ifdef _DEBUG
	#define SESSION_NUM	100
#else
	#define SESSION_NUM 1000
#endif	// _DEBUG

#endif