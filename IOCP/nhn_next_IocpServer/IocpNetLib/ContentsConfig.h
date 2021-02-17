#pragma once

const int MAX_NAME_LEN = 32;
const int MAX_COMMENT_LEN = 256;

const int LISTEN_PORT = 29990;
const int MAX_CONNECTION = 10000;

const char CONNECT_SERVER_ADDR[] = { "127.0.0.1" };

//const int CONNECT_SERVER_PORT = 9001;

#define GQCS_TIMEOUT	10 //INFINITE

#define MAX_IO_THREAD	4
#define MAX_DB_THREAD	4
#define MAX_WORKER_THREAD	(MAX_IO_THREAD+MAX_DB_THREAD)

enum THREAD_TYPE
{
	THREAD_MAIN,
	THREAD_IO_WORKER,
	THREAD_DB_WORKER
};