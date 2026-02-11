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

/* Local Function Prototypes */
static BOOL CALLBACK game_option_proc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * game_option_proc
 */
static BOOL CALLBACK game_option_proc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	GAME_INFO *gi, tmp_gi;
	RECT parent_rect, rect;
	TCHAR buf[BUF_SIZE];
	int left, top;
	int i;

	switch (msg)
	{
		case WM_INITDIALOG:
			if ((gi = (GAME_INFO *)lParam) == NULL) {
				EndDialog(hDlg, FALSE);
				break;
			}
			SetWindowLong(hDlg, GWL_USERDATA, lParam);
			
			break;

		case WM_CLOSE:
			EndDialog(hDlg, FALSE);
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDCANCEL:
					SendMessage(hDlg, WM_CLOSE, 0, 0);
					break;

				case IDOK:
					EndDialog(hDlg, TRUE);
					break;
			}
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

BOOL show_game_option(const HINSTANCE hInst, const HWND hWnd, GAME_INFO *gi)
{
	return DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG_GAME_OPTION), hWnd, game_option_proc, (LPARAM) gi);
}


