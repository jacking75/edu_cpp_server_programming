#pragma once

#include <uv.h>
#include <cstdint>
#include <chrono>
#include <string>


namespace UVC2NetLibz
{
	typedef void(*UV_RECV_CALLBACK_FUNC)(uv_stream_t*, ssize_t, const uv_buf_t*);
	typedef void(*UV_CLOSE_CALLBACK_FUNC)(uv_handle_t*);

	struct ConnectionNetConfig
	{
		uint32_t MaxRecvPacketSize = 0;
		uint32_t MaxTry1RecvBufferSize = 0;
		uint32_t MAXRecvCountPer1Sec = 0; // 정한 것 보다 조금 더 넉넉하게 주는 것을 추천
		uint32_t MaxRecvBufferSize = 0;

		uint32_t MaxSendPacketSize = 0;
		uint32_t MAXSendCountPer1Sec = 0;  // 정한 것 보다 조금 더 넉넉하게 주는 것을 추천
		uint32_t MaxSendBufferSize = 0;
		
	};

	struct NetConfig
	{
		std::string IP;
		uint16_t PortNumber = 0;
		uint16_t Backlog = 0;
		
		uint32_t MaxConnectionCount = 0;
		ConnectionNetConfig Conn;
	};
	
	enum class DisConnectReason : uint16_t
	{		
		none,
		remote_closed,
		forces_close_server,
		unknown,
		end
	};
	inline char DisConnectReasonStr[(int)DisConnectReason::end][32] = { "none", "remote_closed", "forces_close_server", "unknown" };
	
	enum class DisableSendReason : uint16_t
	{
		none,
		uv_write_fail,
		SendCompleted, 
		end
	};
	inline char DisableSendReasonStr[(int)DisableSendReason::end][32] = { "none", "uv_write_fail", "SendCompleted" };


	enum class LogLevel
	{
		trace = 0,
		debug,
		info,
		warn,
		error,
		fetal,
		end
	};

	inline char LogLevelStr[(int)LogLevel::end][6] = { "trace", "debug", "info", "warn", "error", "fetal"  };


	enum class NetErrorCode : int16_t
	{
		none = 0,
		uv_tcp_init_fail = 11,
		uv_accept_fail = 12,
		uv_read_start_fail = 15,
	};


	struct UVWriteReq
	{
		uv_write_t req;
		uv_buf_t buf;
	};


	/*struct LazyedDisConnectSession
	{
		uint32_t SessionIndex;
		uint64_t UniqueId;
		DisConnectReason Reason;
	};*/	

	class Helper
	{
	public:
		Helper() = default;
		~Helper() = default;
				
		static int64_t CurrentMilliSecond()
		{
			auto curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
			return curTime.count();
		}

		static int64_t CurrentSecond()
		{
			auto curTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch());
			return curTime.count();
		}

		static std::string GetUVError(int errcode)
		{
			if (0 == errcode) {
				return "";
			}
			std::string err;
			auto tmpChar = uv_err_name(errcode);
			if (tmpChar) {
				err = tmpChar;
				err += ":";
			}
			else {
				err = "unknown system errcode " + std::to_string((long long)errcode);
				err += ":";
			}
			tmpChar = uv_strerror(errcode);
			if (tmpChar) {
				err += tmpChar;
			}
			return std::move(err);
		}
	};

	inline void (*LogFuncPtr) (const int, const char* pszLogMsg);
}