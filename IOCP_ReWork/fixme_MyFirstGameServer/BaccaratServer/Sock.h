// Sock.h: interface for the CSock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SOCK_H_
#define _SOCK_H_

#include "define.h"




// PacketSend에서 링 버퍼의 End Position의 위치를 바꾸어 준다.
void MoveSendRingBufEndPosition( LPSOCKETCONTEXT lpSockContext, int iPacketSize );
void PostTcpSendRest( LPSOCKETCONTEXT lpSockContext, int iTransferred );
void PostTcpSend( int iSockNum, int iSockAddr[], char* cpPacket, int iPacketSize, BOOL bClose = SOCKETCONTINUE );
void PostTcpSend( int iBegin, char* cpPacket, int iPacketSize );
void NowTcpSend( LPSOCKETCONTEXT lpSockContext, int iN, int iPacketSize );

void RecvTcpBufDequeue( LPSOCKETCONTEXT lpSockContext, char** cpPacket, int* iPacketSize );
void RecvTcpBufEnqueue( LPSOCKETCONTEXT lpSockContext, int iPacketSize );
void GameBufEnqueueTcp( LPSOCKETCONTEXT lpSockContext );
void PostTcpRecv( LPSOCKETCONTEXT lpSockContext );


unsigned int __stdcall AcceptTProc( LPVOID pvParam);
unsigned int __stdcall InWorkerTcpTProc( LPVOID pvParam );

BOOL UserKickOrAgency( LPSOCKETCONTEXT lpSockContext );
void EnqueueClose( LPSOCKETCONTEXT lpSockContext );

int InitSocketIO(void);
int InitWinSock();
int ReInitSocketContext( LPSOCKETCONTEXT lpSockContext );
void InitUserInfo( LPSOCKETCONTEXT lpSockContext );
int InitSocketContext( int maxUser );
int CreateServerIOCP();
int CreateSocketThread();


#endif // !defined(AFX_SOCK_H__E905002B_5CB2_4759_AC85_A6E1B3761DCA__INCLUDED_)
