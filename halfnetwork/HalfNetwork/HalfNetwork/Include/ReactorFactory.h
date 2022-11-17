#ifndef __reactorfactory_h__
#define __reactorfactory_h__

#pragma once

#include "AbstractFactory.h"

namespace HalfNetwork
{

	class ReactorFactory : public AbstractFactory
	{
	public:
		ReactorFactory();
		virtual ~ReactorFactory() {}

	public:
		virtual AbstractAcceptor*	CreateAcceptor(uint16 service_port, uint8 queue_id);
		virtual AbstractConnector* CreateConnector();
		virtual AbstractEventPool* CreateEventPool();
		virtual AbstractServiceAccessor*	CreateServiceAccessor(uint8 send_mode);
		virtual AbstractTimer* CreateTimer();
		virtual const ACE_TCHAR* GetFactoryName();

	private:
		ACE_TCHAR						m_factoryName[FACTORY_NAME_SIZE];
	};

} // namespace HalfNetwork

#endif // __reactorfactory_h__