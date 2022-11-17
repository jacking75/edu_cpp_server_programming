#ifndef __abstracttimer_h__
#define __abstracttimer_h__

#pragma once

#include <ace/Containers.h>

namespace HalfNetwork
{

	/////////////////////////////////////////////
	// Description:
	//   
	/////////////////////////////////////////////

	struct TimerInfo
	{
		uint32 timerID;
		uint32 interval;
		uint32 start;
	};

	typedef ACE_DLList<TimerInfo>			TimerInfoList;
	typedef ACE_DLList_Iterator<TimerInfo>	TimerInfoIter;

	class AbstractTimer
	{
	public:
		virtual ~AbstractTimer() {}

	public:
		virtual	void	Open() = 0;
		virtual	void	Close() = 0;
		virtual bool Add(uint32 timerID, uint32 interval, uint32 start) = 0;
	};

} // namespace HalfNetwork

#endif // __abstracttimer_h__