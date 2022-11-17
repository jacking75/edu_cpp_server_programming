#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ProactorService.h"
#include "MessageHeader.h"
#include <ace/Proactor.h>
#include "TimerUtil.h"
#include "InterlockedValue.h"
#include "ServiceImpl.h"
#include "MessageBlockUtil.h"
#include "HalfNetworkDefine.h"

namespace HalfNetwork
{

	//////////////////////////////////////////////////////////////////////////
	DeleteHandler::DeleteHandler( ACE_Service_Handler* p ) 
		: _parentHandle(p)
		, _timerLock(new InterlockedValue())
	{
		_timerLock->Release();
	}

	DeleteHandler::~DeleteHandler()
	{
		SAFE_DELETE(_timerLock);
	}

	void DeleteHandler::handle_time_out( const ACE_Time_Value& tv, const void* arg )
	{
		if (false == _timerLock->Acquire())
			return;

		ACE_Proactor::instance()->cancel_timer(*this);
		SAFE_DELETE(_parentHandle);
		delete this;
	}

	//////////////////////////////////////////////////////////////////////////
	ProactorService::ProactorService() 
		: _serial(Invalid_ID)
		, _queue_id(0)
		, _receive_buffer_size(SystemConfigInst->Receive_Buffer_Len)
		,	_deleteHandler(new DeleteHandler(this))
		,	_serviceImpl(new ServiceImpl())
	{
		//ACE_DEBUG ((LM_DEBUG, "[%t] ProactorService Create.\n"));
		_serviceImpl->SetCloseFlag(eCF_None);
		_serviceImpl->ReleaseTimerLock();
		_serviceImpl->ReleaseSendLock();
		_peer_ip = new char[IP_ADDR_LEN];
		memset(_peer_ip, 0, IP_ADDR_LEN);
	}

	ProactorService::~ProactorService()
	{
		//ACE_DEBUG ((LM_DEBUG, "[%t] ProactorService Destroy.\n"));
		_UnRegister();
		delete [] _peer_ip;
		_peer_ip = NULL;
		SAFE_DELETE(_serviceImpl);
	}
	
	void ProactorService::open(ACE_HANDLE handle, ACE_Message_Block&)
	{
		this->handle(handle);
		if (this->m_reader.open (*this) != 0 ||
			this->m_writer.open (*this) != 0   )
		{
			_LogError(ACE_OS::last_error());
			ReserveClose();
			return;
		}

		_Register();
		_OnEstablish();
		_InitializeRead();
		_RegisterTimer();
	}

	void ProactorService::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
	{
		ACE_Message_Block &mb = result.message_block();
		if (0 == result.bytes_transferred() || false == result.success())
		{
			mb.release();
			ReserveClose();
			//ACE_DEBUG (( LM_INFO, ACE_TEXT ("(%P|%t) Disconnect\n")));
			return;
		}

		//ACE_DEBUG (( LM_INFO, ACE_TEXT("[%t] _%d_ Read : Size(%d)\n"), _serial, mb.length()));
		_OnReceive(mb);
		_InitializeRead();
	}

	void ProactorService::handle_write_stream(const ACE_Asynch_Write_Stream::Result &result)
	{
		if (false == result.success())
		{
			_LogError(result.error());
			ReserveClose();
			return;
		}
		//ACE_DEBUG (( LM_INFO, 
		//	ACE_TEXT("_%d_ Write Complete : write(%d), transfer(%d)\n"), 
		//	_serial, result.bytes_to_write(), result.bytes_transferred()));

		if (0 < result.message_block().total_length())
		{
			ACE_Message_Block &block = result.message_block();
			//printf("handle_write_stream transferred(%d), write(%d), remain(%d), %d\n", 
			//	result.bytes_transferred(), result.bytes_to_write(), block.total_length(), block.length());
			
			if (false == _SmartSendImpl(&block))
			{
				//_LogError(ACE_OS::last_error());
				//ACE_DEBUG ((LM_ERROR, ACE_TEXT("ProactorService SmartSend writev")));
				block.release();
				ReserveClose();
				_serviceImpl->ReleaseSendLock();
			}
			return;
		}
		ACE_Message_Block &block = result.message_block();
		block.release();
		_serviceImpl->ReleaseSendLock();
	}

