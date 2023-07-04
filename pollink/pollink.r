/*-----------------------------------------------------------------------*/
/*-	        POLLINK.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"pollink.h"

DllMdlApp DLLAPP_POLLINK =
{
    L"POLLINK", L"pollink"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	ダイアログボックス
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_POLLINK = {
   DIALOGATTR_SINKABLE,
   33 * XC,
   GENY(11.8),
   NOHELP,
   MHELP,
   HOOKID_POLLINK_DIALOG,
   NOPARENTID,
   "ﾎﾟﾘｺﾞﾝ内テキストでMSLINK", 
   {
     { { XC*9,  GENY(0.7),  XC*24, 0}, Text,         TEXTID_POLLINK_DBSOURCE,  ON, 0, "", ""},
     { { XC*12, GENY(2.4),  0,     0}, ToggleButton, TOGGLEID_POLLINK_ALL,     ON, 0, "", ""},
     { { XC*1,  GENY(2.4),  0,     0}, ToggleButton, TOGGLEID_POLLINK_IDX,     ON, 0, "", ""},
     { { XC*4,  GENY(2.0),  0,     0}, PushButton,   PUSHID_POLLINK_IDX,       ON, 0, "", ""},
     { { XC*1,  GENY(3.4),  XC*32, 0}, Text,         TEXTID_POLLINK_IDX,       ON, 0, "", ""},
     { { XC*1,  GENY(4.6),  0,     0}, PushButton,   PUSHID_POLLINK_TBL,       ON, 0, "", ""},
     { { XC*1,  GENY(6.1),  XC*32, 0}, Text,         TEXTID_POLLINK_TBL,       ON, 0, "", ""},
     { { XC*13, GENY(7.2),  XC*4,  0}, Text,         TEXTID_POLLINK_TOLERANCE, ON, 0, "", ""},
     { { XC*10, GENY(8.3),  0,     0}, Label,        LABELID_ERR,              ON, 0, "画層  色   線種 線幅", ""},
     { { XC*10, GENY(9.3),  XC*4,  0}, Text,         TEXTID_POLLINK_ERRLV,     ON, 0, "", ""},
     { { XC*14, GENY(9.3),  XC*5,  0}, Text,         TEXTID_POLLINK_ERRCO,     ON, 0, "", ""},
     { { XC*20, GENY(9.3),  XC*3,  0}, Text,         TEXTID_POLLINK_ERRST,     ON, 0, "", ""},
     { { XC*24, GENY(9.3),  XC*4,  0}, Text,         TEXTID_POLLINK_ERRWT,     ON, 0, "", ""},
     { { XC*0,  GENY(10.3), 0,     0}, PushButton,   PUSHID_POLLINK_OK,        ON, 0, "", ""},
     { { XC*13, GENY(10.3), 0,     0}, PushButton,   PUSHID_POLLINK_SELECT,    ON, 0, "", ""},
     { { XC*26, GENY(10.3), 0,     0}, PushButton,   PUSHID_POLLINK_END,       ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	テキストアイテム（データソース名）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_DBSOURCE = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"ﾃﾞｰﾀｿｰｽ",
	"pollink.dbsource"
};


/*-----------------------------------------------------------------------
**	トグルボタン（全て出力）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_POLLINK_ALL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_POLLINK_IDX, NOARG, NOMASK, NOINVERT,
    "MSLINK付きも再出力",
    "pollink.tog_all"
};


/*-----------------------------------------------------------------------
**	トグルボタン（バッチ）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_POLLINK_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_POLLINK_IDX, NOARG, NOMASK, NOINVERT,
    "",
    "pollink.tog_idx"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_POLLINK_IDX = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_POLLINK_IDX,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"IDX"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（バッチ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_IDX = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"pollink.idxName"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（TBL）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_POLLINK_TBL = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_POLLINK_TBL,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"TBL"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（TBL）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_TBL = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"pollink.tblName"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（許容値）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_TOLERANCE = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"許容値[mm]",
	"pollink.tolerance"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（エラー画層）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_ERRLV = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"ｴﾗｰﾚｲﾔ",
	"pollink.err_lv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（エラー色）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_ERRCO = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"0",
	"254",
	NOMASK,
	NOCONCAT,
	"",
	"pollink.err_co"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（エラー線種）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_ERRST = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	1,
	"%d",
	"%d",
	"0",
	"7",
	NOMASK,
	NOCONCAT,
	"",
	"pollink.err_st"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（エラー線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_POLLINK_ERRWT = {
	NOCMD,LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"",
	"pollink.err_wt"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_POLLINK_OK = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_POLLINK_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"実行"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（選択）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_POLLINK_SELECT = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_POLLINK_SELECT,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"選択"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_POLLINK_END = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_POLLINK_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"終了"
};
