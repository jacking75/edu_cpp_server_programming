#ifndef __proactortimer_h__
#define __proactortimer_h__

#pragma once

#include <ace/Asynch_IO.h>
#include "AbstractTimer.h"

namespace HalfNetwork
{
	////////////////////////////////////////////////////////////
	// Description:
	// 
	////////////////////////////////////////////////////////////

	class ProacterTimerImpl : public ACE_Handler
	{
	public:
		virtual void handle_time_out(const ACE_Time_Value& tv, const void* arg);
	};

	////////////////////////////////////////////////////////////
	// Description:
	// 
	////////////////////////////////////////////////////////////

	class ProactorTimer : public AbstractTimer
	{
	public:
		ProactorTimer() {};
		virtual ~ProactorTimer() {};

	public:
		virtual void Open();
		virtual void Close();
		virtual bool Add(uint32 timerID, uint32 interval, uint32 start);

	protected:
		void ClearTimerList();

	private:
		TimerInfoList _timerList;
		ProacterTimerImpl _timerImpl;

	};

} // namespace HalfNetwork

#endif // __proactortimer_h__