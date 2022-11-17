#ifndef __interlockedvalue_h__
#define __interlockedvalue_h__

#pragma once

namespace HalfNetwork
{
	//////////////////////////
	// Description:
	//   
	//////////////////////////
	class InterlockedValue
	{
	public:
		InterlockedValue();
		InterlockedValue(long initialValue);
		~InterlockedValue() {};
		bool	Acquire();
		void	Release();

	public:
		bool	CompareExchange(long exchange, long comparand);
		void	Exchange(long exchange);
		bool	Compare(long comparand);

	private:
#ifndef WIN32
		ACE_Thread_Mutex	_lock;
#endif
		long							_value;
	};
}

#endif // __interlockedvalue_h__