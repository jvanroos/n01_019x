/*
 * n01
 * 
 * Ini.c
 *
 */
//#define _INC_OLE
#include <windows.h>
//#undef _INC_OLE
#include <tchar.h>

#include "general.h"
#include "memory.h"
#include "profile.h"
#include "message.h"

#include "resource.h"

/* Global Variables */
extern OPTION_INFO op;

BOOL ini_get_option(const TCHAR *ini_path)
{
	RECT rect;
//	TCHAR buf[BUF_SIZE];
	WORD lang;

	profile_initialize(ini_path, TRUE);

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	lang = (WORD)PRIMARYLANGID(LANGIDFROMLCID(GetThreadLocale()));

	op.window_rect.left = profile_get_int(TEXT("window"), TEXT("left"), 0, ini_path);
	op.window_rect.top = profile_get_int(TEXT("window"), TEXT("top"), 0, ini_path);
	op.window_rect.right = profile_get_int(TEXT("window"), TEXT("right"), 0, ini_path);
	op.window_rect.bottom = profile_get_int(TEXT("window"), TEXT("bottom"), 0, ini_path);

	if (op.window_rect.left > rect.right || op.window_rect.top > rect.bottom) {
		op.window_rect.left = 0;
		op.window_rect.top = 0;
	}

	op.window_state = profile_get_int(TEXT("window"), TEXT("state"), SW_SHOWDEFAULT, ini_path);

	profile_get_string(TEXT("font"), TEXT("name"), message_get_res(IDS_STRING_DEFAULT_FONT), op.font_name, BUF_SIZE - 1, ini_path);
	op.left_font_size = profile_get_int(TEXT("font"), TEXT("left_font_size"), 40, ini_path);

	profile_free();
	return TRUE;
}

BOOL ini_put_option(const TCHAR *ini_path)
{
//	TCHAR buf[BUF_SIZE];
//	int i;

	profile_initialize(ini_path, TRUE);

	profile_write_int(TEXT("window"), TEXT("left"), op.window_rect.left, ini_path);
	profile_write_int(TEXT("window"), TEXT("top"), op.window_rect.top, ini_path);
	profile_write_int(TEXT("window"), TEXT("right"), op.window_rect.right, ini_path);
	profile_write_int(TEXT("window"), TEXT("bottom"), op.window_rect.bottom, ini_path);
	profile_write_int(TEXT("window"), TEXT("state"), op.window_state, ini_path);

	profile_flush(ini_path);
	profile_free();
	return TRUE;
}
