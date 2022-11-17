#ifndef __HalfNetworkdefine_h__
#define __HalfNetworkdefine_h__

#pragma once

namespace HalfNetwork
{
	const	int32	IP_ADDR_LEN			=	17;
	const int UsecAdjustValue = 1000;
	
	enum ESendMode
	{
		eSM_Interval,			// using timer(default)
		eSM_Direct,				// direct send
	};

	enum ESendState
	{
		eSS_Processing,			// Send operation is incomplete
		eSS_Complete,			// Send operation is complete
	};

	enum ECloseFlag
	{
		eCF_None,				// 
		eCF_Passive,		// normal close
		eCF_Active,			// 
		eCF_Receive,		// close only receive stream
	};

	struct AcceptorInfo
	{
		ACE_TCHAR	ip[IP_ADDR_LEN];
		uint16	port;
		uint8	queue_id;
		bool	suspend;
	};

	class SystemConfig
	{
	public:
		SystemConfig& operator=(const SystemConfig& other)
		{
			Receive_Buffer_Len = other.Receive_Buffer_Len;
			Worker_Thread_Count = other.Worker_Thread_Count;
			Interval_Send_Term = other.Interval_Send_Term;
			Send_Mode = other.Send_Mode;
			ZombieConnectionTerm = other.ZombieConnectionTerm;
			return *this;
		}

	public:
		uint32 Receive_Buffer_Len;
		uint8	Worker_Thread_Count;
		uint32 Interval_Send_Term;
		ESendMode Send_Mode;
		uint32			ZombieConnectionTerm;
	};

	typedef ACE_Singleton<SystemConfig, ACE_Thread_Mutex> SystemConfigSingleton;
	#define SystemConfigInst SystemConfigSingleton::instance()

	#ifdef  HALF_DEBUG_MSG
		#define	HALF_DEBUG(MSG,...) \
			ACE_DEBUG ((LM_DEBUG, MSG, __VA_ARGS__));
	#else
		#define	HALF_DEBUG(MSG,...) do {} while(0)
	#endif
}

#endif // __HalfNetworkdefine_h__