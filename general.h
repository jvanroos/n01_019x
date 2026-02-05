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
	
	int 	view_left;
	int 	recovery_delete;
	int 	left_font_size;
	TCHAR 	font_name[BUF_SIZE];
} OPTION_INFO;	

typedef struct _PLAYER_INFO {
	TCHAR name[NAME_SIZE];
	int start_score;
	int left;
} PLAYER_INFO;

typedef struct _SCORE_INFO {
	PLAYER_INFO player[2];
	int 	start_score;
} SCORE_INFO;

#endif
/* End of source */
