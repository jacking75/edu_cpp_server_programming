#pragma once
#include "PacketHeder.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

class CSession;

///////////////////////////////////////////////////////////
// packet메시지 처리 클래스
///////////////////////////////////////////////////////////
class CMessageContext
{
private:
	int m_prevTick;          ///< 이전 tick(테스트용)
	int m_testCount;		 ///< 특정 패킷을 받은 카운트(테스트용)
	std::string m_name;      ///< 해당 connector의 이름, 커넥터당 recv 쓰레드가 만들어짐(테스트용)
	
public:
	static int RTTCount;        ///< 패킷의 왕복 카운트(테스트용)

public:
	/// 메시지 처리
	virtual bool MessageProc(packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_);

public:
	/// 패킷 deserialize
	template<typename T>
	bool Deserialize(__out T& req, protobuf::io::ArrayInputStream& io_)
	{
		bool ret = req.ParseFromZeroCopyStream(&io_);
		return ret;
	}

	/// connector의 이름을 등록한다
	void SetName(std::string const& name_);

	/// connector의 이름을 가지고 온다
	inline std::string GetName() { return m_name; }

public:
	CMessageContext();
	virtual ~CMessageContext();
};