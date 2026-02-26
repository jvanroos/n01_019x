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
static void add_name_list(const TCHAR *name);
static BOOL CALLBACK game_option_proc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * add_name_list
 */
static void add_name_list(const TCHAR *name) 
{
	int i, j;

	if (*name == TEXT('\0')) {
		return;
	}

	for (i = 0; i < op.name_list_count; i++) {
		if(*op.name_list[i] != TEXT('\0') && lstrcmpi(name, op.name_list[i]) == 0){
			for (j = i; j < op.name_list_count - 1; j++) {
				lstrcpy(op.name_list[j], op.name_list[j + 1]);
			}
			break;
		}
	}

	if (i >= op.name_list_count && op.name_list_count < NAME_LIST_COUNT) {
		op.name_list_count++;
	}
	for (j = op.name_list_count - 1; j > 0; j--) {
		lstrcpy(op.name_list[j], op.name_list[j - 1]);
	}
	lstrcpy(op.name_list[0], name);
}

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

			if (gi->round_limit == 1) {
				CheckDlgButton(hDlg, IDC_CHECK_ROUND_LIMIT, BST_CHECKED);
			}
			SetDlgItemInt(hDlg, IDC_EDIT_ROUND, gi->round, FALSE);
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK_ROUND_LIMIT, 0);
			
			// Leg
			if (gi->leg_limit == 1) {
				CheckDlgButton(hDlg, IDC_CHECK_LEG_LIMIT, BST_CHECKED);
			}
			SetDlgItemInt(hDlg, IDC_EDIT_LEG, gi->max_leg, FALSE);

			// Best of ...
			if (gi->best_of == 1) {
				CheckDlgButton(hDlg, IDC_CHECK_BEST_OF, BST_CHECKED);
			}

			// First
			if (gi->change_first == 1) {
				CheckDlgButton(hDlg, IDC_CHECK_CHANGE_FIRST, BST_CHECKED);
			}
			SendMessage(hDlg, WM_COMMAND, IDC_CHECK_LEG_LIMIT, 0);

			SendDlgItemMessage(hDlg, IDC_COMBO_P1_NAME, CB_SETEXTENDEDUI, TRUE, 0);
			SendDlgItemMessage(hDlg, IDC_COMBO_P2_NAME, CB_SETEXTENDEDUI, TRUE, 0);

			for (i = 0; i < op.name_list_count; i++) {
				if (*op.name_list[i] != TEXT('\0')) {
					SendDlgItemMessage(hDlg, IDC_COMBO_P1_NAME, CB_ADDSTRING, 0, (LPARAM)op.name_list[i]);
					SendDlgItemMessage(hDlg, IDC_COMBO_P2_NAME, CB_ADDSTRING, 0, (LPARAM)op.name_list[i]);
				}
			}
			if (gi->schedule_flag == TRUE) {
				SendDlgItemMessage(hDlg, IDC_COMBO_P1_NAME, WM_SETTEXT, 0, (LPARAM)gi->player_name[0]);
				SendDlgItemMessage(hDlg, IDC_COMBO_P2_NAME, WM_SETTEXT, 0, (LPARAM)gi->player_name[1]);
			} else {
				SendDlgItemMessage(hDlg, IDC_COMBO_P1_NAME, WM_SETTEXT, 0,
					(LPARAM)((*gi->player_name[0] != TEXT('\0')) ? gi->player_name[0] : message_get_res(IDS_STRING_PLAYER1)));
				SendDlgItemMessage(hDlg, IDC_COMBO_P2_NAME, WM_SETTEXT, 0,
					(LPARAM)((*gi->player_name[1] != TEXT('\0')) ? gi->player_name[1] : message_get_res(IDS_STRING_PLAYER2)));
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

				case IDC_CHECK_ROUND_LIMIT:
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_ROUND_LIMIT) == BST_CHECKED) {
						EnableWindow(GetDlgItem(hDlg, IDC_EDIT_ROUND), TRUE);
					} else {
						EnableWindow(GetDlgItem(hDlg, IDC_EDIT_ROUND), FALSE);
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

				// Start Score
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
					
				// Rounds
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_ROUND_LIMIT) == BST_CHECKED) {
						gi->round_limit = 1;
					}

					gi->round = GetDlgItemInt(hDlg, IDC_EDIT_ROUND, NULL, FALSE);
					if (gi->round < 1) { 
						gi->round = 1; 
					}
					if (gi->round > 9999 / 3) { 
						gi->round = 9999 / 3; 
					}

				// Legs
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_LEG_LIMIT) == BST_CHECKED) {
						gi->leg_limit = 1;
					}
					gi->max_leg = GetDlgItemInt(hDlg, IDC_EDIT_LEG, NULL, FALSE);
					if (gi->max_leg < 1) {
						gi->max_leg = 1;
					}

					if (IsDlgButtonChecked(hDlg, IDC_CHECK_BEST_OF) == BST_CHECKED) {
						gi->best_of = 1;
					}
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_CHANGE_FIRST) == BST_CHECKED) {
						gi->change_first = 1;
					}

				// Names
					SendDlgItemMessage(hDlg, IDC_COMBO_P1_NAME, WM_GETTEXT, NAME_SIZE - 1, (LPARAM)gi->player_name[0]);
					SendDlgItemMessage(hDlg, IDC_COMBO_P2_NAME, WM_GETTEXT, NAME_SIZE - 1, (LPARAM)gi->player_name[1]);
					add_name_list(gi->player_name[1]);
					add_name_list(gi->player_name[0]);

					gi->player_start_score[0] = gi->start_score;
					gi->player_start_score[1] = gi->start_score;

					if (IsDlgButtonChecked(hDlg, IDC_CHECK_P1_SCORE) == BST_CHECKED) {
						gi->player_start_score[0] = GetDlgItemInt(hDlg, IDC_EDIT_P1_SCORE, NULL, FALSE);
						if (gi->player_start_score[0] < 2 || gi->player_start_score[0] > 99999) {
							gi->player_start_score[0] = gi->start_score;
						}
					}
					if (IsDlgButtonChecked(hDlg, IDC_CHECK_P2_SCORE) == BST_CHECKED) {
						gi->player_start_score[1] = GetDlgItemInt(hDlg, IDC_EDIT_P2_SCORE, NULL, FALSE);
						if (gi->player_start_score[1] < 2 || gi->player_start_score[1] > 99999) {
							gi->player_start_score[1] = gi->start_score;
						}
					}

				// Save
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


