#pragma once

#include "PacketID.h"
#include "ErrorCode.h"

namespace NCommon
{	
#pragma pack(push, 1)
	struct PktHeader
	{
		unsigned short TotalSize;
		unsigned short Id;
		unsigned char Reserve;
	};

	
	//- 로그인 요청
	const int MAX_USER_ID_SIZE = 16;
	const int MAX_USER_PASSWORD_SIZE = 16;
	struct PktLogInReq
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
		char szPW[MAX_USER_PASSWORD_SIZE+1] = { 0, };
	};

	struct PktLogInRes : PktHeader
	{
		short ErrorCode = (short)ERROR_CODE::NONE;
	};

	
	//- 캐릭터 선택. 총 23 * 7개 캐릭터 
	struct PktSelCharacterReq
	{
		unsigned short CharCode;
	};

	struct PktSelCharacterRes : PktHeader
	{
		short ErrorCode = (short)ERROR_CODE::NONE;
		unsigned short CharCode;
	};
	
	//- 방 입장 - 방번호 선택
	//- 방 나가기
	//- 채팅
	//- 이동 (화면 위치 찍기, 항후 키보드 이동)
	//- 점프
	//- 춤추기 (?)
	
	// 방 리스트 요청
	const int MAX_ROOM_TITLE_SIZE = 16;
	struct PktRoomListReq
	{
		short Page; // 페이지 네이션 값(0부터 시작)
	}
	
	// 방 리스트 응답
	struct PktRoomListRes
	{
		short NextPage; // 다음 페이지 값, 방이 더 이상 없으면 -1
		char RoomTitle[MAX_ROOM_TITLE_SIZE + 1] = {0, }; // 방 제목
		char OwnerID[MAX_USER_ID_SIZE + 1] = {0, }; // 방장 아이디
		short MaxUserNum; // 최대 입장 수
		short CurrentUserNum; // 현재 인원 수
	}
	
	//- 룸에 들어가기 요청
	struct PktRoomEnterReq
	{
		short RoomNum;
	};

	struct PktRoomEnterRes : PktBase
	{
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

	//- 로비 채팅
	const int MAX_LOBBY_CHAT_MSG_SIZE = 256;
	struct PktLobbyChatReq
	{
		wchar_t Msg[MAX_LOBBY_CHAT_MSG_SIZE + 1] = { 0, };
	};

	struct PktLobbyChatRes : PktBase
	{
	};

	struct PktLobbyChatNtf
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
		wchar_t Msg[MAX_LOBBY_CHAT_MSG_SIZE + 1] = { 0, };
	};
	
	/*

	// 방장의 게임 시작 요청
	struct PktRoomMaterGameStartReq
	{};

	struct PktRoomMaterGameStartRes : PktBase
	{};

	struct PktRoomMaterGameStartNtf
	{};


	// 방장이 아닌 사람의 게임 시작 요청
	struct PktRoomGameStartReq
	{};

	struct PktRoomGameStartRes : PktBase
	{};

	struct PktRoomGameStartNtf
	{
		char UserID[MAX_USER_ID_SIZE + 1] = { 0, };
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
	*/
#pragma pack(pop)


	const short PacketHeaderSize = sizeof(PktHeader);
}
