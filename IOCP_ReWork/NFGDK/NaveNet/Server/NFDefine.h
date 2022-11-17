/** 
 *  @file  		NFDefine.h
 *  @brief 		Packet에 관련된 기본 Define 및 스트럭쳐 선언
 *  @remarks 
 *  @author  	강동명(edith2580@gmail.com)
 *  @date  		2009-05-09
 */
#pragma once

namespace NaveNetLib {

	// 패킷 처리를 UpdateQue를 이용해 처리할것인지에 대한 Define
	// 이걸 활성화 시키면 패킷을 처리할때 Receive스레드가 아닌 
	// Process스레드에서 패킷을 업데이트 한다.

	// UPDATEQUE 방식은 Work스레드에서 Packet이 완성되면 PacketQue를 이용해 
	// 패킷을 처리한다. (MMO시에 필요)
	#define ENABLE_UPDATEQUE


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [1] User Define
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define		DEF_MAXUSER				50	 						// Maximum connections for Test

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [2] Server Declarations
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	#define		IOCP_SHUTDOWN			((DWORD) -1L)				// Closing Server Message
	#define		DEF_SERVER_PORT			25000						// PORT Number ( Client와 동일 해야함 )

	enum ESHOW_MSGTYPE
	{
		ACCEPT_FAILED,
		CLOSE_SOCKET,
		DEAD_CONNECTION,
		CONNECT_SUCCESS,
		CONNECT_FAILED,
		DISPATCH_FAILED,
		DOIOSWITCH_FAILED,
		// accept
		ALLOCACPT_FAILED,
		PREPAREACPT_FAILED,
		
		// send, recv
		PREPARERECVSIZE_FAILED,
		PREPARESENDSIZE_FAILED,
		
		ALLOCRECV_FAILED,
		ALLOCSEND_FAILED,
		
		PREPARERECV_FAILED,
		PREPARESEND_FAILED,

		RELEASEACPT_FAILED,
		RELEASERECV_FAILED,
		RELEASESEND_FAILED,

		BINDIOCP_FAILED,

		RECVPOST_FAILED,
		RECVPOSTPENDING_FAILED,

//		ALLOCPROC_FAILED,
		SENDPOST_FAILED,
		SENDPOSTPENDING_FAILED,
	};

	enum CONNECT_EVENT
	{
		CONNECT_NONE,
		CONNECT_true,
		CONNECT_false,
		DISCONNECT_TURE,
		CLOSEOPEN_true,
	};

	/// 열거형 정의 : 클라이언트 작동 상태를 정의 한다 
	typedef enum CONN_STATUS 
	{
		ClientIoUnknown,											/// Raw status
		ClientIoAccept,												/// accept status 
		ClientIoRead,												/// read status  
		ClientIoWrite,												/// write status
	}*pCONN_STATUS;

	/// 확장 오버랩 구조체 : IOCP처리시 사용 
	typedef struct OVERLAPPEDPLUS {
		OVERLAPPED		overlapped;									/// OVERLAPPED struct
		SOCKET			sckListen;									/// listen socket handle
		SOCKET			sckClient;									/// send/recv socket handle
		CONN_STATUS		nConnState;									/// operation flag
		WSABUF			wbuf;										/// WSA buffer						
		DWORD			dwRemain;									/// 
		DWORD			dwBytes;									/// Processing Data Size
		DWORD			dwFlags;									/// 
		void*			pClientConn;								/// Processing Client 
	}*LPOVERLAPPEDPLUS;

}