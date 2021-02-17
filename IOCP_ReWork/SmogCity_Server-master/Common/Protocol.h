
#pragma once

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//#include "Common.h"

constexpr int SERVER_PORT		= 3500;

constexpr int MAX_STR_LEN		= 50;
constexpr int MAX_NAME_LEN		= 10;

constexpr int MAX_PLAYER		= 10;
constexpr int NOTREGISTED		= 0xffff;

constexpr char WEAPON_MACHINEGUN		= 1;
constexpr char WEAPON_ROCKETGUN			= 2;

constexpr char INGAME_LOG_KILL = 0;
constexpr char INGAME_LOG_JOIN = 1;
constexpr char INGAME_LOG_LEAVE = 2;

constexpr int SMOG_MAX_X_INDEX = 10;
constexpr int SMOG_MAX_Y_INDEX = 10;
constexpr int SMOG_MAX_Z_INDEX = 10;

//#include "Struct.h"

enum e_CS_PACKET_TYPE {
	e_Login,
	e_Signup,
	e_Join,
	e_Access_Game_Ok,
	e_Update,
	e_Shoot,
	e_Collision,
	e_Logout,
	e_Back_To_Lobby,
	e_Swap_Weapon,
	e_Reload,
	e_Back_To_MainMenu
};

enum e_SC_PACKET_TYPE {
	e_Login_Ok,
	e_Login_Fail,
	e_Signup_Ok,
	e_Sighup_Fail,
	e_Game_Session_Info,
	e_Join_Ok,
	e_Join_Fail,
	e_Game_Info,
	e_Put_Player,
	e_Update_Player,
	e_Update_Player_Life,
	e_Die_Player,
	e_Respawn_Player,
	e_Remove_Player,
	e_Put_Object,
	e_Remove_Object,
	e_Logout_Ok,
	e_Round_Start,
	e_Round_Win,
	e_Round_Lose,
	e_Game_Victory,
	e_Game_Defeat,
	e_Character_Info,
	e_Swaped_Weapon,
	e_Damaged,
	e_Healed,
	e_Ammo_Update,
	e_InGame_Log,
	e_Smog_OnOff
};

enum e_Collsion_Type {
	e_Rocketgun_Bullet_To_Character,
	e_Rocketgun_Bullet_To_Other,
	e_Machinegun_Bullet_To_Character,
	e_Machinegun_Bullet_To_Other,
	e_Character_To_Mainbase,
	e_Character_To_Mainbase_Out,
	e_Character_To_HealPack_L,
	e_Character_To_HealPack_M
};

enum e_OBJECT_TYPE {
	e_Rocketgun_Bullet,
	e_Machinegun_Bullet,
	e_HealPack_L,
	e_HealPack_M
};

