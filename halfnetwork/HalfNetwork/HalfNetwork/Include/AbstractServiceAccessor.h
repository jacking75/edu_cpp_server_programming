#ifndef __abstractserviceaccessor_h__
#define __abstractserviceaccessor_h__

#pragma once

#include <ace/Message_Block.h>

namespace HalfNetwork
{
	/////////////////////////////////////////////
	// Description:
	//   Service Accessor Interface
	/////////////////////////////////////////////
	class AbstractServiceAccessor
	{
	public:
		virtual ~AbstractServiceAccessor() {}

	public:
		virtual	bool	SendRequest(uint32 streamID, ACE_Message_Block* block) = 0;
		virtual	bool	SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay) = 0;
		virtual	void	DisableService(uint32 streamID) = 0;
		virtual	void	CloseService() = 0;
		virtual uint32	ServiceCount() = 0;
		virtual	void	CloseReceiveStream(uint32 streamID) = 0;

	};

} // namespace HalfNetwork

#endif // __abstractserviceaccessor_h__