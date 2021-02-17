#pragma once
#include "MessageContext.h"

#include "NPC.h"
#include <vector>

class CSession;
class CServerBase;

///////////////////////////////////////////////////////////
// 메시지 처리를 위해 상속받은 임시 클래스
///////////////////////////////////////////////////////////
class CLoginContext : 
	public CMessageContext
{
private:
	CServerBase* m_pServer;   ///< 종속된 서버

public:
	/// 메시지를 수행한다.
	virtual bool MessageProc(CSession* session_, packetdef::PacketID id_, protobuf::io::ArrayInputStream& io_) override;

public:
	CLoginContext(CServerBase * server_);
	virtual ~CLoginContext();
};