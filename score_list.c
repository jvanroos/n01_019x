/*
 * n01
 *
 * score_list.c
 *
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef	_INC_OLE

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"
#include "font.h"

#include "resource.h"

/* Define */
#define WINDOW_CLASS				TEXT("score_list_wnd")

/* Global Variables */
extern HINSTANCE hInst;
extern OPTION_INFO op;
extern SCORE_INFO si;

BOOL score_list_regist(const HINSTANCE hInstance)
{
	return TRUE;
}

HWND score_list_create(const HINSTANCE hInstance, const HWND pWnd, int id, SCORE_INFO *si)
{
	return NULL;
}
