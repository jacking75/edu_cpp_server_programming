#pragma once

//#include <ace/Null_Mutex.h>
//#include <ace/Map_Manager.h>
#include "ThreadPool.h"

class EchoBackChecker;
namespace HalfNetwork 
{
	class CycleChecker;
	class SimpleConfig;
}

class NetworkThread : public HalfNetwork::ThreadPool
{
public:
	NetworkThread(HalfNetwork::SimpleConfig& config);
	virtual ~NetworkThread();

public:
	virtual bool	Begin();
	virtual void	Update();
	virtual void	End();
	void	SetReceiveQueueId(unsigned char id);
	long	GetCurrentConnection();

protected:
	void	_ProcessRecvQueue(unsigned char queue_id);
	void	_OnConnect(unsigned int streamID, const char* ip);
	void	_OnRead(unsigned int streamID, char* buffer, size_t length);

	bool	_DisconnectTest( unsigned int streamID );
	void	_EchoTest( unsigned int streamID, char* buffer, size_t length);
	void	_OnClose(unsigned int streamID);
	
	void	MakeRandomBuffer(char* buffer, unsigned int len);
	void	SendDummyData(unsigned int streamId);

private:
	//ACE_Atomic_Op<ACE_Thread_Mutex, long>	_currentConnection;
	unsigned char		_queueId;
	unsigned int	_sendTerm;
	EchoBackChecker*	_bufferChecker;
	HalfNetwork::CycleChecker*	_disconnectCycle;
	HalfNetwork::CycleChecker*	_debugMsgCycle;
	HalfNetwork::SimpleConfig&	_configObj;
	
};
