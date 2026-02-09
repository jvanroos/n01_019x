/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef	_INC_OLE
#include <tchar.h>
#include <stdio.h>

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"
#include "font.h"
#include "score_player.h"

#include "resource.h"

/* Define */
#define WINDOW_CLASS	TEXT("score_player_wnd")

/* Local Function Prototypes */
static LRESULT CALLBACK score_player_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK score_player_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL score_player_regist(const HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = 0;
	
	wc.lpfnWndProc = (WNDPROC)score_player_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS;

	return RegisterClass(&wc);
}


HWND score_player_create(const HINSTANCE hInstance, const HWND pWnd, int id, PLAYER_INFO *pi) 
{
	HWND hWnd;

	hWnd = CreateWindow(WINDOW_CLASS, 
		TEXT(""),
		WS_BORDER | WS_CHILD,
		0, 0, 0, 0,
		pWnd, (HMENU)id, hInstance, pi);
	return hWnd;
}
