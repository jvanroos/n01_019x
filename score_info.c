/*
 * n01
 *
 * score_info.c
 *
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"

#include "resource.h"

/* Define */

/* Global Variables */
extern OPTION_INFO op;

/* Local Function Prototypes */

BOOL score_info_free(SCORE_INFO *si)
{
	int i;

	if (si->leg != NULL) {
		for (i = 0; i <= si->current_leg; i++) {
			mem_free((void *)&si->leg[i].score[0]);
			mem_free((void *)&si->leg[i].score[1]);
		}
		mem_free((void *)&si->leg);
	}
	mem_free((void *)&si->tmp_check_out[0]);
	mem_free((void *)&si->tmp_check_out[1]);
	si->current_leg = 0;

	return TRUE;
}


BOOL score_info_init(const HWND hWnd, SCORE_INFO *si, GAME_INFO *gi, const BOOL p_init) 
{
	TCHAR err_str[BUF_SIZE];
	int first = 0;
	int i;

	si->history = FALSE;
	si->start_score = gi->start_score;
	si->round_limit = gi->round_limit;
	si->round = gi->round;
	si->leg_limit = gi->leg_limit;
	si->max_leg = gi->max_leg;
	si->best_of = gi->best_of;
	
	// Option settings
	for (i = 0; i < 2; i++) {
		if (p_init == TRUE) {
			ZeroMemory(&si->player[i], sizeof(PLAYER_INFO));
		}else {
			si->player[i].set_stat.ton_count += si->player[i].stat.ton_count;
			si->player[i].set_stat.ton00_count += si->player[i].stat.ton00_count;
			si->player[i].set_stat.ton40_count += si->player[i].stat.ton40_count;
			si->player[i].set_stat.ton80_count += si->player[i].stat.ton80_count;
			si->player[i].set_stat.high_off = (TYPE_SCORE) ((si->player[i].stat.high_off > si->player[i].set_stat.high_off) ?
				si->player[i].stat.high_off : si->player[i].set_stat.high_off);

			if (si->set_mode == TRUE && op.gi_list[si->current_set].start_score != op.gi_list[si->current_set - 1].start_score) {
				si->player[i].set_stat.short_game = 0;
			} else if (si->player[i].stat.short_game > 0 && 
				(si->player[i].set_stat.short_game == 0 || si->player[i].stat.short_game < si->player[i].set_stat.short_game)) {
				si->player[i].set_stat.short_game = si->player[i].stat.short_game;
			}

			if (si->set_mode == TRUE && op.gi_list[si->current_set].start_score != op.gi_list[si->current_set - 1].start_score) {
				si->player[i].set_stat.long_game = 0;
			} else if (si->player[i].stat.long_game > si->player[i].set_stat.long_game) {
				si->player[i].set_stat.long_game = si->player[i].stat.long_game;
			}

			// Avg score
			si->player[i].set_stat.all_score += si->player[i].stat.all_score;
			si->player[i].set_stat.all_darts += si->player[i].stat.all_darts;

		
		}
		
		ZeroMemory(&si->player[i].stat, sizeof(STATISTICS_INFO));

		if (p_init == TRUE || *gi->player_name[i] != TEXT('\0') || gi->com[i] == TRUE) {
			lstrcpy(si->player[i].name, gi->player_name[i]);
		}

		si->player[i].start_score = gi->player_start_score[i];
		if (si->player[i].start_score <=0) {
			si->player[i].start_score = gi->start_score;	
		}
		si->player[i].left = si->player[i].start_score;
		si->player[i].legs = 0;
	}
	
	if (*si->player[0].name == TEXT('\0')) {
		message_copy_res(IDS_STRING_PLAYER1, si->player[0].name);
	}
	if (*si->player[1].name == TEXT('\0')) {
		message_copy_res(IDS_STRING_PLAYER2, si->player[1].name);
	}

	score_info_free(si);

	return TRUE;
}
