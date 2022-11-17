/** 
 *  @file  		NaveServer.h
 *  @brief 		공용 함수 (로그메시지 및 버퍼 관리함수)
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.

// Windows 헤더 파일입니다.
#include <windows.h>
#include <time.h>												// timer 
#include <assert.h>												// assert
#include <process.h>											// Thread 
#include <stdio.h>												// standard I/O
#include <stdlib.h>
//#include <wchar_t.h>

// sock
#include<winsock2.h>											// win32 socket 
#pragma comment(lib,"ws2_32.lib")								

#include<Mswsock.h>												// extension socket library 
#pragma comment(lib,"mswsock.lib")

#include <Ws2tcpip.h>

//TODO: 제거하자  //////////
// 멀티미디어 타이머를 쓰기
#include "mmsystem.h"											
#pragma comment(lib,"winmm.lib")
///////////////////////////

#include <string>

#include "NFPacket.h"
#include "NFDefine.h"

/**
 * @brief	로컬 아이피를 가져옵니다.
 * @param LocalIP	로컬아이피 저장변수.
 * @param bClosedIP 내부아이피를 얻어올때
 * @return 성공여부
 */
bool GetLocalIP(std::string& localIP_Out, bool bClosedIP = false);
