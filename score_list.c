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

#include <uiribbon.h>

#include "general.h"
#include "Memory.h"
#include "String.h"
#include "Message.h"
#include "font.h"
#include "score_list.h"
#include "score_info.h"
#include "resource.h"

/* Define */
#define WINDOW_CLASS				TEXT("score_list_wnd")

#define CHAR_COUNT 					17

/* Global Variables */
extern HINSTANCE hInst;
extern OPTION_INFO op;
extern SCORE_INFO si;

typedef struct _PREV_INFO {
	BOOL prev_flag;

	SCORE_INFO si;
	int view_leg;
	int input_x;
	int input_y;
} PREV_INFO;

typedef struct _DRAW_BUFFER {
	HWND hedit;
	int input_x;
	int input_y;

	int view_leg;
	BOOL	half;
	BOOL	lock;

	int max_y;
	int pos_y;
	int page_y;

	int input_left[2];
	int score_left[2];
	int score_right[2];

	HDC draw_dc;
	HBITMAP draw_bmp;
	HBITMAP draw_ret_bmp;

	HDC back_dc;
	HBITMAP back_bmp;
	HBITMAP back_ret_bmp;
	int back_width;
	int back_height;
	BOOL back_redraw;
	int back_first;

	// Circle
	HDC ellipse_dc;
	HBITMAP ellipse_bmp;
	HBITMAP ellipse_ret_bmp;

	// Font
	HFONT header_font;
	int header_height;
	HFONT score_font;
	int score_height;
	HFONT back_ret_font;
	HFONT draw_ret_font;

	// Brushes
	HBRUSH back_brush;
	HBRUSH odd_back_brush;
	HBRUSH header_back_brush;

	// Pens
	HPEN line_pen;
	HPEN separate_pen;
	HPEN separate_bold_pen;
	HPEN finish_pen;
	HPEN ton_circle_pen;
	int pen_size;

	// Score Info
	SCORE_INFO *si;

	// previous info
	PREV_INFO	prev_info;
	PREV_INFO 	next_info;

} DRAW_BUFFER;

// Prototypes
static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_background(const DRAW_BUFFER *bf, const int first);
static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf)
{
	HDC hdc;
	HFONT ret_font;
	TEXTMETRIC tm;
	RECT rect;
	int i;

	GetClientRect(hWnd, &rect);
	hdc = GetDC(hWnd);

	// Score Font
	bf->score_font = font_create(op.font_name, rect.right / CHAR_COUNT / ((bf->half == TRUE) ? 2 : 1), 0, FALSE, FALSE);
	ret_font = SelectObject(hdc, bf->score_font);
	GetTextMetrics(hdc, &tm);
	SelectObject(hdc, ret_font);
	bf->score_height = tm.tmHeight + tm.tmHeight / 6;
	bf->draw_bmp = CreateCompatibleBitmap(hdc, rect.right, bf->score_height);
	bf->draw_ret_bmp = SelectObject(bf->draw_dc, bf->draw_bmp);
	bf->draw_ret_font = SelectObject(bf->draw_dc, bf->score_font);

	// Header Font
	bf->header_font = font_create(op.font_name, rect.right / CHAR_COUNT / 2, 0, FALSE, FALSE);
	ret_font = SelectObject(hdc, bf->header_font);
	GetTextMetrics(hdc, &tm);
	SelectObject(hdc, ret_font);
	bf->header_height = tm.tmHeight + tm.tmHeight / 6;
	bf->back_width = rect.right;
	bf->back_height = bf->header_height + bf->score_height * 2;
	bf->back_redraw = TRUE;
	bf->back_bmp = CreateCompatibleBitmap(hdc, rect.right, bf->back_height);
	bf->back_ret_bmp = SelectObject(bf->back_dc, bf->back_bmp);
	bf->back_ret_font = SelectObject(bf->back_dc, bf->header_font);
	
	// Input fields
	i = (rect.right * 100) / CHAR_COUNT;
	bf->input_left[0] = 0;
	bf->score_left[0] = i * 3 / 100;
	bf->score_right[0] = i * 7 / 100;
	bf->input_left[1] = i * 10 / 100;
	bf->score_left[1] = i * 13 / 100;
	bf->score_right[1] = rect.right;

	// Score Ellipse
	bf->ellipse_bmp = CreateCompatibleBitmap(hdc, (bf->score_left[1] - bf->input_left[1]) * 2, bf->score_height * 2);
	bf->ellipse_ret_bmp = SelectObject(bf->ellipse_dc, bf->ellipse_bmp);

	ReleaseDC(hWnd, hdc);
	return TRUE;

}

static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf)
{
	if (bf->draw_dc != NULL) {
		SelectObject(bf->draw_dc, bf->draw_ret_bmp);
		SelectObject(bf->draw_dc, bf->draw_ret_font);
	}

	if (bf->back_dc != NULL) {
		SelectObject(bf->back_dc, bf->back_ret_bmp);
		SelectObject(bf->back_dc, bf->back_ret_font);
	}

	if (bf->ellipse_dc != NULL) {
		SelectObject(bf->ellipse_dc, bf->ellipse_ret_bmp);
	}
	
	if (bf->draw_bmp != NULL) {
		DeleteObject(bf->draw_bmp);
		bf->draw_bmp = NULL;
	}

	if (bf->back_bmp != NULL) {
		DeleteObject(bf->back_bmp);
		bf->back_bmp = NULL;
	}

	if (bf->ellipse_bmp != NULL) {
		DeleteObject(bf->ellipse_bmp);
		bf->ellipse_bmp = NULL;
	}

	if (bf->score_font != NULL) {
		DeleteObject(bf->score_font);
		bf->score_font = NULL;
	}

	if (bf->header_font != NULL) {
		DeleteObject(bf->header_font);
		bf->header_font = NULL;
	}
	return TRUE;
}

