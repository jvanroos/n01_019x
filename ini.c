/*
 * n01
 * 
 * Ini.c
 *
 */
//#define _INC_OLE
#include <windows.h>
//#undef _INC_OLE
#include <tchar.h>

#include "general.h"
#include "memory.h"
#include "profile.h"
#include "string.h"
#include "message.h"

#include "resource.h"

/* Global Variables */
extern OPTION_INFO op;

/* Local Function Prototype */
static COLORREF get_color(const TCHAR *buf, const COLORREF def_color);
static void put_color(const TCHAR *ini_path, const TCHAR *key, const COLORREF color, const COLORREF def_color);

static COLORREF get_color(const TCHAR *buf, const COLORREF def_color)
{
	if (*buf == TEXT('\0')) {
		return def_color;
	}
	return _tcstol(buf, NULL, 0);
}

BOOL ini_get_option(const TCHAR *ini_path)
{
	RECT rect;
	TCHAR buf[BUF_SIZE];
	WORD lang;
	int i, j;

	profile_initialize(ini_path, TRUE);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	lang = (WORD)PRIMARYLANGID(LANGIDFROMLCID(GetThreadLocale()));

	op.window_rect.left = profile_get_int(TEXT("window"), TEXT("left"), 0, ini_path);
	op.window_rect.top = profile_get_int(TEXT("window"), TEXT("top"), 0, ini_path);
	op.window_rect.right = profile_get_int(TEXT("window"), TEXT("right"), 0, ini_path);
	op.window_rect.bottom = profile_get_int(TEXT("window"), TEXT("bottom"), 0, ini_path);

	if (op.window_rect.left > rect.right || op.window_rect.top > rect.bottom) {
		op.window_rect.left = 0;
		op.window_rect.top = 0;
	}
	op.window_state = profile_get_int(TEXT("window"), TEXT("state"), SW_SHOWDEFAULT, ini_path);

	// view
	op.view_player = profile_get_int(TEXT("view"), TEXT("player"), 1, ini_path);
	op.view_left = profile_get_int(TEXT("view"), TEXT("left"), 1, ini_path);
	op.view_guide = profile_get_int(TEXT("view"), TEXT("guide"), 1, ini_path);
	op.view_throw_count = profile_get_int(TEXT("view"), TEXT("throw_count"), 1, ini_path);
	op.view_ton_circle = profile_get_int(TEXT("view"), TEXT("ton_circle"), 1, ini_path);
	op.view_separate = profile_get_int(TEXT("view"), TEXT("separate"), 1, ini_path);
	op.view_scroll_bar = profile_get_int(TEXT("view"), TEXT("scroll_bar"), 1, ini_path);

	// game option
	op.gi.start_score = profile_get_int(TEXT("game"), TEXT("start_score"), 501, ini_path);
	op.gi.round_limit = profile_get_int(TEXT("game"), TEXT("round_limit"), ((lang != LANG_JAPANESE) ? 0 : 1), ini_path);
	op.gi.round = profile_get_int(TEXT("game"), TEXT("round"), 15, ini_path);

	op.gi.leg_limit = profile_get_int(TEXT("game"), TEXT("leg_limit"), 0, ini_path);
	op.gi.max_leg = profile_get_int(TEXT("game"), TEXT("max_leg"), 3, ini_path);
	op.gi.best_of = profile_get_int(TEXT("game"), TEXT("best_of"), 1, ini_path);
	op.gi.change_first = profile_get_int(TEXT("game"), TEXT("change_first"), 0, ini_path);

	profile_get_string(TEXT("game"), TEXT("p1_name"), message_get_res(IDS_STRING_PLAYER1), op.gi.player_name[0], NAME_SIZE - 1, ini_path);
	op.gi.player_start_score[0] = profile_get_int(TEXT("game"), TEXT("p1_start_score"), 0, ini_path);
	op.gi.com[0] = profile_get_int(TEXT("game"), TEXT("p1_com"), FALSE, ini_path);
	op.gi.level[0] = profile_get_int(TEXT("game"), TEXT("p1_com_level"), 0, ini_path);

	profile_get_string(TEXT("game"), TEXT("p2_name"), message_get_res(IDS_STRING_PLAYER2), op.gi.player_name[1], NAME_SIZE - 1, ini_path);
	op.gi.player_start_score[1] = profile_get_int(TEXT("game"), TEXT("p2_start_score"), 0, ini_path);
	op.gi.com[1] = profile_get_int(TEXT("game"), TEXT("p2_com"), FALSE, ini_path);
	op.gi.level[1] = profile_get_int(TEXT("game"), TEXT("p2_com_level"), 0, ini_path);

	op.auto_save = profile_get_int(TEXT("game"), TEXT("auto_save"), 0, ini_path);
	profile_get_string(TEXT("game"), TEXT("auto_save_path"), TEXT(""), op.auto_save_path, BUF_SIZE - 1, ini_path);

	op.check_out_mode = profile_get_int(TEXT("game"), TEXT("check_out_mode"), 0, ini_path);

	op.name_list_count = profile_get_int(TEXT("game"), TEXT("name_list_count"), 0, ini_path);
	if (op.name_list_count > NAME_LIST_COUNT) {
		op.name_list_count = NAME_LIST_COUNT;
	}
	for (i = 0; i < op.name_list_count; i++) {
		wsprintf(buf, TEXT("name_list_%d"), i);
		profile_get_string(TEXT("game"), buf, TEXT(""), op.name_list[i], NAME_SIZE - 1, ini_path);
	}

	profile_get_string(TEXT("font"), TEXT("name"), message_get_res(IDS_STRING_DEFAULT_FONT), op.font_name, BUF_SIZE - 1, ini_path);
	op.left_font_size = profile_get_int(TEXT("font"), TEXT("left_font_size"), 40, ini_path);

	// Key Information
	op.key_info_count = profile_get_int(TEXT("key"), TEXT("count"), -1, ini_path);
	if (op.key_info_count == -1) {
		op.key_save = FALSE;
		op.key_info = (KEY_INFO *)mem_alloc(sizeof(KEY_INFO) * 30);
		if (op.key_info == NULL) {
			return FALSE;
		}
	
		op.key_info_count = 0;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F1;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 26;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F2;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 41;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F3;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 45;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F4;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 60;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F5;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 81;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F6;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 85;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F7;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 100;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F8;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 43;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F1;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 55;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F2;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 83;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F3;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 95;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F4;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 121;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F5;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 125;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F6;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 140;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F7;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_ACCEL_INPUT_SCORE + 180;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F8;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_ONE;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = '1';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_TWO;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = '2';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_THREE;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = '3';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_MIDDLE;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = 'M';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_SCORE_LEFT;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = 'T';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_NEW_GAME;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = 'N';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_GAME_HISTORY;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = 'H';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_ARRANGE;
		op.key_info[op.key_info_count].ctrl = FCONTROL;
		op.key_info[op.key_info_count].key = 'A';
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_SCORE_LEFT;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F9;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_MIDDLE;
		op.key_info[op.key_info_count].ctrl = FSHIFT;
		op.key_info[op.key_info_count].key = VK_F9;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_ONE;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F10;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_TWO;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F11;
		op.key_info_count++;

		op.key_info[op.key_info_count].action = ID_MENUITEM_FINISH_THREE;
		op.key_info[op.key_info_count].ctrl = 0;
		op.key_info[op.key_info_count].key = VK_F12;
		op.key_info_count++;


	} else {
		op.key_save = TRUE;
		op.key_info = (KEY_INFO*)mem_alloc(sizeof(KEY_INFO) * op.key_info_count);
		if(op.key_info == NULL) {
			return FALSE;
		}
		for (i = 0; i < op.key_info_count; i++) {
			wsprintf(buf, TEXT("action_%d"), i);
			op.key_info[i].action = profile_get_int(TEXT("key"), buf, 0, ini_path);
			wsprintf(buf, TEXT("ctrl_%d"), i);
			op.key_info[i].ctrl = profile_get_int(TEXT("key"), buf, 0, ini_path);
			wsprintf(buf, TEXT("key_%d"), i);
			op.key_info[i].key = profile_get_int(TEXT("key"), buf, 0, ini_path);
		}
	}


	// Player game information
	op.opi.large_font = profile_get_int(TEXT("player"), TEXT("large_font"), 0, ini_path);

	op.opi.name = profile_get_int(TEXT("player"), TEXT("name"), 1, ini_path);
	op.opi.first = profile_get_int(TEXT("player"), TEXT("first"), 1, ini_path);



	profile_free();
	return TRUE;
}

