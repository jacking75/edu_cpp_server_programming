#include <winsock2.h>
#include <Mswsock.h>
#include "AsynchIOService.h"
#include <stdio.h>
#include <time.h>

size_t AsynchIOService::cIOMINSIZE = 1024;

void AsynchIOService::logmsg(char * format,...)
{
	//logSynch
	Synchronized es(&logSynch);

	time_t ltime;
	char szpath[256] = {0,};
	char szbuff[512] = {0,};
	char szdate[256] = {0,};
	char sztime[256] = {0,};
	
	tm *today;
	
	time(&ltime);
	
	today = localtime( &ltime );

	strftime(szdate,256,"%Y_%m_%d",today);
	
	_strtime(sztime);
	
	_snprintf(szbuff,511 - strlen(szbuff),"%s(%s)[%d]->",szdate,sztime,GetCurrentThreadId());
	
	va_list marker;
	va_start( marker,format);  
	_vsnprintf(szbuff + strlen(szbuff),511 - strlen(szbuff),format,marker);
	va_end( marker );
	
	// *((char *)szbuff + strlen(szbuff)) = '\n';

	DWORD rtwirte = 0;
	//WriteConsole(g_hOut,szbuff,lstrlen(szbuff),&rtwirte,0);
	
	sprintf(szpath,"./asio/%s_[%s].txt", this->name, szdate);

	FILE * f = fopen(szpath,"at");
	
	if(f)
	{
		fwrite(szbuff,strlen(szbuff),1,f);		
		fclose(f);
		f = NULL;
	}
}

AsynchIOService::AsynchIOService(INetworkReceiver* receiver, size_t ioMaxSize, DWORD conThread, char* aname) : receiver(receiver), ioMaxSize(ioMaxSize), conThread(conThread), workers(), iocpHandle(INVALID_HANDLE_VALUE), theASSocks(), entireSynch(), ioFrameSynch(), ioFramePools(), logSynch()
{
	if(aname)
	{
		_snprintf(this->name, sizeof(this->name), "%s\0", aname);
	}
	else
	{
		_snprintf(this->name, sizeof(this->name), "ASIO_%d\0", GetCurrentThreadId());
	}
	if(NULL == receiver)
		throw new ASIOException(WSAEINVAL, "receiver null");

	if(1 > conThread)
		throw new ASIOException(WSAEINVAL, "conThread must over than 0");

	if(cIOMINSIZE > ioMaxSize)
		throw new ASIOException(WSAEMSGSIZE, "ioMaxSize too small than cIOMINSIZE");

	iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, conThread);

	if(INVALID_HANDLE_VALUE == iocpHandle)
		throw new ASIOException(GetLastError(), "iocp creation fail");

	for( DWORD index  = 0; index < conThread ; ++index )
	{
		Thread* nT = new Thread(static_cast<Runnable*>(this));
		workers.push_back(nT);
	}
}

AsynchIOService::~AsynchIOService()
{
	for( DWORD index = 0 ; index < conThread ; ++index )
	{
		delete workers[index];
	}
	workers.clear();

	if(INVALID_HANDLE_VALUE != iocpHandle)
		CloseHandle(iocpHandle);
}


DWORD AsynchIOService::start()
{
	for( DWORD index = 0 ; index < conThread ; ++index )
	{
		workers[index]->Start();
	}

	return 0;
}

DWORD AsynchIOService::stop()
{
	for( DWORD index = 0 ; index < conThread ; ++index )
	{
		//workers[index]->terminated();
		workers[index]->Stop();
	}
	return 0;
}

OverlappedOperation* AsynchIOService::retrieveIOFrame()
{
	Synchronized es(&ioFrameSynch);

	OverlappedOperation* p = NULL;

	if(ioFramePools.size())
	{
		p = ioFramePools.front();
		ioFramePools.pop_front();
	}
	
	if(p) return p;
	return new OverlappedOperation(ioMaxSize);
}

void AsynchIOService::releaseIOFrame(OverlappedOperation* op)
{
	Synchronized es(&ioFrameSynch);

	if(op) ioFramePools.push_back(op);
}

