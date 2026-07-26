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
#include <tchar.h>

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
static void draw_text(const HDC hdc, const TCHAR *str, const int len, const RECT *rect);
static BOOL draw_background(const DRAW_BUFFER *bf, const int first);
static BOOL draw_line(DRAW_BUFFER *bf, const SCORE_INFO *si, const RECT *rect, const int round, int *left);
static void set_scrollbar(const HWND hWnd, DRAW_BUFFER *bf, const SCORE_INFO *si);
static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static void set_scrollbar(const HWND hWnd, DRAW_BUFFER *bf, const SCORE_INFO *si) 
{
	SCROLLINFO sci;
	RECT rect;

	GetClientRect(hWnd, &rect);
	bf->page_y = ((rect.bottom - bf->header_height) + 1) / bf->score_height;

	if (bf->page_y < si->leg[bf->view_leg].max_round + 1) {
		EnableScrollBar(hWnd, SB_VERT, ESB_ENABLE_BOTH);

		bf->max_y = si->leg[bf->view_leg].max_round - (bf->page_y - 1);
		bf->pos_y = (bf->pos_y < bf->max_y) ? bf->pos_y : bf->max_y;

		ZeroMemory(&sci, sizeof(SCROLLINFO));
		sci.cbSize = sizeof(SCROLLINFO);
		sci.fMask  = SIF_POS | SIF_RANGE | ((op.view_scroll_bar == 0) ? SIF_DISABLENOSCROLL : 0);
		sci.nPage = bf->page_y;
		sci.nMax = si->leg[bf->view_leg].max_round;
		sci.nPos = bf->pos_y;
		SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
	} else {
		EnableScrollBar(hWnd, SB_VERT, ESB_DISABLE_BOTH);

		bf->max_y = bf->pos_y = 0;

		ZeroMemory(&sci, sizeof(SCROLLINFO));
		sci.cbSize = sizeof(SCROLLINFO);
		sci.fMask  = SIF_POS | SIF_PAGE | SIF_RANGE | ((op.view_scroll_bar == 0) ? SIF_DISABLENOSCROLL : 0);
		sci.nMax = 1;
		SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
	}
}

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

static void draw_text(const HDC hdc, const TCHAR *str, const int len, const RECT *rect) {

	SIZE sz;
	int left, top;

	GetTextExtentPoint32(hdc, str, len, &sz);
	left = rect->left + (rect->right - rect->left - sz.cx) / 2;
	top = rect->top + (rect->bottom - rect->top - sz.cy) / 2;
	ExtTextOut(hdc, left, top, ETO_CLIPPED, rect, str, len, NULL);
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

	if (op.ci.background != op.ci.odd_background) {
		height = bf->header_height + bf->score_height;
		SetRect(&draw_rect, 0, height, bf->back_width, height + bf->score_height);
		FillRect(bf->back_dc, &draw_rect, bf->back_brush);
	}

	SetRect(&draw_rect, 0, 0, bf->back_width, bf->header_height);
	FillRect(bf->back_dc, &draw_rect, bf->header_back_brush);
	SetRect(&draw_rect, bf->score_right[0], 0, bf->input_left[1], bf->back_height);
	FillRect(bf->back_dc, &draw_rect, bf->header_back_brush);

	SetTextColor(bf->back_dc, op.ci.header_text);
	SetBkColor(bf->back_dc, op.ci.header_background);

	for(j = 0; j < 2; j++) {
		message_copy_res(IDS_STRING_SCORED, buf);
		SetRect(&draw_rect, bf->input_left[j], 0, bf->score_left[j], bf->header_height - 1);
		draw_text(bf->back_dc, buf, lstrlen(buf), &draw_rect);
		if (first == j) {
			message_copy_res(IDS_STRING_FIRST_MARK, buf);
			lstrcat(buf, TEXT(" "));
			message_copy_res(IDS_STRING_TO_GO, buf + lstrlen(buf));
		} else {
			message_copy_res(IDS_STRING_TO_GO, buf);
		}
		SetRect(&draw_rect, bf->input_left[j] + 1, 0, bf->score_right[j], bf->header_height - 1);
		draw_text(bf->back_dc, buf, lstrlen(buf), &draw_rect);
	}

	ret_pen = SelectObject(bf->back_dc, bf->line_pen);

	for (j = 0; j < 2; j++) {
		MoveToEx(bf->back_dc, bf->score_left[j], 0, NULL);
		LineTo(bf->back_dc, bf->score_left[j], bf->back_height);
	}

	return TRUE;
}

