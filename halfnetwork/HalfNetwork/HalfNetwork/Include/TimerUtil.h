#ifndef __timerutil_h__
#define __timerutil_h__

#pragma once

#include <ace/Time_Value.h>

namespace HalfNetwork
{
	inline uint32 GetTick()
	{
#ifdef WIN32
		return GetTickCount();
#else 
		timeval tick;
		gettimeofday (&tick, 0);
		return (tick.tv_sec*1000 + tick.tv_usec/1000);
#endif
	}

	inline void ConvertTimeValue(ACE_Time_Value& tm, uint32 ms)
	{
		const uint32 UsecAdjustValue = 1000;
		tm.set(ms/UsecAdjustValue, (ms%UsecAdjustValue)*UsecAdjustValue);
	}

	class CycleChecker
	{
	public:
		CycleChecker() : _term(0) , _lastMarkedTick(0)
		{}
		CycleChecker(uint32 term) : _term(term), _lastMarkedTick(0)
		{
			UpdateMarkedTick();
		}

		void	Set(uint32 term)
		{
			_term = term;
			UpdateMarkedTick();
		}

		bool	IsExpire()
		{
			uint32 currentTick = HalfNetwork::GetTick();
			if ((currentTick-_lastMarkedTick) < _term)
				return false;
			UpdateMarkedTick(currentTick);
			return true;
		}

	protected:
		void UpdateMarkedTick(uint32 tick)
		{
			_lastMarkedTick = tick;
		}

		void UpdateMarkedTick()
		{
			_lastMarkedTick = HalfNetwork::GetTick();
		}

	private:
		uint32	_term;
		uint32	_lastMarkedTick;
	};

}

#endif // __timerutil_h__