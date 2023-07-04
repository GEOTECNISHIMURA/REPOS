/*-----------------------------------------------------------------------*/
/*-	        LINE3TO4.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLLのID
#define  DLLAPP_LINE3TO4			1

/*-	タスクＩＤ							-*/
#define		TD_LINE3TO4  		    "LINE3TO4"


/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		LINE3TO4_ID  			100


/*-	ダイアログボックスＩＤ				-*/
#define		DIALOGID_MAIN			200


/*-	ダイアログボックスフック関数ＩＤ	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	プッシュボタンＩＤ					-*/
#define		PUSHID_TABLE			310
#define		PUSHID_IDX		    	311
#define		PUSHID_OK				300
#define		PUSHID_END			    301


/*-	ダイアログアイテムフック関数ＩＤ	-*/
#define		HOOKID_PUSH_TABLE		310
#define		HOOKID_PUSH_IDX		    311
#define		HOOKID_PUSH_OK	        300
#define		HOOKID_PUSH_END	        301


/*-	テキストＩＤ					-*/
#define		TEXTID_TABLE			401
#define		TEXTID_IDX  			402

/*-	トグルボタンＩＤ					-*/
#define		TOGGLEID_IDX			500

/*-	ダイアログアイテムフック関数ＩＤ	-*/
#define		HOOKID_TOGGLE_IDX		500


typedef struct	LINE3TO4_INFO {
    int     tog_idx;
	WChar	idxName[512];
	WChar	tblName[512];
}	LINE3TO4;
