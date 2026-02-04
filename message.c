/*
 * n01
 *
 * Message.c
 *
 */

/* Include Files */

#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

/* Define */
#define BUF_SIZE			256

/* Global Variables */
extern HINSTANCE hInst;

/* Local Function Prototypes */



BOOL message_get_error(const int err_code, TCHAR *err_str) {

	if (err_str == NULL) {
		return FALSE;
	}
	*err_str = TEXT('\0');
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_code, 0, err_str, BUF_SIZE - 1, NULL);
	return TRUE;
}


TCHAR *message_get_res(const UINT id) {

	static TCHAR buf[BUF_SIZE];

	*buf = TEXT('\0');
	LoadString(hInst, id, buf, BUF_SIZE - 1);
	return buf;
}

TCHAR *message_copy_res(const UINT id, TCHAR *buf) {

	*buf = TEXT('\0');
	LoadString(hInst, id, buf, BUF_SIZE - 1);
	return buf;
}
