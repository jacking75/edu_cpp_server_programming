#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ProactorAcceptor.h"
#include "HalfNetworkDefine.h"
#include <ace/OS_NS_string.h>


namespace HalfNetwork
{

	////////////////////
	// CustomAcceptor
	////////////////////
	CustomAsynchAcceptor::CustomAsynchAcceptor() 
		: _queueId(0)
		, _receive_buffer_size(0)
		,	_suspendAccept(FALSE)
	{

	}
	ProactorService* CustomAsynchAcceptor::make_handler()
	{
		ProactorService* handler = new ProactorService();
		handler->QueueID(_queueId);
		if (0 != _receive_buffer_size)
			handler->ReceiveBufferSize(_receive_buffer_size);
		return handler;
	}

	int CustomAsynchAcceptor::validate_connection(
								const ACE_Asynch_Accept::Result& result,
								const ACE_INET_Addr& remote,
								const ACE_INET_Addr& local)
	{
		ACE_UNUSED_ARG(result);
		ACE_UNUSED_ARG(remote);
		ACE_UNUSED_ARG(local);
		if (TRUE == _suspendAccept.value())
			return -1;

		return 0;
	}

	void CustomAsynchAcceptor::QueueID(uint8 id)
	{
		_queueId = id;
	}

	void CustomAsynchAcceptor::Suspend()
	{
		_suspendAccept = TRUE;
	}

	void CustomAsynchAcceptor::Resume()
	{
		_suspendAccept = FALSE;
	}

	bool CustomAsynchAcceptor::IsSuspend()
	{
		return (_suspendAccept.value() == TRUE);
	}

	void CustomAsynchAcceptor::ReceiveBufferSize( uint32 size )
	{
		_receive_buffer_size = size;
	}

	///////////////////////
	// ProactorAcceptor
	///////////////////////
	const uint32 InitialAcceptCount = 1024;

	ProactorAcceptor::ProactorAcceptor(uint16 service_port, uint8 queue_id) : 
		_servicePort(service_port), 
		_queueId(queue_id),
		_receiveBufferSize(0),
		_initialAcceptcount(0)
	{
		_acceptIp = new ACE_TCHAR[IP_ADDR_LEN];
		ACE_OS::memset(_acceptIp, 0, IP_ADDR_LEN);
	}

	ProactorAcceptor::~ProactorAcceptor()
	{
		delete [] _acceptIp;
		_acceptIp = NULL;
	}

	bool ProactorAcceptor::Open()
	{
		_acceptor.QueueID(_queueId);
		if (0 != _receiveBufferSize)
			_acceptor.ReceiveBufferSize(_receiveBufferSize);
		if (false == StartListen())
			return false;

		return true;
	}

	void ProactorAcceptor::Close()
	{
		_acceptor.cancel();
	}

	bool ProactorAcceptor::StartListen()
	{
		ACE_INET_Addr listen_addr;
		if (0 == ACE_OS::strlen(_acceptIp))
			listen_addr.set(_servicePort);
		else
			listen_addr.set(_servicePort, _acceptIp);

		if (0 == _initialAcceptcount)
			_initialAcceptcount = HalfNetwork::InitialAcceptCount;

		if (0 != _acceptor.open(listen_addr,
								  0,		// bytes_to_read
								  true,		// pass_addresses
								  ACE_DEFAULT_BACKLOG,
								  true,		// reuse_addr
								  0,		// proactor
								  true,		// validate_new_connection
								  true,		// reissue_accept
								  _initialAcceptcount))    // number_of_initial_accepts
		{
			ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT("acceptor open")), 1);
			return false;
		}

		return true;
	}

	uint16	ProactorAcceptor::ServicePort()
	{
		return _servicePort;
	}

	void ProactorAcceptor::Suspend()
	{
		_acceptor.Suspend();
	}

	void ProactorAcceptor::Resume()
	{
		_acceptor.Resume();

	}

	void ProactorAcceptor::SetAcceptIP(const ACE_TCHAR* ip)
	{
		ACE_OS::strncpy(_acceptIp, ip, IP_ADDR_LEN);
	}

	void ProactorAcceptor::GetInfo(AcceptorInfo& info)
	{
		info.port = _servicePort;
		info.queue_id = _queueId;
		ACE_OS::strcpy(info.ip, _acceptIp);
		info.suspend = _acceptor.IsSuspend();
	}

	void ProactorAcceptor::ReceiveBufferSize( uint32 size )
	{
		_receiveBufferSize = size;
	}

	void ProactorAcceptor::InitialAcceptCount( uint32 count )
	{
		_initialAcceptcount = count;
	}
} // namespace HalfNetwork
