#include "WrappingServer.h"
#include "Define.h"
#include "PacketHeder.h"
#include "ZoneManager.h"
#include "ThreadRegister.hpp"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "TestQuery.hpp"
#include "TestSProc.hpp"
#include "ServerTaskUpdate.h"
#include "ServerTaskDisconnected.h"



void CWrappingServer::Init(CServerBase::_StInitInfo const& serverInfo_)
{
	__super::Init(serverInfo_);
	
	m_receiver.AttachWorkerHandler(packetdef::PACKET_REG_LOGIN, &m_loginContext);
	m_receiver.AttachModule(&m_sessionModule);
	m_listen.AttachModule(&m_sessionModule);
	m_transfer.AttachModule(&m_sessionModule);

	CZoneManager::_stInit init;
	init.regionCount = RECV_THREAD_COUNT;
	init.server = this;
	CZoneManager::GetInstance().Init(init);
	m_receiver.AttachTaskExecutor(&CZoneManager::GetInstance());
}


bool CWrappingServer::Start()
{
#if _USE_ADO_DB_MYSQL || _USE_ADO_DB_MSSQL
	CAdoOleDB::_StInitInfo init;

	init.dbName = L"projectc";
	init.ipAddress = L"127.0.0.1";
	init.port = 3306;
	init.userID = L"root";
	init.pass = L"0320";
	init.dbType = CAdoOleDB::ACCOUNT_DB;
	init.nSessionThreadCount = DB_SESSION_THREAD_COUNT;


	//init.dbName = L"test";
	//init.ipAddress = L"127.0.0.1";
	//init.port = 3306;
	//init.userID = L"sa";
	//init.pass = L"skip";
	//init.dbType = CAdoOleDB::ACCOUNT_DB;
	//init.nSessionThreadCount = DB_SESSION_THREAD_COUNT;

	m_db.Init(init);
	m_db.Start();

	CTestQuery* task1 = ALLOCATE_POOL_MALLOC(CTestQuery, sizeof(CTestQuery));
	new(task1) CTestQuery;
	task1->SetThreadRegion(INIT_REGION_ID);
	task1->user_id = 9970;
	m_db.PushTask(task1);

	CTestQuery* task2 = ALLOCATE_POOL_MALLOC(CTestQuery, sizeof(CTestQuery));
	new(task2) CTestQuery;
	task2->SetThreadRegion(INIT_REGION_ID);
	task2->user_id = 2;
	m_db.PushTask(task2);

	CTestQuery* task3 = ALLOCATE_POOL_MALLOC(CTestQuery, sizeof(CTestQuery));
	new(task3) CTestQuery;
	task3->SetThreadRegion(1);
	task3->user_id = 754;
	m_db.PushTask(task3);

	CTestSProc* task4 = ALLOCATE_POOL_MALLOC(CTestSProc, sizeof(CTestSProc));
	new(task4) CTestSProc;
	task4->SetThreadRegion(INIT_REGION_ID);
	task4->user_name = "xxxx";
	m_db.PushTask(task4);

	CTestSProc* task5 = ALLOCATE_POOL_MALLOC(CTestSProc, sizeof(CTestSProc));
	new(task5) CTestSProc;
	task5->SetThreadRegion(INIT_REGION_ID);
	task5->user_name = "yyyy";
	m_db.PushTask(task5);
#endif

	if (!__super::Start())
	{
		// 로그
		return false;
	}

	m_timerThreadHandle = std::thread([&]()
	{
		while (IsRunning())
		{
			::Sleep(ZONE_UPDATE_TICK);
			CreateWorldTask<CServerTaskUpdate>();
		}
	});


	return true;
}


template<typename T>
void CWrappingServer::CreateWorldTask()
{
	T* task = ALLOCATE_POOL_MALLOC(T, sizeof(T));
	new(task) T(GetRecvIOPortHandler().size());
	PushTask(task);
}

template<typename T>
void CWrappingServer::CreateZoneTask(ZONE_ID zoneId_)
{
	T* task = ALLOCATE_POOL_MALLOC(T, sizeof(T));
	new(task) T(zoneId_);
	PushTask(task);
}

template<typename T>
void CWrappingServer::CreateSessionTask(CSession* session_)
{
	T* task = ALLOCATE_POOL_MALLOC(T, sizeof(T));
	new(task) T(session_);
	PushTask(task);
}


void CWrappingServer::SessionClose(CSession* session_)
{
	if (session_->GetState() == CSession::E_STATE::STATE_INIT)
	{
		/// 로그인을 받기 전에 session이 끊어지면 session만 종료시킨다.
		session_->CloseSocket();
		CSessionManager::BackPoolObject(session_);
	}
	else
	{
		CreateSessionTask<CServerTaskDisconnected>(session_);
	}
}


void CWrappingServer::Join()
{
	__super::Join();
	m_timerThreadHandle.join();
}


void CWrappingServer::Process(CServerTask* task_)
{
	CServerTask::E_TYPE type = task_->GetTaskType();
	if (type == CServerTask::E_TYPE::WORLD_TASK_TYPE)
	{
		for (auto it : GetRecvIOPortHandler())
		{
			::PostQueuedCompletionStatus(it, 0, NULL, reinterpret_cast<LPOVERLAPPED>(task_));
		}
	}
	else
	{
		HANDLE handle = GetRecvIOPortHandler()[task_->GetThreadRegion()];
		::PostQueuedCompletionStatus(handle, 0, NULL, reinterpret_cast<LPOVERLAPPED>(task_));
	}
}


CWrappingServer::CWrappingServer()
	: m_loginContext(this)
{
}

CWrappingServer::~CWrappingServer()
{
}