#include <iostream>
#include <assert.h>
#include <thread>

#include "lib_acl.h"
#include "acl_cpp/lib_acl.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
	: client_(client)
	{
	}

private:
	~io_callback(void)
	{
		std::cout << "delete io_callback now ..." << std::endl;
	}

public:
	
	bool read_callback(char* data, int len)
	{
		std::cout << "read_callback: " << data << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));

		client_->write(data, len);

		return true;
	}

	bool write_callback(void)
	{
		return true;
	}

	void close_callback(void)
	{
		delete this;
	}

	bool timeout_callback(void)
	{
		std::cout << "Timeout ..." << std::endl;
		return true;
	}

private:
	acl::aio_socket_stream* client_;
};

class io_accept_callback : public acl::aio_accept_callback
{
public:
	io_accept_callback(void) {}
	~io_accept_callback(void)
	{
		printf(">>io_accept_callback over!\n");
	}

	bool accept_callback(acl::aio_socket_stream* client)
	{
		std::cout << "call accept_callback" << std::endl;

		io_callback* callback = new io_callback(client);

		client->add_read_callback(callback);

		client->add_write_callback(callback);

		client->add_close_callback(callback);

		client->add_timeout_callback(callback);

		int count = 0, timeout = 10;
		client->read(count, timeout);

		// thread sleep
		//std::this_thread::sleep_for(std::chrono::seconds(10));

		return true;
	}
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		" -s listen_addr\r\n"
		" -k[use kernel event: epoll/iocp/kqueue/devpool]\n",
		procname);
}

int main(int argc, char* argv[])
{
	bool use_kernel = false;
	acl::string addr("127.0.0.1:9001");
	int  ch;

	while ((ch = getopt(argc, argv, "hks:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'k':
			use_kernel = true;
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();

	// acl::ENGINE_KERNEL 이 선택되면 아래 중 하나가 사용된다
	// win32: iocp, Linux: epoll, FreeBsd: kqueue, Solaris: devpoll
	acl::aio_handle handle(use_kernel ? acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	//수신 대기 비동기 스트림 만들기
	acl::aio_listen_stream* sstream = new acl::aio_listen_stream(&handle);

	if (!sstream->open(addr)) {
		std::cout << "open " << addr.c_str() << " error!" << std::endl;
		sstream->close();
		
		handle.check();

		getchar();
		return 1;
	}

	io_accept_callback callback;
	sstream->add_accept_callback(&callback);
	std::cout << "Listen: " << addr.c_str() << " ok!" << std::endl;

	while (true) {
		//false를 반환하면 계속 진행하지 않으므로 종료해야 한다는 의미입니다.
		if (!handle.check()) {
			std::cout << "aio_server stop now ..." << std::endl;
			break;
		}
	}

	//수신 스트림을 닫고 스트림 개체를 해제합니다
	sstream->close();

	//듣기 스트림을 닫을 수 있는지 확인하려면 여기를 다시 확인해야 합니다.
	handle.check();

	return 0;
}
