#include "stdafx.h"
#include "Sender.h"
#include "Session.h"
#include "Define.h"
#include "PacketHeder.h"
#include "Common/VirtualMemoryPool.hpp"
#ifndef __TBB_malloc_proxy
#include "tcmalloc.h"
#else
#include "tbb/tbbmalloc_proxy.h"
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


CSender::CSender()
	: m_bRunning(false)
{
}

CSender::~CSender()
{
	m_bRunning = false;
}

bool CSender::Start(CSession* session_)
{
	m_waitingEvent = ::WSACreateEvent();
	m_pSession = session_;
	m_bRunning = true;
	m_hThread = std::thread(&CSender::SendProc, this);
	return true;
}

bool CSender::Push(packetdef::PacketID id_, protobuf::Message* message_)
{
	if (!(m_pSession && m_pSession->IsConnected()))
		return false;

	if (message_ == NULL)
	{
		// 로그
		return false;
	}

	int nSize = packetdef::HeaderSize + message_->ByteSize();
	if (nSize > PACKET_SIZE)
	{
		// 로그
		return false;
	}

	SPacketTransInfo* entity;
	SPacketTransInfo::_StInit param(id_, message_->ByteSize(), this);
	if (!GetPoolObject(entity, &param))
	{
		// 로그
		return false;
	}

	packetdef::PacketHeader packetHeader(id_, message_->ByteSize());
	protobuf::io::ArrayOutputStream output_array_stream(entity->message, nSize);
	protobuf::io::CodedOutputStream os(&output_array_stream);

	os.WriteRaw(&packetHeader, packetdef::HeaderSize);
	message_->SerializePartialToCodedStream(&os);

	m_queue.push(entity);
	::WSASetEvent(m_waitingEvent);

	return true;
}


void CSender::Close()
{
	m_bRunning = false;
}

unsigned CALLBACK CSender::SendProc()
{
	while (m_bRunning)
	{
		SPacketTransInfo* entity;
		while (m_queue.try_pop(entity))
		{
			if (entity)
			{
				int nSize = packetdef::HeaderSize + entity->header.dataSize;
//				printf("send size %d\n", nSize);
				
				if (::send(m_pSession->GetSocket(), static_cast<const char*>(entity->message), nSize, 0) == SOCKET_ERROR)
				{
					int err = WSAGetLastError();
					if (err != WSA_IO_PENDING)
					{
						printf("send error (%d)\n", err);
					}
				}

				BackPoolObject(entity);
			}
		}

		::WSAResetEvent(m_waitingEvent);
		::WaitForSingleObject(m_waitingEvent, INFINITE);
	}

	::WSACloseEvent(m_waitingEvent);
	return 0;
}


