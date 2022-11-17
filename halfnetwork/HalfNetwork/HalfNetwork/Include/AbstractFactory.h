#ifndef __abstractfactory_h__
#define __abstractfactory_h__

#pragma once

namespace HalfNetwork
{

	class AbstractAcceptor;
	class AbstractConnector;
	class AbstractEventPool;
	class AbstractServiceAccessor;
	class AbstractTimer;

	const uint32	FACTORY_NAME_SIZE = 32;

	class AbstractFactory
	{
	public:
		AbstractFactory() {}
		virtual ~AbstractFactory() {}

	public:
		virtual	AbstractAcceptor*	CreateAcceptor(uint16 service_port, uint8 queue_id) = 0;
		virtual	AbstractConnector* CreateConnector() = 0;
		virtual	AbstractEventPool* CreateEventPool() = 0;
		virtual	AbstractServiceAccessor*	CreateServiceAccessor(uint8 send_mode) = 0;
		virtual AbstractTimer* CreateTimer() = 0;
		virtual	const ACE_TCHAR* GetFactoryName() = 0;
	};

} // namespace HalfNetwork

#endif // __abstractfactory_h__