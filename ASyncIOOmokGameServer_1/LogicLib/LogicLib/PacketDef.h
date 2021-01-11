#pragma once

#include "PacketID.h"
#include <string>
#include "ErrorCode.h"

namespace OmokServerLib
{
#pragma pack(push, 1)
	struct PktHeader
	{
		UINT16 TotalSize;
		UINT16 Id;
		UINT8 Type;
	};

	const UINT32 PACKET_HEADER_SIZE = sizeof(PktHeader);

	struct PktBase : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};

	//- 로그인 요청
	const int MAX_USER_ID_SIZE = 16;
	const int MAX_USER_PASSWORD_SIZE = 16;

	struct PktLogInReq : public PktHeader
	{
		char szID[MAX_USER_ID_SIZE] = { 0, };
		char szPW[MAX_USER_PASSWORD_SIZE] = { 0, };
	};

	struct PktLogInRes : PktBase 
	{

	};


	struct PktMatchReq : public PktHeader
	{

	};

	struct PktMatchRes : PktBase
	{

	};


	//- 룸에 들어가기 요청
	const int MAX_ROOM_TITLE_SIZE = 16;
	struct PktRoomEnterReq : public PktHeader
	{
		INT16 RoomIndex;
	};

	struct PktRoomEnterRes : PktBase
	{

	};


	struct PktPutStoneReq : public PktHeader
	{
		int x;
		int y;
	};


	struct PktGameReadyRes : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktTimeOutTurnChange : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktPutStoneRes : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktGameResultNtf : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	//- 룸에 있는 유저에게 새로 들어온 유저 정보 통보
	struct PktRoomEnterUserInfoNtf : PktBase
	{
		INT64 UserUniqueId;
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};


	//- 룸 나가기 요청
	struct PktRoomLeaveReq : public PktHeader 
	{
	
	};

	struct PktRoomLeaveRes : PktBase
	{

	};

	//- 룸에서 나가는 유저 통보(로비에 있는 유저에게)
	struct PktRoomLeaveUserInfoNtf : PktBase
	{
		INT64 UserUniqueId;
	};


	//- 룸 채팅
	const int MAX_ROOM_CHAT_MSG_SIZE = 256;
	struct PktRoomChatReq : public PktHeader
	{
		char Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};

	struct PktRoomChatRes : PktBase
	{
	};

	struct PktRoomChatNtf : PktBase
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
		char Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};


#pragma pack(pop)

}