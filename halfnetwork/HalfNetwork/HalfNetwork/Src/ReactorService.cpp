
#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "HalfNetworkDefine.h"

#include "ReactorService.h"
#include <ace/Reactor.h>
#include "MessageHeader.h"
#include "TimerUtil.h"
#include "ServiceImpl.h"
#include "InterlockedValue.h"
#include "MessageBlockUtil.h"

namespace HalfNetwork
{
	//////////////////////////////////////////////////////////////////////////
	ReactorService::ReactorService() 
		: _closeLock(new InterlockedValue())
		, _serial(Invalid_ID)
		, _queue_id(0)
		, _receive_buffer_size(SystemConfigInst->Receive_Buffer_Len)
		,	_serviceImpl(new ServiceImpl())
		,	_sentCloseMessage(false)
	{
		_peer_ip = new char[IP_ADDR_LEN];
		_serviceImpl->SetCloseFlag(eCF_None);
		_serviceImpl->ReleaseTimerLock();
		_serviceImpl->ReleaseSendLock();
		_closeLock->Release();
	}

	ReactorService::~ReactorService()
	{
		delete [] _peer_ip;
		_peer_ip = NULL;
		//ACE_DEBUG ((LM_DEBUG, "[%t] ReactorService Destroy.\n"));
		_CleanUp();
		SAFE_DELETE(_serviceImpl);
		SAFE_DELETE(_closeLock);
	}

	int ReactorService::open()
	{
		ACE_INET_Addr peer_addr;
		if (0 == this->peer().get_remote_addr(peer_addr))
		{
			peer_addr.get_host_addr(_peer_ip, IP_ADDR_LEN);
		}

		_Register();
		_OnEstablish();
		_RegisterTimer();

		return this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
	}

	int	ReactorService::open(void* p)
	{
		ACE_UNUSED_ARG(p);
		return open();
	}

	int ReactorService::handle_input(ACE_HANDLE fd)
	{
		ACE_Message_Block* block = 
			_serviceImpl->AllocateBlock(_receive_buffer_size);
		ACE_ASSERT(NULL != block);

		ssize_t recv_length = 0;
		if ((recv_length = this->peer().recv(block->wr_ptr(), block->space())) <= 0)
		{
			//_NotifyClose();
			//ACE_DEBUG ((LM_DEBUG,
			//	ACE_TEXT ("(%P|%t) Connection closed\n")));
			block->release();
			return -1;
		}
		block->wr_ptr(recv_length);
		_OnReceive(block);
		return 0;
	}

	int ReactorService::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
	{
		ACE_UNUSED_ARG(handle);
		if (mask == ACE_Event_Handler::WRITE_MASK)
			return 0;

		if (false == _closeLock->Acquire())
			return 0;

		_NotifyClose();
		//_Close();
		delete this;
		return 0;
	}

	int ReactorService::handle_timeout(const ACE_Time_Value &current_time, const void* act)
	{
		ACE_UNUSED_ARG(act);
		if ( false == _serviceImpl->AcquireTimerLock())
			return 0;

		if (false == _IsCloseFlagActivate())
		{
			_SendQueuedBlock();
		}
		else
		{
			_serviceImpl->ReleaseTimerLock();
			return -1;
		}

		_serviceImpl->ReleaseTimerLock();
		_CheckZombieConnection();
		return 0;
	}

	ACE_HANDLE	ReactorService::get_handle() const 
	{ 
		return this->peer().get_handle(); 
	}

	void ReactorService::set_handle(ACE_HANDLE handle) 
	{
		return this->peer().set_handle(handle);
	}

	//ACE_SOCK_Stream& ReactorService::peer() 
	//{ 
	//	return this->_sock; 
	//}

	void ReactorService::_Register()
	{
		_serial = ReactorServiceMap->Register(this);
		ACE_ASSERT(_serial != Invalid_ID);
	}

	void ReactorService::_UnRegister()
	{
		if (Invalid_ID == _serial)
			return;

		ReactorServiceMap->UnRegister(_serial);
	}

	void ReactorService::_CloseHandle()
	{
		if (ACE_INVALID_HANDLE == get_handle())
			return;

		this->peer().close();
	}

	void ReactorService::_RemoveHandler()
	{
		this->reactor()->cancel_timer(this);
		ACE_Reactor_Mask mask = 
			ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
		this->reactor()->remove_handler(this, mask);
	}

	void ReactorService::_CleanUp()
	{
		_RemoveHandler();
		_CloseHandle();
		_UnRegister();
	}

