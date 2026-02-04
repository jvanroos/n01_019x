/*
 * n01
 *
 * score_left.c
 *
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef	_INC_OLE
#include <tchar.h>

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"
#include "score_left.h"

#include "resource.h"

/* Define */
#define WINDOW_CLASS 			TEXT("score_left_wnd")
#define BORDER_SIZE				30

/* Global Variables */
extern HINSTANCE hInst;
extern OPTION_INFO op;

typedef struct _DRAW_BUFFER {
	HDC draw_dc;
	HBITMAP draw_bmp;
	HBITMAP draw_ret_bmp;
	HFONT score_font;
	HFONT ret_font;
	int font_size;
	HRGN hrgn;
	HBRUSH back_brush;
	HBRUSH active_border_brush;
	BOOL current;
	PLAYER_INFO *pi;
} DRAW_BUFFER;


/* Function Prototypes */
static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_score(const HWND hWnd, const DRAW_BUFFER *bf);
static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf);
static LRESULT CALLBACK score_left_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

/*
 * draw_free - Release of drawing information
 */
static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf)
{
	if (bf->draw_dc != NULL) {
		SelectObject(bf->draw_dc, bf->ret_font);
		SelectObject(bf->draw_dc, bf->draw_ret_bmp);
		DeleteObject(bf->draw_bmp);
		bf->draw_bmp = NULL;
	}
	if (bf->score_font != NULL) {
		DeleteObject(bf->score_font);
		bf->score_font = NULL;
	}
	DeleteObject(bf->hrgn);
	return TRUE;
}

/*
 * draw_score - ????????
 */
static BOOL draw_score(const HWND hWnd, const DRAW_BUFFER *bf)
{

	return TRUE;
}

/*
 * draw_init - Initialization of drawing information
 */
static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf)
{

	return TRUE;
}

/*
 * score_left_proc
 *
 */
static LRESULT CALLBACK score_left_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	DRAW_BUFFER		*bf;
	PLAYER_INFO 	*pi;
	HDC				hdc;
	TEXTMETRIC		tm;
	HFONT			hfont, ret_font;
	PAINTSTRUCT		ps;
	SIZE			sz;
	int 			size;
	int				ret;

	switch(msg) {
		case WM_CREATE:
			
			pi = (PLAYER_INFO *)((CREATESTRUCT *)lParam)->lpCreateParams;
			if (pi == NULL) {
				return -1;
			}

			bf = (DRAW_BUFFER *)mem_calloc(sizeof(DRAW_BUFFER));
			if (bf == NULL) {
				return -1;
			}
			
			bf->pi = pi;

			// Drawing information
			hdc = GetDC(hWnd);
			bf->draw_dc = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			draw_init(hWnd, bf);
			SetBkMode(bf->draw_dc, TRANSPARENT);
			bf->back_brush = CreateSolidBrush(RGB(255, 200, 0));	// TODO: Nog te vervangen.
			bf->active_border_brush = CreateSolidBrush(RGB(0, 0, 255)); // TODO: Nog te vervangen.
			bf->pi->left = 501;	// TODO: Moet uit de GAME_INFO komen.
			draw_score(hWnd, bf);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)bf);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			bf = (DRAW_BUFFER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (bf != NULL) {
				draw_free(hWnd, bf);
				if (bf->draw_dc != NULL) {
					DeleteDC(bf->draw_dc);
					bf->draw_dc = NULL;
				}
				DeleteObject(bf->back_brush);
				DeleteObject(bf->active_border_brush);
				mem_free((void *)&bf);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;

}

/*
 * score_left_regist - Register score left window
 *
 */
BOOL score_left_regist(const HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC) score_left_proc;
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


/*
 * score_left_create - Create score left window
 *
 */
HWND score_left_create(const HINSTANCE hInstance, const HWND pWnd, int id, PLAYER_INFO *pi) 
{
	HWND hWnd;

	hWnd = CreateWindow(WINDOW_CLASS, 
		TEXT(""), 
		WS_BORDER | WS_CHILD,
		0, 0, 0, 0,
		pWnd,
		(HMENU)id,
		hInstance, pi); 

	return hWnd;
}
