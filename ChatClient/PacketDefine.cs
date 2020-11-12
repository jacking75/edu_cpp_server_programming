using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace csharp_test_client
{
    class PacketDef
    {
        public const Int16 PACKET_HEADER_SIZE = 5;
        public const int MAX_USER_ID_BYTE_LENGTH = 16;
        public const int MAX_USER_PW_BYTE_LENGTH = 16;
        public const int MAX_ROOM_CHAT_MSG_SIZE = 256;
    }

    public enum PACKET_ID : ushort
    {
        // 로그인
        LOGIN_REQ = 21,
        LOGIN_RES = 22,

        LOBBY_LIST_REQ = 26,
        LOBBY_LIST_RES = 27,

        LOBBY_ENTER_REQ = 31,
        LOBBY_ENTER_RES = 32,

        LOBBY_LEAVE_REQ = 46,
        LOBBY_LEAVE_RES = 47,
        
        ROOM_ENTER_REQ = 61,
        ROOM_ENTER_RES = 62,        
        ROOM_NEW_USER_NTF = 63,
        ROOM_USER_LIST_NTF = 64,

        ROOM_LEAVE_REQ = 66,
        ROOM_LEAVE_RES = 67,
        ROOM_LEAVE_USER_NTF = 68,
        
        MATCH_USER_REQ = 72,
        MATCH_USER_RES = 73,

        ROOM_CHAT_REQ = 76,
        ROOM_CHAT_RES = 77,
        ROOM_CHAT_NOTIFY = 78,

        PUT_STONE_REQ = 79,
        PUT_STONE_RES = 80,

        PUT_STONE_TURN = 114,
        PUT_STONE_INFO_NOTIFY = 115,
        GAME_END_RESULT = 116,

        GAME_START_REQ = 118,
        GAME_START_RES = 119,
        //PACKET_ID_ERROR_NTF = 703,

        // Ping(Heart-beat)
        //PING_REQ = 706,
        //PING_RES = 707,

        //PACKET_ID_ROOM_RELAY_REQ = 741,
        //PACKET_ID_ROOM_RELAY_NTF = 742,
    }


    public enum ERROR_CODE : Int16
    {
        ERROR_NONE = 0,



        ERROR_CODE_USER_MGR_INVALID_USER_UNIQUEID = 112,

        ERROR_CODE_PUBLIC_CHANNEL_IN_USER = 114,

        ERROR_CODE_PUBLIC_CHANNEL_INVALIDE_NUMBER = 115,


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

        MATCHING_FAIL = 278,

        ROOM_LEAVE_INVALID_DOMAIN = 286,
        ROOM_LEAVE_INVALID_LOBBY_INDEX = 287,
        ROOM_LEAVE_INVALID_ROOM_INDEX = 288,
        ROOM_LEAVE_NOT_CREATED = 289,
        ROOM_LEAVE_NOT_MEMBER = 290,

        ROOM_CHAT_INVALID_DOMAIN = 296,
        ROOM_CHAT_INVALID_LOBBY_INDEX = 297,
        ROOM_CHAT_INVALID_ROOM_INDEX = 298,

        GAME_NOT_YOUR_TURN = 300,
        GAME_PUT_ALREADY_EXIST = 301,
        GAME_PUT_SAM_SAM = 302,
        GAME_PUT_POS_ERROR = 303,

        GAME_RESULT_BLACK_WIN = 304,
        GAME_RESULT_WHITE_WIN = 305,

        NOT_READY_EXIST = 306,
        NOT_YOUR_TURN = 307,

        ALREADY_GAME_STATE = 308,
        ALREADY_READY_STATE = 309,
        USER_STATE_NOT_GAME = 310
    }
}
