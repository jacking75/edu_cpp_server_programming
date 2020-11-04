#pragma once

#include "PacketID.h"
#include "ErrorCode.h"
#include <string>

namespace NCommon
{
#pragma pack(push, 1)
	struct PktHeader
	{
		short TotalSize;
		short Id;
		unsigned char Reserve;
	};

	struct PktBase
	{
		short ErrorCode = (short)NServerNetLib::ERROR_CODE::NONE;
		void SetError(NServerNetLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};

	//- 로그인 요청
	const int MAX_USER_ID_SIZE = 16;
	const int MAX_USER_PASSWORD_SIZE = 16;
	struct PktLogInReq
	{
		char szID[MAX_USER_ID_SIZE + 1] = { 0, };
		char szPW[MAX_USER_PASSWORD_SIZE + 1] = { 0, };
	};

	struct PktLogInRes : PktBase
	{

	};


	struct PktMatchReq
	{
		
	};

	struct PktMatchRes : PktBase
	{
		
	};


	//- 룸에 들어가기 요청
	const int MAX_ROOM_TITLE_SIZE = 16;
	struct PktRoomEnterReq
	{
		short RoomIndex;
	};

	struct PktRoomEnterRes : PktBase
	{

	};


	struct PktPutStoneReq
	{
		int x;
		int y;
	};


	struct PktGameReadyRes : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	
	struct PktPutStoneRes : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktPutStoneInfoNtf : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
		int xPos;
		int yPos;
	};
	struct PktGameResultNtf : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	//- 룸에 있는 유저에게 새로 들어온 유저 정보 통보
	struct PktRoomEnterUserInfoNtf
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};


	//- 룸 나가기 요청
	struct PktRoomLeaveReq {};

	struct PktRoomLeaveRes : PktBase
	{
	
	};

	//- 룸에서 나가는 유저 통보(로비에 있는 유저에게)
	struct PktRoomLeaveUserInfoNtf
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};


	//- 룸 채팅
	const int MAX_ROOM_CHAT_MSG_SIZE = 256;
	struct PktRoomChatReq
	{
		wchar_t Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};

	struct PktRoomChatRes : PktBase
	{
	};

	struct PktRoomChatNtf
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
		wchar_t Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};

	const int DEV_ECHO_DATA_MAX_SIZE = 1024;

	struct PktDevEchoReq
	{
		short DataSize;
		char Datas[DEV_ECHO_DATA_MAX_SIZE];
	};

	struct PktDevEchoRes : PktBase
	{
		short DataSize;
		char Datas[DEV_ECHO_DATA_MAX_SIZE];
	};

#pragma pack(pop)



}