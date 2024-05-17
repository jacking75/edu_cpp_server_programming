
#include "SessionManager.h"

#include <assert.h>
#pragma comment (lib, "libuv.lib")
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Userenv.lib")

#include <cstdio>
#include <unordered_map>
#include <thread>

#include <uv.h>

namespace UVC2NetLibz 
{
	class TCPServer 
	{
	 public:
		 TCPServer() = default;
		 virtual ~TCPServer() = default;

		 virtual void OnConnect(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) = 0;
		 virtual void OnClose(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_) = 0;
		 virtual void OnReceive(const uint32_t sessionIndex_, const uint64_t sessionUniqueId,
								const uint16_t dataSize_, char* pData) = 0;		 	  		 

		 int Init(NetConfig config_)
		 {			 
			 mListenSocket_.data = this;
			 mPort = config_.PortNumber;
			 mBacklog = config_.Backlog;

			 mLoop = uv_default_loop();

			 auto ret = uv_tcp_init(mLoop, &mListenSocket_);
			 if (ret) 
			 {
				 WriteLogUVError("Init() - error: %s", ret);
				 return 11;
			 }

			 if (Bind4(config_.IP.c_str(), mPort) == false)
			 {
				 return 12;
			 }

			 if (Listen() == false)
			 {
				 return 14;
			 }


			 mSessionMgr.PrepareSessionPool(config_.MaxConnectionCount);
			 			 
			 return 0;
		 }


	protected:
		 int Start()
		 {
			 mAsyncHandleStopUV.data = this;
			 uv_async_init(mLoop, &mAsyncHandleStopUV, AsyncStopUV);

			 return uv_run(mLoop, UV_RUN_DEFAULT);
		 }
	  
		 void End()
		 {
			 LogFuncPtr((int)UVC2NetLibz::LogLevel::info, "TCPServer::End() - Start ~");

			 uv_async_send(&mAsyncHandleStopUV);
			 //std::this_thread::sleep_for(std::chrono::milliseconds(128));

			 mSessionMgr.Dispose();

			 LogFuncPtr((int)UVC2NetLibz::LogLevel::info, "TCPServer::End() - Finish");
		 }

		 bool SendMsg(const uint32_t sessionIndex_, const uint16_t dataSize_, const char* pData_)
		 {
			 auto session = mSessionMgr.GetSessionObj(sessionIndex_);
			 return session->SendData(dataSize_, pData_);
		 }

		 static void OnAccept(uv_stream_t* server, int status)
		 {
			 TCPServer* thisClasss = (TCPServer*)server->data;

			 if (status) 
			 {
				 thisClasss->WriteLogUVError("OnAccept() - Error: %s", status);
				 return;
			 }

			 // 세션 할당하기
			 auto curTimeSec = Helper::CurrentSecond();
			 auto pSession = thisClasss->mSessionMgr.IssueClientSession(curTimeSec);
			 if (pSession == nullptr)
			 {	
				 //TODO 테스트 - uv_accept를 호출하지 않도록 홀수번 접속일 때 여기에 들어오게 한다.
				 // 1. 시간을 기다리면 클라이언트는 자동으로 접속이 끊어지나
				 // 1-1. 이후에 다른 클라이언트가 접속하면 OnAccept 에서는 어떤 에러가 발생할까?
				 // 2. 클라이언트가 대기하는 동안 다른 클라이언트가 접속하면 둘 다 잘 처리가 되나?
				 LogFuncPtr((int)LogLevel::debug, "TCPserver::OnAccept() - Empry Session Pool");
				
				 // 사용할 수 있는 세션이 없는 상태이므로 연결을 끊고, 잠시 대기한다
				 // 아마 지금 연결된 클라이언트는 다른 연결이 생겨서 다시 이 함수가 호출되면 처리되거나 혹은 시간이 지나서 끊어지거나 또는 유저가 접속을 끊을 것이다.
				 Sleep(1000);
				 return;
			 }
			 
			 ++thisClasss->mConnectSequenceId;
			 auto sessionUniqueId = thisClasss->mConnectSequenceId;
			 auto sessionIndex = pSession->GetIndex();

			 if (auto ret = pSession->OnConnect(server, thisClasss->mLoop, sessionUniqueId, 
									OnReceiveProxy, OnCloseCompletedProx);
				 ret != NetErrorCode::none)
			 {
				 if (ret != NetErrorCode::uv_read_start_fail)
				 {
					 thisClasss->mSessionMgr.ReturnSession(sessionIndex, curTimeSec);
				 }
				 return;
			 }			 		 
			 			 
			 thisClasss->OnConnect(sessionIndex, sessionUniqueId);
		 }
		 
