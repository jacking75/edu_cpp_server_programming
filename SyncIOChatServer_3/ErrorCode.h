#pragma once

namespace NServerNetLib
{
	// 에러 코드는 1 ~ 200까지 사용한다.
	enum class NET_ERROR_CODE : short
	{
		NONE = 0,

		SERVER_SOCKET_CREATE_FAIL = 11,
		SERVER_SOCKET_SO_REUSEADDR_FAIL = 12,
		SERVER_SOCKET_BIND_FAIL = 14,
		SERVER_SOCKET_LISTEN_FAIL = 15,
		SERVER_SOCKET_FIONBIO_FAIL = 16,

		SEND_CLOSE_SOCKET = 21,
		SEND_SIZE_ZERO = 22,
		CLIENT_SEND_BUFFER_FULL = 23,
		CLIENT_FLUSH_SEND_BUFF_REMOTE_CLOSE = 24,
		
		ACCEPT_API_ERROR = 26,
		ACCEPT_MAX_SESSION_COUNT = 27,
		ACCEPT_API_WSAEWOULDBLOCK = 28,

		RECV_API_ERROR = 32,
		RECV_BUFFER_OVERFLOW = 32,
		RECV_REMOTE_CLOSE = 33,
		RECV_PROCESS_NOT_CONNECTED = 34,
		RECV_CLIENT_MAX_PACKET = 35,
	};

	// 201 부터 시작한다.
	enum class ERROR_CODE : short
	{
		NONE = 0,

		UNASSIGNED_ERROR = 201,

		MAIN_INIT_NETWORK_INIT_FAIL = 206,

		USER_MGR_ID_DUPLICATION = 211,
		USER_MGR_MAX_USER_COUNT = 212,
		USER_MGR_INVALID_SESSION_INDEX = 213,
		USER_MGR_NOT_CONFIRM_USER = 214,
		USER_MGR_REMOVE_INVALID_SESSION = 221,

		ROOM_ENTER_INVALID_DOMAIN = 271,
		ROOM_ENTER_INVALID_LOBBY_INDEX = 272,
		ROOM_ENTER_INVALID_ROOM_INDEX = 273,
		ROOM_ENTER_CREATE_FAIL = 274,
		ROOM_ENTER_NOT_CREATED = 275,
		ROOM_ENTER_MEMBER_FULL = 276,
		ROOM_ENTER_EMPTY_ROOM = 277,


		ROOM_LEAVE_INVALID_DOMAIN = 286,
		ROOM_LEAVE_INVALID_LOBBY_INDEX = 287,
		ROOM_LEAVE_INVALID_ROOM_INDEX = 288,
		ROOM_LEAVE_NOT_CREATED = 289,
		ROOM_LEAVE_NOT_MEMBER = 290,

		ROOM_CHAT_INVALID_DOMAIN = 296,
		ROOM_CHAT_INVALID_LOBBY_INDEX = 297,
		ROOM_CHAT_INVALID_ROOM_INDEX = 298,
		
	};
	
	const int MAX_NET_ERROR_STRING_COUNT = 64;
	struct NetError
	{
		NetError(NET_ERROR_CODE code)
		{
			Error = code;
		}

		NET_ERROR_CODE Error = NET_ERROR_CODE::NONE;
		wchar_t Msg[MAX_NET_ERROR_STRING_COUNT] = { 0, };
		int Vlaue = 0;
	};
}


enum ERORR_CODDE : short
{
	NONE = 0,
};
