#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_HOUKOU			1
#define  DLLAPP_HOUKOU_CUT      2
#define  DLLAPP_HOUKOU_UNCUT    3
#define  DLLAPP_HOUKOU_YA       4

/*-	タスクＩＤ			　　-*/
#define		TD_HOUKOU		L"HOUKOU"
#define		TD_UNCUT		L"UNCUT"
#define		TD_CUT			L"CUT"
#define		TD_YA			L"YA"
#define		TD_SITYU		L"SITYU"

/*-	プラグマＩＤ（リソースＩＤ）	　　-*/
#define		HOUKOU_ID		100
#define		UNCUT_ID		105
#define		CUT_ID			110
#define		YA_ID			111
#define		SITYU_ID		112

/*-	ダイアログボックスＩＤ		　　-*/
#define		DIALOGID_HOUKOU			300

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_HOUKOU_DIALOG	400

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_OPTION_CUT		500
#define		HOOKID_OPTION_LAYER		501

#define		HOOKID_PUSH_OK			510
#define		HOOKID_PUSH_END			511

/*-	テキストＩＤ		　　-*/
#define		TEXTID_SCALE		700
#define		TEXTID_LV			701
#define		TEXTID_CO			702
#define		TEXTID_LC			703
#define		TEXTID_WT			704
#define		TEXTID_CELL			705

/*-	プッシュボタンＩＤ		　　-*/
#define		PUSHID_OK			800
#define		PUSHID_END			801

/*-	オプションボタンＩＤ		　　-*/
#define		OPTIONID_CUT		900
#define		OPTIONID_LAYER		901
#define		OPTIONID_CELL		902


typedef struct	HOUSET_INFO {
    int		cut;        /*- 0:結ぶ; 1:結ばない　2:矢印　3:支柱-*/
    int		layer;      /*- 0:カレント　1:セルと同じ;  2:設定する -*/
    long	scale;
    int		lv, co, lc, wt;
    WChar	cell[7];
    bool		zukei;
}	HOUSET;
