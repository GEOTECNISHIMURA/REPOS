/*-----------------------------------------------------------------------*/
/*-	        DELELE.H		ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/

#ifndef     __deleleH__
#define     __deleleH__


#include <Mstn\MicroStation.r.h>	// WChar用

// DLLのID
#define  DLLAPP_REFMERGE			1

/*-	タスクＩＤ			　　-*/
#define		TD_REFMERGE		L"REFMERGE"

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_REFMERGE_DIALOG	200

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_BAT		300
#define		HOOKID_PUSH_OK		301
#define		HOOKID_PUSH_END		302

#define		HOOKID_TOGGLE_BAT	320
#define		HOOKID_TOGGLE_MODE	321

/*-	ダイアログボックスＩＤ		　　-*/
#define		DIALOGID_REFMERGE		500

/*-	テキストＩＤ		　　-*/
#define		TEXTID_BAT		600

/*-	プッシュボタンＩＤ		　　-*/
#define		PUSHID_BAT		700
#define		PUSHID_OK		701
#define		PUSHID_END		702

/*-	オプションボタンＩＤ		　　-*/
#define		TOGGLEID_BAT        800
#define		TOGGLEID_MODE		801

typedef struct	REFMERGE_INFO {
	    int tog_bat;
		int	mode;
		WChar	bat[256];
	}	REFMERGE;

#endif