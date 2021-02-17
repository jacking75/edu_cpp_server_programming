#pragma once

#include "ContentsConfig.h"

class Session;
class IOThread;

struct OverlappedSendContext;
struct OverlappedPreRecvContext;
struct OverlappedRecvContext;
struct OverlappedDisconnectContext;
struct OverlappedAcceptContext;


class IocpManager
{
public:
	IocpManager();
	~IocpManager();

	bool Initialize();
	void Finalize();

	bool StartIoThreads();
	void StartAccept();

	
	HANDLE GetComletionPort()	{ return mCompletionPort; }

	SOCKET* GetListenSocket()  { return &mListenSocket;  }

	static char mAcceptBuf[64];


private:
	static unsigned int WINAPI IoWorkerThread(LPVOID lpParam);


	HANDLE	mCompletionPort;
	SOCKET	mListenSocket;

	IOThread* mIoWorkerThread[MAX_IO_THREAD];
};


extern IocpManager* GIocpManager;

