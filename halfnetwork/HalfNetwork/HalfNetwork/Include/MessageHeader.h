#ifndef __messageheader_h__
#define __messageheader_h__

#pragma once
#include "MessageHeader.h"

namespace HalfNetwork
{
	//////////////////////////////////////////////////////////////////////////////
	// Description:
	//   MessageHeader enumeration for communication(between app / network layer)
	//////////////////////////////////////////////////////////////////////////////
	enum EMessageHeader
	{
		eMH_Establish,	// connect, accept
		eMH_Read,				// receive
		eMH_Write,			// send
		eMH_Close,			// connection close
		eMH_Timer,			// timer
		eMH_Custom,			// User custom type
	};

	///////////////////////////////////////////////////////////////////////////
	// Description:
	//   Message struct for communication(between app layer and network layer)
	///////////////////////////////////////////////////////////////////////////
	struct MessagePostee
	{
		uint32 			stream_id;
		uint8			queue_id;
		EMessageHeader	command;
	};

}	// namespace HalfNetwork

#endif // __messageheader_h__