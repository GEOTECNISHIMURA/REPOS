/*-----------------------------------------------------------------------*/
/*-	        file_c.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_FILE_C			1

/*-	タスクＩＤ							-*/
#define		TD_FILE_C  	    "file_c"


/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		FILE_C_ID  	            100

/*-	ダイアログボックスＩＤ				-*/
#define     DIALOGID_MAIN      	    200

/*-	ダイアログボックスフック関数ＩＤ	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	プッシュボタンＩＤ					-*/
#define     PUSHID_OK              	300
#define     PUSHID_END              301
#define     PUSHID_BAT              302
#define     PUSHID_CELL             303

/*-	アイテムフック関数ＩＤ	-*/
#define     HOOKID_PUSH_OK		    300
#define     HOOKID_PUSH_END		    301
#define     HOOKID_PUSH_BAT	        302
#define     HOOKID_PUSH_CELL        303


/*-	トグルボタンＩＤ					-*/
#define     TOGGLEID_REFFILE		401
#define     TOGGLEID_VIEWTOP 		402
#define     TOGGLEID_FILEDESIGN   	403
#define     TOGGLEID_TEXTNODES    	404
#define     TOGGLEID_LEVELS       	405
#define     TOGGLEID_CELL         	406

/*-	アイテムフック関数ＩＤ	-*/
#define		HOOKID_TOGGLE_CELL		406


/*-	テキストアイテムＩＤ					-*/
#define     TEXTID_BAT		        501
#define     TEXTID_CELL 		    502


typedef struct FILE_C_INFO {
	int	    reffile;
	int	    viewtop;
	int	    textnodes;
	int	    levels;
	int	    tog_cell;
	WChar    cellib[512];
	int	    filedesign;
	WChar    idxname[512];
    } FILE_C;
