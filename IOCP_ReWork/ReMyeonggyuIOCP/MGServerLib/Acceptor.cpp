#include <winsock2.h>
#include "Acceptor.h"
#include "AsynchIOService.h"
#include <stdio.h>
#include <time.h>

Acceptor::Acceptor(INetworkSender* theAsynchEventManager, AsynchSocket* prototype, const char* address, u_short port) :
m_prototype(prototype), theAsynchEventManager(theAsynchEventManager)
{
	memset(filestr, 0, sizeof(filestr));
	sprintf(filestr, "%05d", port);
	socketValue = socket( AF_INET, SOCK_STREAM, 0 );

	if (socketValue == INVALID_SOCKET) {
		throw new ASIOException(GetLastError(), "ListenSocket dont create");
	}

	SOCKADDR_IN sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);    
	sin.sin_addr.s_addr = inet_addr(address);

	char estr[512]={0,};

    if ( bind( socketValue, (LPSOCKADDR)&sin, sizeof(sin) )== SOCKET_ERROR )
    {        
		DWORD error = GetLastError();		
		CHAR errmsg[512]={0,};  
		if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 
			0,
			error,
			0,
			errmsg, 
			511,
			NULL))
		{
			;
		}

		if( 0 != error )
		{
			closesocket( socketValue );
			
			_snprintf(estr, sizeof(estr), "실패 : bind [%s]", errmsg );

			throw new ASIOException(error, estr);
		}
    }

    if (listen( socketValue, 511 )== SOCKET_ERROR )
    {        
		DWORD error = GetLastError();
		CHAR errmsg[512]={0,};  
		if (!FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 
			0,
			error,
			0,
			errmsg, 
			511,
			NULL))
		{
			;
		}

		if( 0 != error )
		{
			closesocket( socketValue );
			_snprintf(estr, sizeof(estr), "실패 : listen [%s]", errmsg );
			throw new ASIOException(error, estr);
		}
    }

	logmsg(" %s %d accept started\n", address, port);
}

void Acceptor::logmsg(char * format,...)
{
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
	
	_snprintf(szbuff,511 - strlen(szbuff),"%s(%s)->",szdate,sztime);
	
	va_list marker;
	va_start( marker,format);  
	_vsnprintf(szbuff + strlen(szbuff),511 - strlen(szbuff),format,marker);
	va_end( marker );
	
	// *((char *)szbuff + strlen(szbuff)) = '\n';

	//DWORD rtwirte = 0;
	//WriteConsole(g_hOut,szbuff,lstrlen(szbuff),&rtwirte,0);
	
	sprintf(szpath,"./asio/Accept_[%s][%s].txt",filestr, szdate);

	FILE * f = fopen(szpath,"at");
	
	if(f)
	{
		fwrite(szbuff,strlen(szbuff),1,f);		
		fclose(f);
		f = NULL;
	}
}

void Acceptor::Run()
{
	SOCKET remoteClientSocket = INVALID_SOCKET;
	SOCKADDR_IN addr;

	int len = sizeof(addr);

	logmsg("acceptor thread started %d thread\n", ::GetCurrentThreadId());
	
	while(IsStart())
	{		
		Sleep(1);

		remoteClientSocket = accept(socketValue, (sockaddr*)&addr, &len);
		
		if(remoteClientSocket == INVALID_SOCKET)
		{
			//error
			logmsg("accept error %d error\n", ::GetLastError());
			continue;
			
		}

		logmsg(" accepted %d.%d.%d.%d \n" , addr.sin_addr.S_un.S_un_b.s_b1, addr.sin_addr.S_un.S_un_b.s_b2, addr.sin_addr.S_un.S_un_b.s_b3, addr.sin_addr.S_un.S_un_b.s_b4);			

		theAsynchEventManager->registerSocket(remoteClientSocket, m_prototype, addr);		
	
	}
}
