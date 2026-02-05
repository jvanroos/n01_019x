/*
 * n01
 *
 * Font.c
 *
 */

/* Include Files */
#define _INC_OLE
#include <windows.h>
#undef  _INC_OLE

/* Define */

/* Global Variables */

/* Local Function Prototypes */

/*
 * font_create
 */
HFONT font_create(const TCHAR *FontName, const int FontSize, const int weight, const BOOL under_line, const BOOL fixed)
{
	LOGFONT lf;
	HDC hdc;

	ZeroMemory(&lf, sizeof(LOGFONT));

	hdc = GetDC(NULL);
	lf.lfHeight = -(int)((FontSize * GetDeviceCaps(hdc, LOGPIXELSY)) / 72);
	ReleaseDC(NULL, hdc);

	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = weight;
	lf.lfItalic = FALSE;
	lf.lfUnderline = (BYTE)under_line;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = (BYTE)(((fixed == TRUE) ? FIXED_PITCH : DEFAULT_PITCH) | FF_DONTCARE);
	lstrcpy(lf.lfFaceName, FontName);
	return CreateFontIndirect((CONST LOGFONT *)&lf);
}
