/*-----------------------------------------------------------------------*/
/*-	        ATTACH8.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLLのID
#define  DLLAPP_ATTACH			1

/*-	タスクＩＤ			    			-*/
#define		TD_MAIN					"ATTACH"

/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		MAIN_ID					100   /*- メイン   	-*/


/*-	ダイアログボックスＩＤ		    	-*/
#define		DIALOGID_ATTACH8		200

/*-	ダイアログボックスフック関数ＩＤ    -*/
#define		HOOKID_ATTACH8_DIALOG	200


/*-	オプションボタンＩＤ				-*/
#define		OPTIONID_MODE			300

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define		HOOKID_OPTION_MODE		300


/*-	プッシュボタンＩＤ				    -*/
#define		PUSHID_IDX				400
#define		PUSHID_REF1				401
#define		PUSHID_REF2				402
#define		PUSHID_REF3				403
#define		PUSHID_REF4				404
#define		PUSHID_REF5				405
#define		PUSHID_REF6				406
#define		PUSHID_REF7				407
#define		PUSHID_REF8				408
#define		PUSHID_REF9				409
#define		PUSHID_REF10			410
#define		PUSHID_OK				411
#define		PUSHID_END				412

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define		HOOKID_PUSH_IDX			400
#define		HOOKID_PUSH_REF1		401
#define		HOOKID_PUSH_REF2		402
#define		HOOKID_PUSH_REF3		403
#define		HOOKID_PUSH_REF4		404
#define		HOOKID_PUSH_REF5		405
#define		HOOKID_PUSH_REF6		406
#define		HOOKID_PUSH_REF7		407
#define		HOOKID_PUSH_REF8		408
#define		HOOKID_PUSH_REF9		409
#define		HOOKID_PUSH_REF10		410
#define		HOOKID_PUSH_OK			411
#define		HOOKID_PUSH_END			412


/*-	テキストアイテムＩＤ		    	-*/
#define		TEXTID_IDX				500
#define		TEXTID_REF1				501
#define		TEXTID_REF2				502
#define		TEXTID_REF3				503
#define		TEXTID_REF4				504
#define		TEXTID_REF5				505
#define		TEXTID_REF6				506
#define		TEXTID_REF7				507
#define		TEXTID_REF8				508
#define		TEXTID_REF9				509
#define		TEXTID_REF10			510


/*-	トグルボタンＩＤ				    -*/
#define		TOGGLEID_REF1			601
#define		TOGGLEID_REF2			602
#define		TOGGLEID_REF3			603
#define		TOGGLEID_REF4			604
#define		TOGGLEID_REF5			605
#define		TOGGLEID_REF6			606
#define		TOGGLEID_REF7			607
#define		TOGGLEID_REF8			608
#define		TOGGLEID_REF9			609
#define		TOGGLEID_REF10			610
#define		TOGGLEID_REF1_KOTEI		611
#define		TOGGLEID_REF2_KOTEI		612
#define		TOGGLEID_REF3_KOTEI		613
#define		TOGGLEID_REF4_KOTEI		614
#define		TOGGLEID_REF5_KOTEI		615
#define		TOGGLEID_REF6_KOTEI		616
#define		TOGGLEID_REF7_KOTEI		617
#define		TOGGLEID_REF8_KOTEI		618
#define		TOGGLEID_REF9_KOTEI		619
#define		TOGGLEID_REF10_KOTEI	620
#define		TOGGLEID_DETACH			621

/*-	ダイアログアイテムフック関数ＩＤ    -*/
#define		HOOKID_TOGGLE_REF1		601
#define		HOOKID_TOGGLE_REF2		602
#define		HOOKID_TOGGLE_REF3		603
#define		HOOKID_TOGGLE_REF4		604
#define		HOOKID_TOGGLE_REF5		605
#define		HOOKID_TOGGLE_REF6		606
#define		HOOKID_TOGGLE_REF7		607
#define		HOOKID_TOGGLE_REF8		608
#define		HOOKID_TOGGLE_REF9		609
#define		HOOKID_TOGGLE_REF10		610


typedef struct	ATTACH8_INFO {
	int		mode, tog[10], tog_kotei[10], tog_detach;
	WChar	idxName[256];
	WChar	refName[10][256];
} ATTACH8;
