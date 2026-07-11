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

#define BOX_COUNT					6
#define CHAR_COUNT					(10 * BOX_COUNT)

/* Global Variables */
extern HINSTANCE hInst;
extern OPTION_INFO op;

typedef struct _DRAW_BUFFER {
	HDC 		draw_dc;
	HBITMAP		draw_bmp;
	HBITMAP		draw_ret_bmp;

	HFONT		guide_font;
	HFONT 		ret_font;

	HBRUSH		back_brush;
	HBRUSH		box_back_brush;
	HBRUSH		box_select_brush;

	HPEN		box_frame_pen;
	HPEN		box_dis_frame_pen;

	BOOL		mousedown;
	int			sel_index;
	int 		mouse_index;

	HWND		hToolTip;
	int			tip_index;

	int 		key;

} DRAW_BUFFER;

/* Local Function Prototypes */
static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf);
static BOOL draw_guide(const HWND hWnd, const DRAW_BUFFER *bf);
static LRESULT CALLBACK score_guide_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam);

static BOOL draw_init(const HWND hWnd, DRAW_BUFFER *bf)
{
	HDC hdc;
	RECT rect;
	int size;

	GetClientRect(hWnd, &rect);

	hdc = GetDC(hWnd);
	bf->draw_bmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	bf->draw_ret_bmp = SelectObject(bf->draw_dc, bf->draw_bmp);
	ReleaseDC(hWnd, hdc);

	size = (rect.right / CHAR_COUNT < 9) ? 9 : rect.right / CHAR_COUNT;
	bf->guide_font = font_create_menu(size, 0, FALSE);
	bf->ret_font = SelectObject(bf->draw_dc, bf->guide_font);
	return TRUE;
}

static BOOL draw_free(const HWND hWnd, DRAW_BUFFER *bf) 
{
	if(bf->draw_dc != NULL) {
		SelectObject(bf->draw_dc, bf->ret_font);
		SelectObject(bf->draw_dc, bf->draw_ret_bmp);
		DeleteObject(bf->draw_bmp);
		bf->draw_bmp = NULL;
	}
	if(bf->guide_font != NULL) {
		DeleteObject(bf->guide_font);
		bf->guide_font = NULL;
	}

	return TRUE;
}

