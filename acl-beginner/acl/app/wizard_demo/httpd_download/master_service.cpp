#include "stdafx.h"
#include "http_servlet.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_filepath;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "filepath", "file.exe", &var_cfg_filepath },

	{ 0, 0, 0 }
};

int  var_cfg_bool;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "bool", 1, &var_cfg_bool },

	{ 0, 0, 0 }
};

int  var_cfg_int;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "int", 120, &var_cfg_int, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::thread_on_read(acl::socket_stream* conn)
{
	http_servlet* servlet = (http_servlet*) conn->get_ctx();
	if (servlet == NULL)
		logger_fatal("servlet null!");

	return servlet->doRun();
}

bool master_service::thread_on_accept(acl::socket_stream* conn)
{
	logger("connect from %s, fd: %d", conn->get_peer(true),
		conn->sock_handle());
	conn->set_rw_timeout(5);

	acl::session* session = new acl::memcache_session("127.0.0.1");
	http_servlet* servlet = new http_servlet(var_cfg_filepath,
			conn, session);
	conn->set_ctx(servlet);

	return true;
}

bool master_service::thread_on_timeout(acl::socket_stream* conn)
{
	logger("read timeout from %s, fd: %d", conn->get_peer(),
		conn->sock_handle());
	return false;
}

void master_service::thread_on_close(acl::socket_stream* conn)
{
	logger("disconnect from %s, fd: %d", conn->get_peer(),
		conn->sock_handle());

	http_servlet* servlet = (http_servlet*) conn->get_ctx();
	acl::session* session = &servlet->getSession();
	delete session;
	delete servlet;
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}

void master_service::proc_on_init()
{
}

void master_service::proc_on_listen(acl::server_socket& ss)
{
	printf("Listen ok, addr=%s\r\n", ss.get_addr());
}

bool master_service::proc_exit_timer(size_t nclients, size_t nthreads)
{
	if (nclients == 0 || nthreads == 0)
	{
		logger("clients count: %d, threads count: %d",
			(int) nclients, (int) nthreads);
		return true;
	}

	return false;
}

void master_service::proc_on_exit()
{
}
