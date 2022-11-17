#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>


namespace NetLib
{
	struct NetConfig
	{
		UINT16 m_PortNumber = 0;
		INT32 m_WorkThreadCount = INVALID_VALUE;
		INT32 m_MaxRecvOverlappedBufferSize = INVALID_VALUE;
		INT32 m_MaxSendOverlappedBufferSize = INVALID_VALUE;
		INT32 m_MaxRecvConnectionBufferCount = INVALID_VALUE;
		INT32 m_MaxSendConnectionBufferCount = INVALID_VALUE;
		INT32 m_MaxPacketSize = INVALID_VALUE;
		INT32 m_MaxConnectionCount = INVALID_VALUE;
		INT32 m_MaxMessagePoolCount = INVALID_VALUE;
		INT32 m_ExtraMessagePoolCount = INVALID_VALUE;
		INT32 m_PerformancePacketMillisecondsTime = INVALID_VALUE;
		INT32 m_PostMessagesThreadsCount = INVALID_VALUE;
	};

	struct ConnectionNetConfig
	{
		INT32 MaxRecvConnectionBufferCount;
		INT32 MaxSendConnectionBufferCount;
		INT32 MaxRecvOverlappedBufferSize;
		INT32 MaxSendOverlappedBufferSize;
	};



	enum E_OPERATION_TYPE : INT8
	{
		OP_NONE = 0,

		OP_SEND,
		OP_RECV,
		OP_ACCEPT,

		OP_CONNECTION,
		OP_CLOSE,
		OP_RECV_PACKET,

		OP_END
	};

	struct OVERLAPPED_EX
	{
		OVERLAPPED Overlapped;
		WSABUF OverlappedExWsaBuf;

		E_OPERATION_TYPE OverlappedExOperationType;

		int	OverlappedExTotalByte;
		DWORD OverlappedExRemainByte;
		char* pOverlappedExSocketMessage;

		INT32 ConnectionIndex = 0;
		//TODO ConnectionUnique 추가하자

		OVERLAPPED_EX(INT32 connectionIndex)
		{
			ZeroMemory(this, sizeof(OVERLAPPED_EX));
			ConnectionIndex = connectionIndex;
		}
	};


	struct Message
	{
		E_OPERATION_TYPE OperationType = OP_NONE;
		char* pContents = nullptr;

		void Clear(void)
		{
			OperationType = OP_NONE;
			pContents = nullptr;
		}
		void SetMessage(E_OPERATION_TYPE SetOperationType, char* pSetContents)
		{
			OperationType = SetOperationType;
			pContents = pSetContents;
		}
	};

	const int MAX_IP_LENGTH = 20;
	const int MAX_ADDR_LENGTH = 64;
}