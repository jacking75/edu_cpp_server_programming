#include "Session.h"


CSession::CSession(CConnector const* connector_) :
	m_pConnector(connector_),
	m_bIsConnect(false)
{
	m_hSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
}

CSession::~CSession()
{
}

CConnector const* CSession::GetConnector()
{
	return m_pConnector;
}

bool CSession::IsConnected() const
{
	return m_bIsConnect;
}

void CSession::SetConnectionState(bool is_)
{
	m_bIsConnect = is_;
}

SOCKET CSession::GetSocket() const
{
	return m_hSock;
}

void CSession::CloseSocket()
{
	::closesocket(m_hSock);
}

