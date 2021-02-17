
#pragma once

#ifndef __LIB_IOCPMODULE_H__
#define __LIB_IOCPMODULE_H__

#include "../Common/Common.h"
#include "../Common/LIB_SESSIONDATA.h"

namespace NETWORKLIB
{
	class LIB_IOCPModule
	{
	public :
		LIB_IOCPModule();
		~LIB_IOCPModule();
	private :
		HANDLE	m_CompletionPort;
	public :
		const BOOL		START_IOCP();
		const BOOL		REGISTER_CLIENT(SOCKET ClientSocket, COMMONLIB::LIB_SESSIONDATA* Player);	// 템플릿 가능할까?
		const HANDLE	GetCompletionPort();
	};
}

#endif	// __LIB_IOCPMODULE_H__