		 static void OnReceiveProxy(uv_stream_t* sessionHandle_, ssize_t nread, const uv_buf_t* buf)
		 {
			 auto pSession = (Session*)sessionHandle_->data;
			 auto thisClassObj = (TCPServer*)pSession->GetServerObjPtr();

			 auto sessionIndex = pSession->GetIndex();
			 auto sessionUniqueId = pSession->GetUniqueId();
			 			 
			 DisConnectReason reason = DisConnectReason::none;

			 if (nread < 0) 
			 {
				 /* Error or EOF */
				 if (nread == UV_EOF) 
				 {
					 reason = DisConnectReason::forces_close_server;
				 }
				 else if (nread == UV_ECONNRESET) 
				 {
					 // 리모트가 접속을 끊으면 이쪽으로 온다(클라-서버 머신을 다르게 해서도 그런가?)
					 reason = DisConnectReason::remote_closed;
				 }
				 else 
				 {
					 reason = DisConnectReason::unknown;
					 //WriteLogUVError("OnReceiveProxy() - Error: %s", nread);
				 }

				 pSession->BeforeOnClose(reason);

				 uv_close((uv_handle_t*)sessionHandle_, OnCloseCompletedProx);
				 return;
			 }
			 else if (0 == nread) 
			 {
				 /* Everything OK, but nothing read. */
			 }
			 else
			 {
				 thisClassObj->OnReceive(sessionIndex, sessionUniqueId, (uint16_t)nread, (char*)buf->base);
				 pSession->ReadRecvData((uint32_t)nread);
			 }
		 }
		 		 	  
		 static void OnCloseCompletedProx(uv_handle_t* sessionHandle_)
		 {
			 auto pSession = (Session*)sessionHandle_->data;			 
			 auto sessionIndex = pSession->GetIndex();
			 auto sessionUniqueId = pSession->GetUniqueId();
			 
			 auto thisClassObj = (TCPServer*)pSession->GetServerObjPtr();
			 thisClassObj->OnClose(sessionIndex, sessionUniqueId);
			 
			 thisClassObj->mSessionMgr.ReturnSession(sessionIndex, sessionUniqueId);
		 }

		 static void AsyncStopUV(uv_async_t* handle)
		 {
			 auto thisClassObj = (TCPServer*)handle->data;
			 uv_stop(thisClassObj->mLoop);
		 }

		 bool Bind4(const char* ip_, const uint16_t port_)
		 {
			 struct sockaddr_in addr;
			 auto ret = uv_ip4_addr(ip_, port_, &addr);
			 if (ret) 
			 {
				 WriteLogUVError("Bind4() - error %s", ret);
				 return false;
			 }

			 ret = uv_tcp_bind(&mListenSocket_, (const struct sockaddr*) & addr, 0);
			 if (ret) 
			 {
				 WriteLogUVError("Bind4() - error %s", ret);
				 return false;
			 }

			 return true;
		 }

		 bool Bind6(const char* ip_, const uint16_t port_)
		 {
			 struct sockaddr_in6 addr;
			 auto ret = uv_ip6_addr(ip_, port_, &addr);
			 if (ret) 
			 {
				 WriteLogUVError("Bind6() - error %s", ret);
				 return false;
			 }

			 ret = uv_tcp_bind(&mListenSocket_, (const struct sockaddr*) & addr, 0);
			 if (ret) 
			 {
				 WriteLogUVError("Bind6() - error %s", ret);
				 return false;
			 }

			 return true;
		 }

		 bool Listen()
		 {
			 int ret = uv_listen((uv_stream_t*)&mListenSocket_, mBacklog, OnAccept);
			 if (ret)
			 {
				 WriteLogUVError("Listen() error %s", ret);
				 return false;
			 }

			 return true;
		 }

		 		 	 
		 Session* GetSession(const uint32_t sessionIndex_, const uint64_t sessionUniqueId_)
		 {
			 auto session = mSessionMgr.GetSessionObj(sessionIndex_, sessionUniqueId_);
			 return session;
		 }

		 static void WriteLogUVError(const char* format_, const int uvStatus_)
		 {
			 char logmsg[256] = { 0, };
			 sprintf_s(logmsg, format_, Helper::GetUVError(uvStatus_).c_str());
			 LogFuncPtr((int)LogLevel::error, logmsg);
		 }

		 static void SocketOptionNoDelay(uv_tcp_t handle_)
		 {
			 auto ret = uv_tcp_nodelay(&handle_, 1);
			 if (ret) {
				 //errmsg_ = GetUVError(iret);
			 }
		 }


		 int mPort;
		 int mBacklog;

		 uint64_t mConnectSequenceId = 0;

		 SessionManager mSessionMgr;

		 uv_tcp_t mListenSocket_;
		 uv_async_t mAsyncHandleStopUV;
		 uv_loop_t* mLoop;		 		 
	};
}
