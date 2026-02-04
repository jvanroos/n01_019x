/*
 * n01
 *
 * String.c
 *
 */

/* Include Files */
#include <windows.h>
#include <tchar.h>

#include "Memory.h"

/* Define */
#define to_lower(c)		((c >= TEXT('A') && c <= TEXT('Z')) ? (c - TEXT('A') + TEXT('a')) : c)

BOOL trim(TCHAR *buf)
{
	TCHAR *p, *r;

	// Get the pointer excluding leading and trailing whitespace
	for (p = buf; (*p == TEXT(' ') || *p == TEXT('\t')) && *p != TEXT('\0'); p++)
		;
	for (r = buf + lstrlen(buf) - 1; r > p && (*r == TEXT(' ') || *r == TEXT('\t')); r--)
		;
	*(r + 1) = TEXT('\0');

	// Copy to original string
	lstrcpy(buf, p);
	return TRUE;
}

/*
 * str2hash - Get the hash value of a string
 */
UINT str2hash(const TCHAR *str)
{
	UINT hash = 0;

	for (; *str != TEXT('\0'); str++) {
		if (*str != TEXT(' ')) {
			hash ^= ((hash << 4) + to_lower(*str));
		}
	}
	return hash;
}
