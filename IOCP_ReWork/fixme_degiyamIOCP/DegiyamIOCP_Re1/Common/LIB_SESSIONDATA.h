
#pragma once

#ifndef __SESSIONDATA_H__
#define __SESSIONDATA_H__

#include "Common.h"
#include "CircularQueue.h"
#include "LIB_LOCK.h"

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
		LIB_SESSIONDATA(int wSessionID);
		~LIB_SESSIONDATA();
	private :
		int				m_wSessionID;
		BOOL				m_bIsReference;

		LIB_CRITICALLOCK	m_SendLock;
		bool				m_Sending = false;
		CircularQueue		m_SendQueue;

	public :
		PerSocketContext	m_SocketCtx;
		BYTE				wCryptKey;
	public :
		const	int	GetSessionID();
		const	BOOL	GetReference();
		const	VOID	SetReference();
		const	VOID	SetClientAddr(SOCKADDR_IN* pClntAddr);
		const	VOID	ClearSession();
		const	VOID	ClearSocketContext();
		const	VOID	CreateCryptKey();

		const bool PostSend(const char* pData, const int dataSize);
		const bool FlushSend();
		const bool CompleteSend();
	};

	typedef std::map<DWORD, LIB_SESSIONDATA*> PLAYERMAP;

}

#endif	// __SESSIONDATA_H__