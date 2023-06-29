/*----------------------------------------------------------------------+
|   $Workfile:   conter.h  $
+----------------------------------------------------------------------*/
#include <msdefs.h>
#include <dlogitem.h>


/*-	タスクＩＤ			　　-*/
#define		TD_CONTER		"CONTER"
#define		TD_NEW			"NEW"
#define		TD_MODIF		"MODIF"
#define		TD_ONETANI		"ONETANI"


/*-	プラグマＩＤ（リソースＩＤ）	　　-*/
#define		CONTER_ID				100
#define		NEW_ID					101
#define		MODIF_ID				102
#define		ONETANI_ID				103


/*-	ダイアログボックスＩＤ　　-*/
#define		DIALOGID_CONTER			200

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_CONTER_DIALOG	200


/*-	プッシュボタンアイテムＩＤ　　-*/
#define PUSHID_CONTER_INCRE    		301
#define PUSHID_CONTER_DECRE    		302
#define PUSHID_CONTER_KLV    		306
#define PUSHID_CONTER_SLV    		307
#define PUSHID_CONTER_OTLV    		308
#define PUSHID_CONTER_OK     		303
#define PUSHID_CONTER_SETZ   		304
#define PUSHID_CONTER_END     		305

/*-	ダイアログボックスアイテムＩＤ　　-*/
#define HOOKID_PUSH_CONTER_INCRE    301
#define HOOKID_PUSH_CONTER_DECRE    302
#define HOOKID_PUSH_CONTER_KLV      306
#define HOOKID_PUSH_CONTER_SLV      307
#define HOOKID_PUSH_CONTER_OTLV     308
#define HOOKID_PUSH_CONTER_OK     	303
#define HOOKID_PUSH_CONTER_SETZ     304
#define HOOKID_PUSH_CONTER_END     	305


/*-	テキストアイテムＩＤ　　-*/
#define TEXTID_CONTER_ZZ    		400
#define TEXTID_CONTER_PITCH    		401
#define TEXTID_CONTER_GENKOU    	402
#define TEXTID_CONTER_KLV    		403
#define TEXTID_CONTER_SLV    		404
#define TEXTID_CONTER_OTLV    		405
#define TEXTID_CONTER_HANI    		406


/*-	オプションボタンアイテムＩＤ　　-*/
#define OPTIONID_CONTER_MODE   		500

/*-	ダイアログボックスアイテムＩＤ　　-*/
#define HOOKID_OPTION_CONTER_MODE   500


/*-	トグルボタンアイテムＩＤ　　-*/
#define TOGGLEID_CONTER_BSPLN   	600

/*-	ダイアログボックスアイテムＩＤ　　-*/
#define HOOKID_TOGGLE_CONTER_BSPLN  600


typedef struct CONTER_INFO
{
   int		mode;	/* 新規/修正 */
   double	zz;		/* 高さ		 */
   double	pitch;	/* 間隔		 */
   int		bspln;	/* B-ｽﾌﾟﾗｲﾝ	 */
   double	genkou;	/* 弦高		 */
   int		klv;	/* 計曲		 */
   int		slv;	/* 主曲		 */
   int		otlv;	/* 尾根・谷	 */
   int		hani;	/* 範囲		 */
} CONTER;
#endif
