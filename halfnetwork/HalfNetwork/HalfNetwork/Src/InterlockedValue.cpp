#include <ace/Singleton.h>
#include "InterlockedValue.h"

namespace HalfNetwork
{

	InterlockedValue::InterlockedValue()
	{
	}

	InterlockedValue::InterlockedValue( long initialValue ) : _value(initialValue)
	{
	}

	bool InterlockedValue::Acquire()
	{
		return CompareExchange(1, 0);
	}

	void InterlockedValue::Release()
	{
		Exchange(0);
	}

	bool InterlockedValue::CompareExchange(long exchange, long comparand)
	{
#ifdef WIN32
		return (comparand == InterlockedCompareExchange(&_value, exchange, comparand));
#else
		ACE_Guard<ACE_Thread_Mutex> guard(_lock);
		if (_value != comparand)
			return false;
		_value = exchange;
		return true;
#endif
	}

	void InterlockedValue::Exchange(long exchange)
	{
#ifdef WIN32
		InterlockedExchange(&_value, exchange);
#else
		ACE_Guard<ACE_Thread_Mutex> guard(_lock);
		_value = exchange;
#endif
	}

	bool InterlockedValue::Compare(long comparand)
	{
#ifdef WIN32
		return (_value == comparand);
#else
		ACE_Guard<ACE_Thread_Mutex> guard(_lock);
		return (_value == comparand);
#endif
	}

} // namespace HalfNetwork