static BOOL draw_background(const DRAW_BUFFER *bf, const int first)
{	
	RECT draw_rect;
	HPEN ret_pen;
	RECT rect;
	TCHAR buf[BUF_SIZE];	
	int height;
	int j;

	SetRect(&rect, 0, 0, bf->back_width, bf->back_height);
	FillRect(bf->back_dc, &rect, bf->back_brush);

	return TRUE;
}

static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	DRAW_BUFFER *bf;
	HDC hdc;
	PAINTSTRUCT ps;

	TCHAR buf[BUF_SIZE];
	switch(msg) {
		case WM_CREATE:
			bf = (DRAW_BUFFER*)mem_calloc(sizeof(DRAW_BUFFER));
			if (bf == NULL) {
				message_get_error(GetLastError(), buf);
				MessageBox(hWnd, buf, APP_NAME, MB_ICONERROR);
				return -1;
			}

			bf->si = (SCORE_INFO *)((CREATESTRUCT *)lParam)->lpCreateParams;
			if (bf->si == NULL) {
				bf->si = &si;
			}
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)bf);

			hdc = GetDC(hWnd);
			bf->draw_dc = CreateCompatibleDC(hdc);
			bf->back_dc = CreateCompatibleDC(hdc);
			bf->ellipse_dc = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			if(GetDeviceCaps(bf->draw_dc, BITSPIXEL) >= 16) {
				SetStretchBltMode(bf->draw_dc, HALFTONE);
				SetBrushOrgEx(bf->draw_dc, 0, 0, NULL);
				bf->pen_size = 4;
			} else {
				bf->pen_size = 2;
			}
			draw_init(hWnd, bf);
			SetBkMode(bf->draw_dc, TRANSPARENT);
			bf->back_brush = CreateSolidBrush(RGB(255, 255, 255)); // TODO: op.ci.background
			bf->odd_back_brush = CreateSolidBrush(RGB(255, 255, 200)); // TODO: op.ci.odd_background
			bf->header_back_brush = CreateSolidBrush(RGB(255, 255, 100)); // TODO: op.ci.header_background
			bf->line_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255)); // TODO: op.ci.line
			bf->separate_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // TODO: op.ci.separate
			bf->separate_bold_pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // TODO: op.ci.separate
			bf->finish_pen = CreatePen(PS_SOLID, bf->pen_size, RGB(0, 255, 0)); // TODO: op.ci.scored_text;
			bf->ton_circle_pen = CreatePen(PS_SOLID, bf->pen_size, RGB(125, 125, 125)); // TODO: op.ci.ton_circle

			ImmAssociateContext(hWnd, (HIMC)NULL);

			// set_scrollbar(hWnd, bf, bf->si);

//			bf->hedit = nedit_create(hInst, hWnd, 0);
//			SendMessage(bf->hedit, EM_LIMITTEXT, INPUT_LIMIT, 0);
//			SendMessage(bf->hedit, WM_SETFONT, (WPARAM)bf->score_font, MAKELPARAM(TRUE, 0));
		break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			bf = (DRAW_BUFFER*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf != NULL) {
				draw_free(hWnd, bf);
				if (bf->draw_dc != NULL) {
					DeleteDC(bf->draw_dc);
					bf->draw_dc = NULL;
				}
				if (bf->back_dc != NULL) {
					DeleteDC(bf->back_dc);
					bf->back_dc = NULL;
				}
				if (bf->ellipse_dc != NULL) {
					DeleteDC(bf->ellipse_dc);
					bf->ellipse_dc = NULL;
				}
				DeleteObject(bf->back_brush);
				DeleteObject(bf->odd_back_brush);
				DeleteObject(bf->header_back_brush);
				DeleteObject(bf->line_pen);
				DeleteObject(bf->separate_pen);
				DeleteObject(bf->separate_bold_pen);
				DeleteObject(bf->finish_pen);
				DeleteObject(bf->ton_circle_pen);

				score_info_free(&bf->next_info.si);
				score_info_free(&bf->prev_info.si);
				mem_free((void *)&bf);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		
		case WM_PAINT:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL){
				break;
			}
			hdc = BeginPaint(hWnd, &ps);
/*
			if (bf->back_redraw == TRUE || bf->back_first != bf->si->leg[bf->view_leg].first) {
				bf->back_redraw = FALSE;
				bf->back_first = bf->si->leg[bf->view_leg].first;
				draw_background(bf, bf->back_first);
			}
*/
			EndPaint(hWnd, &ps);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
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
	HWND hWnd;

	hWnd = CreateWindow(WINDOW_CLASS, 
		TEXT(""),
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP | ((op.view_scroll_bar == 1) ? WS_VSCROLL : 0), 
		0, 0, 100, 100, pWnd, (HMENU)id, hInstance, si);
	return hWnd;
}

/* End of source */
