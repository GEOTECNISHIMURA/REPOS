/*-----------------------------------------------------------------------*/
/*-	        LINK.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_LINK			    1
#define  DLLAPP_LINK_SINGLE			2
#define  DLLAPP_LINK_ALL 			3
#define  DLLAPP_LINK_FILE			4
#define  DLLAPP_LINK_DEL 			5
#define  DLLAPP_LINK_NAME			6


/*-	タスクＩＤ			　　-*/
#define		TD_LINK		L"LINK"
#define		TD_SINGLE	L"LINK_SINGLE"
#define		TD_ALL		L"LINK_ALL"
#define		TD_FILE		L"LINK_FILE"
#define		TD_DEL		L"LINK_DEL"
#define		TD_NAME		L"LINK_NAME"


/*-	プラグマＩＤ（リソースＩＤ）	　　-*/
#define		LINK_ID			100
#define		SINGLE_ID		101
#define		ALL_ID			102
#define		FILE_ID			103
#define		DEL_ID			104
#define		NAME_ID			105


/*-	ダイアログボックスＩＤ		　　-*/
#define		DIALOGID_LINK			200

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_LINK_DIALOG		200


/*-	テキストＩＤ		　　-*/
#define		TEXTID_LAYER			300
#define		TEXTID_BAT				301
#define		TEXTID_DBSOURCE			302
#define		TEXTID_TABLE			304
#define		TEXTID_MSLINK			303


/*-	プッシュボタンＩＤ		　　-*/
#define		PUSHID_LINK_LAYER		400
#define		PUSHID_LINK_BAT			401
#define		PUSHID_LINK_OK			402
#define		PUSHID_LINK_END			403
#define		PUSHID_LINK_DEL			404

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_LINK_LAYER	400
#define		HOOKID_PUSH_LINK_BAT	401
#define		HOOKID_PUSH_LINK_OK		402
#define		HOOKID_PUSH_LINK_END	403
#define		HOOKID_PUSH_LINK_DEL	404


/*-	オプションボタンＩＤ		　　-*/
#define		OPTIONID_KIND			500
#define		OPTIONID_KETA			501

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_OPTION_KIND		500


/*-	トグルボタンＩＤ		　　-*/
#define		TOGGLEID_ZUMEN			600
#define		TOGGLEID_DEL_DB			601

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_TOGGLE_ZUMEN		600


typedef struct	LINKSET_INFO {
    int		nn;
    int		kind;
    ULong	mslink;
    WChar	layer[128];
    WChar	bat[128];
    WChar	dbsource[100];
    WChar	table[100];
    int		zumen;
    int		keta;
    int		tdel;
}   LINKSET;

