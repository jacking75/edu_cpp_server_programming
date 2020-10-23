#pragma once

#include "Define.h"
#include "ErrorCode.h"

namespace NServerNetLib
{
	class ITcpNetwork
	{
	public:
		ITcpNetwork() {}
		virtual ~ITcpNetwork() {}

		virtual NET_ERROR_CODE Init(const ServerConfig* pConfig) { return NET_ERROR_CODE::NONE; }

		virtual void Run() {}

		virtual RecvPacketInfo GetPacketInfo() { return RecvPacketInfo(); }
	};
}