static BOOL draw_line(DRAW_BUFFER *bf, const SCORE_INFO *si, const RECT *rect, const int round, int *left_score)
{
	RECT draw_rect;
	HPEN ret_pen;
	HBRUSH ret_brush;
	HBRUSH ret_ellipse_brush;
	TCHAR buf[BUF_SIZE];
	int i,j;

	BitBlt(bf->draw_dc, rect->left, 0, rect->right, bf->score_height, bf->back_dc, rect->left,
		(round / 2 == (round + 1) / 2) ? bf->header_height : (bf->header_height + bf->score_height),
		SRCCOPY);

	if (op.view_separate == 1) {
		ret_pen = SelectObject(bf->draw_dc, bf->separate_pen);
		if (round % 5 == 0) {
			MoveToEx(bf->draw_dc, rect->left, bf->score_height - 1, NULL);
			LineTo(bf->draw_dc, rect->right, bf->score_height - 1);
		}
		SelectObject(bf->draw_dc, ret_pen);
	}

	// To Go
	if (round == 0) {
		SetTextColor(bf->draw_dc, op.ci.togo_text);
		SetBkColor(bf->draw_dc, op.ci.background);
		for (j = 0; j < 2; j++) {
			SetRect(&draw_rect, bf->input_left[j], 0, bf->score_left[j], bf->score_height - 1);
			FillRect(bf->draw_dc, &draw_rect, bf->header_back_brush);

			_itot(si->player[j].start_score, buf, 10);
			SetRect(&draw_rect, bf->score_left[j] + 1, 0, bf->score_right[j], bf->score_height);
			draw_text(bf->draw_dc, buf, lstrlen(buf), &draw_rect);
		}
		return TRUE;
	}
	return TRUE;
}

