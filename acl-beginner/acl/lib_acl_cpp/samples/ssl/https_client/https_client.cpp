#include "stdafx.h"
#include <iostream>
#include "util.h"
#include "https_client.h"

#define	DEBUG	100

https_client::https_client(const char* server_addr, const char* domain,
	bool keep_alive, int count, int length)
: server_addr_(server_addr)
, domain_(domain)
, keep_alive_(keep_alive)
, accept_gzip_(true)
, count_(count)
, length_(length)
, show_body_(false)
, ssl_conf_(NULL)
{
	(void) length_;
}

https_client::~https_client()
{
}

void https_client::set_show_body(bool yes)
{
	show_body_ = yes;
}

void https_client::set_ssl_conf(acl::sslbase_conf *conf)
{
	ssl_conf_ = conf;
}

void https_client::accept_gzip(bool yes)
{
	accept_gzip_ = yes;
}

bool https_client::connect_server(acl::http_client& client)
{
	logger_debug(DEBUG, 1, "begin connect server");

	if (client.open(server_addr_.c_str(), 60, 60, true) == false)
	{
		logger_error("failed to connect server %s", server_addr_.c_str());
		return false;
	}
	else
		logger_debug(DEBUG, 1, "connect server ok");

	if (ssl_conf_)
	{
		logger("begin open ssl");

		acl::sslbase_io* ssl = ssl_conf_->create(false);
		ssl->set_sni_host(domain_);
		if (client.get_stream().setup_hook(ssl) == ssl)
		{
			logger_error("open ssl client error");
			ssl->destroy();
			return false;
		}
		else
			logger("open ssl ok");
	}

	return true;
}

int https_client::http_request(int count)
{
	acl::http_client* client = new acl::http_client;

	if (connect_server(*client) == false)
	{
		delete client;
		return 0;
	}

	acl::http_header header;
	header.set_url("/")
		.accept_gzip(accept_gzip_)
		.add_entry("Accept-Languge", "zh-cn,en;q=0.5")
		.set_host(domain_.c_str())
		.set_keep_alive(keep_alive_);

	char  buf[8192];
	int   i = 0;

	for (; i < count; i++)
	{
		if (client->write_head(header) == false)
		{
			logger_error("write header error");
			delete client;
			return i;
		}
		if (client->read_head() == false)
		{
			logger_error("read header error");
			delete client;
			return i;
		}

		while (true)
		{
			int ret = client->read_body(buf, sizeof(buf) - 1);
			if (ret < 0)
			{
				logger_error("read body error");
				delete client;
				return i;
			}
			else if (ret == 0)
				break;
			if (show_body_)
			{
				buf[ret] = 0;
				printf("[%s]\r\n", buf);
				fflush(stdout);
			}
		}

		if (!keep_alive_)
		{
			delete client;
			client = new acl::http_client;
			if (connect_server(*client) == false)
			{
				delete client;
				return i;
			}
		}
		else
			client->reset();

		if (i % 1000 == 0)
		{
			acl::string tmp;
			tmp.format("total: %d, curr: %d", count, i);
			ACL_METER_TIME(tmp.c_str());
		}

		printf("--------------------------------------------\r\n");
	}

	delete client;

	return i;
}

void* https_client::run()
{
	struct timeval begin;
	gettimeofday(&begin, NULL);

	int n = http_request(count_);
	if (n != count_)
		printf(">>>>SOME TASK NOT COMPLETE!<<<<<\r\n");

	struct timeval end;
	gettimeofday(&end, NULL);
	double spent = util::stamp_sub(&end, &begin);
	printf("total: %d, spent: %.2f, speed: %.2f\r\n",
		count_, spent, (n * 1000) / (spent > 1 ? spent : 1));

	return NULL;
}
