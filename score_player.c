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
#define SMALL_MIN_SIZE	9
#define LARGE_MIN_SIZE	14

#define LEFT_MARGIN		(rect.right / 6)
#define SCROLL_HEIGHT	bf->name_height

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
static BOOL draw_name(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_player(const HWND hWnd, DRAW_BUFFER *bf);
static int get_draw_height(const HWND hWnd, DRAW_BUFFER *bf, const BOOL arrange_flag);
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
		bf->name_font = font_create_menu(font_size, FW_BOLD, FALSE);
		ret_font = SelectObject(hdc, bf->name_font);
		GetTextMetrics(hdc, &tm);
		bf->name_height = tm.tmHeight + tm.tmHeight / 8;
		SelectObject(hdc, ret_font);

		bf->name_bmp = CreateCompatibleBitmap(hdc, rect.right, bf->name_height);
		bf->name_ret_bmp = SelectObject(bf->name_dc, bf->name_bmp);
		
		draw_name(hWnd, bf);

	} else {
		bf->name_font = NULL;
		bf->name_height = 0;
	}
	
	return TRUE;
}

static BOOL draw_name(const HWND hWnd, DRAW_BUFFER *bf) 
{
	RECT 	draw_rect, rect;
	HFONT 	ret_font;
	TCHAR 	buf[BUF_SIZE];

	if (bf->pi == NULL) 
	{
		return FALSE;
	}

	if (op.opi.name != 0 || bf->show_all == TRUE) {
		GetClientRect(hWnd, &rect);
		SetRect(&draw_rect, 0, 0, rect.right, bf->name_height);
		FillRect(bf->name_dc, &draw_rect, bf->name_back_brush);

		ret_font = SelectObject(bf->name_dc, bf->name_font);
		SetTextColor(bf->name_dc, RGB(255,255,255));	// TO DO: change to op.ci.player_name_text
		SetBkColor(bf->name_dc, RGB(64,128,255));	// TO DO: change to op.ci.player_name_background
		if (bf->pi->com == TRUE) {
			wsprintf(buf, message_get_res(IDS_STRING_COM), bf->pi->level + 1);
		} else {
			lstrcpy(buf, bf->pi->name);
		}
		DrawText(bf->name_dc, buf, lstrlen(buf), &draw_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		SelectObject(bf->name_dc, ret_font);
	}
	return TRUE;
}

static int get_draw_height(const HWND hWnd, DRAW_BUFFER *bf, const BOOL arrange_flag) {
	int height = 0;
	int font_height;

	return height;
}


static void draw_text(const HDC hdc, const TCHAR *str, const int len, const RECT *rect, const int format) {
	SIZE sz;
	int left, top;

	GetTextExtentPoint32(hdc, str, len, &sz);
	switch (format) {
		case DT_LEFT:
		default:
			left = rect->left;
			break;
		
		case DT_RIGHT:
			left = rect->right - sz.cx;
			break;

		case DT_CENTER:
			left = rect->left + (rect->right - rect->left - sz.cx) / 2;
			break;
	}
	top = rect->top + (rect->bottom - rect->top - sz.cy) / 2;
	TextOut(hdc, left, top, str, len);
}

static BOOL draw_player(const HWND hWnd, DRAW_BUFFER *bf) {
	
	RECT draw_rect, rect;
	HFONT ret_font;
	SIZE sz;
	TCHAR buf[BUF_SIZE];
	int left, right;
	int title_left;
	int height = 0;
	int i, j;

	GetClientRect(hWnd, &rect);
	rect.bottom = bf->bmp_height;
	FillRect(bf->draw_dc, &rect, bf->back_brush);

	if (bf->pi == NULL) {
		return FALSE;
	}

	SetTextColor(bf->draw_dc, RGB(0, 0, 0));	// TO DO
	SetBkColor(bf->draw_dc, RGB(100, 150, 200)); // TO DO 
	ret_font = SelectObject(bf->draw_dc, bf->info_font);

	if (op.opi.first != 0 && bf->show_all == FALSE) {
		if (bf->first == 1) {
			SetRect(&draw_rect, 0, height, rect.right, height + bf->font_height);
			draw_text(bf->draw_dc, message_get_res(IDS_STRING_P_FIRST), lstrlen(message_get_res(IDS_STRING_P_FIRST)), &draw_rect, DT_CENTER);
		}
		height += bf->font_height;
	}


	return TRUE;
}

static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf)
{	
	if (bf->draw_dc != NULL) {
		SelectObject(bf->draw_dc, bf->draw_ret_bmp);
		DeleteObject(bf->draw_bmp);
		bf->draw_bmp = NULL;
	}

	if (bf->name_dc != NULL) {
		SelectObject(bf->name_dc, bf->draw_ret_bmp);
		DeleteObject(bf->name_bmp);
		bf->name_bmp = NULL;
	}

	if (bf->name_font != NULL) {
		DeleteObject(bf->name_font);
		bf->name_font = NULL;
	}

	if (bf->info_font != NULL) {
		DeleteObject(bf->info_font);
		bf->info_font = NULL;
	}

	if (bf->small_font != NULL) {
		DeleteObject(bf->small_font);
		bf->small_font = NULL;
	}

	if (bf->large_font != NULL) {
		DeleteObject(bf->large_font);
		bf->large_font = NULL;
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
			
			draw_init(hWnd, bf);
			draw_player(hWnd, bf);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)bf);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (bf !=NULL) {
				draw_free(hWnd, bf);
				if (bf->draw_dc != NULL) {
					DeleteDC(bf->draw_dc);
					bf->draw_dc = NULL;
				}
				if (bf->name_dc != NULL) {
					DeleteDC(bf->name_dc);
					bf->name_dc = NULL;
				}
				DeleteObject(bf->back_brush);
				DeleteObject(bf->name_back_brush);
				mem_free((void *)&bf);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_SIZE:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (bf == NULL) {
				break;
			}
			draw_free(hWnd, bf);
			draw_init(hWnd, bf);
			draw_player(hWnd, bf);
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			break;

		case WM_PAINT:
			bf = (DRAW_BUFFER *) GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (bf == NULL) {
				break;
			}
			hdc = BeginPaint(hWnd, &ps);
			
			if (ps.rcPaint.top < bf->name_height) {
				BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, bf->name_height,
					bf->name_dc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				ps.rcPaint.top, bf->name_height;
			}
			BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom,
				bf->draw_dc, ps.rcPaint.left, ps.rcPaint.top + bf->top - bf->name_height, SRCCOPY);
		
			if (bf->top_button == TRUE) {
				GetClientRect(hWnd, &rect);
				rect.top = bf->name_height;
				rect.bottom = bf->name_height + SCROLL_HEIGHT;
				DrawFrameControl(hdc, &rect, DFC_SCROLL, DFCS_SCROLLUP | ((GetAsyncKeyState(VK_LBUTTON) < 0) ? DFCS_PUSHED : 0));
			}
			
			if (bf->bottom_button == TRUE) {
				GetClientRect(hWnd, &rect);
				rect.top = rect.bottom - SCROLL_HEIGHT;
				DrawFrameControl(hdc, &rect, DFC_SCROLL, DFCS_SCROLLDOWN | ((GetAsyncKeyState(VK_LBUTTON) < 0) ? DFCS_PUSHED : 0));
			}	
			EndPaint(hWnd, &ps);
			break;

		case WM_PLAYER_SET_MODE:
			break;

		case WM_PLAYER_REDRAW:
			bf = (DRAW_BUFFER *) GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (bf == NULL) {
				break;
			}

			draw_player(hWnd, bf);
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
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
		pWnd, 
		(HMENU)id, 
		hInstance, pi);
	
	return hWnd;
}
