#include <string>
#include "HalfNetworkType.h"
#include "ProactorTimer.h"
#include <ace/Proactor.h>
#include "NetworkFacade.h"
#include "TimerUtil.h"

namespace HalfNetwork
{
	//////////////////////////////////////////////////////////////////////////

	void ProacterTimerImpl::handle_time_out( const ACE_Time_Value& tv, const void* arg )
	{
		uint32 timerID = 0;
		memcpy(&timerID, arg, sizeof(uint32));
		NetworkInstance->PushTimerMessage(timerID);
	}

	//////////////////////////////////////////////////////////////////////////

	bool ProactorTimer::Add( uint32 timerID, uint32 interval, uint32 start)
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

	void ProactorTimer::Open()
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
			ACE_Proactor::instance()->schedule_timer(_timerImpl, &elem->timerID, startDelay, intervalDelay);
			++iter;
		}
	}

	void ProactorTimer::Close()
	{
		if (_timerList.is_empty())
			return;

		ACE_Proactor::instance()->cancel_timer(_timerImpl);
		ClearTimerList();
	}

	void ProactorTimer::ClearTimerList()
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