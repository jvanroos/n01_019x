/* Include Files */
#include <windows.h>
#include <commctrl.h>

#include "general.h"
#include "ini.h"
#include "memory.h"
#include "message.h"
#include "score_left.h"
#include "resource.h"
#include "game_option.h"
#include "score_info.h"
#include "score_player.h"

/* Define */
#define MAIN_WND_CLASS				TEXT("n01_wnd")
#define WINDOW_TITLE				TEXT("n01")

#define INI_FILE					TEXT("n01.ini")
/* Global Variables */	
HINSTANCE hInst;

OPTION_INFO 	op;					// defined in general.h
SCORE_INFO		si;

TCHAR work_path[MAX_PATH];
TCHAR ini_path[MAX_PATH];

typedef struct _WINDOW_INFO {
	HWND	hWnd;
	HWND	score_list_wnd;
	HWND	score_left_wnd[2];
	HWND	score_player_wnd[2];
	HWND	score_guide_wnd;
} WINDOW_INFO;

/* Local Function Prototypes */
static LRESULT CALLBACK MainWndProc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);
static void 			get_path(const HINSTANCE hInstance);


/**
 Main Window Procedure - Handles the messages
**/
static LRESULT CALLBACK MainWndProc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WINDOW_INFO wi;

	RECT rect;
	int left, right;
	int left_height, guide_height;
	int i;

	switch (msg) {
		case WM_CREATE:

			wi.hWnd = hWnd;
			wi.score_left_wnd[0] = score_left_create(hInst, hWnd, 0, &si.player[0]);
			wi.score_left_wnd[1] = score_left_create(hInst, hWnd, 0, &si.player[1]);
			wi.score_player_wnd[0] = score_player_create(hInst, hWnd, 0, &si.player[0]);
			wi.score_player_wnd[1] = score_player_create(hInst, hWnd, 0, &si.player[1]);

			if (op.view_player == 1) {
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_MENUITEM_SHOW_PLAYER, MF_CHECKED);
				ShowWindow(wi.score_player_wnd[0], SW_SHOW);
				ShowWindow(wi.score_player_wnd[1], SW_SHOW);
			}

			if (op.view_left == 1) {
				CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_MENUITEM_SHOW_LEFT, MF_CHECKED);
				ShowWindow(wi.score_left_wnd[0], SW_SHOW);
				ShowWindow(wi.score_left_wnd[1], SW_SHOW);

				// TO DO: replace this to ID_MENU_ITEM_OPTION 
				SendMessage(wi.score_left_wnd[0], WM_LEFT_DRAW_INIT, 0, 0);
			}

			break;

		case WM_EXITSIZEMOVE:
			if (IsWindowVisible(hWnd) != 0 && IsIconic(hWnd) == 0 && IsZoomed(hWnd) == 0) {
				GetWindowRect(hWnd, (LPRECT)&op.window_rect);
				op.window_rect.right -= op.window_rect.left;
				op.window_rect.bottom -= op.window_rect.top;
			}
			break;

		case WM_SIZE:
			op.window_state = (IsZoomed(hWnd) == 0) ? SW_SHOWDEFAULT : SW_MAXIMIZE;
			
			GetClientRect(hWnd, &rect);
			left = 0;
			right = rect.right;
			left_height = 0;
			guide_height = 10;
			
			if (op.view_left == 1) {
				left_height = SendMessage(wi.score_left_wnd[0], WM_LEFT_GET_HEIGHT, rect.right/2, (LPARAM)&i);
				SendMessage(wi.score_left_wnd[0], WM_LEFT_SET_FONT_SIZE, 0, i);
				SendMessage(wi.score_left_wnd[1], WM_LEFT_SET_FONT_SIZE, 0, i);

				MoveWindow(wi.score_left_wnd[0], 0, rect.bottom - left_height - guide_height,
					rect.right / 2 - 2, left_height, TRUE);
				MoveWindow(wi.score_left_wnd[1], rect.right / 2 + 2, rect.bottom - left_height - guide_height,
					rect.right / 2 - 2, left_height, TRUE);
				left_height += 4;
			}

			if (op.view_player == 1) {
				i = (rect.right * 100) / 5;
				MoveWindow(wi.score_player_wnd[0], 0, 0,
					i / 100, rect.bottom - left_height - guide_height, TRUE);
				MoveWindow(wi.score_player_wnd[1], i * 4 / 100, 0,
					i / 100, rect.bottom - left_height - guide_height, TRUE);
				left = i / 100 + 4;
				right = i * 3 / 100 - 8;
			}
			break;

		case WM_CLOSE:
			if (MessageBox(hWnd, message_get_res(IDS_STRING_EXIT), APP_NAME, MB_ICONQUESTION | MB_YESNO) == IDNO) {
				break;
			}
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			ini_put_option(ini_path);
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {

				case ID_MENUITEM_NEW_GAME:
					if (show_game_option(hInst, hWnd, &op.gi) == FALSE) {
						break;
					}
					SendMessage(hWnd, WM_N01_GAME_ON, FALSE, 0);
					break;

				case ID_MENUITEM_SHOW_LEFT:
					op.view_left = !op.view_left;
					ShowWindow(wi.score_left_wnd[0], (op.view_left == 1) ? SW_SHOW : SW_HIDE);
					ShowWindow(wi.score_left_wnd[1], (op.view_left == 1) ? SW_SHOW : SW_HIDE);
					CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_MENUITEM_SHOW_LEFT, (op.view_left == 1) ? MF_CHECKED : MF_UNCHECKED); 
					SendMessage(hWnd, WM_SIZE, 0, 0);
					break;  

				case ID_MENUITEM_SHOW_PLAYER:
					op.view_player = !op.view_player;
					ShowWindow(wi.score_player_wnd[0], (op.view_player == 1) ? SW_SHOW : SW_HIDE);
					ShowWindow(wi.score_player_wnd[1], (op.view_player == 1) ? SW_SHOW : SW_HIDE);
					CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_MENUITEM_SHOW_PLAYER, (op.view_player == 1) ? MF_CHECKED : MF_UNCHECKED);
					SendMessage(hWnd, WM_SIZE, 0, 0);
					break;			

				case WM_WINDOW_SET_CURRENT:
					SendMessage(wi.score_left_wnd[wParam], WM_LEFT_SET_CURRENT, TRUE, 0);
					SendMessage(wi.score_left_wnd[!wParam], WM_LEFT_SET_CURRENT, FALSE, 0);
					break;

				case ID_MENUITEM_ABOUT:
					MessageBox(hWnd,
						APP_NAME
						TEXT("\nCopyright (C) 2025- by Johnny Van Roosbroeck. All rights reserved.\n\n")
						TEXT("Website: http://www.speedplay.be/\nE-MAIL: info@speedplay.be"),
						TEXT("About"), MB_OK | MB_ICONINFORMATION);
					break;

				case ID_MENUITEM_EXIT:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			break;

		case WM_WINDOW_REDRAW:
			SendMessage(wi.score_left_wnd[wParam], WM_LEFT_REDRAW, 0, 0);
			SendMessage(wi.score_player_wnd[wParam], WM_PLAYER_REDRAW, 0, 0);
			break;

		case WM_N01_GAME_ON:
			si.set_mode = wParam;
			si.current_set = 0;
			if (score_info_init(hWnd, &si, (wParam == TRUE) ? &op.gi_list[si.current_set] : &op.gi, TRUE) == FALSE) {
				return FALSE;
			}
			SendMessage(wi.score_player_wnd[0], WM_PLAYER_SET_MODE, (si.leg_limit == 1) ? TRUE : FALSE, 0);
			SendMessage(wi.score_player_wnd[1], WM_PLAYER_SET_MODE, (si.leg_limit == 1) ? TRUE : FALSE, 0);

			// TO DO: remove this line
			SendMessage(wi.score_left_wnd[0], WM_LEFT_REDRAW, 0, 0);
			SendMessage(wi.score_left_wnd[1], WM_LEFT_REDRAW, 1, 0);
			break;

		case WM_WINDOW_SET_FIRST:
			SendMessage(wi.score_player_wnd[0], WM_WINDOW_SET_FIRST, (si.leg[si.current_leg].first == 0), 0);
			SendMessage(wi.score_player_wnd[1], WM_WINDOW_SET_FIRST, (si.leg[si.current_leg].first == 1), 0);

			if (wParam == TRUE) {
				SendMessage(wi.score_player_wnd[0], WM_PLAYER_REDRAW, 0, 0);
				SendMessage(wi.score_player_wnd[1], WM_PLAYER_REDRAW, 0, 0);
			}
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

