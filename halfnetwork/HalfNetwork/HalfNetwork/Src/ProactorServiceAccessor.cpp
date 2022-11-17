#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "HalfNetworkDefine.h"
#include "ProactorServiceAccessor.h"
#include "ProactorService.h"
#include "TimerUtil.h"
#include <ace/Event.h>
#include <ace/OS_NS_sys_time.h>

namespace HalfNetwork
{
	bool IntervalProactorSend::Send(uint32 streamID, ACE_Message_Block* block)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
			return false;
		return service->IntervalSend(block);
	}

	bool IntervalProactorSend::SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
			return false;
		return service->ReserveSend(block, delay);
	}

	bool DirectProactorSend::Send(uint32 streamID, ACE_Message_Block* block)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
		{
			//OutputDebugString(_T("DirectProactorSend::Send #1\n"));
			return false;
		}
		return service->DirectSend(block);
	}

	bool DirectProactorSend::SendReserve(uint32 streamID, ACE_Message_Block* block, uint32 delay)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
			return false;
		return service->ReserveSend(block, delay);
	}

	ProactorServiceAccessor::ProactorServiceAccessor(uint8 send_mode) : _send_strategy(NULL)
	{
		switch(send_mode)
		{
		case eSM_Interval:
			_send_strategy = new IntervalProactorSend();
			break;
		case eSM_Direct:
			_send_strategy = new DirectProactorSend();
			break;
		}
	}

	ProactorServiceAccessor::~ProactorServiceAccessor()
	{
		SAFE_DELETE(_send_strategy);
	}

	bool ProactorServiceAccessor::SendRequest(uint32 streamID, ACE_Message_Block* block)
	{
		return _send_strategy->Send(streamID, block);
	}

	bool ProactorServiceAccessor::SendReserve(uint32 streamID, 
																						ACE_Message_Block* block, 
																						uint32 delay)
	{
		return _send_strategy->SendReserve(streamID, block, delay);
	}

	void ProactorServiceAccessor::DisableService(const uint32 streamID)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
			return;
		service->ActiveClose();
	}

	void ProactorServiceAccessor::CloseService()
	{
		for (size_t i=0;i<Max_ProactorService; ++i)
		{
			ProactorService* service = ProactorServiceMap->Get((uint32)i);
			if (NULL == service)
				continue;

			service->ReserveClose();
		}

		ACE_Event wait_event;
		ACE_Time_Value* wait_time = new ACE_Time_Value(ACE_OS::gettimeofday() + ACE_Time_Value(0, 100*UsecAdjustValue));
		while (0 != ProactorServiceMap->Size())
		{
			wait_event.wait(wait_time);
		}
		delete wait_time;
	}

	uint32 ProactorServiceAccessor::ServiceCount()
	{
		return ProactorServiceMap->Size();
	}

	void ProactorServiceAccessor::CloseReceiveStream(uint32 streamID)
	{
		ProactorService* service = ProactorServiceMap->Get(streamID);
		if (NULL == service)
			return;
		service->ReceiveClose();
	}

} // namespace HalfNetwork