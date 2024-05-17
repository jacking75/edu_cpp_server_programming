#pragma once

enum class ERROR_CODE : unsigned short
{
	none = 0,

	user_mgr_invalid_user_index = 11,
	user_mgr_invalid_user_uniqueid = 12,
	user_mgr_adduser_already = 14,

	login_user_already = 31,
	login_user_used_all_obj = 32,
	login_user_invalid_pw = 33,

	//new_room_used_all_obj = 41,
	//new_room_fail_enter = 42,

	//enter_room_not_finde_user = 51,
	//enter_room_invalid_user_status = 52,
	//enter_room_not_used_status = 53,
	enter_room_full_user = 54,

	room_enteruser_invalid_index = 61,
	room_leaveuser_invalid_index = 62,
	room_not_used = 64,
	room_too_many_packet = 67,
		
	chat_room_invalid_room_number = 81,
};