/**
 InitInstance
**/
static HWND InitInstance(HINSTANCE hInstance, int CmdShow)
{
	HWND hWnd = NULL;
	RECT rect;
	
	if (op.window_rect.left == 0 && op.window_rect.top == 0 &&
		op.window_rect.right == 0 && op.window_rect.bottom == 0) {
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		rect.right -= rect.left;
		rect.bottom -= rect.top;
	} else {
		SetRect(&rect, op.window_rect.left, op.window_rect.top, 
			op.window_rect.right, op.window_rect.bottom);
	}

	hWnd = CreateWindowEx(0,
		MAIN_WND_CLASS,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		rect.left, 
		rect.top, 
		rect.right, 
		rect.bottom, 
		NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		return NULL;
	}

	ShowWindow(hWnd, CmdShow);
	UpdateWindow(hWnd);
	return hWnd;
}

static void get_path(const HINSTANCE hInstance)
{
	TCHAR *p, *r;

	GetModuleFileName(hInstance, work_path, MAX_PATH - 1);
	for(p = r = work_path; *p != TEXT('\0'); p++) {
#ifndef UNICODE
		if(IsDBCSLeadByte((BYTE)*p) == TRUE) {
			p++;
			continue;
		}
#endif
		if(*p == TEXT('\\') || *p == TEXT('/')) {
				r = p;
		}
	}
	*r = TEXT('\0');

	wsprintf(ini_path, TEXT("%s\\%s"), work_path, INI_FILE);
}


/**
 Init Application
**/
static BOOL InitApplication(const HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = MAIN_WND_CLASS;
	return RegisterClass(&wc);
}

/**
 WINMAIN
**/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){

	MSG msg;
	HWND hWnd;
	HANDLE hAccel;
//	TCHAR path[MAX_PATH];
	TCHAR err_str[MAX_PATH];

	hInst = hInstance;
	
	InitCommonControls();

	get_path(hInst);
	if (ini_get_option(ini_path) == FALSE) {
		message_get_error(GetLastError(), err_str);
		MessageBox(NULL, err_str, APP_NAME, MB_ICONERROR);
		return 0;
	}
	
	if (score_left_regist(hInstance) == FALSE ||
		score_player_regist(hInstance) == FALSE) {
		message_get_error(GetLastError(), err_str);
		MessageBox(NULL, err_str, APP_NAME, MB_ICONERROR);
		return 0;
	}

	if (InitApplication(hInstance) == FALSE) {
		return 0;
	}

	if ((hWnd = InitInstance(hInstance,  op.window_state)) == NULL) {
		return 0;
	}

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	while (GetMessage(&msg, NULL, 0, 0) == TRUE) {
		if (TranslateAccelerator(hWnd, hAccel, &msg) == TRUE) {
			continue;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

#ifdef _DEBUG
	mem_debug();
#endif
	return msg.wParam;
}

