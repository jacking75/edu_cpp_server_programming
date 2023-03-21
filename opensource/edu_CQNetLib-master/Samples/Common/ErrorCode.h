#pragma once

//에러 코드 정의
enum class ErrorCode : short
{
	//성공에 관한 코드
	ERR_NONE						= 0x00000000,			//일반적인 모든 성공
	
	ERR_NONE_CREATENICKNAME			= 0x00000002,			//닉네임 생성 성공
	ERR_NONE_CREATECHARACTER		= 0x00000003,			//캐릭터 생성 성공
	ERR_NONE_CONNECTGAMESERVER		= 0x00000004,			//게임서버 접속 허용
	ERR_NONE_CREATEACCOUNT			= 0x00000005,			//계정생성 성공
	ERR_NONE_SAVEINVENTORYINFO		= 0x00000006,			//인벤토리 저장 성공
	ERR_NONE_SERVICESTART			= 0x00000007,			//서비스 시작 성공
	ERR_NONE_SERVICESTOP			= 0x00000008,			//서비스 중지 성공
	ERR_NONE_CONNECTSTART			= 0x00000009,			//서버 연결 성공
	ERR_EXIST						= 0x00001000,			//에러가 있는지 체크하는 기준

	//로긴 관련 에러 코드
	ERR_LOGIN_SAMEID				= 0x00001001,			//같은 아이디가 존재합니다.
	ERR_LOGIN_SERVER				= 0x00001002,			//로긴 서버 에러
	ERR_LOGIN_ID					= 0x00001003,			//ID가 존재하지 않습니다.
	ERR_LOGIN_PW					= 0x00001004,			//PW가 틀립니다.
	ERR_LOGIN_SAMENICKNAME     		= 0x00001005,			//같은 닉네임이 존재 합니다.
	ERR_LOGIN_NOEXISTCHACRACTER		= 0x00001006,			//캐릭터가 없다. 처음 들어온 사용자 임
	ERR_LOGIN_NOEXISTNICKNAME		= 0x00001007,			//닉네임이 없다
	ERR_LOGIN_COMMUNITYSERVER		= 0x00001008,			//커뮤니티 서버에 로긴할 때 에러났다.


	//게임룸 관련 에러 코드
	ERR_GAMEROOM_FULLROOM			= 0x00001100,			//게임룸을 더이상 생성할수 없다.
	ERR_GAMEROOM_NOTEXIST			= 0x00001101,			//게임룸을 찾을수 없습니다.
	ERR_GAMEROOM_FULLUSER			= 0x00001102,			//사용자가 가득차서 방에 들어갈수 없습니다.
	ERR_GAMEROOM_NOTREADY			= 0x00001103,			//준비되지 않은 사용자가 있습니다
	ERR_GAMEROOM_ALREADYSTART		= 0x00001104,			//이미 플레이중인 방이다.
	ERR_GAMEROOM_NOMUSIC			= 0x00001105,			//없는 음악이다.
	ERR_GAMEROOM_FULLAUDIENCE   	= 0x00001106,			//관전자 슬롯이 가득 찼습니다.
	ERR_GAMEROOM_FULLPLAYER			= 0x00001107,			//플레이어 슬롯이 가득 찼습니다.
	ERR_GAMEROOM_NOTEAM				= 0x00001108,			//팀이 잘못되었다.(팀인원이 맞지 않는다.)
	ERR_GAMEROOM_SELECTEDINSTRUMENT	= 0x00001109,			//이미 선택된 악기 입니다.
	ERR_GAMEROOM_NOSELECTMUSIC		= 0x00001110,			//선택된 음악이 없어 방을 시작하지 못한다.
	ERR_GAMEROOM_NOSELECTINSTRUMENT = 0x00001111,			//악기를 선택하지 않아서 시작하지 못한다.
	ERR_GAMEROOM_WRONGSLOTNO		= 0x00001112,			//잘못된 슬롯번호
	ERR_GAMEROOM_WRONGROOMPW		= 0x00001113,			//잘못된 패스워드
	ERR_GAMEROOM_NOQUICKSTART		= 0x00001114,			//들어갈 방이 없어서 퀵스타트 못함
	ERR_GAMEROOM_FAILEDCREATEROOM	= 0x00001115,			//게임룸 만들기를 실패하였다.
	
	//인벤토리 관련 에러 코드
	ERR_INVENTORY_NOCHARACTER		= 0x00001201,			//해당 캐릭터는 존재하지 않는다.
	ERR_INVENTORY_NOITEM			= 0x00001202,			//없는 아이템을 장착하려고 했다.
	ERR_SQL_FAILED					= 0x00009001,			//디비 에러

	
	//커뮤니티 관련 에러 코드
	ERR_COMMUNITY_SERVER			= 0x00003001,			//커뮤니티 서버 에러
	ERR_COMMUNITY_NOTCONNECTDB		= 0x00003002,			//게임DB서버와 연결되지 않았습니다.
	ERR_COMMUNITY_NOTCONNECTGS		= 0x00003003,			//게임서버와 연결되지 않았습니다.
	
	ERR_COMMUNITY_NOUSER			= 0x00003101,			//사용자를 찾을수 없습니다.
	ERR_COMMUNITY_NOFRIEND			= 0x00003102,			//친구를 찾을수 없습니다.
	ERR_COMMUNITY_NOTEXISTID		= 0x00003103,			//존재하지 않는 ID입니다.
	ERR_COMMUNITY_ALREADYFRIEND		= 0x00003104,			//이미 등록된 친구입니다.
	ERR_COMMUNITY_NOCONFIRMID		= 0x00003105,			//ID는 영문자 및 숫자만 가능합니다.

	ERR_COMMUNITY_NOTSENDMSG		= 0x00003201,			//메시지를 보내지 못했습니다.

	//서비스 에러 코드
	ERR_SERVICE_START				= 0x00004001,			//서비스 시작을 못했습니다.
	ERR_SERVICE_STOP				= 0x00004002,			//서비스 중지를 못했습니다.
	ERR_PROCESS_START				= 0x00004003,			//프로세스 시작을 못했습니다.
	ERR_CONNECT_START				= 0x00004004,			//서버 연결을 못했습니다.
	

	ERR_COMMUNITY_NOTCONNECTCS		= 0x00002003,		//커뮤니티서버와 연결되지 않았습니다.
	
	ERR_BUFFER_FAILED				= 0x00005001,			//버퍼 에러
};

