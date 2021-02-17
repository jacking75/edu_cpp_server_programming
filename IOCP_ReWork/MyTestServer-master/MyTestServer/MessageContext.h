#pragma once
#include "PacketHeder.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

class CSession;

///////////////////////////////////////////////////////////
// packet메시지 처리에 대한 베이스
///////////////////////////////////////////////////////////
class CMessageContext
{
public:
	/// 메시지 처리
	virtual bool MessageProc(CSession* session_, packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_);

public:
	/// 패킷 deserialize
	template<typename T>
	bool Deserialize(__out T& req, protobuf::io::ArrayInputStream& io_)
	{
		bool ret = req.ParseFromZeroCopyStream(&io_);
		return ret;
	}

public:
	CMessageContext();
	virtual ~CMessageContext();
};