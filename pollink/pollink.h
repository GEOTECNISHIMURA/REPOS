/*-----------------------------------------------------------------------*/
/*-	        POLLINK.H		ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLLのID
#define  DLLAPP_POLLINK			1

/*-	タスクＩＤ			                -*/
#define		TD_POLLINK					    "POLLINK"


/*-	プラグマＩＤ（リソースＩＤ）	    -*/
#define		POLLINK_ID					    100


/*-	ダイアログボックスＩＤ		    	-*/
#define		DIALOGID_POLLINK			    200

/*-	ダイアログボックスフック関数ＩＤ    -*/
#define		HOOKID_POLLINK_DIALOG		    200


/*-	プッシュボタンＩＤ		    		-*/
#define		PUSHID_POLLINK_IDX			    300
#define		PUSHID_POLLINK_TBL			    302
#define		PUSHID_POLLINK_OK			    303
#define		PUSHID_POLLINK_SELECT			305
#define		PUSHID_POLLINK_END			    304

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_POLLINK_IDX		    300
#define		HOOKID_PUSH_POLLINK_TBL		    302
#define		HOOKID_PUSH_POLLINK_OK		    303
#define		HOOKID_PUSH_POLLINK_SELECT	    305
#define		HOOKID_PUSH_POLLINK_END		    304


/*-	テキストＩＤ		        		-*/
#define		TEXTID_POLLINK_DBSOURCE	        402
#define		TEXTID_POLLINK_IDX	            400
#define		TEXTID_POLLINK_TBL	            401
#define		TEXTID_POLLINK_TOLERANCE	    403

#define		TEXTID_POLLINK_ERRLV	        410
#define		TEXTID_POLLINK_ERRCO	        411
#define		TEXTID_POLLINK_ERRST	        412
#define		TEXTID_POLLINK_ERRWT	        413


/*-	トグルボタンＩＤ		        	-*/
#define		TOGGLEID_POLLINK_IDX		    500
#define		TOGGLEID_POLLINK_ALL		    501

/*-	ダイアログボックスアイテムフック関数ＩＤ　　-*/
#define		HOOKID_TOGGLE_POLLINK_IDX	    500
#define		HOOKID_TOGGLE_POLLINK_ALL	    501


/*-	ラベルＩＤ		                	-*/
#define		LABELID_ERR					    600


typedef struct	POLLINK_INFO {
    WChar	dbsource[256];
    int		tog_idx, tog_all, tolerance;
    WChar	idxName[256];
    WChar	tblName[256];
	UInt32  err_lv, err_co, err_wt;
    int	    err_st;
}   POLLINK;

