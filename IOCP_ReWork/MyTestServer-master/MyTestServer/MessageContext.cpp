#include "stdafx.h"
#include "MessageContext.h"


bool CMessageContext::MessageProc(CSession* session_, packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_)
{
	// don't call this
	return false;
}


CMessageContext::CMessageContext()
{
}

CMessageContext::~CMessageContext()
{
}