void AsynchIOService::Run(Thread* info)
{
	logmsg("%d asio thread startd\n", GetCurrentThreadId());
	// 아래의 지역변수들은 읽기전용이 아니고,
	//	GQCS호출후 값이 설정되기 때문에, volatile을 명시할 필요가 없다?
	DWORD bytesTransfer;
	DWORD error;
	BOOL retVal;
	LPOVERLAPPED lpoverlapped;
	ULONG_PTR keyValue;
	DWORD waittime = 500;
	
	//info->isStart()가 반환하는 값은 반드시 volatile이어야 한다.
	while(info->IsStart())
	{
		keyValue = 0;
		lpoverlapped = NULL;

		// infinit로 웨이트 시키지 말자. 쓰레드 종료처리가 모호해진다.
		retVal = GetQueuedCompletionStatus(iocpHandle, &bytesTransfer, &keyValue, &lpoverlapped, waittime);

		error = GetLastError();

		// check timeout
		if(0 == retVal && NULL == lpoverlapped && WAIT_TIMEOUT == error) continue;

		if(NULL == lpoverlapped && keyValue)
		{
			dispatchError(error, bytesTransfer, lpoverlapped, keyValue);
		}

		if(retVal && lpoverlapped && keyValue)
		{
			dispatchCompletion(bytesTransfer, lpoverlapped, keyValue);
		}
		else
		{
			dispatchError(error, bytesTransfer, lpoverlapped, keyValue);
		}
	}

	// temp code cleare ASIO objects

}


void AsynchIOService::dispatchError(DWORD error, DWORD bytesTransfer, LPOVERLAPPED lpoverlapped, ULONG_PTR keyValue)
{
	// dont enter for reason, wait_timeout...
	OverlappedOperation* op = NULL;
	if(lpoverlapped) op = (OverlappedOperation*)lpoverlapped;
	AsynchSocket* socket = NULL;

	if(keyValue)
	{
		tASSOCKMAPITER iter = theASSocks.find(keyValue);
		if(iter != theASSocks.end()) socket = (*iter).second;

		if(NULL == socket)
		{
			// temp code , already disappeared... oops critical error
			if(op) releaseIOFrame(op);
			return;
		}
	}

	if(op)
	{
		if( OverlappedOperation::eOpType_Connect == op->optype )
		{
			ASSOCKDESCEX desc;
			socket->getASSOCKDESCEX(desc);
			receiver->notifyConnectingResult(op->requesterID, desc, error);
		}

		releaseSocket(socket,3);
		releaseIOFrame(op);
	}
	else
	{
		// temp code , keyvalue는 존재하는데,op가 존재하지 않는 경우는???
	}
}

void AsynchIOService::dispatchCompletion(DWORD bytesTransfer, LPOVERLAPPED lpoverlapped, ULONG_PTR keyValue)
{
	OverlappedOperation* op = (OverlappedOperation*)lpoverlapped;
	AsynchSocket* socket = NULL;
	
	// find from container
	{
		Synchronized bs(&entireSynch);
		tASSOCKMAPITER iter = theASSocks.find(keyValue);
		if(iter != theASSocks.end()) socket = (*iter).second;

		if(NULL == socket)
		{
			// temp code , already disappeared... oops critical error
			releaseIOFrame(op);
			return;
		}
	}
	
	switch(op->optype)
	{
	case OverlappedOperation::eOpType_Connect:
		{
			// ConnectEx를 통해 수행되어진 소켓은 ConnectEx를 실제로 완료하고 난후 해당 처리를 수행해야만
			// 다른 버클리 소켓 함수의 서비스를 받을 수 있다.
			int err = setsockopt(socket->getSOCKET(), SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0 );
			ASSOCKDESCEX desc;
			socket->getASSOCKDESCEX(desc);

			if(0 != err)
			{
				int le = GetLastError();
				receiver->notifyConnectingResult(op->requesterID, desc, le);
				releaseIOFrame(op);
				releaseSocket(socket, le);
			}
			else
			{
				receiver->notifyConnectingResult(op->requesterID, desc, 0);
				postingRead(socket, op);
			}
			
		}
		break;

	case OverlappedOperation::eOpType_Read:
		{		
			
			if(bytesTransfer)
			{				
				int rt = -1;
				try
				{
					rt = socket->handleCompletionOfReceive(receiver, this, bytesTransfer, op->datas);				
				}
				catch(...)
				{
					logmsg("Catch Exception along handleCompletionOfReceive\n");
					throw;
				}
				if(rt)
				{
					
					//종료시킨다...
					socket->disconnect();
					releaseSocket(socket, 0);
					releaseIOFrame(op);
					
				}
				else
					postingRead(socket, op);
			}
			else
			{
				//종료시킨다...
				socket->disconnect();
				releaseSocket(socket, 0);
				releaseIOFrame(op);
			}
		}
		break;

	case OverlappedOperation::eOpType_Write:
		{
			releaseSocket(socket, 0);
			releaseIOFrame(op);
		}
		break;

	default:
		{
			// temp code, it's critical error
		}
		break;
	}
}

