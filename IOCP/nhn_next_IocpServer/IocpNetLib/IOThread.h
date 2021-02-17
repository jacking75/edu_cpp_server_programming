#pragma once


class IOThread
{
public:
	IOThread(HANDLE hThread, HANDLE hCompletionPort);
	~IOThread();

	DWORD Run();

	void DoIocpJob();
	void DoSendJob();

	HANDLE GetHandle() { return mThreadHandle;  }
	
	void SetManagedSendIOClientSessionIndex(const int maxClientSessionCount, const int thisThreadIndex, const int maxThreadCount);

private:

	HANDLE mThreadHandle = INVALID_HANDLE_VALUE;
	HANDLE mCompletionPort = INVALID_HANDLE_VALUE;

	int m_SendIOClientSessionBeginIndex = 0;
	int m_SendIOClientSessionEndIndex = 0;
};

