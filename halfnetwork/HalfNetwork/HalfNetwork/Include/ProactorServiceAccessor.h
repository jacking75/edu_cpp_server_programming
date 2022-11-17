#ifndef __proactorserviceaccessor_h__
#define __proactorserviceaccessor_h__

#pragma once
#include "AbstractServiceAccessor.h"

namespace HalfNetwork
{

	class AbstractProactorSend
	{
	public:
		virtual ~AbstractProactorSend() {}

	public:
		virtual	bool	Send(uint32 streamID, ACE_Message_Block* block) = 0;
		virtual	bool	SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay) = 0;
	};

	class IntervalProactorSend : public AbstractProactorSend
	{
	public:
		virtual ~IntervalProactorSend() {}

	public:
		bool	Send(uint32 streamID, ACE_Message_Block* block);
		bool	SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay);
	};

	class DirectProactorSend : public AbstractProactorSend
	{
	public:
		virtual ~DirectProactorSend() {}

	public:
		bool	Send(uint32 streamID, ACE_Message_Block* block);
		bool	SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay);
	};

	/////////////////////////////////////////////
	// Description:
	// ProactorService Accessor
	/////////////////////////////////////////////
	class ProactorServiceAccessor : public AbstractServiceAccessor
	{
	public:
		ProactorServiceAccessor(uint8 send_mode);
		virtual ~ProactorServiceAccessor();

	public:
		virtual bool	SendRequest(uint32 streamID, ACE_Message_Block* block);
		virtual bool	SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay);
		virtual void	DisableService(uint32 streamID);
		virtual void	CloseService();
		virtual uint32	ServiceCount();
		virtual void	CloseReceiveStream(uint32 streamID);


	private:
		AbstractProactorSend*	_send_strategy;

	};

} // namespace HalfNetwork

#endif // __proactorserviceaccessor_h__