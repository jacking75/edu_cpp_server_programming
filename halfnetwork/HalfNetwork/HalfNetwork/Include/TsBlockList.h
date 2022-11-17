#ifndef __tsblocklist_h__
#define __tsblocklist_h__

#pragma once

#include <ace/Thread_Mutex.h>

class ACE_Message_Block;

namespace HalfNetwork
{
	//////////////////////////////////
	// Description:
	// Thread safe Block linked list
	//////////////////////////////////
	class TsBlockList
	{
	public:
		TsBlockList();
		~TsBlockList();

	public:
		void				InsertTail(ACE_Message_Block* block);
		ACE_Message_Block*	Pop();
		uint32				TotalLength();

	private:
		ACE_Message_Block*		_head_block;
		ACE_Thread_Mutex		_lock;
	};

} // namespace HalfNetwork


#endif // __tsblocklist_h__