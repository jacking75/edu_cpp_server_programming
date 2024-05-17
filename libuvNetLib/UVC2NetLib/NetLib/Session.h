#pragma once

#include "RingBuffer.h"
#include "CommonDef.h"

namespace UVC2NetLibz 
{
	class Session
	{
		const int64_t CONNECTED = 1;
		const int64_t ENABLE_VALUE = 1;
	public:
		Session() = default;
		~Session() = default;
		
		int GetIndex() { return mIndex; }

		uint64_t GetUniqueId() { return mUnqiueId; }

		void* GetServerObjPtr() { return mServerPtr; }

		std::tuple<uint32_t, char*> GetPacketDataRead() { return mRecvBuffer.Read(); }

		void ReadRecvData(const uint32_t readSize_) { mRecvBuffer.IncremantReadPos(readSize_); }

		bool IsConnected() { return mConnected == CONNECTED; }

		bool IsEnableNetwork() 
		{ 
			return mConnected == CONNECTED && mEnableNetwork == ENABLE_VALUE;
		}
		
		bool IsEnableSend()
		{
			return mConnected == CONNECTED && mEnableSend == ENABLE_VALUE;
		}

		DisConnectReason GetDisConnectReason() { return mDisConnReason; }

		
		void Dispose()
		{
		}

		void Init(const uint32_t index_)
		{
			mIndex = index_;		

			//TODO 설정 값 사용하기
			mRecvBuffer.Create(4096, 1024); 
			mSendBuffer.Create(4096, 0);
		}

		NetErrorCode OnConnect(uv_stream_t* server, uv_loop_t* loop_, const uint64_t uniqueId_,
			UV_RECV_CALLBACK_FUNC callbackFunc, 
			UV_CLOSE_CALLBACK_FUNC closeCB)
		{
			auto iret = uv_tcp_init(loop_, &mHandle);
			if (iret != 0) 
			{
				// 실패인 경우는 uv_tcp_init 에서 소켓 처리를 하고 있다.
				//tcpsock->errmsg_ = GetUVError(iret);
				return NetErrorCode::uv_tcp_init_fail;
			}
			
			mServerPtr = server->data;
			mHandle.data = this;

			iret = uv_accept((uv_stream_t*)server, (uv_stream_t*)&mHandle);
			if (iret) 
			{
				// 실패인 경우는 uv_accept 에서 소켓 처리를 하고 있다.
				//tcpsock->errmsg_ = GetUVError(iret);
				return NetErrorCode::uv_accept_fail;
			}
			
			mUnqiueId = uniqueId_;
			mConnected = CONNECTED;
			mEnableNetwork = ENABLE_VALUE;
			mEnableSend = ENABLE_VALUE;
			mRecvBuffer.Clear();
			mSendBuffer.Clear();

			iret = uv_read_start((uv_stream_t*)&mHandle, AllocBufferForRecv, callbackFunc);
			if (iret) 
			{
				uv_close((uv_handle_t*)&mHandle, closeCB);
				//tcpsock->errmsg_ = GetUVError(iret);
				return NetErrorCode::uv_read_start_fail;
			}

			return NetErrorCode::none;
		}

		void BeforeOnClose(DisConnectReason reason_)
		{
			mConnected = 0;
			mEnableNetwork = 0;
			mEnableSend = 0;
			mDisConnReason = reason_;
		}

		

		bool SendData(const uint16_t dataSize_, const char* pData_)
		{			
			if (IsEnableSend() == false)
			{
				return false;
			}

			std::lock_guard<std::mutex> lock(mSendLock);

			mSendBuffer.Write(dataSize_, pData_);
			return true;
		}

		bool SendIO()
		{
			std::lock_guard<std::mutex> lock(mSendLock);

			auto [dataSize, pData] = mSendBuffer.Read();
			if (dataSize <= 0)
			{
				return false;
			}

			mWriteReq.req.data = this;
			mWriteReq.buf.len = dataSize;
			mWriteReq.buf.base = pData;

			auto ret = uv_write((uv_write_t*)&mWriteReq, (uv_stream_t*)&mHandle, &mWriteReq.buf, 1, SendCompleted);
			if (ret) 
			{
				//errmsg_ = "send data error." + GetUVError(ret);
				DisableSend(DisableSendReason::uv_write_fail, ret);
				return false;
			}

			return true;
		}
				

	private:
		void DisableSend(const DisableSendReason reason_, const int uvError_)
		{
			mEnableSend = 0;

			char logmsg[256] = { 0, };
			sprintf_s(logmsg, "Session::DisableSend() - %s, uvErr:%d", 
					DisableSendReasonStr[(uint16_t)reason_], uvError_);
			LogFuncPtr((int)LogLevel::error, logmsg);
		}

		static void AllocBufferForRecv(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
		{
			auto thisClassObj = (Session*)handle->data;

			buf->base = thisClassObj->mRecvBuffer.GetWriteablePtr();
			buf->len = thisClassObj->mRecvBuffer.GetMaxTry1RecvBufferSize();
		}

		static void SendCompleted(uv_write_t* req, int status)
		{
			auto pSession = (Session*)req->data;
			if (status) 
			{
				pSession->DisableSend(DisableSendReason::SendCompleted, status);
				return;
			}


			std::lock_guard<std::mutex> lock(pSession->mSendLock);

			auto writeReq = (UVWriteReq*)req;
			pSession->mSendBuffer.IncremantReadPos(writeReq->buf.len);
		}



		uint32_t mIndex = 0;
		void* mServerPtr = nullptr;

		uint64_t mUnqiueId = 0;
		
		int64_t mConnected = 0;		
		int64_t	mEnableNetwork = 0;
		DisConnectReason mDisConnReason = DisConnectReason::none;
		
		ReadWriteRingBuffer mRecvBuffer;

		int64_t	mEnableSend = 0;
		std::mutex mSendLock;
		ReadWriteRingBuffer mSendBuffer;

		uv_tcp_t mHandle;
		UVWriteReq mWriteReq;
	};

}
