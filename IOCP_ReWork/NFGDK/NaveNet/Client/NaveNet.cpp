#include "NaveNet.h"

namespace NaveClientNetLib {

	int SockRemotePort( SOCKET Sock ) 
	{
		sockaddr_in	servaddr;
		int len;

		len = sizeof( servaddr );
		if ( getpeername( Sock, (struct sockaddr *)&servaddr, (int*)&len ) == -1 )
			return -1;

		return ntohs( servaddr.sin_port);
	}

	int SockRemoteIP( SOCKET Sock, unsigned char ip[4] ) 
	{
		sockaddr_in addr;
		int len;
		unsigned char *a;

		len = sizeof( addr );
		if( -1 == getpeername( Sock, (struct sockaddr *)&addr, (int*)&len ) )
			return -1;

		a = (unsigned char *)&(addr.sin_addr);
		ip[0] = a[0];
		ip[1] = a[1];
		ip[2] = a[2];
		ip[3] = a[3];
		return 0;
	}


	bool GetHostIPByDomain(IN_ADDR &Addr, const char *szDomain)
	{
		PHOSTENT pHost = gethostbyname(szDomain);
		if(NULL == pHost)
		{
			return false;
		}

		memcpy(&(Addr.S_un.S_addr), pHost->h_addr_list[0], sizeof(IN_ADDR));
		return true;
	}


}