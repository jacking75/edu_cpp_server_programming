// fiber_server.c


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fiber/lib_fiber.h"

#include "patch.h" // in the samples path

static size_t      __stack_size = 128000;
static const char* __listen_ip = "127.0.0.1";
static int         __listen_port = 9001;

static void fiber_client(ACL_FIBER* fb, void* ctx)
{
	SOCKET* pfd = (SOCKET*)ctx;
	char buf[8192];

	while (1) {
#if defined(_WIN32)
		int ret = acl_fiber_recv(*pfd, buf, sizeof(buf), 0);
#else
		int ret = recv(*pfd, buf, sizeof(buf), 0);
#endif
		if (ret == 0) {
			break;
		}
		else if (ret < 0) {
			if (acl_fiber_last_error() == FIBER_EINTR) {
				continue;
			}
			break;
		}
#if defined(_WIN32)
		if (acl_fiber_send(*pfd, buf, ret, 0) < 0) {
#else
		if (send(*pfd, buf, ret, 0) < 0) {
#endif			
			break;
		}
		}

	socket_close(*pfd);
	free(pfd);
	}

static void fiber_accept(ACL_FIBER * fb, void* ctx)
{
	const char* addr = (const char*)ctx;
	SOCKET lfd = socket_listen(__listen_ip, __listen_port);

	assert(lfd >= 0);

	for (;;) {
		SOCKET* pfd, cfd = socket_accept(lfd);
		if (cfd == INVALID_SOCKET) {
			printf("accept error %s\r\n", acl_fiber_last_serror());
			break;
		}
		pfd = (SOCKET*)malloc(sizeof(SOCKET));
		*pfd = cfd;

		// create and start one fiber to handle the client socket IO
		acl_fiber_create(fiber_client, pfd, __stack_size);
	}

	socket_close(lfd);
	exit(0);
}

// FIBER_EVENT_KERNEL represents the event type on
// Linux(epoll), BSD(kqueue), Mac(kqueue), Windows(iocp)
// FIBER_EVENT_POLL: poll on Linux/BSD/Mac/Windows
// FIBER_EVENT_SELECT: select on Linux/BSD/Mac/Windows
// FIBER_EVENT_WMSG: Win GUI message on Windows
// acl_fiber_create/acl_fiber_schedule_with are in `lib_fiber.h`.
// socket_listen/socket_accept/socket_close are in patch.c of the samples path.

int main(void)
{
	int event_mode = FIBER_EVENT_KERNEL;

#if defined(_WIN32)
	socket_init();
#endif

	// create one fiber to accept connections
	acl_fiber_create(fiber_accept, NULL, __stack_size);

	// start the fiber schedule process
	acl_fiber_schedule_with(event_mode);

#if defined(_WIN32)
	socket_end();
#endif

	return 0;
}