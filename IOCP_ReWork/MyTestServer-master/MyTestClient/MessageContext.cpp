#include "stdafx.h"
#include "MessageContext.h"
#include <winsock2.h>

#include "sample.pb.h"
#include "PacketHeder.h"

using namespace google;


int CMessageContext::RTTCount = 1;

bool CMessageContext::MessageProc(packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_)
{
	//printf("recv id (%d)\n", id_);

	switch (id_)
	{
	case packetdef::PacketID::LoginAck:

		if (m_testCount == 0)
		{
			m_prevTick = ::GetTickCount();
		}

		packets::LoginAck ack;
		Deserialize<packets::LoginAck>(ack, io_);
		++m_testCount;

		if (m_testCount >= RTTCount)
		{
			int tick = ::GetTickCount() - m_prevTick;
			printf("%s : %d RTT(tick) => %d\n", m_name.c_str(), m_testCount, tick);
			m_testCount = 0;
		}

	}
	return true;
}

void CMessageContext::SetName(std::string const& name_)
{
	m_name = name_;
}

CMessageContext::CMessageContext()
	: m_testCount(0)
{
}

CMessageContext::~CMessageContext()
{
}