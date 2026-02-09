/*
 * n01
 *
 * score_player.h
 *
 */

#ifndef _INC_SCORE_PLAYER_H
#define _INC_SCORE_PLAYER_H

/* Include Files */
#include "general.h"

/* Define */

/* Struct */

/* Function Prototypes */
BOOL score_player_regist(const HINSTANCE hInstance);
HWND score_player_create(const HINSTANCE hInstance, const HWND pWnd, int id, PLAYER_INFO *pi);

#endif
