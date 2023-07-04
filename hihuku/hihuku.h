/*-----------------------------------------------------------------------*/
/*-	        HIHUKU.H		ﾍｯﾀﾞｰﾌｧｲﾙ				-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLLのID
#define  DLLAPP_HIHUKU			1
#define  DLLAPP_HIHUKU_SINGLE	2

/*-	タスクＩＤ			　　			-*/
#define		TD_HIHUKU			L"HIHUKU"
#define		TD_SINGLE			L"SINGLE"

/*-	プラグマＩＤ（リソースＩＤ）	　　-*/
#define		HIHUKU_ID				100
#define		SINGLE_ID				101


/*-	ダイアログボックスＩＤ		　　	-*/
#define		DIALOGID_HIHUKU			200

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_HIHUKU_DIALOG	200


/*-	プッシュボタンＩＤ		　　		-*/
#define		PUSHID_TLAYER			301
#define		PUSHID_BLAYER			302
#define		PUSHID_KLAYER			303
#define		PUSHID_OK				304
#define		PUSHID_END				305

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_TLAYER		301
#define		HOOKID_PUSH_BLAYER		302
#define		HOOKID_PUSH_KLAYER		303
#define		HOOKID_PUSH_OK			304
#define		HOOKID_PUSH_END			305


/*-	オプションボタンＩＤ		　　	-*/
#define		OPTIONID_SCALE			400


/*-	ラベルＩＤ		　　				-*/
#define		LABELID_HIHUKU			500


/*-	テキストＩＤ		　　-*/
#define		TEXTID_HIHUKU_CELL		600
#define		TEXTID_HIHUKU_TLV		610
#define		TEXTID_HIHUKU_TCO		611
#define		TEXTID_HIHUKU_TLC		612
#define		TEXTID_HIHUKU_TWT		613

#define		TEXTID_HIHUKU_BLV		620
#define		TEXTID_HIHUKU_BCO		621
#define		TEXTID_HIHUKU_BLC		622
#define		TEXTID_HIHUKU_BWT		623

#define		TEXTID_HIHUKU_KLV		630
#define		TEXTID_HIHUKU_KCO		631
#define		TEXTID_HIHUKU_KLC		632
#define		TEXTID_HIHUKU_KWT		633


/*-	トグルボタンＩＤ		　　	-*/
#define		TOGGLEID_SINGLE			700


typedef struct	HIHUKU_INFO {
    WChar    cell[7];
	UInt32		tlv, tco, twt;
	UInt32		blv, bco, bwt;
	UInt32      klv, kco, kwt;
	int      tlc, blc, klc;
    int		tog;
    int		scale;
}	HIHUKU;

