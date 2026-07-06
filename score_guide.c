/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef	_INC_OLE
#include <commctrl.h>
#include <tchar.h>

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"
#include "font.h"
#include "score_guide.h"

#include "resource.h"

/* Define */
#define WINDOW_CLASS				TEXT("score_guide_wnd")

static LRESULT CALLBACK score_guide_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		
	default: 
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

BOOL score_guide_regist(const HINSTANCE hInstance) {
	WNDCLASS wc;

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = (WNDPROC)score_guide_proc;
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

HWND score_guide_create(const HINSTANCE hInstance, const HWND pWnd, int id) {
	HWND hWnd;

	hWnd = CreateWindow(WINDOW_CLASS,
		TEXT(""),
		WS_CHILD,
		0, 0, 0, 0, pWnd, (HMENU) id, hInstance, 0);
	return hWnd;
}
