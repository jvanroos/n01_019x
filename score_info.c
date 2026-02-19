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
	
	for (i = 0; i < 2; i++) {
		if (p_init == TRUE || *gi->player_name[i] != TEXT('\0') || gi->com[i] == TRUE) {
			lstrcpy(si->player[i].name, gi->player_name[i]);
		}
		si->player[i].start_score = gi->player_start_score[i];

	}

	score_info_free(si);

	return TRUE;
}