DWORD AsynchIOService::postingRead(AsynchSocket* socket, OverlappedOperation* op)
{
	
	if(NULL == socket || NULL == op)
	{
		if(socket) releaseSocket(socket, 5);
		return WSAEINVAL;
	}

	// we don't need synch in this method
	//	socket has synchronized at before context, and that has refcounter
	DWORD bytes=0;
	DWORD flag=0;
	op->initHeader();
	op->length = op->maxLength;
	op->optype = OverlappedOperation::eOpType_Read;
	WSABUF buf;
	buf.buf = op->datas;
	buf.len = (ULONG)op->length;

	int nRetVal = WSARecv(socket->getSOCKET(), &buf, 1, &bytes, &flag, (LPWSAOVERLAPPED)op, 0);
	int error = WSAGetLastError();

	if ( nRetVal == SOCKET_ERROR && error != WSA_IO_PENDING) 
	{	
		releaseIOFrame(op);
		releaseSocket(socket, 4);
		return error;
	}	
	

	return 0;
}


//DWORD AsynchIOService::postingSend(ASSOCKUID socketUniqueId, LARGE_INTEGER* tick, size_t length, char* data)
//{
//}

DWORD AsynchIOService::postingSend(ASSOCKDESC& sockdesc, size_t length, char* data)
{
	
	if( 0>= length || ioMaxSize < length ) return WSAEMSGSIZE;

	//synch along entire method
	//	왜 이 함수를 전부 블럭시켜야 하는가?
	//	find시에만 블럭시키면 안될까?
	//		다른 쓰레드에서 동일한 socketUniquedId에 대해 이 함수를 또 호출하면 동기화가 깨질것이다.
	//		그외는??
	//			read에 의해 지워지는 경우는 sending에 의해 카운터가 증가되지 않았을 경우엔
	//			블럭이 안되어 있으면 read에서 지워서 크러쉬 발생
	//				find하면서 무조건 카운터를 증가시킨다. 그러면 이 문제는 해결
	//				하지만 이 함수 자체가 동시에 처리되는 동기화는 어떻게 해결?
	//				이런 부분은 postingSend를 수행하는 로직계층에서 제어를 해야한다.
	//					즉 루즈한 동기화를 할것인가? 강력한 동기화를 할것인가?
	//	일단은 전체를 동기화시키는걸로...
	Synchronized es(&entireSynch);

	//find assocket with socketUniquedId
	AsynchSocket* socket = 0;	
	tASSOCKMAPITER iter = theASSocks.find(sockdesc.assockUid);
	if(iter == theASSocks.end()) return WSAEINVAL;
	socket = (*iter).second;
	if(socket == 0)
	{
		return 1111111;
	}

	ASSOCKDESCEX desc;
	socket->getASSOCKDESCEX(desc);	

	if(desc.tick.QuadPart != sockdesc.tick.QuadPart) return WSAEINVAL;

	if(0 != socket->getClosed()) return WSAEINVAL;
	//_ASSERTE( _CrtCheckMemory() );
	socket->enterIO();
	OverlappedOperation* op = retrieveIOFrame();
	op->initHeader();
	//_ASSERTE( _CrtCheckMemory() );
	// 실제 어플리케이션에 맞게 헤더를 붙인다던지 crypto를 하던지 필요하겠죠?
	size_t wsize = socket->makePacket(op->datas, op->maxLength, data, length);

	if(0 == wsize)
	{
		releaseIOFrame(op);
		releaseSocket(socket, 7);
		return 2222222;
	}

	op->length = wsize;

	WSABUF buf;
	DWORD bytes=0;
	DWORD flag=0;
	//_ASSERTE( _CrtCheckMemory() );
	buf.buf = op->datas;
	buf.len = (ULONG)op->length;

	int nRetVal = WSASend(socket->getSOCKET(), &buf, 1, &bytes, flag, (LPWSAOVERLAPPED)op, 0);

	int error = WSAGetLastError();			

	if ( nRetVal == SOCKET_ERROR) 
	{
		if(error != WSA_IO_PENDING && error != ERROR_SUCCESS)
		{				
			releaseIOFrame(op);
			releaseSocket(socket, error);
			return error;
		}
	}
	//_ASSERTE( _CrtCheckMemory() );

	return 0;
}

