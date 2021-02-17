#pragma once
#include "stdafx.h"
#include <map>

#ifdef __TBB_malloc_proxy
#define ALLOCATE_POOL_MALLOC(type_, size_) static_cast<type_*>(malloc(size_));
#define ALLOCATE_POOL_NEW(result_, type_, ...) new type_(##__VA_ARGS__); 
#define SAFE_POOL_FREE(a_) if (a_ != NULL) { free(a_);\
                        a_ = NULL; }
#define SAFE_POOL_DELETE(a_, type_) if (a_ != NULL) { delete a_;\
						a_ = NULL; }

#else
#define ALLOCATE_POOL_MALLOC(type_, size_) static_cast<type_*>(tc_malloc(size_));
#define ALLOCATE_POOL_NEW(result_, type_, ...) static_cast<type_*>(tc_malloc(sizeof(type_))); \
new(result_) type_(##__VA_ARGS__); 
#define SAFE_POOL_FREE(a_) if (a_ != NULL) tc_free(a_);\
                        a_ = NULL;
#define SAFE_POOL_DELETE(a_, type_) a_->~type_(); \
SAFE_POOL_FREE(a_); a_ = NULL;
#endif

#define SAFE_DELETE(a_) if (a_ != NULL) delete(a_);\
                        a_ = NULL;

#define POOL_SIZE 1000
#define POOL_INTERVAL 100
#define PACKET_SIZE 1460

#define	PORT_NUM			34521
#define	MAX_CONNECTER		5000
#define TRANSFER_THREAD_COUNT 2
#define RECV_THREAD_COUNT     6
#define SEND_THREAD_COUNT     2
#define DB_SESSION_THREAD_COUNT  4
#define ZONE_UPDATE_TICK      50


const __int64 session_cache_time = 1000 * 10;
#define SESSION_CACHE_RELEASE_TIME session_cache_time

const unsigned long default_check_cache_tick = 1000;
#define DEFAULT_CHECK_CACHE_TICK default_check_cache_tick


typedef __int64 SESSION_ID;
#define INVALID_SESSION_ID (SESSION_ID)-1


typedef __int64 USER_ID;
#define INVALID_USER_ID (USER_ID)-1

typedef unsigned __int32 SERIAL_ID;
#define INVALID_SERIAL_ID (SERIAL_ID)0

typedef unsigned short ZONE_ID;
#define INVALID_ZONE_ID (ZONE_ID)-1
#define INIT_ZONE_ID (ZONE_ID)0

typedef unsigned int SECTOR_ID;
#define INVALID_SECTOR_ID (SECTOR_ID)-1;
#define INIT_SECTOR_ID (SECTOR_ID)0;

typedef int     THREAD_REGION;
#define INVALID_REGION_ID (THREAD_REGION)-1
#define INIT_REGION_ID (THREAD_REGION)0

typedef unsigned __int16 MAP_ID;
#define INVALID_MAP_ID (MAP_ID)-1


class CZone;
typedef std::map<ZONE_ID, CZone*> ZONE_MAP;


#ifdef _DEBUG
#define DYNAMIC_CAST(T) dynamic_cast<T>
#else
#define DYNAMIC_CAST(T) static_cast<T>
#endif