	void ReactorService::_Close()
	{
		_NotifyClose();
		_CloseHandle();
		//ACE_Time_Value intervalTime(5, 0);
		//this->reactor()->schedule_timer(_deleteHandler, 0, intervalTime, intervalTime);
	}

	void ReactorService::_RegisterTimer()
	{
		ACE_Time_Value intervalTime;
		ConvertTimeValue(intervalTime, SystemConfigInst->Interval_Send_Term);
		this->reactor()->schedule_timer(this, 0, ACE_Time_Value::zero, intervalTime);
	}

	void ReactorService::_ActiveClose()
	{
		if (ACE_INVALID_HANDLE == get_handle())
			return;

		this->peer().close_writer();
	}

	void ReactorService::QueueID(uint8 id)
	{
		_queue_id = id;
	}

	void ReactorService::ReceiveBufferSize( uint32 size )
	{
		_receive_buffer_size = size;
	}

	void ReactorService::ActiveClose()
	{
		_serviceImpl->SetCloseFlag(eCF_Active);
	}

	void ReactorService::ReceiveClose()
	{
		_serviceImpl->SetCloseFlag(eCF_Receive);
	}

	void ReactorService::ReserveClose()
	{
		_serviceImpl->SetCloseFlag(eCF_Passive);
	}

	bool ReactorService::IntervalSend(ACE_Message_Block* block)
	{
		return _PushQueue(block, 0);
		//return _SmartSend(block);
	}

	bool ReactorService::DirectSend(ACE_Message_Block* block)
	{
		return _SmartSend(block);
	}

	void ReactorService::_OnEstablish()
	{	
		ACE_Message_Block* block = _serviceImpl->AllocateBlock(IP_ADDR_LEN);
		block->copy((const char*)_peer_ip, IP_ADDR_LEN);
		_serviceImpl->PushEventBlock(eMH_Establish, _queue_id, _serial, block);
	}

	void ReactorService::_OnReceive(ACE_Message_Block* block)
	{
		_serviceImpl->PushEventBlock(eMH_Read, _queue_id, _serial, block);
	}

	void ReactorService::_NotifyClose()
	{
		if (_sentCloseMessage)
			return;
		_sentCloseMessage = true;
		ACE_Message_Block* block = new ACE_Message_Block();
		_serviceImpl->PushEventBlock(eMH_Close, _queue_id, _serial, block);
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_None == closeFlag)
			ReserveClose();
	}

	bool ReactorService::_SmartSend(ACE_Message_Block* block)
	{
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_None != closeFlag)
		{
			block->release();
			return false;
		}
		if (NULL != block->cont())
		{
			ACE_Message_Block* margedBlock = _serviceImpl->AllocateBlock(block->total_length());
			MakeMergedBlock(block, margedBlock);
			block->release();
			block = margedBlock;
		}

		if (false == _serviceImpl->AcquireSendLock())
		{
			return _PushQueue(block, 0);
		}
		ssize_t send_length = this->peer().send(block->rd_ptr(), block->length());
		_serviceImpl->ReleaseSendLock();
		if (send_length >= (ssize_t)block->length())
		{
			block->release();
			return true;
		}

		//printf("_SmartSend fail(%d, %d)\n", send_length, error_no);
		if (send_length == -1 && ACE_OS::last_error() != EWOULDBLOCK)
		{
			ReserveClose();
			block->release();
			return false;
		}

		if (send_length > 0)
			block->rd_ptr(send_length);
		return _PushQueue(block, 0);
	}

	void ReactorService::_SendQueuedBlock()
	{
		if (false == _serviceImpl->TrySendLock())
			return;
		ACE_Message_Block* block = NULL;
		if (false == _PopQueue(&block))
			return;
		_SmartSend(block);
	}

	bool ReactorService::_PushQueue(ACE_Message_Block* block, uint32 tick)
	{
		return _serviceImpl->PushQueue(block, tick);
	}

	bool ReactorService::_PopQueue(ACE_Message_Block** param_block)
	{
		return _serviceImpl->PopQueue(param_block);
	}

	bool ReactorService::_IsCloseFlagActivate()
	{
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_Passive == closeFlag)
		{
			_NotifyClose();
			//_Close();
			return true;
		}
		else if (eCF_Active == closeFlag)
		{
			_ActiveClose();
			ReserveClose();
			return true;
		}
		return false;
	}

	void ReactorService::_CheckZombieConnection()
	{
		if (false == _serviceImpl->IsZombieConnection())
			return;

		ActiveClose();
		ReserveClose();
	}
} // namespace HalfNetwork