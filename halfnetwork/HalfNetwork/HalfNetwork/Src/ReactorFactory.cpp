
#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"

#include "ReactorFactory.h"
#include "ReactorAcceptor.h"
#include "ReactorConnector.h"
#include "ReactorPool.h"
#include "ReactorServiceAccessor.h"
#include "ReactorTimer.h"

namespace HalfNetwork
{

	ReactorFactory::ReactorFactory()
	{
		ACE_OS::strcpy(m_factoryName, ACE_TEXT("Reactor"));
	}

	AbstractAcceptor* ReactorFactory::CreateAcceptor(uint16 service_port, uint8 queue_id)
	{
		return new ReactorAcceptor(service_port, queue_id);
	}

	AbstractConnector* ReactorFactory::CreateConnector()
	{
		return new ReactorConnector();
	}

	AbstractEventPool* ReactorFactory::CreateEventPool()
	{
		return new ReactorPool();
	}

	AbstractServiceAccessor* ReactorFactory::CreateServiceAccessor(uint8 send_mode)
	{
		return new ReactorServiceAccessor(send_mode);
	}

	AbstractTimer* ReactorFactory::CreateTimer()
	{
		return new ReactorTimer();
	}

	const ACE_TCHAR* ReactorFactory::GetFactoryName()
	{
		return m_factoryName;
	}

} // namespace HalfNetwork