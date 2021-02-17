
#pragma once

#ifndef __SESSIONDATA_H__
#define __SESSIONDATA_H__

#include "Common.h"

namespace COMMONLIB
{
	typedef struct tagPerIoContext
	{
		WSAOVERLAPPED	overlapped;
		char			Buffer[MAX_BUFFER];
		WSABUF			wsaBuf;
	}	PerIoContext, *PPerIoContext;

	typedef struct tagPerSocketContext
	{
		SOCKET			clntSocket;
		SOCKADDR_IN		clntAddrInfo;
		PPerIoContext	recvContext;
		PPerIoContext	sendContext;
	}	PerSocketContext, *PPerSocketContext;

	class LIB_SESSIONDATA
	{
	public :
		LIB_SESSIONDATA(WORD wSessionID);
		~LIB_SESSIONDATA();
	private :
		WORD				m_wSessionID;
		BOOL				m_bIsReference;
	public :
		PerSocketContext	m_SocketCtx;
		BYTE				wCryptKey;
	public :
		const	WORD	GetSessionID();
		const	BOOL	GetReference();
		const	VOID	SetReference();
		const	VOID	SetClientAddr(SOCKADDR_IN* pClntAddr);
		const	VOID	ClearSession();
		const	VOID	ClearSocketContext();
		const	VOID	CreateCryptKey();
	};

	typedef std::map<DWORD, LIB_SESSIONDATA*> PLAYERMAP;
}

#endif	// __SESSIONDATA_H__