/*-----------------------------------------------------------------------*/
/*-	        DELELE.H		ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
//#include	<mdlio.h>
//#include	<dlogitem.h>

#ifndef     __deleleH__
#define     __deleleH__


#include <Mstn\MicroStation.r.h>	// WChar用

// DLLのID
#define  DLLAPP_DELELE			1
#define  DLLAPP_DELELE_FILE		2
#define  DLLAPP_DELELE_DEL		3

/*-	タスクＩＤ			　　-*/
#define		TD_DELELE		L"DELELE"
#define		TD_FILE			L"FILEA"
#define		TD_DEL			L"DELA"


/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_DELELE_DIALOG	200

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_TABLE	300
#define		HOOKID_PUSH_BAT		301
#define		HOOKID_PUSH_OK		302
#define		HOOKID_PUSH_END		303

#define		HOOKID_OPTION_MODE	320

/*-	ダイアログボックスＩＤ		　　-*/
#define		DIALOGID_DELELE		500

/*-	テキストＩＤ		　　-*/
#define		TEXTID_TABLE		600
#define		TEXTID_BAT		601

/*-	プッシュボタンＩＤ		　　-*/
#define		PUSHID_TABLE		700
#define		PUSHID_BAT		701
#define		PUSHID_OK		702
#define		PUSHID_END		703

/*-	オプションボタンＩＤ		　　-*/
#define		OPTIONID_MODE		800

typedef struct	DELSET_INFO {
		WChar	table[128];
		int	mode;
		WChar	bat[128];
		int	nn;
	}	DELSET;

#endif