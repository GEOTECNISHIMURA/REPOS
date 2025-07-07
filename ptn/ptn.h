/*-----------------------------------------------------------------------*/
/*-	        PTN.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLLのID
#define  DLLAPP_PTN			1
#define  DLLAPP_PTN_MAIN	2
#define  DLLAPP_PTN_FILE  	3

/*-	タスクＩＤ			                -*/
#define     TD_PTN                  L"PTN"
#define     TD_FILE                 L"FILE"
#define     TD_MAIN                 L"MAIN"


/*-	プラグマＩＤ（リソースＩＤ）        -*/
#define     PTN_ID                  100
#define     FILE_ID                 101
#define     MAIN_ID                 102


/*-	ダイアログボックスＩＤ              -*/
#define     DIALOGID_PTN            200

/*-	ダイアログボックスフック関数ＩＤ    -*/
#define     HOOKID_PTN_DIALOG       200


/*-	プッシュボタンＩＤ                  -*/
#define     PUSHID_TABLE            402
#define     PUSHID_BAT              403
#define     PUSHID_CEL              404
#define     PUSHID_OK               400
#define     PUSHID_END              401

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define     HOOKID_PUSH_TABLE       402
#define     HOOKID_PUSH_BAT         403
#define     HOOKID_PUSH_CEL         404
#define     HOOKID_TOGGLE_PTLV      600
#define     HOOKID_PUSH_OK          400
#define     HOOKID_PUSH_END         401
#define     HOOKID_OPTION_MODE      700


/*-	テキストアイテムＩＤ                -*/
#define     TEXTID_TABLE            500
#define     TEXTID_BAT              501
#define     TEXTID_CEL              502
#define     TEXTID_LV               510


/*- トグルボタンＩＤ                    -*/
#define     TOGGLEID_PTLV           600
#define     TOGGLEID_FENCE          601
#define     TOGGLEID_BUNKATU        602

/*- オプションボタンＩＤ                -*/
#define		OPTIONID_MODE           700
#define		OPTIONID_OPT            701


typedef struct	PTN_INFO {
	WChar		table[128];
	int			mode, opt;
	WChar		bat[128];
	WChar		cel[128];
	int			ptlv, fence, bunkatu;
	UInt32      lv;
	int			nn, ct;
}	PTN;
