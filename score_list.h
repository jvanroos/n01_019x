/*
 * n01
 *
 * score_list.h
 *
 */

#ifndef _INC_SCORE_LIST_H
#define _INC_SCORE_LIST_H

BOOL score_list_regist(const HINSTANCE hInstance);
HWND score_list_create(const HINSTANCE hInstance, const HWND pWnd, int id, SCORE_INFO *si);

/* Define */
#define WM_SCORE_INIT_LEG			(WM_APP + 100)
#define WM_SCORE_REDRAW				(WM_APP + 105)

#endif
