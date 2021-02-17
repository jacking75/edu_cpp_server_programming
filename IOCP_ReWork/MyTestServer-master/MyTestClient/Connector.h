#pragma once

#include "Common/ThreadRegister.h"
#include "Define.h"
#include "Session.h"
#include "Receiver.h"
#include "Sender.h"


class CMessageContext;

///////////////////////////////////////////////////////////
// CConnector 클래스
///////////////////////////////////////////////////////////
class CConnector
{
public:
	struct _StInit
	{
		std::string strConnectionAddr;
		unsigned short nPort;

		_StInit& operator=(const _StInit& rhs_) {
			strConnectionAddr = rhs_.strConnectionAddr;
			nPort = rhs_.nPort;
			return *this;
		}

		explicit _StInit(const _StInit& rhs_)
		{
			this->operator=(rhs_);
		}

		_StInit()
		{
		}
	};

private:
	_StInit m_conInfo;	   ///< 커넥션을 할 서버의 정보
	CSession* m_pSession;  ///< 커넥터의 세션 포인터
	CReceiver m_receiver;  ///< 커넥터에 종속하는 receiver
	CSender m_sender;      ///< 커넥터에 종속하는 sender
	std::string m_name;    ///< 커넥션의 구분을 위한 이름(테스트용 아이디)


public:
	/// recv 메시지를 받아 올때 해당하는 context를 등록하여 초기화한다
	void Init(CMessageContext* workerHander_);

	/// 해당 커넥션이 접속이 되었는가
	bool IsConnected() const;

	/// 등록되어 있는 커넥션 정보를 가지고 접속 및 recv, send 등을 시작한다
	bool Start(_StInit const& init_);

	/// 커넥션을 닫고 관련 동작을 종료한다
	void Close();

	/// 해당 커넥션의 receiver를 가지고 온다
	CReceiver* GetReceiver();

	/// 해당 커넥션의 sender를 가지고 온다
	CSender* GetSender();

	/// 해당 커넥션의 세션을 가지고 온다
	CSession* GetSession();


public:
	CConnector();
	~CConnector();
};

// 접속자.
extern CConnector Connector;