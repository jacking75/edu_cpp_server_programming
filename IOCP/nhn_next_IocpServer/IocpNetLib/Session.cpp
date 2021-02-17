#include <WinSock2.h>
#include <Mswsock.h>
#include <stdio.h>

#include "ContentsConfig.h"
#include "Exception.h"
#include "OverlappedIOContext.h"
#include "Session.h"
#include "IocpManager.h"


Session::Session(size_t sendBufSize, size_t recvBufSize) 
: mSendBuffer(sendBufSize), mRecvBuffer(recvBufSize), mConnected(0), mRefCount(0), mSendPendingCount(0)
{
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}



void Session::DisconnectRequest(DisconnectReason dr)
{
	SetDisconnectReason(dr);	
	closesocket(mSocket);
}

void Session::DisconnectCompletion(DisconnectReason dr)
{
	if (0 == InterlockedExchange(&mConnected, 0)) {
		return;
	}
	
	if (mRefCount != 0) {
		return;
	}

	SetDisconnectReason(dr);
	closesocket(mSocket);

	OnDisconnect(); // 이것은 여기서 호출하지 말고 패킷 처리하는 곳에서 하는 것이 좋다.

	OnRelease();
}

bool Session::PostRecv()
{
	if (!IsConnected()) {
		return false;
	}

	if (0 == mRecvBuffer.GetFreeSpaceSize()) {
		return false;
	}

	OverlappedRecvContext* recvContext = new OverlappedRecvContext(this);

	DWORD recvbytes = 0;
	DWORD flags = 0;
	recvContext->mWsaBuf.len = (ULONG)mRecvBuffer.GetFreeSpaceSize();
	recvContext->mWsaBuf.buf = mRecvBuffer.GetBuffer();


	/// start real recv
	if (SOCKET_ERROR == WSARecv(mSocket, &recvContext->mWsaBuf, 1, &recvbytes, &flags, (LPWSAOVERLAPPED)recvContext, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			DeleteIoContext(recvContext);
			printf_s("Session::PostRecv Error : %d\n", GetLastError());
			return false;
		}
	}

	return true;
}


bool Session::PostSend(const char* data, size_t len)
{	
	FastSpinlockGuard criticalSection(mSessionLock);

	if (!IsConnected()) { 
		return false;
	}

	if (mSendBuffer.GetFreeSpaceSize() < len) {
		return false;
	}

	/// flush later...
	//LSendRequestSessionList->push_back(this);
	
	char* destData = mSendBuffer.GetBuffer();

	memcpy(destData, data, len);

	mSendBuffer.Commit(len);

	return true;
}


bool Session::FlushSend()
{
	if (!IsConnected()) {
		return false;
	}
		

	FastSpinlockGuard criticalSection(mSessionLock);

	if (!IsConnected()) // 동기화를 위해 한번 더 조사한다.
	{
		return false;
	}

	/// 보낼 데이터가 없는 경우
	if (0 == mSendBuffer.GetContiguiousBytes())
	{
		/// 보낼 데이터도 없는 경우
		if (0 == mSendPendingCount) {
			return true;
		}
		
		return false;
	}

	/// 이전의 send가 완료 안된 경우
	if (mSendPendingCount > 0) {
		return false;
	}

	
	OverlappedSendContext* sendContext = new OverlappedSendContext(this);

	DWORD sendbytes = 0;
	DWORD flags = 0;
	sendContext->mWsaBuf.len = (ULONG)mSendBuffer.GetContiguiousBytes();
	sendContext->mWsaBuf.buf = mSendBuffer.GetBufferStart();

	/// start async send
	if (SOCKET_ERROR == WSASend(mSocket, &sendContext->mWsaBuf, 1, &sendbytes, flags, (LPWSAOVERLAPPED)sendContext, NULL))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			DeleteIoContext(sendContext);
			printf_s("Session::FlushSend Error : %d\n", GetLastError());

			DisconnectRequest(DR_SENDFLUSH_ERROR);
			return true;
		}

	}

	mSendPendingCount++;

	return mSendPendingCount == 1;
}

void Session::SendCompletion(DWORD transferred)
{
	FastSpinlockGuard criticalSection(mSessionLock);

	if (!IsConnected()){
		return;
	}

	mSendBuffer.Remove(transferred);

	mSendPendingCount--;
}

void Session::OnReceive(size_t len)
{
	//TODO: 개발용도. 실제 사용될 때 동작하면 안 된다. mRecvBuffer.GetBufferStart(), mRecvBuffer.GetContiguiousBytes(), mRecvBuffer.Remove
	EchoBack();
}

void Session::RecvCompletion(DWORD transferred)
{
	mRecvBuffer.Commit(transferred);

	OnReceive(transferred);
}

void Session::EchoBack()
{
	size_t len = mRecvBuffer.GetContiguiousBytes();

	if (len == 0) {
		return;
	}

	if (false == PostSend(mRecvBuffer.GetBufferStart(), len)) {
		return;
	}

	mRecvBuffer.Remove(len);

}

bool Session::SetDisconnectReason(DisconnectReason dr)
{
	if (m_DisconnectReason == DisconnectReason::DR_NONE) {
		m_DisconnectReason = dr;
		return true;
	}

	return false;
}

void Session::AddRef()
{
	CRASH_ASSERT(InterlockedIncrement(&mRefCount) > 0);
}

void Session::ReleaseRef()
{
	long ret = InterlockedDecrement(&mRefCount);
	CRASH_ASSERT(ret >= 0);
}