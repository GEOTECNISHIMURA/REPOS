/*-----------------------------------------------------------------------*/
/*-		file_c.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"file_c.h"

DllMdlApp DLLAPP_FILE_C =
{
    L"file_c", L"File_c"          // taskid, dllName
}


/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_MAIN = {
    DIALOGATTR_AUTOUNLOADAPP,
    50 * XC,
    GENY(11),
    NOHELP,
    MHELP,
    NOHOOK,
    NOPARENTID,
    "file_c  Ver.1.1",
    {
        { {  1*XC, GENY(0.8),  0,     0}, ToggleButton, TOGGLEID_REFFILE,    ON, 0, "", ""},
        { {  1*XC, GENY(1.9),  0,     0}, ToggleButton, TOGGLEID_VIEWTOP,    ON, 0, "", ""},
        { {  1*XC, GENY(3.0),  0,     0}, ToggleButton, TOGGLEID_TEXTNODES,  ON, 0, "", ""},
        { {  1*XC, GENY(4.1),  0,     0}, ToggleButton, TOGGLEID_LEVELS,     ON, 0, "", ""},
        { {  1*XC, GENY(5.2),  0,     0}, ToggleButton, TOGGLEID_CELL,       ON, 0, "", ""},
        { {  3*XC, GENY(5.0),  0,     0}, PushButton,   PUSHID_CELL,         ON, 0, "", ""},
        { { 16*XC, GENY(5.4),  XC*30, 0}, Text,         TEXTID_CELL,         ON, 0, "", ""},
        { {  1*XC, GENY(6.6),  0,     0}, ToggleButton, TOGGLEID_FILEDESIGN, ON, 0, "", ""},
        { {  7*XC, GENY(7.8),  0,     0}, PushButton,   PUSHID_BAT,          ON, 0, "", ""},
        { { 16*XC, GENY(8.2),  XC*30, 0}, Text,         TEXTID_BAT,          ON, 0, "", ""},
        { {  1*XC, GENY(9.5),  0,     0}, PushButton,   PUSHID_OK,           ON, 0, "", ""},
        { { 42*XC, GENY(9.5),  0,     0}, PushButton,   PUSHID_END,          ON, 0, "", ""},
    }
};


/*----------------------------------------------------------------------+
|   トグルボタン（参照ファイルのデタッチ） 			    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REFFILE ={
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "参照ファイルのデタッチ",
    "file_c.reffile"
};


/*----------------------------------------------------------------------+
|   トグルボタン（ビュートップで保存） 			    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_VIEWTOP ={
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "ビュートップ",
    "file_c.viewtop"
};


/*----------------------------------------------------------------------+
|   トグルボタン（文字ノードＯＮ／ＯＦＦ）	    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_TEXTNODES ={
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG, NOMASK, NOINVERT,
    "文字ノードＯＮ／ＯＦＦ",
    "file_c.textnodes"
};


/*----------------------------------------------------------------------+
|   トグルボタン（画層を全てＯＮ） 			    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_LEVELS = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "画層を全てＯＮ(View 1)",
    "file_c.levels"
};


/*----------------------------------------------------------------------+
|   トグルボタン（セルライブラリ） 			    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CELL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    HOOKID_TOGGLE_CELL,
    NOARG, NOMASK, NOINVERT,
    "",
    "file_c.tog_cell"
};


/*----------------------------------------------------------------------+
|   プッシュボタン（セルライブラリ） 			    	|
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_CELL = {
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELP,
    HOOKID_PUSH_CELL,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "ｾﾙﾗｲﾌﾞﾗﾘ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（セルライブラリ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CELL = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"file_c.cellib"
}


/*----------------------------------------------------------------------+
|   トグルボタン（設定値保存） 			    	|
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_FILEDESIGN ={
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "設定値保存",
    "file_c.filedesign"
};


/*----------------------------------------------------------------------+
|   プッシュボタン（バッチ） 			    	|
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_BAT ={
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELP,
    HOOKID_PUSH_BAT,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "バッチ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（バッチ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_BAT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"file_c.idxname"
}


/*----------------------------------------------------------------------+
|   プッシュボタン（実行） 			    	|
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_OK ={
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELP,
    HOOKID_PUSH_OK,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "実行"
};


/*----------------------------------------------------------------------+
|   プッシュボタン（終了） 			    	|
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_END ={
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELP,
    HOOKID_PUSH_END,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "終了"
};
