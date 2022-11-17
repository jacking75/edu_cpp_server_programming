#include <string>
#include <ace/Singleton.h>
#include "HalfNetworkType.h"
#include "ProactorFactory.h"
#include "ProactorAcceptor.h"
#include "ProactorConnector.h"
#include "ProactorPool.h"
#include "ProactorServiceAccessor.h"
#include "ProactorTimer.h"

namespace HalfNetwork
{

	ProactorFactory::ProactorFactory()
	{
		ACE_OS::strcpy(m_factoryName, ACE_TEXT("Proactor"));
	}

	AbstractAcceptor* ProactorFactory::CreateAcceptor(uint16 service_port, uint8 queue_id)
	{
		return new ProactorAcceptor(service_port, queue_id);
	}

	AbstractConnector* ProactorFactory::CreateConnector()
	{
		return new ProactorConnector();
	}

	AbstractEventPool* ProactorFactory::CreateEventPool()
	{
		return new ProactorPool();
	}

	AbstractServiceAccessor* ProactorFactory::CreateServiceAccessor(uint8 send_mode)
	{
		return new ProactorServiceAccessor(send_mode);
	}

	AbstractTimer* ProactorFactory::CreateTimer()
	{
		return new ProactorTimer();
	}

	const ACE_TCHAR* ProactorFactory::GetFactoryName()
	{
		return m_factoryName;
	}

} // namespace HalfNetwork