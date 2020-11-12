#pragma once


namespace NCommon
{
	enum class PACKET_ID : short
	{
		LOGIN_IN_REQ = 21,
		LOGIN_IN_RES = 22,

		LOBBY_LIST_REQ = 26,
		LOBBY_LIST_RES = 27,

		LOBBY_ENTER_REQ = 31,
		LOBBY_ENTER_RES = 32,

		LOBBY_LEAVE_REQ = 46,
		LOBBY_LEAVE_RES = 47,

		ROOM_ENTER_REQ = 61,
		ROOM_ENTER_RES = 62,
		ROOM_ENTER_NEW_USER_NTF = 63,

		ROOM_LEAVE_REQ = 66,
		ROOM_LEAVE_RES = 67,
		ROOM_LEAVE_USER_NTF = 68,

		ROOM_CHANGED_INFO_NTF = 71,

		MATCH_USER_REQ = 72,
		MATCH_USER_RES = 73,

		ROOM_CHAT_REQ = 76,
		ROOM_CHAT_RES = 77,
		ROOM_CHAT_NTF = 78,

		PUT_STONE_REQ = 79,
		PUT_STONE_RES = 80,

		LOBBY_CHAT_REQ = 81,
		LOBBY_CHAT_RES = 82,
		LOBBY_CHAT_NTF = 83,

		ROOM_MASTER_GAME_START_REQ = 101,
		ROOM_MASTER_GAME_START_RES = 102,
		ROOM_MASTER_GAME_START_NTF = 103,

		ROOM_GAME_START_REQ = 111,
		ROOM_GAME_START_RES = 112,
		ROOM_GAME_START_NTF = 113,

		PUT_STONE_TURN = 114,
		PUT_STONE_INFO_NOTIFY = 115,

		GAME_END_RESULT = 116,

		GAME_START_REQ = 118,
		GAME_START_RES = 119,

		MAX = 256
	};
}