static LRESULT CALLBACK score_list_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam)
{
	DRAW_BUFFER *bf;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	TCHAR buf[BUF_SIZE];
	int left_score[2];
	int i, j;
	int x, y;

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
			bf->back_brush = CreateSolidBrush(op.ci.background); 
			bf->odd_back_brush = CreateSolidBrush(op.ci.odd_background); 
			bf->header_back_brush = CreateSolidBrush(op.ci.header_background);  
			bf->line_pen = CreatePen(PS_SOLID, 1, op.ci.line); 
			bf->separate_pen = CreatePen(PS_SOLID, 1, op.ci.separate); 
			bf->separate_bold_pen = CreatePen(PS_SOLID, 2, op.ci.separate); 
			bf->finish_pen = CreatePen(PS_SOLID, bf->pen_size, op.ci.scored_text); 
			bf->ton_circle_pen = CreatePen(PS_SOLID, bf->pen_size, op.ci.ton_circle); 

			ImmAssociateContext(hWnd, (HIMC)NULL);

			set_scrollbar(hWnd, bf, bf->si);

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

			if (bf->back_redraw == TRUE || bf->back_first != bf->si->leg[bf->view_leg].first) {
				bf->back_redraw = FALSE;
				bf->back_first = bf->si->leg[bf->view_leg].first;
				draw_background(bf, bf->back_first);
			}

			if (ps.rcPaint.top < bf->header_height) {
				BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, bf->header_height,
					bf->back_dc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
				ps.rcPaint.top = bf->header_height;
			}

			y = ((ps.rcPaint.top - bf->header_height) / bf->score_height) * bf->score_height + bf->header_height;
			j = bf->pos_y + (y - bf->header_height) / bf->score_height;
			left_score[0] = bf->si->player[0].start_score;
			left_score[1] = bf->si->player[1].start_score;
			for (i = 0; i < j - 1 && i < bf->si->leg[bf->view_leg].current_round; i++) {
				left_score[0] -= bf->si->leg[bf->view_leg].score[0][i];
				left_score[1] -= bf->si->leg[bf->view_leg].score[1][i];
			}
			for (i = j; y < ps.rcPaint.bottom; y += bf->score_height, i++) {
				if (bf->si->round_limit != 0 && i > bf->si->leg[bf->view_leg].max_round) {
					SetRect(&rect, ps.rcPaint.left, y, ps.rcPaint.right, ps.rcPaint.bottom);
					FillRect(hdc, &rect, bf->header_back_brush);
					break;
				}
				draw_line(bf, bf->si, &ps.rcPaint, i, left_score);
				BitBlt(hdc, ps.rcPaint.left, y, ps.rcPaint.right, bf->score_height,
					bf->draw_dc, ps.rcPaint.left, 0, SRCCOPY);
			}

			EndPaint(hWnd, &ps);
			break;

		case WM_SIZE:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL){
				break;
			}
			
			draw_free(hWnd, bf);		
			draw_init(hWnd, bf);
			SendMessage(hWnd, WM_SCORE_REDRAW, 0, 0);	
			
			break;

		case WM_SCORE_REDRAW:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL){
				break;
			}
			if(wParam == 0) {
				InvalidateRect(hWnd, NULL, FALSE);
			} else {
				GetClientRect(hWnd, &rect);
				i = bf->header_height +
					((bf->si->leg[bf->view_leg].current_round + 1) - bf->pos_y) * bf->score_height + bf->score_height;
				SetRect(&rect,
					0, bf->header_height + wParam * bf->score_height,
					rect.right, (i > rect.bottom) ? rect.bottom : i);
				InvalidateRect(hWnd, &rect, FALSE);
			}
			UpdateWindow(hWnd);
			break;

		case WM_SCORE_INIT_LEG:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL){
				break;
			}

			bf->view_leg = bf->si->current_leg;

			if (lParam == TRUE) {
				score_info_free(&bf->prev_info.si);
				score_info_free(&bf->next_info.si);
				bf->prev_info.prev_flag = FALSE;
				bf->next_info.prev_flag = FALSE;
//				SendMessage(GetParent(hWnd), WM_WINDOW_SET_PREV_MENU, FALSE, 0);
			}
			
			if (bf->next_info.prev_flag == FALSE || wParam == TRUE) {
				LEG_INFO leg;

				leg = bf->si->leg[bf->view_leg];
				ZeroMemory(&bf->si->leg[bf->view_leg], sizeof(LEG_INFO));
				bf->si->leg[bf->view_leg].current_player = leg.first;
				bf->si->leg[bf->view_leg].current_round = 0;
				bf->si->leg[bf->view_leg].alloc_round = (bf->si->round_limit == 0) ? ALLOC_ROUND : bf->si->round;	
				bf->si->leg[bf->view_leg].max_round = (bf->si->round_limit == 0) ? 1 : bf->si->round;		
				bf->si->leg[bf->view_leg].first = leg.first;
				for (i = 0; i < 2; i++) {
					bf->si->leg[bf->view_leg].score[i] = leg.score[i];
					bf->si->leg[bf->view_leg].score[i][0] = 0;
					bf->si->leg[bf->view_leg].all_score[i] = bf->si->player[i].start_score;
					bf->si->player[i].left = bf->si->player[i].start_score;
				}
				bf->input_x = leg.first;
				bf->input_y = -1;
			}

			SendMessage(hWnd, WM_SCORE_REDRAW, 0, 0);

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
