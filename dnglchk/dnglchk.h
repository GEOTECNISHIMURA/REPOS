/*-----------------------------------------------------------------------*/
/*-         DNGLCHK.H           ﾍｯﾀﾞｰﾌｧｲﾙ                               -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_DNGLCHK			    1
#define  DLLAPP_DNGLCHK_FILE	    2
#define  DLLAPP_DNGLCHK_CHK			3
#define  DLLAPP_DNGLCHK_SINGLE		4


/*- タスクＩＤ                      　　-*/
#define     TD_DNGLCHK          L"DNGLCHK"
#define     TD_FILE             L"DNGLCHK_FILE"
#define     TD_CHK              L"DNGLCHK_CHK"
#define     TD_SINGLE           L"DNGLCHK_SINGLE"


/*- プラグマＩＤ（リソースＩＤ）    　　-*/
#define     DNGLCHK_ID              100
#define     FILE_ID                 101
#define     CHK_ID                  102
#define     SINGLE_ID               103


/*- ダイアログボックスＩＤ          　　-*/
#define     DIALOGID_DNGLCHK        200

/*- ダイアログボックスフック関数ＩＤ　　-*/
#define     HOOKID_DNGLCHK_DIALOG   200


/*- プッシュボタンＩＤ              　　-*/
#define     PUSHID_OK               400
#define     PUSHID_END              401
#define     PUSHID_TABLE            402

/*- ダイアログアイテムフック関数ＩＤ　　-*/
#define     HOOKID_PUSH_OK          400
#define     HOOKID_PUSH_END         401
#define     HOOKID_PUSH_TABLE       402


/*- プッシュボタンＩＤ              　　-*/
#define     OPTIONID_KIND           700


/*- テキストアイテムＩＤ            　　-*/
#define     TEXTID_TABLE            501
#define     TEXTID_SIZE             502
#define     TEXTID_ANGLE            503
#define     TEXTID_LV1              510
#define     TEXTID_CO1              511
#define     TEXTID_LC1              512
#define     TEXTID_WT1              513
#define     TEXTID_LV2              514
#define     TEXTID_CO2              515
#define     TEXTID_LC2              516
#define     TEXTID_WT2              517

/*- ラベルアイテムＩＤ              　　-*/
#define     LABELID_CHK             600

typedef struct  DNGLCHK_INFO {
    int         lv1, co1, lc1, wt1;
    int         lv2, co2, lc2, wt2;
    double      size;
    double      angle;
    WChar        table[128];
    int         kind;
}   DNGLCHK;
