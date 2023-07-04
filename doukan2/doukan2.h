/*-----------------------------------------------------------------------*/
/*-	        DOUKAN2.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>
// DLLのID
#define  DLLAPP_DOUKAN2			1

/*-	タスクＩＤ			    			-*/
#define		TD_MAIN						"DOUKAN2"

/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		MAIN_ID						100   /*- メイン   	-*/


/*-	ダイアログボックスＩＤ		    	-*/
#define		DIALOGID_DOUKAN2			200

/*-	ダイアログボックスフック関数ＩＤ    -*/
#define		HOOKID_DOUKAN2_DIALOG		200


/*-	プッシュボタンＩＤ				    -*/
#define		PUSHID_IDX					400
#define		PUSHID_TBL					401
#define		PUSHID_65					402
#define		PUSHID_TEST					403
#define		PUSHID_BUNKATU				404
#define		PUSHID_GG					405
#define		PUSHID_KANRI				406
#define		PUSHID_END					407

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define		HOOKID_PUSH_IDX				400
#define		HOOKID_PUSH_TBL				401
#define		HOOKID_PUSH_65				402
#define		HOOKID_PUSH_TEST			403
#define		HOOKID_PUSH_BUNKATU			404
#define		HOOKID_PUSH_GG				405
#define		HOOKID_PUSH_KANRI			406
#define		HOOKID_PUSH_END				407


/*-	テキストアイテムＩＤ		    	-*/
#define		TEXTID_IDX					500
#define		TEXTID_TBL					501
#define		TEXTID_ERR_LV				503
#define		TEXTID_ERR_CO				504
#define		TEXTID_ERR_WT				505
#define		TEXTID_ERR_ST				506


/*-	トグルボタンＩＤ				    -*/
#define		TOGGLEID_IDX				600

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define		HOOKID_TOGGLE_IDX			600


/*-	ラベルＩＤ    -*/
#define		LABELID_LAYER				700


typedef struct	DOUKAN2_INFO {
	int		tog_idx;
	int		elv, eco, ewt, est;
	WChar	idxName[256], tblName[256];
} DOUKAN2;
