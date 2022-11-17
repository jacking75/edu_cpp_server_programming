#ifndef __abstractacceptor_h__
#define __abstractacceptor_h__

#pragma once

namespace HalfNetwork
{

	struct AcceptorInfo;
	/////////////////////////////////////////////
	// Description:
	//   Acceptor Interface
	/////////////////////////////////////////////
	class AbstractAcceptor
	{
	public:
		virtual ~AbstractAcceptor() {}

	public:
		virtual	bool	Open() = 0;
		virtual	void	Close() = 0;

	public:
		virtual	uint16	ServicePort() = 0;
		virtual	void	Suspend() = 0;
		virtual	void	Resume() = 0;
		virtual	void	SetAcceptIP(const ACE_TCHAR* ip) = 0;
		virtual void	GetInfo(AcceptorInfo& info) = 0;
		virtual void	ReceiveBufferSize(uint32 size) = 0;
		virtual void	InitialAcceptCount(uint32 count) = 0;
	};

} // namespace HalfNetwork

#endif // __abstractacceptor_h__