/** 
 *  @file		NaveNet.h
 *  @brief		Network Base 클래스
 *  @remarks	
 *  @author		강동명(edith2580@gmail.com)
 *  @date		2009-04-02
 */

/*
 2017. 05.25 최흥배 수정
*/

#pragma once

#ifdef _WIN32
/// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define WIN32_LEAN_AND_MEAN		

// Windows 헤더 파일입니다.
#include <windows.h>
#include <time.h>												// timer 
#include <assert.h>												// assert
#include <process.h>											// Thread 
#include <stdio.h>												// standard I/O
#include <stdlib.h>

// sock
#include <winsock2.h>											// win32 socket 
#pragma comment(lib,"ws2_32.lib")								

#include <Mswsock.h>											// extension socket library 
#pragma comment(lib,"mswsock.lib")
#endif

//TODO: 제거해야 한다 ////
// 멀티미디어 타이머를 쓰기
#include "mmsystem.h"	
#pragma comment(lib,"winmm.lib")
///////////////////////

#include <mutex>

#include "NFPacket.h"
//#include <Nave/NFSync.h>

namespace NaveClientNetLib {

	const int MAX_PACKET	=	512;					/// 최대 Packet 생성 개수

	#define WM_CONNECT				WM_APP + 0x1001		/// CONNECT 메시지
	#define WM_RECV_MSG				WM_APP + 0x1002		/// RECV 메시지
	#define WM_SOCK_CLOSE			WM_APP + 0x1003		/// CLOSE 메시지

	/** 
	*  @class        NFPacketQueue
	*  @brief        Packet Queue 처리용 클래스
	*  @remarks      
	*                
	*  @par          
	*  @author  Edith
	*  @date    2009-04-04
	*/
	class NFPacketQueue
	{
		/// Queue 에서 사용할 INDEX 구조체
		struct INDEX
		{
			/// 다음 인덱스 위치
			INDEX*		pNext;
			/// 패킷 
			NFPacket	Packet;

			INDEX()
			{
				pNext = NULL;
				Packet.Init();
			}
		};

	public:
		/// NFPacketQueue 생성자
		NFPacketQueue()
		{
			pList = NULL;
			pHead = NULL;
			pTail = NULL;
			nQueueCnt = 0;
			nMaxQueueCnt = 0;
		}

		/// NFPacketQueue 소멸자
		~NFPacketQueue()
		{
			Release();
		}

		/// Queue를 초기화 합니다.
		VOID Release()
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if(pList)
			{
				delete [] pList;
				pList = NULL;
			}
		}

		/// 패킷 큐를 생성합니다.
		BOOL Create(int nMAXCnt = MAX_PACKET)
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if (nMAXCnt <= 0) {
				return FALSE;
			}

			if(pList)
			{
				delete [] pList;
				pList = NULL;
			}

			nMaxQueueCnt = nMAXCnt;

			if((pList = new INDEX[nMaxQueueCnt]) == NULL)
			{
				nMaxQueueCnt = 0;
				return FALSE;
			}

			for(int i = nMaxQueueCnt - 1; i >= 0 ; i--)
			{
				if( (i+1) == nMaxQueueCnt)
				{
					pList[i].pNext = &pList[0];
					continue;
				}
				pList[i].pNext = &pList[i+1];		
			}
			pHead = pTail = &pList[0];
			return TRUE;
		}

		/// 큐의 개수를 얻어옵니다.
		int GetQueCnt()
		{
			std::lock_guard<std::mutex> Sync(m_Lock);
			return nQueueCnt;
		}

		/// 큐를 리셋합니다.
		void Reset()
		{
			std::lock_guard<std::mutex> Sync(m_Lock);
			pHead = pTail = &pList[0];
			nQueueCnt = 0;
		}

		/// 패킷을 추가합니다.
		bool Add(NFPacket& Packet)
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if (!pList) {
				return false;
			}

			if(nQueueCnt == nMaxQueueCnt)
			{
				nQueueCnt = 0;
				pHead = pTail = &pList[0];
				return false;
			}
			//if(szData[0] == NULL) return FALSE;
			if (Packet.m_Header.Size <= 0) {
				return false;
			}

			if (Packet.m_Header.Size >= DEF_MAXPACKETSIZE) {
				return false;
			}

			// Head의 Size는 Header 사이즈 포함한 사이즈다.
			int PacketSize = Packet.m_Header.Size-sizeof(Packet.m_Header);

			// 맞춰서 복사.. 
			CopyMemory(&pTail->Packet.m_Header, &Packet.m_Header, sizeof(Packet.m_Header));
			CopyMemory(pTail->Packet.m_Packet,Packet.m_Packet,PacketSize);
			pTail->Packet.m_Packet[PacketSize] = 0;


			pTail = pTail->pNext;

			InterlockedIncrement((LONG*)&nQueueCnt);

			return true;
		}

		/**
		* @brief	패킷 정보를 얻어옵니다.
		* @param Packet 패킷 포인터
		* @return	패킷의 사이즈
		*/
		int GetPnt(NFPacket** Packet)
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if (!pList) {
				return -1;
			}

			if (nQueueCnt == 0) {
				return -1;
			}

			int nLen = -1;
			*Packet = &pHead->Packet;
			nLen = pHead->Packet.m_Header.Size;

			return nLen;
		}

		/// 첫번재 패킷을 삭제합니다.
		void Del()
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if (!pList) {
				return;
			}

			if (nQueueCnt == 0) {
				return;
			}

			int nLen = -1;

			pHead->Packet.Init();
			pHead = pHead->pNext;

			InterlockedDecrement((LONG*)&nQueueCnt);
		}

		/**
		* @brief	패킷 정보를 얻어옵니다.
		* @param Packet 패킷 포인터
		* @return	패킷 사이즈
		*/
		int Get(NFPacket& Packet)
		{
			std::lock_guard<std::mutex> Sync(m_Lock);

			if (!pList) {
				return -1;
			}

			if (nQueueCnt == 0) {
				return -1;
			}

			int nLen = -1;
			int PacketSize = pHead->Packet.m_Header.Size-sizeof(pHead->Packet.m_Header);

			CopyMemory(&Packet.m_Header, &pHead->Packet.m_Header, sizeof(Packet.m_Header));
			CopyMemory(Packet.m_Packet, pHead->Packet.m_Packet, PacketSize);
			Packet.m_Packet[PacketSize] = 0;

			nLen = Packet.m_Header.Size;

			pHead->Packet.Init();
			pHead = pHead->pNext;

			InterlockedDecrement((LONG*)&nQueueCnt);

			return nLen;
		}

	private:
		/// 패킷의 리스트
		INDEX*			pList;
		/// 패킷 리스트의 시작
		INDEX*			pHead;
		/// 패킷 리스트의 끝
		INDEX*			pTail;

		/// 현재 사용되는 큐의 개수
		int				nQueueCnt;
		/// 최대 큐의 개수
		int				nMaxQueueCnt;
		/// 동기화 변수
		//Nave::NFSync	nfSync;
		std::mutex		m_Lock;
	};

	/// Socket의 Port를얻어온다.
	int SockRemotePort( SOCKET Sock );

	/// Socket의 IP를얻어온다.
	int SockRemoteIP( SOCKET Sock, unsigned char ip[4] );
	
	/// Domain이름으로 ip를 얻어옵니다.
	bool GetHostIPByDomain(IN_ADDR &Addr, const char *szDomain);

}