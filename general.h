#ifndef _INC_N01_GENERAL_H
#define _INC_N01_GENERAL_H


/* Define */
#define BUF_SIZE						256
#define NAME_SIZE						64
#define APP_NAME						TEXT("n01 Ver 0.1.9")
#define APP_VERSION						18


#define MAX_ROUND						60
#define NAME_LIST_COUNT					64

#define WM_WINDOW_SET_CURRENT			(WM_APP + 8)

// typedef
typedef struct _GAME_INFO {
	int start_score;
	int round_limit;
	int round;
	int leg_limit;
	int max_leg;
	int best_of;
	TCHAR player_name[2][NAME_SIZE];
	int player_start_score[2];
	BOOL com[2];
	int level[2];
	BOOL schedule_flag;
	BOOL change_first;
} GAME_INFO;

typedef struct _OP_PLAYER_INFO {
	int 	name;
	int 	first;

	int 	total_sets;
	int 	total_legs;
	int		total_tons;
	int 	total_100;
	int 	total_140;
	int 	total_180s;
	int 	total_high_off;
	int 	total_short;
	int 	total_long;

	int 	avg_score;
	int 	avg_darts;
	int 	avg_first9;
	int 	avg_check_out;
	int 	avg_check_out_count;
	int 	avg_keep;
	int 	avg_keep_count;
	int 	avg_break;
	int 	avg_break_count;

	int 	arrange;

	int 	large_font;
	int 	scroll;
	int 	avg_per_round;
} OP_PLAYER_INFO;

typedef struct _OPTION_INFO {
	RECT 	window_rect;
	int		window_state;

	int 	view_player;
	int 	view_left;

	int 	recovery_delete;
	int 	left_font_size;
	TCHAR 	font_name[BUF_SIZE];

	// Game Option
	GAME_INFO gi;
	GAME_INFO *gi_list;
	int gi_list_count;
	TCHAR name_list[NAME_LIST_COUNT][NAME_SIZE];
	int name_list_count;

	// Option Player
	OP_PLAYER_INFO opi;
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