#pragma pack(push, 1)
struct PACKET_HEADER {			// 써야할지 말아야할지 생각해보자.
	BYTE size;
	BYTE type;
};
// CS PACKET
struct CS_PACKET_LOGIN {
	BYTE size;
	BYTE type;
	TCHAR id[MAX_NAME_LEN];
	TCHAR pw[MAX_NAME_LEN];
};
struct CS_PACKET_SIGNUP {
	BYTE size;
	BYTE type;
	TCHAR id[MAX_NAME_LEN];
	TCHAR pw[MAX_NAME_LEN];
};
struct CS_PACKET_JOIN {
	BYTE size;
	BYTE type;
	WORD roomnum;
};
struct CS_PACKET_ACCESS_GAME_OK {
	BYTE size;
	BYTE type;
	WORD id;
};
struct CS_PACKET_UPDATE {
	BYTE size;
	BYTE type;
	WORD id;
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT vx;
	FLOAT vy;
	FLOAT vz;
	FLOAT yaw;
	FLOAT pitch;
	FLOAT roll;
	bool isup;
	bool isfly;
	bool isshoot;
	//bool isreloading;
	bool isground;
	bool isrun;
	bool isfalling;
	FLOAT speed;
	FLOAT direction;
	FLOAT rotatoryaw;
	FLOAT rotatorpitch;
	FLOAT rotatorroll;
};
struct CS_PACKET_SHOOT {
	BYTE size;
	BYTE type;
	WORD id;
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT vx;
	FLOAT vy;
	FLOAT vz;
	FLOAT yaw;
	FLOAT pitch;
	FLOAT roll;
};
struct CS_PACKET_MOVE {
	BYTE size;
	BYTE type;					// FORWARD, BACK, LEFT, RIGHT, UP, DOWN
	WORD id;
};
struct CS_PACKET_COLLISION {
	BYTE size;
	BYTE type;
	WORD oid;
	WORD cid;
	e_Collsion_Type etype;
};
struct CS_PACKET_LOGOUT {
	BYTE size;
	BYTE type;
	WORD id;
};
struct CS_PACKET_BACK_TO_LOBBY {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_SWAP_WEAPON {
	BYTE size;
	BYTE type;
	BYTE weaponnum;
};
struct CS_PACKET_RELOAD {
	BYTE size;
	BYTE type;
};
struct CS_PACKET_BACK_TO_MAINMENU {
	BYTE size;
	BYTE type;
};
// SC PACKET
struct SC_PACKET_LOGIN_OK {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_LOGIN_FAIL {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_SIGNUP_OK {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_SIGNUP_FAIL {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_GAME_SESSION_INFO {
	BYTE size;
	BYTE type;
	WORD roomnum;
	bool isstart;
	BYTE currentnum;
};
struct SC_PACKET_JOIN_OK {
	BYTE size;
	BYTE type;
};
struct SC_PACKET_JOIN_FAIL {
	BYTE size;
	BYTE type;
};
struct SC_PACKET_GAME_INFO {
	BYTE size;
	BYTE type;
	bool mbdownoccupying;
	bool mbupoccupying;
	FLOAT mbdowntime;
	FLOAT mbuptime;
	bool mbdownoccupied;
	bool mbupoccupied;
	char downteampercent;
	char upteampercent;
	bool isengaing;
	char downwincount;
	char upwincount;
};
struct SC_PACKET_PUT_PLAYER {
	BYTE  size;
	BYTE  type;
	WORD  id;
	FLOAT life;
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT vx;
	FLOAT vy;
	FLOAT vz;
	FLOAT yaw;
	FLOAT pitch;
	FLOAT roll;
	bool  isdown;
	bool  islive;
	BYTE  weaponnum;
};
struct SC_PACKET_REMOVE_PLAYER {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_UPDATE_PLAYER {
	BYTE  size;
	BYTE  type;
	WORD  id;
	FLOAT life;
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT vx;
	FLOAT vy;
	FLOAT vz;
	FLOAT yaw;
	FLOAT pitch;
	FLOAT roll;
	bool isup;
	bool isfly;
	bool isshoot;
	bool isreloading;
	bool isground;
	bool isrun;
	bool isfalling;
	bool ischangingweapon;
	FLOAT speed;
	FLOAT direction;
	FLOAT rotatoryaw;
	FLOAT rotatorpitch;
	FLOAT rotatorroll;
	//BYTE  weaponnum;
};
struct SC_PACKET_UPDATE_PLAYER_LIFE {
	BYTE size;
	BYTE type;
	WORD id;
	FLOAT life;
};
struct SC_PACKET_DIE_PLAYER {
	BYTE  size;
	BYTE  type;
	WORD  id;
};
struct SC_PACKET_RESPAWN_PLAYER {
	BYTE  size;
	BYTE  type;
	WORD  id;
};
struct SC_PACKET_PUT_OBJECT {
	BYTE			size;
	BYTE			type;
	WORD			id;
	WORD			whoid;
	bool			isdown;
	e_OBJECT_TYPE	etype;
	FLOAT			x;
	FLOAT			y;
	FLOAT			z;
	FLOAT			vx;
	FLOAT			vy;
	FLOAT			vz;
	FLOAT			yaw;
	FLOAT			pitch;
	FLOAT			roll;
};
struct SC_PACKET_REMOVE_OBJECT {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_LOGOUT_OK {
	BYTE size;
	BYTE type;
	WORD id;
};
struct SC_PACKET_ROUND_RESULT {
	BYTE size;
	BYTE type;
};
struct SC_PACKET_GAME_RESULT {
	BYTE size;
	BYTE type;
};
struct SC_PACKET_CHARACTER_INFO {
	BYTE size;
	BYTE type;
	WORD gameamount;
	WORD gamevictory;
	WORD gamedefeat;
};
struct SC_PACKET_SWAPED_WEAPON {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE weaponnum;
};
struct SC_PACKET_DAMAGED {
	BYTE size;
	BYTE type;
	FLOAT life;
};
struct SC_PACKET_HEALED {
	BYTE size;
	BYTE type;
	FLOAT life;
};
struct SC_PACKET_AMMO_UPDATE {
	BYTE size;
	BYTE type;
	BYTE now;
	BYTE max;
};
struct SC_PACKET_INGAME_LOG {
	BYTE size;
	BYTE type;
	TCHAR from[MAX_NAME_LEN];
	BYTE logtype;
	TCHAR target[MAX_NAME_LEN];
};
struct SC_PACKET_SMOG_ONOFF {
	BYTE size;
	BYTE type;
	BYTE xIndex;
	BYTE yIndex;
	BYTE zIndex;
	bool issmog;
};
#pragma pack(pop)
#endif	// __PROTOCOL_H__