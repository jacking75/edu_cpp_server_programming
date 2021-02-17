#include "Connector.h"
//#include "Network/Session.h"
#include "define.h"
#include "MessageContext.h"
#include "Packet/sample.pb.h"


CConnector::CConnector()
	: m_receiver(),
	m_sender(),
	m_pSession(NULL)
{
}


CConnector::~CConnector()
{
	Close();
}


void CConnector::Init(CMessageContext* workerHander_)
{
	if (workerHander_ == NULL)
	{
		// 로그
		return;
	}

	m_name = workerHander_->GetName();
	GetReceiver()->SetWorkHandler(workerHander_);
}


bool CConnector::IsConnected() const
{
	if (m_pSession && m_pSession->IsConnected())
	{
		return true;
	}
	return false;
}


bool CConnector::Start(_StInit const& init_)
{
	if (GetReceiver()->GetWorkHandler() == NULL) 
	{
		// 로그
		return false;
	}

	m_conInfo = init_;
	m_pSession = new CSession(this);
	if (m_pSession == NULL)
	{
		// 로그
		return false;
	}

	// 서버 주소
	sockaddr_in ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(ServerAddr));

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(init_.nPort);
	ServerAddr.sin_addr.s_addr = inet_addr(init_.strConnectionAddr.c_str());

//	u_long on = TRUE;
//	int retval = ::ioctlsocket(m_pSession->GetSocket(), FIONBIO, &on);

	if (connect(m_pSession->GetSocket(), (sockaddr*)&ServerAddr, sizeof(ServerAddr)) != 0)
	{
		// 로그
		Close();
		return false;
	}

	if (!m_receiver.Start(m_pSession))
	{
		// 로그
		Close();
		return false;
	}

	if (!m_sender.Start(m_pSession))
	{
		// 로그
		Close();
		return false;
	}

	printf("connected...\n");

	m_pSession->SetConnectionState(true);

	return true;
}


void CConnector::Close()
{
	m_sender.Close();
	m_receiver.Close();
	m_pSession->CloseSocket();
}


CSender* CConnector::GetSender()
{
	return &m_sender;
}


CReceiver* CConnector::GetReceiver()
{
	return &m_receiver;
}


CSession* CConnector::GetSession()
{
	return m_pSession;
}

