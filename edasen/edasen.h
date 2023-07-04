/*-----------------------------------------------------------------------*/
/*-	        EDASEN.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_EDASEN			1

/*-	タスクＩＤ							-*/
#define		TD_EDASEN  	            "EDASEN"


/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		EDASEN_ID  	            100


/*-	ダイアログボックスＩＤ				-*/
#define		DIALOGID_MAIN		    200


/*-	ダイアログボックスフック関数ＩＤ	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	プッシュボタンＩＤ					-*/
#define		PUSHID_TABLE			310
#define		PUSHID_OK				300
#define		PUSHID_END			    301


/*-	ダイアログアイテムフック関数ＩＤ	-*/
#define		HOOKID_PUSH_TABLE		310
#define		HOOKID_PUSH_OK	        300
#define		HOOKID_PUSH_END	        301


/*-	テキストＩＤ					-*/
#define		TEXTID_TABLE			401

#define		TEXTID_EDA_LV			411
#define		TEXTID_EDA_CO			412
#define		TEXTID_EDA_ST			413
#define		TEXTID_EDA_WT			414

#define		TEXTID_ERR_LV			421
#define		TEXTID_ERR_CO			422
#define		TEXTID_ERR_ST			423
#define		TEXTID_ERR_WT			424
#define		TEXTID_ERR_RR			425

#define		TEXTID_LENG			    430

/*-	ダイアログアイテムフック関数ＩＤ	-*/
#define		LABELID_LAYER			501

typedef struct	EDASEN_INFO {
	WChar	tblName[256];
	UInt32  eda_lv, eda_co, eda_wt;
	int	    eda_st;
	UInt32  err_lv, err_co, err_wt;
	int	    err_st;
    double  err_rr, leng;
}	EDASEN;
