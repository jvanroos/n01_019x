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
#include <imm.h>

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
			SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lParam);
			
			// Position window in center of parent window
			GetWindowRect(GetParent(hDlg), &parent_rect);
			GetWindowRect(hDlg, &rect);
			left = parent_rect.left + ((parent_rect.right - parent_rect.left) - (rect.right - rect.left)) / 2;
			if (left < 0) left = 0;
			top = parent_rect.top + ((parent_rect.bottom - parent_rect.top) - (rect.bottom - rect.top)) / 2;
			if (top < 0) top = 0;
			SetWindowPos(hDlg, 0, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

			// Set the ranges of the controls
			SendDlgItemMessage(hDlg, IDC_SPIN_SCORE, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 2));
			SendDlgItemMessage(hDlg, IDC_SPIN_ROUND, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 1));
			SendDlgItemMessage(hDlg, IDC_SPIN_LEG, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 1));
			SendDlgItemMessage(hDlg, IDC_SPIN_P1_SCORE, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 2));
			SendDlgItemMessage(hDlg, IDC_SPIN_P2_SCORE, UDM_SETRANGE, 0, (LPARAM)MAKELONG(UD_MAXVAL, 2));

			SendDlgItemMessage(hDlg, IDC_EDIT_SCORE, EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hDlg, IDC_EDIT_ROUND, EM_LIMITTEXT, 4, 0);
			SendDlgItemMessage(hDlg, IDC_EDIT_LEG, EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hDlg, IDC_EDIT_P1_SCORE, EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hDlg, IDC_EDIT_P2_SCORE, EM_LIMITTEXT, 5, 0);
			ImmAssociateContext(GetDlgItem(hDlg, IDC_EDIT_SCORE), (HIMC)NULL);
			ImmAssociateContext(GetDlgItem(hDlg, IDC_EDIT_ROUND), (HIMC)NULL);
			ImmAssociateContext(GetDlgItem(hDlg, IDC_EDIT_P1_SCORE), (HIMC)NULL);
			ImmAssociateContext(GetDlgItem(hDlg, IDC_EDIT_P2_SCORE), (HIMC)NULL);
			ImmAssociateContext(GetDlgItem(hDlg, IDC_EDIT_LEG), (HIMC)NULL);
			
			SetDlgItemInt(hDlg, IDC_EDIT_SCORE, gi->start_score, FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SCORE), FALSE);

			switch(gi->start_score) {
				case 301:
					CheckDlgButton(hDlg, IDC_RADIO_301, 1);
					break;
				case 501:
					CheckDlgButton(hDlg, IDC_RADIO_501, 1);
					break;
				case 1001:
					CheckDlgButton(hDlg, IDC_RADIO_1001, 1);
					break;
				default:
					CheckDlgButton(hDlg, IDC_RADIO_ETC, 1);
					EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SCORE), TRUE);
				break;
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg, FALSE);
			break;
		
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_RADIO_301:
				case IDC_RADIO_501:
				case IDC_RADIO_1001:
				case IDC_RADIO_ETC:
					EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SCORE), FALSE);
					if (IsDlgButtonChecked(hDlg, IDC_RADIO_301) == 1) {
						SetDlgItemInt(hDlg, IDC_EDIT_ROUND, 10, FALSE);
					}else if (IsDlgButtonChecked(hDlg, IDC_RADIO_501) == 1) {
						SetDlgItemInt(hDlg, IDC_EDIT_ROUND, 15, FALSE);
					} else if (IsDlgButtonChecked(hDlg, IDC_RADIO_1001) == 1) {
						SetDlgItemInt(hDlg, IDC_EDIT_ROUND, 30, FALSE);
					} else {
						EnableWindow(GetDlgItem(hDlg, IDC_EDIT_SCORE), TRUE);
						SetDlgItemInt(hDlg, IDC_EDIT_ROUND, MAX_ROUND, FALSE);
					}
					break;

				case IDCANCEL:
					SendMessage(hDlg, WM_CLOSE, 0, 0);
					break;

				case IDOK:
				case IDC_BUTTON_SAVE:
					gi = (GAME_INFO *)GetWindowLongPtr(hDlg, GWLP_USERDATA);
					if (gi == NULL) {
						EndDialog(hDlg, TRUE);
						break;
					}

					tmp_gi = *gi;
					ZeroMemory(gi, sizeof(GAME_INFO));

					if (IsDlgButtonChecked(hDlg, IDC_RADIO_301) == 1) {
						gi->start_score = 301;
					} else if (IsDlgButtonChecked(hDlg, IDC_RADIO_501) == 1) {
						gi->start_score = 501;
					} else if (IsDlgButtonChecked(hDlg, IDC_RADIO_1001) == 1) {
						gi->start_score = 1001;
					} else {
						gi->start_score = GetDlgItemInt(hDlg, IDC_EDIT_SCORE, NULL, FALSE);
						if (gi->start_score < 2) {
							gi->start_score = 2;
						}
						if (gi->start_score > 99999) {
							gi->start_score = 99999;
						}
					}

					if(LOWORD(wParam) == IDC_BUTTON_SAVE) {
						ini_put_game_option(ini_path);
						*gi = tmp_gi;
						MessageBox(hDlg, message_get_res(IDS_STRING_OP_GAME_SAVE), APP_NAME, MB_ICONINFORMATION);
					} else {
						EndDialog(hDlg, TRUE);
					}
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


