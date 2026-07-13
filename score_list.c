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

static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

BOOL score_list_regist(const HINSTANCE hInstance)
{	
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)score_list_proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS;

	return RegisterClass(&wc);
}

HWND score_list_create(const HINSTANCE hInstance, const HWND pWnd, int id, SCORE_INFO *si)
{
	return NULL;
}
