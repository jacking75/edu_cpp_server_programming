#pragma once


#include "Define.h"
#include "Thread.h"
#include "Synchronized.h"
#include <deque>
#include <map>


// assocuid는 누가 관리할것인가?
//	기존에는 AsynchSocket클래스 계층에서 중앙관리햇엇다.
//		그래서 그 특징은 세부유형에 상관없이 번호가 중앙집중관리되어지는 형태로 되어 있다.
//		즉 AsynchIOService가 AsynchSocket에게 assockid를 물어보는 형태로 관리되어 있다.
//		생성과 삭제시에 글로벌하게 동기화가 일어난다.
//		똑같은 리시버가 받으면 동일한 번호가 생기는 문제점은 asio 객체로 구분할것인가?

class AsynchIOService : public INetworkSender, public Runnable
{
protected:
	INetworkReceiver*		receiver;
	DWORD					conThread;
	std::deque< Thread* >	workers;
	size_t					ioMaxSize;
	HANDLE					iocpHandle;
	
	typedef std::map<ULONG_PTR, AsyncIOSocket*> tASSOCKMAP;
	typedef std::map<ULONG_PTR, AsyncIOSocket*>::iterator tASSOCKMAPITER;
	tASSOCKMAP theASSocks;

	CriticalSectionLockWrapper		entireSynch;
	CriticalSectionLockWrapper		ioFrameSynch;
	CriticalSectionLockWrapper		logSynch;

	std::deque< OverlappedOperation* >	ioFramePools;

	char						name[ 128 ];

private:
	static size_t			cIOMINSIZE;

protected:
	OverlappedOperation* retrieveIOFrame();
	void releaseIOFrame(OverlappedOperation* op);

	void releaseSocket(AsyncIOSocket* socket, DWORD why);


public:
	static size_t GetIOMinSize()
	{
		return cIOMINSIZE;
	}

public:
	// receiver, 
	AsynchIOService(INetworkReceiver* receiver, size_t ioMaxSize = cIOMINSIZE, DWORD conThread=1, char* aname = NULL);
	virtual ~AsynchIOService();

	DWORD start();

	DWORD stop();

protected:
	void dispatchError(DWORD error, DWORD bytesTransfer, LPOVERLAPPED lpoverlapped, ULONG_PTR keyValue);
	void dispatchCompletion(DWORD bytesTransfer, LPOVERLAPPED lpoverlapped, ULONG_PTR keyValue);
	DWORD postingRead(AsyncIOSocket* socket, OverlappedOperation* op);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// implementation for INetworkSender
public:
	//virtual DWORD postingSend(ASSOCKUID socketUniqueId, LARGE_INTEGER* tick, size_t length, char* data);
	virtual DWORD postingSend(AsyncSocketDesc& sockdesc, size_t length, char* data);
	virtual DWORD disconnectSocket(ULONG_PTR uniqueId, LARGE_INTEGER* tick);
	virtual DWORD disconnectSocket(AsyncSocketDesc& sockdesc);
	virtual DWORD connectSocket(INT32 reqeusterID, AsyncIOSocket* prototype, char* ip, u_short port);
	virtual DWORD releaseSocketUniqueId(ULONG_PTR socketUniqueId );
	virtual DWORD registerSocket(SOCKET sockid, AsyncIOSocket* prototype, SOCKADDR_IN& addrin);
	virtual size_t getIOMaxSize() { return ioMaxSize; }

	void logmsg(char * format,...);

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// implementation for Runnable
public:
	void Run(Thread* info);

//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



};