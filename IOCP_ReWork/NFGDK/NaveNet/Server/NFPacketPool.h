/** 
 *  @file  		NFPacketPool.h
 *  @brief 		Packet Pool 관리객체, 이곳에서 Buffer를 할당하거나 해제한다.
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

#include "NaveServer.h"
#include "NFMemPool.h"

namespace NaveNetLib {

	//////////////////////////////////////////////////////////////////////////////////////////////
	// NFPacketPool Class
	//////////////////////////////////////////////////////////////////////////////////////////////
	class NFPacketPool  
	{
	//---------------------- Member Functions ----------------------//
	public:
		NFPacketPool();												// Contructor 
		virtual ~NFPacketPool();										// Destructor 

		bool Create(int nMaxUse			= DEF_MAXUSER,				// 최대 유저
					char* lpszFileName	= NULL,						// 로그 파일 이름
					int nMaxPacketSize	= DEF_MAXPACKETSIZE,
					int nAcptBuffCnt    = 1,
					int nRecvBuffCnt    = 8,
					int nSendBuffCnt    = 16);						// Create Packet Pool

		void Release();

		// Operations
		bool AllocAcptPacket(LPOVERLAPPEDPLUS &newolp);				// Accept 패킷 생성 
		bool AllocRecvPacket(LPOVERLAPPEDPLUS &newolp);				// Receive 패킷 생성 
		bool AllocSendPacket(LPOVERLAPPEDPLUS &newolp);				// Send Buffer 생성 
		bool AllocProcBuffer(char* &newbuf);						// Send Buffer 생성

		bool FreeAcptPacket(LPOVERLAPPEDPLUS olp);					// Accept 패킷 해제 
		bool FreeRecvPacket(LPOVERLAPPEDPLUS olp);					// Receive 패킷 해제 
		bool FreeSendPacket(LPOVERLAPPEDPLUS olp);					// Send Buffer 해제
		bool FreeProcBuffer(char* buffer);							// Send Buffer 해제

	private:
		bool				LogOpen(char *lpszFileName);			// 로그 파일 열기 
		bool				LogWrite(char *lpszFmt, ...);			// 로그 파일 만들기 
		void				LogClose();								// 로그 파일 닫기 

	//---------------------- Member Variables ----------------------//
	private:
		FILE*				m_hLogFile;								// 로그 파일을 만들기 위한 파일 핸들 객체 

		//TODO: 보통의 객체 풀로 대체한다.
		NFMemPool			<OVERLAPPEDPLUS>* m_pAcptPool;			// Accept Packet
		NFMemPool			<OVERLAPPEDPLUS>* m_pRecvPool;			// Receive Packet
		NFMemPool			<OVERLAPPEDPLUS>* m_pProcPool;			// Process Packet

		int					m_nMaxPacketSize;						// 패킷 사이즈 정한것
	};

}