void AsynchIOService::releaseSocket(AsynchSocket* socket, DWORD why)
{
	Synchronized es(&entireSynch);

	ASSOCKDESCEX closedDesc;	

	// 레퍼런스카운터를 1개 줄인다.
	LONG refCount = socket->exitIO();
	
	// 레퍼런스카운터가 0이면 더이상 사용하지 않으므로 삭제하기 위한 준비를 한다.
	if(0 == refCount)
	{
		socket->getASSOCKDESCEX(closedDesc);
		theASSocks.erase(closedDesc.assockUid);		

		// 레퍼런스카운터가 0인 경우 이 루틴이 수행된다.
		//notify this socket info to problem domain
		//	temp code 데드락을 없앨려면 동기화를 빠져나오고 호출하게 한다?
		receiver->notifyReleaseSocket(closedDesc);

		//
//		printf("%lu sockets\n", theASSocks.size());

		//해당소켓을 지운다.
		delete socket;	
	}	
	
}

DWORD AsynchIOService::disconnectSocket(ULONG_PTR uniqueId, LARGE_INTEGER* tick)
{
	if(NULL == tick) return WSAEBADF;

	ASSOCKDESC desc;
	desc.assockUid = uniqueId;
	desc.tick = *tick;	
	return disconnectSocket(desc);
}

DWORD AsynchIOService::disconnectSocket(ASSOCKDESC& sockdesc)
{		
	Synchronized es(&entireSynch);

	AsynchSocket* as = 0;	
	tASSOCKMAPITER iter = theASSocks.find(sockdesc.assockUid);
	if(iter == theASSocks.end()) return WSAEINVAL;
	as = (*iter).second;

	if(as == 0)
	{
		return WSAEINVAL;
	}

	ASSOCKDESCEX desc;
	as->getASSOCKDESCEX(desc);	

	if(desc.tick.QuadPart == sockdesc.tick.QuadPart)
		as->disconnect();
	else
	{
		return WSAEINVAL;
	}			

	return 0;
}

