/*-----------------------------------------------------------------------*/
/*-		削除			                        	-*/
/*-									-*/
/*-	        DELELE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"delele.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELELE =
{
    L"DELELE", L"Delele"          // taskid, dllName
}


/*-----------------------------------------------------------------------
**	ダイアログボックス
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DELELE = {
   DIALOGATTR_SINKABLE,
   22 * XC,
   GENY(10),
   NOHELP,
   MHELP,
   HOOKID_DELELE_DIALOG,
   NOPARENTID,
   "削除 Ver.1.4", 
   {
     { {XC*1,   GENY(1),  0,     0}, PushButton,  PUSHID_TABLE, ON, 0, "",         ""},
     { {XC*1,   GENY(2.5),XC*20, 0}, Text,        TEXTID_TABLE, ON, 0, "",         ""},
     { {XC*6.5, GENY(4),  0,     0}, OptionButton,OPTIONID_MODE,ON, 0, "",         ""},
     { {XC*1,   GENY(5),  0,     0}, PushButton,  PUSHID_BAT,   ON, 0, "",         ""},
     { {XC*1,   GENY(6.5),XC*20, 0}, Text,        TEXTID_BAT,   ON, 0, "",         ""},
     { {XC*1,   GENY(8),  XC*6,  0}, PushButton,  PUSHID_OK,    ON, 0, "",         ""},
     { {XC*15,  GENY(8),  XC*6,  0}, PushButton,  PUSHID_END,   ON, 0, "",         ""},
     { {XC*1,   GENY(4),  0,     0}, Label,       0,            ON, 0, "処理",     ""},
   }
};


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブル）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TABLE = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TABLE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"テーブル"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（テーブル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TABLE = {
	NOCMD,
	LCMD,
	NOSYNONYM,
	NOHELP,
	LHELPCMD,
	NOHOOK,
	NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"delset.table"
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（モード）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_MODE,
	NOARG,
	"",
	"delset.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "1ﾌｧｲﾙ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "バッチ"},
        }
};


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_BAT = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_BAT,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"バッチ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（バッチ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_BAT = {
	NOCMD,
	LCMD,
	NOSYNONYM,
	NOHELP,
	LHELPCMD,
	NOHOOK,
	NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"delset.bat"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_OK = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"実行"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_END = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"終了"
};
