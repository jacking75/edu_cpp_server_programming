#pragma once

#include "Session.h"
#include "Player.h"

class ClientSessionManager;


class ClientSession : public Session, public ObjectPool<ClientSession>
{
public:
	ClientSession();
	virtual ~ClientSession();

	void SessionReset();

	bool PostAccept();
	bool AcceptCompletion();
	
	virtual void OnReceive(size_t len) {}
	virtual void OnDisconnect();
	virtual void OnRelease();

public:
	Player			mPlayer;

private:
	
	SOCKADDR_IN		mClientAddr ;

	
	friend class ClientSessionManager;
} ;



