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

	const int MAX_USER_ID_SIZE = 16;
	const int MAX_USER_PASSWORD_SIZE = 16;

	struct PktLogInReq : public PktHeader
	{
		char szID[MAX_USER_ID_SIZE] = { 0, };
		char szPW[MAX_USER_PASSWORD_SIZE] = { 0, };
	};

	struct PktLogInRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};


	struct PktMatchReq : public PktHeader
	{

	};

	struct PktMatchRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};

	struct PktRoomEnterReq : public PktHeader
	{
		INT16 RoomIndex;
	};

	struct PktRoomEnterRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};


	struct PktPutStoneReq : public PktHeader
	{
		int x;
		int y;
	};


	struct PktGameReadyRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktTimeOutTurnChange : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktPutStoneRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktGameResultNtf : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	//- 룸에 있는 유저에게 새로 들어온 유저 정보 통보
	struct PktRoomEnterUserInfoNtf : public PktHeader
	{
		INT64 UserUniqueId;
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
	};

	struct PktRoomLeaveReq : public PktHeader 
	{
	
	};

	struct PktRoomLeaveRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};

	//- 룸에서 나가는 유저 통보(로비에 있는 유저에게)
	struct PktRoomLeaveUserInfoNtf : public PktHeader
	{
		INT64 UserUniqueId;
	};

	const int MAX_ROOM_CHAT_MSG_SIZE = 256;
	struct PktRoomChatReq : public PktHeader
	{
		char Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};

	struct PktRoomChatRes : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
	};

	struct PktRoomChatNtf : public PktHeader
	{
		short ErrorCode = (short)OmokServerLib::ERROR_CODE::NONE;
		void SetError(OmokServerLib::ERROR_CODE error) { ErrorCode = (short)error; }
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
		char Msg[MAX_ROOM_CHAT_MSG_SIZE + 1] = { 0, };
	};


#pragma pack(pop)

}