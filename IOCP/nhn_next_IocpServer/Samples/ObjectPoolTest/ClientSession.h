#pragma once


#include "ObjectPool.h"


class ClientSession : public ObjectPool<ClientSession>
{
public:
	ClientSession() {}
	virtual ~ClientSession() {}

	void SessionReset() {}
	


private:
	
	__int64 m_Dummy1 = 1;
	__int64 m_Dummy2 = 2;
	__int64 m_Dummy3 = 3;
	__int64 m_Dummy4 = 4;
} ;



