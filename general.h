#ifndef _INC_N01_GENERAL_H
#define _INC_N01_GENERAL_H


/* Define */
#define BUF_SIZE						256
#define NAME_SIZE						64
#define APP_NAME						TEXT("n01 Ver 0.1.9")
#define APP_VERSION						18

typedef struct _OPTION_INFO {
	RECT 	window_rect;
	int		window_state;

	int 	recovery_delete;
} OPTION_INFO;	

#endif
/* End of source */
