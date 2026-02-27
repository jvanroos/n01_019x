#ifndef _INC_N01_GENERAL_H
#define _INC_N01_GENERAL_H


/* Define */
#define BUF_SIZE					256
#define NAME_SIZE					64
#define APP_NAME					TEXT("n01 Ver 0.1.9")
#define APP_VERSION					18


#define MAX_ROUND					60
#define ALLOC_ROUND					10
#define NAME_LIST_COUNT				64

#define WM_WINDOW_REDRAW			(WM_APP + 1)
#define WM_WINDOW_SET_CURRENT		(WM_APP + 8)
#define WM_N01_GAME_ON				(WM_APP + 531)

typedef char TYPE_CHECK_OUT;

// typedef
typedef short TYPE_SCORE;
typedef char TYPE_CHECK_OUT;

/* Struct */
typedef struct _GAME_INFO {
	int 	start_score;
	int 	round_limit;
	int 	round;
	int 	leg_limit;
	int 	max_leg;
	int 	best_of;
	TCHAR 	player_name[2][NAME_SIZE];
	int 	player_start_score[2];
	BOOL 	com[2];
	int 	level[2];
	BOOL 	schedule_flag;
	BOOL 	change_first;
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
	RECT window_rect;
	int	window_state;

	int view_player;
	int view_left;
	int view_guide;
	int view_throw_count;
	int view_ton_circle;
	int view_separate;
	int view_scroll_bar;

	int auto_save;
	TCHAR 	auto_save_path[BUF_SIZE];
	int check_out_mode;
	int recovery;

	int check_out_font_size;

	// com
	int com_default_number;
	int com_default_segment;

	// Game Option
	GAME_INFO gi;
	GAME_INFO *gi_list;
	int gi_list_count;
	TCHAR name_list[NAME_LIST_COUNT][NAME_SIZE];
	int name_list_count;

	// Game Histroy
	RECT history_rect;
	RECT score_list_rect;

	// Arrange
	RECT arrange_rect;
//	ARRANGE_INFO *arrange_info;
	int arrange_info_count;
//	ARRANGE_INFO *com_arrange_info;
	int com_arrange_info_count;

	// Option view
//	COLOR_INFO ci;
	TCHAR font_name[BUF_SIZE];

	// Option key
//	KEY_INFO *key_info;
	int key_info_count;
	BOOL key_save;

	// Option Player
	OP_PLAYER_INFO opi;

	// Plug-in
//	PLUGIN_INFO *plugin_info;
	int plugin_info_count;

	int recovery_delete;
	int left_font_size;
	int com_timer;
} OPTION_INFO;	

typedef struct _STATISTICS_INFO {
	int ton_count;
	int ton00_count;
	int ton20_count;
	int ton40_count;
	int ton80_count;
	TYPE_SCORE high_off;
	int short_game;
	int long_game;

	// 
	int all_score;
	int all_darts;
	// 
	int win_darts;
	int win_count;
	// 
	int first9_score;
	int first9_darts;
	// 
	int check_out_aim;
	int check_out;
	// 
	int all_keep_legs;
	int win_keep_legs;
	// 
	int all_break_legs;
	int win_break_legs;

	// Finish stats
	int success_2_80;
	int failure_2_80;
	int success_81_130;
	int failure_81_130;
	int success_131;
	int failure_131;
} STATISTICS_INFO;

typedef struct _PLAYER_INFO {
	TCHAR name[NAME_SIZE];

	int start_score;
	int left;

	int sets;
	int legs;

	STATISTICS_INFO stat;
	STATISTICS_INFO set_stat;

	BOOL lock;
	BOOL check_out_mode;

	BOOL com;
	int level;
	TYPE_SCORE com_score;

} PLAYER_INFO;

typedef struct _LEG_INFO {
	int first;

	int current_round;
	int current_player;

	int max_round;
	int alloc_round;

	TYPE_SCORE *score[2];

	BOOL end_flag;
	int winner;
	int darts;
	TYPE_SCORE out_left;

	int ton_count[2];
	int ton00_count[2];
	int ton20_count[2];
	int ton40_count[2];
	int ton80_count[2];
	int all_score[2];
	int all_darts[2];
	int first9_score[2];
	int first9_darts[2];
	int check_out_aim[2];
	int failure_2_80[2];
	int failure_81_130[2];
	int failure_131[2];
} LEG_INFO;

typedef struct _SCORE_INFO {
	BOOL set_mode;
	BOOL history;

	int start_score;

	int current_set;
	int current_leg;

	int leg_limit;
	int max_leg;
	int best_of;

	int round_limit;
	int round;

	SYSTEMTIME start_time;

	PLAYER_INFO player[2];
	LEG_INFO *leg;
	TYPE_CHECK_OUT *tmp_check_out[2];
} SCORE_INFO;

#endif
/* End of source */