	void ProactorService::handle_time_out(const ACE_Time_Value& tv, const void* arg)
	{
		ACE_UNUSED_ARG(tv);
		ACE_UNUSED_ARG(arg);
		if ( false == _serviceImpl->AcquireTimerLock())
			return;

		if (false == _IsCloseFlagActivate())
			_SendQueuedBlock();

		_serviceImpl->ReleaseTimerLock();
		_CheckZombieConnection();
	}

	void ProactorService::addresses(const ACE_INET_Addr& remote, const ACE_INET_Addr& local)
	{
		ACE_UNUSED_ARG(local);
		remote.get_host_addr(_peer_ip, IP_ADDR_LEN);
		//remote.addr_to_string(_peer_ip, Max_Host_Name_Len);
	}

	void ProactorService::QueueID(uint8 id)
	{
		_queue_id = id;
	}

	void ProactorService::ReceiveBufferSize( uint32 size )
	{
		_receive_buffer_size = size;
	}

	void ProactorService::ActiveClose()
	{
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_None == closeFlag)
			_serviceImpl->SetCloseFlag(eCF_Active);
	}

	void ProactorService::ReserveClose()
	{
		//ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		//if (eCF_Passive != closeFlag )
		_serviceImpl->SetCloseFlag(eCF_Passive);
	}

	void ProactorService::ReceiveClose()
	{
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_None == closeFlag)
			_serviceImpl->SetCloseFlag(eCF_Receive);
	}

	bool ProactorService::IntervalSend(ACE_Message_Block* block)
	{
		return _PushQueue(block, 0);
	}

	bool ProactorService::DirectSend(ACE_Message_Block* block)
	{
		return _SmartSend(block);
	}

	bool ProactorService::ReserveSend(ACE_Message_Block* block, uint32 delay)
	{
		return _PushQueue(block, GetTick()+delay);
	}

	void ProactorService::_InitializeRead()
	{
		ACE_Message_Block* block = _serviceImpl->AllocateBlock(_receive_buffer_size);
		ACE_ASSERT(block);
		if (0 != this->m_reader.read(*block, block->space()))
		{
			//_LogError(ACE_OS::last_error());
			//ACE_DEBUG ((LM_ERROR, ACE_TEXT("ProactorService begin read")));
			block->release();
			ReserveClose();
			return;
		}
	}

	void ProactorService::_LogError(uint32 error_code)
	{
		ACE_DEBUG ((LM_ERROR, ACE_TEXT("_%d_ Error(%s)\n"), _serial, ACE::sock_error(error_code)));
	}

	void ProactorService::_CloseHandle()
	{
		if (ACE_INVALID_HANDLE == this->handle())
			return;

		ACE_OS::closesocket(this->handle());
		this->handle(ACE_INVALID_HANDLE);
	}

	void ProactorService::_CancelTimer()
	{
		ACE_Proactor::instance()->cancel_timer(*this);
	}

	void ProactorService::_ActiveClose()
	{
		if (ACE_INVALID_HANDLE == this->handle())
			return;

		ACE_OS::shutdown(this->handle(), ACE_SHUTDOWN_WRITE);
	}

	void ProactorService::_ReceiveClose()
	{
		if (ACE_INVALID_HANDLE == this->handle())
			return;

		ACE_OS::shutdown(this->handle(), ACE_SHUTDOWN_READ);
	}

	void ProactorService::_OnEstablish()
	{
		//ACE_DEBUG ((LM_DEBUG, "[%t] OnAccept.\n"));
		
		ACE_Message_Block* block = _serviceImpl->AllocateBlock(IP_ADDR_LEN);
		block->copy((const char*)_peer_ip, IP_ADDR_LEN);
		_serviceImpl->PushEventBlock(eMH_Establish, _queue_id, _serial, block);
	}

	void ProactorService::_OnReceive(ACE_Message_Block& block)
	{
		//ACE_DEBUG ((LM_DEBUG, "[%t] OnReceive (%d), (%d)byte.\n", _serial, block.length()));
		_serviceImpl->PushEventBlock(eMH_Read, _queue_id, _serial, &block);
	}

	void ProactorService::_Close()
	{
		_CancelTimer();
		_CloseHandle();
		//ACE_DEBUG ((LM_DEBUG, "[%t] ProactorService OnClose.\n"));
		ACE_Message_Block* block = new ACE_Message_Block();
		_serviceImpl->PushEventBlock(eMH_Close, _queue_id, _serial, block);
		ACE_Time_Value intervalTime(5, 0);
		ACE_Proactor::instance()->schedule_timer(*_deleteHandler, 0, intervalTime, intervalTime);
		//delete this;
	}

	void ProactorService::_Register()
	{
		_serial = ProactorServiceMap->Register(this);
		//ACE_DEBUG ((LM_DEBUG, "[%t] ProactorService Register(%d).\n", _serial));
		ACE_ASSERT(_serial != Invalid_ID);
	}

	void ProactorService::_UnRegister()
	{
		if (Invalid_ID == _serial)
			return;
		ProactorServiceMap->UnRegister(_serial);
	}

	void ProactorService::_RegisterTimer()
	{
		ACE_Time_Value intervalTime;
		ConvertTimeValue(intervalTime, SystemConfigInst->Interval_Send_Term);
		ACE_Proactor::instance()->schedule_timer(*this, 0, ACE_Time_Value::zero, intervalTime);
	}

	bool ProactorService::_PushQueue(ACE_Message_Block* block, uint32 tick)
	{
		return _serviceImpl->PushQueue(block, tick);
	}

	bool ProactorService::_PopQueue(ACE_Message_Block** param_block)
	{
		return _serviceImpl->PopQueue(param_block);
	}

	void ProactorService::_SendQueuedBlock()
	{
		if (false == _serviceImpl->TrySendLock())
			return;
		ACE_Message_Block* block = NULL;
		if (false == _PopQueue(&block))
			return;
		_SmartSend(block);
	}

	bool ProactorService::_SmartSend(ACE_Message_Block* block)
	{
		if (false == _serviceImpl->AcquireSendLock())
		{
			return _PushQueue(block, 0);
		}
		//ACE_DEBUG (( LM_INFO, ACE_TEXT("_%d_ SmartSend : Length(%d)"), _serial, block->length()));
		if (false == _SmartSendImpl(block))
		{
			block->release();
			ReserveClose();
			return false;
		}
		return true;
	}

	bool ProactorService::_SmartSendImpl( ACE_Message_Block* block )
	{
		if (NULL == block->cont())
		{
			if ( -1 == m_writer.write(*block, block->length()))
				return false;
		}
		else
		{
#ifndef WIN32
			ACE_Message_Block* margedBlock = _serviceImpl->AllocateBlock(block->total_length());
			MakeMergedBlock(block, margedBlock);
			block->release();
			if ( -1 == m_writer.write(*margedBlock, margedBlock->length()))
#else
			if ( -1 == m_writer.writev(*block, block->total_length()))
#endif
				return false;
		}
		return true;
	}


	bool ProactorService::_IsCloseFlagActivate()
	{
		ECloseFlag closeFlag = (ECloseFlag)_serviceImpl->GetCloseFlag();
		if (eCF_Passive == closeFlag)
		{
			_Close();
			return true;
		}
		else if (eCF_Active == closeFlag)
		{
			_ActiveClose();
			ReserveClose();
			return true;
		}
		else if (eCF_Receive == closeFlag)
		{
			_ReceiveClose();
			_serviceImpl->SetCloseFlag(eCF_None);
			return false;
		}
		return false;
	}

	void ProactorService::_CheckZombieConnection()
	{
		if (false == _serviceImpl->IsZombieConnection())
			return;

		ActiveClose();
		//ReserveClose();
	}
} // namespace HalfNetwork