DWORD AsynchIOService::connectSocket(INT32 reqeusterID, AsynchSocket* prototype, char* ip, u_short port)
{
	//synch along entire method
	Synchronized es(&entireSynch);

	// clone prototype? or already cloned prototype instance?
	SOCKADDR_IN sockAddr;
	short spo = port;
	memset(&sockAddr,0,sizeof(sockAddr));
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(ip);
	sockAddr.sin_port = htons(spo);

	SOCKET clientsock = socket( AF_INET, SOCK_STREAM, 0 );

	if( INVALID_SOCKET == clientsock )
	{
		return GetLastError();
	}

	LPFN_CONNECTEX ConnectEx=0;
	GUID guid = WSAID_CONNECTEX;
	DWORD ctls = 0;
	int nRet = WSAIoctl( clientsock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(GUID), &ConnectEx, sizeof(LPFN_CONNECTEX), &ctls, 0, 0 );

	if( 0 != nRet )
	{
		return WSAGetLastError();
	}

	// 소켓관리객체를 생성
	AsynchSocket* socket = static_cast<AsynchSocket*>(prototype->clone());	
	socket->setSOCKET(clientsock);
	socket->setSender(this);

	// Is already contains at theASSocks?
	tASSOCKMAPITER iter = theASSocks.find(socket->getASSOCKUID());
	if(iter != theASSocks.end())
	{
		// temp code delete as
		return WSAEBADF;
	}

	// iocp에 등록
	HANDLE hret = 0;
	DWORD error = 0;
	hret = CreateIoCompletionPort((HANDLE)clientsock, iocpHandle, socket->getASSOCKUID(), 0);
	if(hret == NULL)
	{
		// temp code delete as
		return GetLastError();
	}

	theASSocks[socket->getASSOCKUID()] = socket;

	// io카운터 증가 시키고
	socket->enterIO();	

	OverlappedOperation* op = retrieveIOFrame();
	op->optype = OverlappedOperation::eOpType_Connect;
	op->requesterID = reqeusterID;

	//DWORD sent = 0;
	int namelen = (int)sizeof(sockAddr);
	SOCKADDR_IN localAddr;
	memset(&localAddr, 0, sizeof(localAddr)); 
	localAddr.sin_family = AF_INET;

	nRet = bind(clientsock, (sockaddr*)&localAddr, sizeof(localAddr));

	if( 0 != nRet )
	{
		int error = GetLastError();
		releaseIOFrame(op);
		releaseSocket(socket, error);
		return error;
	}

	BOOL rr = ConnectEx( clientsock, (sockaddr*)&sockAddr, namelen, 0, 0, 0, (LPOVERLAPPED)&op->overlapped );

	if( FALSE == rr )
	{
		error = WSAGetLastError();

		if( ERROR_IO_PENDING != error )
		{
			releaseIOFrame(op);
			releaseSocket(socket, error);
		}
	}

	return 0;
}

// id값은 asynchsocket 추상클래스 영역에서 글로벌하게 관리한다. 상속받은 측에서 따로 할일은 없고, 단지 이 함수만 호출되면 끝~
//	복수의 asynchioservice에 대해서도 글로벌하게 관리됨.
DWORD AsynchIOService::releaseSocketUniqueId(ULONG_PTR socketUniqueId )
{
	AsynchSocket::releaseASSOCKUID(socketUniqueId);
	return 0;
}

DWORD AsynchIOService::registerSocket(SOCKET sockid, AsynchSocket* prototype, SOCKADDR_IN& addrin)
{
	volatile bool bSuccess = false;
	ASSOCKDESCEX desc;
	{
		//synch along entire method
		Synchronized es(&entireSynch);

		// clone prototype? or already cloned prototype instance?
		if (INVALID_SOCKET == sockid) {
			return WSAENOTSOCK;
		}

		if (NULL == prototype) {
			return WSAEACCES;
		}

		//logmsg("socket clone\n");

		AsynchSocket* as = static_cast<AsynchSocket*>(prototype->clone());
		if (NULL == as) {
			return WSAENOBUFS;
		}

		// Is already contains at theASSocks?
		tASSOCKMAPITER iter = theASSocks.find(as->getASSOCKUID());
		if(iter != theASSocks.end())
		{
			// temp code delete as
			return WSAEBADF;
		}

		as->setSOCKET(sockid);
		as->setSender(this);

		// register sockid to iocphandle
		HANDLE hret = CreateIoCompletionPort((HANDLE)sockid, iocpHandle, as->getASSOCKUID(), 0);
		if(hret == NULL)
		{
			//DWORD error = GetLastError();
			
			// temp code delete as
			//socket->disconnect();		
			//delete socket;
			return WSAEBADF;
		}

		// add prototype instance into theASSocks
		theASSocks[as->getASSOCKUID()] = as;

//		printf("%lu sockets[add]\n", theASSocks.size());

		//if(true==bSuccess)
		{
			//logmsg("registersocket 1\n");
			as->getASSOCKDESCEX(desc);
			// assockdescex must volatile local variable
			receiver->notifyRegisterSocket(desc, addrin);
		}


		// posting read [by reference counter mechanism]
		//	increase ref counter, and call postRead
		//	why? ref counter is read(1), write(1)
		auto ref = as->enterIO();

		//logmsg("first read\n");

		OverlappedOperation* oop = retrieveIOFrame();
		// if postingRead fail, already exitIO in postingRead
		if( 0 == postingRead(as, oop ) )
		{
			bSuccess = true;			
			//desc.psender = this;
			as->handleConnected();
		}
		else
		{
			// oops!!! temp code
		}
		//synch end...
	}

	

	return 0;
}


