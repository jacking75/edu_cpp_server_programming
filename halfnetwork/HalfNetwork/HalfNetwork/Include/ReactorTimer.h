#ifndef __reactortimer_h__
#define __reactortimer_h__

#pragma once

#include <ace/Service_Object.h>
#include "AbstractTimer.h"

namespace HalfNetwork
{

	class ReactorTimerImpl : public ACE_Service_Object
	{
	public:
		virtual int handle_timeout (const ACE_Time_Value &current_time,
																const void *act = 0);

		virtual int handle_close( ACE_HANDLE, ACE_Reactor_Mask mask );
	};
	////////////////////////////////////////////////////////////
	// Description:
	// 
	////////////////////////////////////////////////////////////
	class ReactorTimer : public AbstractTimer
	{
	public:
		ReactorTimer() {};
		virtual ~ReactorTimer() {};

	public:
		virtual void Open();
		virtual void Close();
		virtual bool Add(uint32 timerID, uint32 interval, uint32 start);

	protected:
		void ClearTimerList();

	private:
		TimerInfoList _timerList;
		ReactorTimerImpl _timerImpl;

	};

} // namespace HalfNetwork

#endif // __reactortimer_h__