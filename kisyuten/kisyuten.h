/*-----------------------------------------------------------------------*/
/*-	        KISYUTEN.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_KISYUTEN			1

/*-	タスクＩＤ			　　			-*/
#define		TD_MAIN						"KISYUTEN"

/*-	プラグマＩＤ（リソースＩＤ）　		-*/
#define		MAIN_ID						100   /*- メイン   	-*/


/*-	ダイアログボックスＩＤ		　　	-*/
#define		DIALOGID_MAIN				200

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_MAIN_DIALOG			200


/*-	トグルボタンＩＤ		　　		-*/
#define		TOGGLEID_IDX				300

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_TOGGLE_IDX			300


/*-	プッシュボタンＩＤ		　　		-*/
#define		PUSHID_IDX					400
#define		PUSHID_TABLE				401
#define		PUSHID_CELLIB				404
#define		PUSHID_OK					402
#define		PUSHID_END					403

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_IDX				400
#define		HOOKID_PUSH_TABLE			401
#define		HOOKID_PUSH_CELLIB			404
#define		HOOKID_PUSH_OK				402
#define		HOOKID_PUSH_END				403


/*-	テキストアイテムＩＤ		　　	-*/
#define		TEXTID_IDX					500
#define		TEXTID_TABLE				501
#define		TEXTID_CELLIB				504
#define		TEXTID_KITEN				502
#define		TEXTID_SYUTEN				503
#define		TEXTID_LV					505
#define		TEXTID_CO					506
#define		TEXTID_WT					507
#define		TEXTID_ST					508


/*-	オプションボタンＩＤ		　　		-*/
#define		OPTIONID_RENZOKU			600


/*-	ラベルＩＤ		　　		-*/
#define		LABELID_CELL				700


typedef struct	KISYUTEN_INFO {
	int		tog_idx, renzoku;
	WChar	idx[256], table[256];
	WChar   cellib[256];
	WChar	kiten[7], syuten[7];
	UInt32  lv, co, wt;
	int     st;
} KISYUTEN;