static void put_color(const TCHAR *ini_path, const TCHAR *key, const COLORREF color, const COLORREF def_color) 
{
	TCHAR buf[BUF_SIZE];

	if (def_color == color) {
		profile_write_string(TEXT("color"), key, TEXT(""), ini_path);
	} else {
		wsprintf(buf, TEXT("0x%061X"), color);
		profile_write_string(TEXT("color"), key, buf, ini_path);
	}
}

BOOL ini_put_option(const TCHAR *ini_path)
{
	TCHAR buf[BUF_SIZE];
	int i;

	profile_initialize(ini_path, TRUE);

	profile_write_int(TEXT("window"), TEXT("left"), op.window_rect.left, ini_path);
	profile_write_int(TEXT("window"), TEXT("top"), op.window_rect.top, ini_path);
	profile_write_int(TEXT("window"), TEXT("right"), op.window_rect.right, ini_path);
	profile_write_int(TEXT("window"), TEXT("bottom"), op.window_rect.bottom, ini_path);
	profile_write_int(TEXT("window"), TEXT("state"), op.window_state, ini_path);

	profile_write_int(TEXT("game"), TEXT("check_out_mode"), op.check_out_mode, ini_path);

	profile_write_int(TEXT("game"), TEXT("name_list_count"), op.name_list_count, ini_path);
	for (i = 0; i < op.name_list_count; i++) {
		wsprintf(buf, TEXT("name_list_%d"), i);
		profile_write_string(TEXT("game"), buf, op.name_list[i], ini_path);
	}

	profile_write_int(TEXT("view"), TEXT("player"), op.view_player, ini_path);
	profile_write_int(TEXT("view"), TEXT("left"), op.view_left, ini_path);

	profile_write_string(TEXT("font"), TEXT("name"), op.font_name, ini_path);
	profile_write_int(TEXT("font"), TEXT("left_font_size"), op.left_font_size, ini_path);
	
	profile_write_int(TEXT("view"), TEXT("guide"), op.view_guide, ini_path);
	profile_write_int(TEXT("view"), TEXT("throw_count"), op.view_throw_count, ini_path);
	profile_write_int(TEXT("view"), TEXT("ton_circle"), op.view_ton_circle, ini_path);
	profile_write_int(TEXT("view"), TEXT("separate"), op.view_separate, ini_path);
	profile_write_int(TEXT("view"), TEXT("scroll_bar"), op.view_scroll_bar, ini_path);

	// Player Information
	profile_write_int(TEXT("player"), TEXT("large_font"), op.opi.large_font, ini_path);
	profile_write_int(TEXT("player"), TEXT("scroll"), op.opi.scroll, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_per_round"), op.opi.avg_per_round, ini_path);
	
	profile_write_int(TEXT("player"), TEXT("name"), op.opi.name, ini_path);
	profile_write_int(TEXT("player"), TEXT("first"), op.opi.first, ini_path);

	profile_write_int(TEXT("player"), TEXT("total_sets"), op.opi.total_sets, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_legs"), op.opi.total_legs, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_tons"), op.opi.total_tons, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_100"), op.opi.total_100, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_140"), op.opi.total_140, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_180s"), op.opi.total_180s, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_high_off"), op.opi.total_high_off, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_short"), op.opi.total_short, ini_path);
	profile_write_int(TEXT("player"), TEXT("total_long"), op.opi.total_long, ini_path);

	profile_write_int(TEXT("player"), TEXT("avg_score"), op.opi.avg_score, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_darts"), op.opi.avg_darts, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_first9"), op.opi.avg_first9, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_check_out"), op.opi.avg_check_out, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_check_out_count"), op.opi.avg_check_out_count, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_keep"), op.opi.avg_keep, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_keep_count"), op.opi.avg_keep_count, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_break"), op.opi.avg_break, ini_path);
	profile_write_int(TEXT("player"), TEXT("avg_break_count"), op.opi.avg_break_count, ini_path);

	if (op.key_save == TRUE) {
		profile_write_int(TEXT("key"), TEXT("count"), op.key_info_count, ini_path);
		for (i = 0; i < op.key_info_count; i++) {
			wsprintf(buf, TEXT("action_%d"), i);
			profile_write_int(TEXT("key"), buf, op.key_info[i].action, ini_path);
			wsprintf(buf, TEXT("ctrl_%d"), i);
			profile_write_int(TEXT("key"), buf, op.key_info[i].ctrl, ini_path);
			wsprintf(buf, TEXT("key_%d"), i);
			profile_write_int(TEXT("key"), buf, op.key_info[i].key, ini_path);
		}
	}

	profile_flush(ini_path);
	profile_free();
	return TRUE;
}

