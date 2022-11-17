#ifndef __abstracteventpool_h__
#define __abstracteventpool_h__

#pragma once

namespace HalfNetwork
{
	/////////////////////////////////////////////
	// Description:
	//   EventPool Interface
	/////////////////////////////////////////////
	class AbstractEventPool
	{
	public:
		AbstractEventPool() {}
		virtual ~AbstractEventPool() {}

	public:
		virtual	bool	Open(uint8 pool_size) = 0;
		virtual	void	Close() = 0;
		virtual	uint8	GetPoolSize() = 0;
	};

} // namespace HalfNetwork

#endif // __abstracteventpool_h__