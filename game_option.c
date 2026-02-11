/*
 * n01
 *
 * option_game.c
 *
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE
#include <commctrl.h>

#include "General.h"
#include "Message.h"
#include "ini.h"

#include "resource.h"

/* Global Variables */
extern OPTION_INFO op;
extern TCHAR ini_path[MAX_PATH];

BOOL show_game_option(const HINSTANCE hInst, const HWND hWnd, GAME_INFO *gi)
{
	return TRUE;
}


