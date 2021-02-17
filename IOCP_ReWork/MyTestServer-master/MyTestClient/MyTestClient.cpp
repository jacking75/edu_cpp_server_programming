#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>

#include "sample.pb.h"
#include "PacketHeder.h"
#include "Define.h"
#include "Connector.h"
#include "MessageContext.h"
#ifdef __TBB_malloc_proxy
#include "tbb/scalable_allocator.h"
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

using namespace google;


int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)   
		return -1;

#ifdef __TBB_malloc_proxy
	scalable_allocation_mode(TBBMALLOC_SET_SOFT_HEAP_LIMIT, 1024);
#endif

	printf("input connection count : ");
	int count;
	scanf_s("%d", &count);

	printf("RTT count : ");
	scanf_s("%d", &CMessageContext::RTTCount);

	CConnector* connectorArray = new CConnector[count];
	CMessageContext* contextArray = new CMessageContext[count];

	CConnector::_StInit init;
	init.strConnectionAddr = "127.0.0.1";
	init.nPort = 34521;

	for (int i = 0; i < count; ++i)
	{
		char buf[10];
		_itoa_s(i + 1, buf, 10);

		contextArray[i].SetName(buf);
		connectorArray[i].Init(&contextArray[i]);
		connectorArray[i].Start(init);
	}

	while (true)
	{
		::getchar();

		printf("%d RTT test... \n", CMessageContext::RTTCount);

		for (int i = 0; i < CMessageContext::RTTCount; ++i)
		{
			for (int j = 0; j < count; ++j)
			{
				packets::LoginReq loginReq;
				loginReq.set_id(contextArray[j].GetName());
				connectorArray[j].GetSender()->Push(packetdef::LoginReq, &loginReq);
			}
		}
	};

	delete[] connectorArray;
	delete[] contextArray;

	WSACleanup();
    return 0;
}

