#include "stdafx.h"
#include "Receiver.h"
#include "Session.h"
#include "define.h"
#include "PacketHeder.h"
#include "MessageContext.h"
#ifndef __TBB_malloc_proxy
#include "tcmalloc.h"
#else
#include "tbb/tbbmalloc_proxy.h"
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


using namespace google;

CReceiver::CReceiver()
	: m_bRunning(false)
{
}

CReceiver::~CReceiver()
{
	m_bRunning = false;
}

bool CReceiver::Start(CSession* session_)
{
	m_bRunning = true;
	m_pSession = session_;
	m_hThread = std::thread(&CReceiver::RecvProc, this);
	return true;
}

void CReceiver::SetWorkHandler(CMessageContext* const context_)
{
	m_pContext = context_;
}

CMessageContext const* CReceiver::GetWorkHandler()
{
	return m_pContext;
}

void CReceiver::Close()
{
	m_bRunning = false;
}

void _RecvProc(protobuf::io::CodedInputStream& cs, CMessageContext* context, char*& remainBuf, int& remainBufLen, int recvSize)
{
	int totalTransSize = 0;
	int remainSize = 0;
	int expectedSize = 0;
	const void* rawData = NULL;

	packetdef::PacketHeader packetHeader;
	while (cs.ReadRaw(&packetHeader, packetdef::HeaderSize))
	{
		expectedSize = packetHeader.dataSize;
		int packetSize = expectedSize + packetdef::HeaderSize;
		if (totalTransSize + packetSize > recvSize)
		{
			break;
		}

		cs.GetDirectBufferPointer(&rawData, &remainSize);
		if (remainSize < expectedSize)
		{
			expectedSize = -packetdef::HeaderSize;
			break;
		}

		cs.Skip(expectedSize);

		//printf("recv size (%d)\n", translatedSize);
		totalTransSize += packetSize;

		protobuf::io::ArrayInputStream pis(rawData, expectedSize);
		context->MessageProc(packetHeader.id, pis);
	}

	
	if (totalTransSize < recvSize)
	{
		int bufSize = recvSize - totalTransSize;
		
		::memcpy_s(
			remainBuf + remainBufLen,
			bufSize,
			static_cast<const char*>(rawData) + expectedSize,
			bufSize);
		
		remainBufLen += bufSize;
	}
}

unsigned CALLBACK CReceiver::RecvProc()
{
	_WSABUF       wsa;
	OVERLAPPED  overlapped;
	const int max_packet_size = PACKET_SIZE * 2;
	char* tempBuf = new char[max_packet_size];
	char* remainBuf = new char[max_packet_size];
	int   remainBufLen = 0;
	wsa.buf = new char[max_packet_size];
	
	::memset(remainBuf, 0, max_packet_size);
	::memset(wsa.buf, 0, max_packet_size);
	::memset(&overlapped, 0, sizeof(OVERLAPPED));

	while (m_bRunning)
	{
		int flags = 0;
		int recvSize;

		if ((recvSize = ::recv(m_pSession->GetSocket(), wsa.buf, PACKET_SIZE, 0)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING)
			{
				printf("recv error (%d)\n", err);
				return 0;
			}
		}

		char *workbuf;
		if (remainBufLen > 0)
		{
			int size = remainBufLen + recvSize;
			if (size < packetdef::HeaderSize)
			{
				::memcpy_s(
					remainBuf + remainBufLen,
					recvSize,
					static_cast<const char*>(wsa.buf),
					recvSize);
				remainBufLen += recvSize;
				continue;
			}

			workbuf = tempBuf;	
			protobuf::io::ArrayOutputStream os(workbuf, size);
			protobuf::io::CodedOutputStream cos(&os);

			cos.WriteRaw(remainBuf, remainBufLen);
			cos.WriteRaw(wsa.buf, recvSize);
		
			protobuf::io::ArrayInputStream is(workbuf, size);
			protobuf::io::CodedInputStream cs(&is);

			remainBufLen = 0;
			_RecvProc(cs, m_pContext, remainBuf, remainBufLen, size);			
		}
		else
		{
			if (recvSize < packetdef::HeaderSize)
			{
				::memcpy_s(
					remainBuf + remainBufLen,
					recvSize,
					static_cast<const char*>(wsa.buf),
					recvSize);

				remainBufLen += recvSize;
				continue;
			}

			workbuf = wsa.buf;
			protobuf::io::ArrayInputStream is(workbuf, recvSize);
			protobuf::io::CodedInputStream cs(&is);

			_RecvProc(cs, m_pContext, remainBuf, remainBufLen, recvSize);
		}
	}
	return 0;
}

