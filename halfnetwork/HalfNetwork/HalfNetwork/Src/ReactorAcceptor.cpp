#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "HalfNetworkDefine.h"
#include <ace/Reactor.h>
#include "ReactorAcceptor.h"
#include "ReactorService.h"

namespace HalfNetwork
{
	/////////////////////////////////////////
	// CustomSockAcceptor
	/////////////////////////////////////////
	CustomSockAcceptor::CustomSockAcceptor(uint16 service_port, uint8 queue_id)	: 
		_service_port(service_port), 
		_queue_id(queue_id), 
		_receive_buffer_size(0),
		_suspend_accept(FALSE)
	{
		_accept_ip = new ACE_TCHAR[IP_ADDR_LEN];
		ACE_OS::memset(_accept_ip, 0, IP_ADDR_LEN);
	}

	CustomSockAcceptor::~CustomSockAcceptor()
	{
		delete [] _accept_ip;
		_accept_ip = NULL;
	}

	bool CustomSockAcceptor::Open()
	{
		if (false == _StartListen())
			return false;

		return true;
	}

	void CustomSockAcceptor::Close()
	{
		if (this->_acceptor.get_handle() == ACE_INVALID_HANDLE)
			return;

		ACE_Reactor_Mask mask = 
			ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
		if (-1 == this->reactor()->remove_handler(this, mask))
		{
			ACE_ERROR((LM_ERROR,
				ACE_TEXT ("%p\n"),
				ACE_TEXT ("ReactorAcceptor.Close")));
		}
		this->_acceptor.close();
	}

	bool CustomSockAcceptor::_StartListen()
	{
		ACE_INET_Addr listen_addr;
		if (0 == ACE_OS::strlen(_accept_ip))
			listen_addr.set(_service_port);
		else
			listen_addr.set(_service_port, _accept_ip);

		if (-1 == _acceptor.open(listen_addr, 1))
			ACE_ERROR_RETURN ((LM_ERROR,
			ACE_TEXT ("%p\n"),
			ACE_TEXT ("ReactorAcceptor.StartListen")),
			false);
		return (-1 != ACE_Reactor::instance()->register_handler
			(this, ACE_Event_Handler::ACCEPT_MASK));
	}

	ACE_HANDLE CustomSockAcceptor::get_handle() const
	{ 
		return this->_acceptor.get_handle();
	}

	int CustomSockAcceptor::handle_input(ACE_HANDLE fd)
	{
		ReactorService* service = new ReactorService();
		if (-1 == _acceptor.accept(service->peer()))
		{
			ACE_ERROR_RETURN ((LM_ERROR,
				ACE_TEXT ("(%P|%t) %p\n"),
				ACE_TEXT ("Failed to accept ")
				ACE_TEXT ("service connection")),
				-1);
			delete service;
			service = NULL;
			return 0;
		}
		service->QueueID(_queue_id);
		if (0 != _receive_buffer_size)
			service->ReceiveBufferSize(_receive_buffer_size);
		service->reactor(this->reactor());
		if (-1 == service->open())
			service->handle_close(ACE_INVALID_HANDLE, 0);

		if (TRUE == _suspend_accept.value())
			service->handle_close(ACE_INVALID_HANDLE, 0);

		return 0;
	}

	int CustomSockAcceptor::handle_close(ACE_HANDLE handle,ACE_Reactor_Mask close_mask)
	{
		Close();
		return 0;
	}

	void CustomSockAcceptor::SetAcceptIP(const ACE_TCHAR* ip)
	{
		ACE_OS::strncpy(_accept_ip, ip, IP_ADDR_LEN);
	}

	void CustomSockAcceptor::Suspend()
	{
		_suspend_accept = TRUE;
	}

	void CustomSockAcceptor::Resume()
	{
		_suspend_accept = FALSE;
	}

	void CustomSockAcceptor::GetInfo(AcceptorInfo& info)
	{
		info.port = _service_port;
		info.queue_id = _queue_id;
		ACE_OS::strcpy(info.ip, _accept_ip);

		info.suspend = (_suspend_accept.value() == TRUE);
	}

	void CustomSockAcceptor::ReceiveBufferSize( uint32 size )
	{
		_receive_buffer_size = size;
	}

	/////////////////////////////////////////
	// ReactorAcceptor
	/////////////////////////////////////////
	ReactorAcceptor::ReactorAcceptor(uint16 service_port, uint8 queue_id) 
		: _service_port(service_port)
		, _custom_acceptor(service_port, queue_id)
	{
	}

	ReactorAcceptor::~ReactorAcceptor()
	{
	}

	bool ReactorAcceptor::Open()
	{
		if (false == _custom_acceptor.Open())
			return false;

		return true;
	}

	void ReactorAcceptor::Close()
	{
		_custom_acceptor.Close();
	}

	uint16	ReactorAcceptor::ServicePort()
	{
		return _service_port;
	}

	void ReactorAcceptor::Suspend()
	{
		_custom_acceptor.Suspend();
	}

	void ReactorAcceptor::Resume()
	{
		_custom_acceptor.Resume();
	}

	void ReactorAcceptor::SetAcceptIP(const ACE_TCHAR* ip)
	{
		_custom_acceptor.SetAcceptIP(ip);
	}

	void ReactorAcceptor::GetInfo(AcceptorInfo& info)
	{
		_custom_acceptor.GetInfo(info);
	}

	void ReactorAcceptor::ReceiveBufferSize( uint32 size )
	{
		_custom_acceptor.ReceiveBufferSize(size);
	}

	void ReactorAcceptor::InitialAcceptCount( uint32 count )
	{
	}
} // namespace HalfNetwork
