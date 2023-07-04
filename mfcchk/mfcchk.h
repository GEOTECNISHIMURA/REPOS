/*-----------------------------------------------------------------------*/
/*-         MHCCHK.H   ﾍｯﾀﾞｰﾌｧｲﾙ                                        -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_MFCCHK			    1
#define  DLLAPP_MFCCHK_FILE	        2
#define  DLLAPP_MFCCHK_CHK			3

/*- タスクＩＤ          　　            -*/
#define     TD_MFCCHK               L"MFCCHK"
#define     TD_FILE                 L"MFCCHK_FILE"
#define     TD_CHK                  L"MFCCHK_CHK"

/*- ダイアログボックスＩＤ          　　-*/
#define     DIALOGID_MFCCHK             200

/*- ダイアログボックスフック関数ＩＤ　　-*/
#define     HOOKID_MFCCHK_DIALOG        200


/*- プッシュボタンＩＤ      　　        -*/
#define     PUSHID_TABLE                402
#define     PUSHID_BAT                  403
#define     PUSHID_LAYER                404
#define     PUSHID_OK                   400
#define     PUSHID_END                  401

/*- ダイアログアイテムフック関数ＩＤ　　-*/
#define     HOOKID_PUSH_TABLE           402
#define     HOOKID_PUSH_BAT             403
#define     HOOKID_PUSH_LAYER           404
#define     HOOKID_PUSH_OK              400
#define     HOOKID_PUSH_END             401


/*- テキストアイテムＩＤ            　　-*/
#define     TEXTID_TABLE                500
#define     TEXTID_BAT                  501
#define     TEXTID_LV                   510
#define     TEXTID_CO                   511
#define     TEXTID_LC                   512
#define     TEXTID_WT                   513
#define     TEXTID_CT                   514
#define     TEXTID_ZEN                  515
#define     TEXTID_HAN                  516
#define     TEXTID_SIZE                 517


/*- ダイアログアイテムフック関数ＩＤ　　-*/
#define     HOOKID_ERRCT_TEXT           514


/*- トグルボタンＩＤ                　　-*/
#define     TOGGLEID_ERR                600
#define     TOGGLEID_FONT               601
#define     TOGGLEID_SPACE              602

/*- ダイアログアイテムフック関数ＩＤ　　-*/
#define     HOOKID_TOGGLE_ERR           600
#define     HOOKID_TOGGLE_FONT          601


/*-    オプションボタンＩＤ        　　-*/
#define     OPTIONID_MODE               700


/*- ダイアログアイテムフック関数ＩＤ　　-*/
#define     HOOKID_OPTION_MODE          700


/*- カラーピッカーアイテムＩＤ      　　-*/
#define     COLORPICKERID_ELEMCO        800

/*- シノニムリソースＩＤ            　　-*/
#define     SYNONYMID_ELECO             900


typedef struct  MFCCHK_INFO {
    WChar   table[256];
    int     mode;
    WChar   bat[256];
	UInt32  lv, co, wt;
    int     lc;
    int     ct;
    int     err;
    int     nn;
    int     zen;
    int     han;
    int     font;
    int     space;
    double  size;
}   MFCCHK;