BOOL ini_put_game_option(const TCHAR *ini_path) 
{
	profile_initialize(ini_path, TRUE);

	profile_write_int(TEXT("game"), TEXT("start_score"), op.gi.start_score, ini_path);
	profile_write_int(TEXT("game"), TEXT("round_limit"), op.gi.round_limit, ini_path);
	profile_write_int(TEXT("game"), TEXT("round"), op.gi.round, ini_path);

	profile_write_int(TEXT("game"), TEXT("leg_limit"), op.gi.leg_limit, ini_path);
	profile_write_int(TEXT("game"), TEXT("max_leg"), op.gi.max_leg, ini_path);
	profile_write_int(TEXT("game"), TEXT("best_of"), op.gi.best_of, ini_path);
	profile_write_int(TEXT("game"), TEXT("change_first"), op.gi.change_first, ini_path);

	profile_write_string(TEXT("game"), TEXT("p1_name"), op.gi.player_name[0], ini_path);
	profile_write_int(TEXT("game"), TEXT("p1_start_score"), op.gi.player_start_score[0], ini_path);
	profile_write_int(TEXT("game"), TEXT("p1_com"), op.gi.com[0], ini_path);
	profile_write_int(TEXT("game"), TEXT("p1_com_level"), op.gi.level[0], ini_path);

	profile_write_string(TEXT("game"), TEXT("p2_name"), op.gi.player_name[1], ini_path);
	profile_write_int(TEXT("game"), TEXT("p2_start_score"), op.gi.player_start_score[1], ini_path);
	profile_write_int(TEXT("game"), TEXT("p2_com"), op.gi.com[1], ini_path);
	profile_write_int(TEXT("game"), TEXT("p2_com_level"), op.gi.level[1], ini_path);

	profile_flush(ini_path);
	profile_free();
	return TRUE;
}
