#pragma once
#include "CircularBuffer.h"
#include "OverlappedIOContext.h"


class Session
{
public:
	Session(size_t sendBufSize, size_t recvBufSize);
	virtual ~Session() {}

	virtual void OnReceive(size_t len);
	virtual void OnDisconnect() {}
	virtual void OnRelease() {}


	bool IsConnected() const { return !!mConnected; }

	void DisconnectRequest(DisconnectReason dr) ;

	bool PostRecv() ;

	bool PostSend(const char* data, size_t len);
	bool FlushSend() ;

	void DisconnectCompletion(DisconnectReason dr) ;
	void SendCompletion(DWORD transferred) ;
	void RecvCompletion(DWORD transferred) ;
	
	void	SetSocket(SOCKET sock) { mSocket = sock; }
	SOCKET	GetSocket() const { return mSocket; }

	void AddRef();
	void ReleaseRef();

	void EchoBack();

	bool GetDisconnectReason() { return m_DisconnectReason; }
	bool SetDisconnectReason(DisconnectReason dr);

protected:

	SOCKET			mSocket;

	CircularBuffer	mRecvBuffer;
	CircularBuffer	mSendBuffer;
	FastSpinlock	mSessionLock;
	int				mSendPendingCount;

	volatile long	mRefCount;
	volatile long	mConnected;


	DisconnectReason m_DisconnectReason = DisconnectReason::DR_NONE;

};