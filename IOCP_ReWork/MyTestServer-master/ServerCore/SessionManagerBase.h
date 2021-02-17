#pragma once
#include <winsock2.h>

///////////////////////////////////////////////////////////
// CSessionManagerBase Class
// CSessionManagerBase class는 반드시 해당 클래스를 상속받아야 함
///////////////////////////////////////////////////////////
class CSession;
class CSessionManagerBase
{
public:
	/// 세션을 만든다
	virtual CSession* CreateSession(SOCKET const& socket_) = 0;
};