static BOOL draw_guide(const HWND hWnd, const DRAW_BUFFER *bf)
{
	RECT draw_rect, rect;
	HPEN ret_pen;
	HBRUSH ret_brush;
	SIZE sz;
	TCHAR buf[BUF_SIZE];
	int left, right;
	int i, j;
	int height;
	int key_width;

	GetClientRect(hWnd, &rect);
	FillRect(bf->draw_dc, &rect, bf->back_brush);

	GetTextExtentPoint32(bf->draw_dc, TEXT("F188"), lstrlen(TEXT("F188")), &sz);
	key_width = sz.cx;

	left = 0;
	height = rect.bottom / 2;
	for (j = 0; j < 12 ; j++) {
		if (j == BOX_COUNT) {
			// end of first row
			rect.top += height;
			left = 0;
		}
		for(i = 0; i < op.key_info_count; i++) {
			if (op.key_info[i].key == VK_F1 + j && op.key_info[i].ctrl == bf->key) {
				break;
			}
		}

		if(bf->sel_index - 1 == j && i < op.key_info_count) {
			SetRect(&draw_rect, left, rect.top, left + rect.right / BOX_COUNT, rect.top + height);
			FillRect(bf->draw_dc, &draw_rect, bf->box_select_brush);
		}
		if(i < op.key_info_count) {
			ret_pen = SelectObject(bf->draw_dc, bf->box_frame_pen);
			ret_brush = SelectObject(bf->draw_dc, bf->box_back_brush);
			SetTextColor(bf->draw_dc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(bf->draw_dc, GetSysColor(COLOR_WINDOW));
		}else {
			ret_pen = SelectObject(bf->draw_dc, bf->box_dis_frame_pen);
			ret_brush = SelectObject(bf->draw_dc, bf->back_brush);
			SetTextColor(bf->draw_dc, GetSysColor(COLOR_3DSHADOW));
			SetBkColor(bf->draw_dc, GetSysColor(COLOR_3DFACE));
		}
		SetRect(&draw_rect, left + 1, rect.top + 1, left + key_width, rect.top + height - 1);
		RoundRect(bf->draw_dc, draw_rect.left, draw_rect.top, draw_rect.right, draw_rect.bottom, key_width / 6, key_width / 6);
		SelectObject(bf->draw_dc, ret_pen);
		SelectObject(bf->draw_dc, ret_brush);
		wsprintf(buf, TEXT("F%d"), j + 1);
		DrawText(bf->draw_dc, buf, lstrlen(buf), &draw_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		right = left + rect.right / BOX_COUNT;
		left += key_width + 2;
		for (i = 0; i < op.key_info_count; i++) {
			if (op.key_info[i].key != VK_F1 + j || op.key_info[i].ctrl != bf->key) {
				continue;
			}
			if (bf->sel_index - 1 == j) {
				SetTextColor(bf->draw_dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkColor(bf->draw_dc, GetSysColor(COLOR_HIGHLIGHT));
			} else {
				SetTextColor(bf->draw_dc, GetSysColor(COLOR_WINDOWTEXT));
				SetBkColor(bf->draw_dc, GetSysColor(COLOR_3DFACE));
			}
			if (op.key_info[i].action >= ID_ACCEL_INPUT_SCORE && op.key_info[i].action <= ID_ACCEL_INPUT_SCORE + 180) {
				_itot(op.key_info[i].action - ID_ACCEL_INPUT_SCORE, buf, 10);
			} else {
//				str_noprefix_cpy(buf, message_get_res(op.key_info[i].action));
			}
			SetRect(&draw_rect, left, rect.top, right, rect.top + height);
			DrawText(bf->draw_dc, buf, lstrlen(buf), &draw_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
			break;
		}
		left = right;
	}
	return TRUE;
}

static LRESULT CALLBACK score_guide_proc(const HWND hWnd, const UINT msg, WPARAM wParam, LPARAM lParam) {
	
	DRAW_BUFFER	*bf;
	HDC	hdc;
	TEXTMETRIC tm;
	HFONT hfont, ret_font;
	PAINTSTRUCT ps;
	RECT rect;
	MSG _msg;
	int size, ret;
	int i;

	switch(msg) {
		case WM_CREATE:
			bf = (DRAW_BUFFER*) mem_calloc(sizeof(DRAW_BUFFER));
			if (bf == NULL) {
				return -1;
			}
			
			hdc = GetDC(hWnd);
			bf->draw_dc = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			draw_init(hWnd, bf);
			bf->back_brush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
			bf->box_back_brush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			bf->box_select_brush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
			bf->box_frame_pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
			bf->box_dis_frame_pen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
			draw_guide(hWnd, bf);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)bf);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf != NULL) {
				draw_free(hWnd, bf);
				if(bf->draw_dc != NULL) {
					DeleteDC(bf->draw_dc);
					bf->draw_dc = NULL;
				}
				DeleteObject(bf->back_brush);
				DeleteObject(bf->box_back_brush);
				DeleteObject(bf->box_select_brush);
				DeleteObject(bf->box_frame_pen);
				DeleteObject(bf->box_dis_frame_pen);
				mem_free((void *) &bf);
			}
			return DefWindowProc(hWnd, msg, wParam, lParam);

		case WM_SIZE:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL) {
				break;
			}
			draw_free(hWnd, bf);
			draw_init(hWnd, bf);
			draw_guide(hWnd, bf);
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			break;

		case WM_PAINT:
			bf = (DRAW_BUFFER *)GetWindowLong(hWnd, GWL_USERDATA);
			if (bf == NULL) {
				break;
			}
			hdc = BeginPaint(hWnd, &ps);

			BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom,
				bf->draw_dc, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

			EndPaint(hWnd, &ps);
			break;

		case WM_GUIDE_GET_HEIGHT:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL) {
				break;
			}
			size = (wParam / CHAR_COUNT < 9) ? 9 : wParam / CHAR_COUNT;
			hfont = font_create_menu(size, 0, FALSE);
			ret_font = SelectObject(bf->draw_dc, hfont);
			GetTextMetrics(bf->draw_dc, &tm);
			ret = tm.tmHeight * 2 + 8;
			SelectObject(bf->draw_dc, ret_font);
			DeleteObject(hfont);
			return ret;

		case WM_GUIDE_REDRAW:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL) {
				break;
			}
			draw_guide(hWnd, bf);
			InvalidateRect(hWnd, NULL, FALSE);
			UpdateWindow(hWnd);
			break;

		case WM_GUIDE_DRAW_INIT:
			bf = (DRAW_BUFFER *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(bf == NULL) {
				break;
			}
			draw_free(hWnd, bf);
			draw_init(hWnd, bf);
			SendMessage(hWnd, WM_GUIDE_REDRAW, 0, 0);
			break;

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
