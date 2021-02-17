#pragma once
#include <winsock2.h>


class CConnector;

///////////////////////////////////////////////////////////
// CSession 클래스
// (session에 해당하는 connector포인터)
///////////////////////////////////////////////////////////
class CSession
{
private:
	SOCKET m_hSock;                  ///< 소켓 핸들
	const CConnector* m_pConnector;  ///< session에 해당하는 connector포인터
	bool m_bIsConnect;               ///< connect에 해당하는 곳의 접속 여부

public:
	/// 관련 connector를 가지고 온다
	CConnector const* GetConnector();

	/// 접속 유무
	bool IsConnected() const;

	/// 접속 상태 세팅
	void SetConnectionState(bool is_);

	/// 소켓 핸들을 가지고 온다
	SOCKET GetSocket() const;

	/// 소켓을 닫는다
	void CloseSocket();

public:
	explicit CSession(CConnector const* connector_);
	~CSession();
};
