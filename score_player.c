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

#define CHAR_COUNT		10
#define LARGE_COUNT		6
#define HISTORY_COUNT	14

#define CHAR_MIN_SIZE	9

/* Global Variables */
extern HINSTANCE hInst;
extern OPTION_INFO op;

typedef struct _DRAW_BUFFER {
	int top;
	int height;

	HDC draw_dc;
	HBITMAP draw_bmp;
	HBITMAP draw_ret_bmp;
	int bmp_height;

	HDC name_dc;
	HBITMAP name_bmp;
	HBITMAP name_ret_bmp;
	int name_height;

	HFONT name_font;
	HFONT info_font;
	int font_width;
	int font_height;
	HFONT small_font;
	int small_font_height;
	HFONT large_font;
	int large_font_height;

	int height_margin;

	HBRUSH back_brush;
	HBRUSH name_back_brush;

	BOOL first;
	BOOL set_mode;
	BOOL lock;
	BOOL show_all;
	BOOL history;
	BOOL option;

	BOOL top_button;
	BOOL bottom_button;

	PLAYER_INFO *pi;
	STATISTICS_INFO *set_stat;
} DRAW_BUFFER;

/* Local Function Prototypes */
static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf);

static LRESULT CALLBACK score_player_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf) 
{
	HDC hdc;
	RECT rect;
	HFONT ret_font;
	TEXTMETRIC tm;
	int font_size;
	int small_font_size;
	int large_font_size;
	int char_count;

	// Get Client rectangle 
	GetClientRect(hWnd, &rect);
	hdc = GetDC(hWnd);

	if (bf->history == TRUE) {
		char_count = HISTORY_COUNT;
	} else {
		char_count = CHAR_COUNT;
	}

	if (op.opi.name != 0 || bf->show_all == TRUE) {
		font_size = (rect.right / char_count < CHAR_MIN_SIZE) ? CHAR_MIN_SIZE : rect.right / char_count;
	}
	
	return TRUE;
}

static LRESULT CALLBACK score_player_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	static STATISTICS_INFO tmp_stat;
	DRAW_BUFFER *bf;
	PLAYER_INFO *pi;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	POINT apos;

	switch(msg) {
		case WM_CREATE:
			// retrieve PLAYER_INFO from LPARAM
			pi = (PLAYER_INFO *)((CREATESTRUCT *)lParam)->lpCreateParams;

			// Reserve space for the draw buffer
			bf = (DRAW_BUFFER *)mem_calloc(sizeof(DRAW_BUFFER));
			if(bf == NULL) {
				return -1;
			}

			// copy PLAYER_INFO to buffer->player_info
			bf->pi = pi;

			if (bf->pi != NULL) {
				bf->set_stat = &bf->pi->set_stat;
			} else {
				bf->set_stat = &tmp_stat;
			}

			if (op.opi.scroll == 0) {
				bf->lock = TRUE;
			}

			hdc = GetDC(hWnd);
			bf->draw_dc = CreateCompatibleDC(hdc);
			bf->name_dc = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			bf->back_brush = CreateSolidBrush(RGB(255,255,255)); // TO DO: change to op.ci.player_background
			bf->name_back_brush = CreateSolidBrush(RGB(255,0,0)); // TO DO: op.ci.player_name_background
			
			// draw_init(hWnd, bf);
			// draw_player(hWnd, bf);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)bf);
			break;

		case WM_PLAYER_SET_MODE:
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			break;

		case WM_SIZE:
			break;

		case WM_PAINT:
			break;

		case WM_PLAYER_REDRAW:
			break;

		case WM_PLAYER_DRAW_INIT:
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
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
