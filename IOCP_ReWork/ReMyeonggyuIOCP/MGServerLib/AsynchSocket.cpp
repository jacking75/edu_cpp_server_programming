#include "Asios.h"

// 여러 AsynchIOService가 아래의 객체를 사용한다면
//	assockuidGen 자체가 synchronization을 만족해야 한다.
ULONG_PTRGenerator			AsynchSocket::assockuidGen;
CriticalSectionLockWrapper	AsynchSocket::assockuidGenLock;

AsynchSocket* AsynchSocket::sAsynchSocketPrototype = NULL;

bool AsynchSocket::disconnect()
{
	if(INVALID_SOCKET == sockID) return false;

	// iocp에 요청한 모든 작업을 취소시킨다.
	//CancelIo((HANDLE)getHashCode());
	closesocket(sockID);

	// 닫혔음을 설정한다.
	closed = 1;	

	// 이값을 설정하지 않으면, 레퍼런스카운터에 문제가 생긴다.
	// 닫히기 직전의 소켓에 작업을 요청하면 iocp로 부터 해당 요청 작업에 대한 완료가 일어나지 않는 경우가 종종 있다.
	sockID = INVALID_SOCKET;

	return true;
}

size_t AsynchSocket::makePacket(char* dest, size_t destmaxsize, char* src, size_t srcsize)
{
	// how validation destmaxsize???

	if(0 >= srcsize || destmaxsize < srcsize) return (size_t)0;

	CopyMemory(dest, src, srcsize);

	return srcsize;
}

int AsynchSocket::handleCompletionOfReceive(INetworkReceiver* receiver, INetworkSender* sender, DWORD bytesTransfer, char* datas)
{
	ASSOCKDESCEX desc;
	getASSOCKDESCEX(desc);
	//desc.psender
	receiver->notifyMessage(desc, (size_t)bytesTransfer, datas);
	return 0;
}