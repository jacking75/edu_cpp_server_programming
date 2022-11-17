#include <string>
#include "HalfNetworkType.h"
#include "ReactorTimer.h"
#include <ace/Reactor.h>
#include "NetworkFacade.h"
#include "TimerUtil.h"

namespace HalfNetwork
{
	//////////////////////////////////////////////////////////////////////////
	int ReactorTimerImpl::handle_timeout(const ACE_Time_Value &current_time, 
																			 const void *act)
	{
		uint32 timerID = 0;
		memcpy(&timerID, act, sizeof(uint32));
		NetworkInstance->PushTimerMessage(timerID);
		return 0;
	}

	int ReactorTimerImpl::handle_close( ACE_HANDLE, ACE_Reactor_Mask mask )
	{
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	bool ReactorTimer::Add( uint32 timerID, uint32 interval, uint32 start)
	{
		TimerInfo* element = new TimerInfo();
		element->timerID = timerID;
		element->interval = interval;
		element->start = start;
		if (0 == element->start)
			element->start = element->interval;
		_timerList.insert_tail(element);
		return true;
	}

	void ReactorTimer::Open()
	{
		if (_timerList.is_empty())
			return;

		TimerInfoIter iter(_timerList);
		while(false == iter.done())
		{
			TimerInfo* elem = iter.next();
			ACE_Time_Value intervalDelay, startDelay;
			ConvertTimeValue(intervalDelay, elem->interval);
			ConvertTimeValue(startDelay, elem->start);
			ACE_Reactor::instance()->schedule_timer(&_timerImpl, &elem->timerID, startDelay, intervalDelay);
			++iter;
		}
	}

	void ReactorTimer::Close()
	{
		if (_timerList.is_empty())
			return;

		ACE_Reactor::instance()->cancel_timer(&_timerImpl);
		ClearTimerList();
	}

	void ReactorTimer::ClearTimerList()
	{
		TimerInfoIter iter(_timerList);
		while(false == iter.done())
		{
			TimerInfo* element = iter.next();
			SAFE_DELETE(element);
			TimerInfoIter remove_iter = iter;
			++iter;
			remove_iter.remove();
		}
	}

} // namespace HalfNetwork