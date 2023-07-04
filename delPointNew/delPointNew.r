/*-----------------------------------------------------------------------*/
/*-		delPointNew.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"delPointNew.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELPOINTNEW =
{
    L"DELPOINTNEW", L"DelPointNew"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_MAIN = {
   DIALOGATTR_SINKABLE,
   34 * XC,
   GENY(6.5),
   NOHELP,
   MHELP,
   HOOKID_MAIN_DIALOG,
   NOPARENTID,
   "点の間引き",
   {
      { { XC*1,  GENY(0.7), 0,     0}, PushButton, PUSHID_TABLE,       ON, 0, "", ""},
      { { XC*10, GENY(1.1), XC*24, 0}, Text,       TEXTID_TABLE,       ON, 0, "", ""},
      { { XC*1,  GENY(2.2), 0,     0}, PushButton, PUSHID_TABLE_KUKAN, ON, 0, "", ""},
      { { XC*10, GENY(2.6), XC*24, 0}, Text,       TEXTID_TABLE_KUKAN, ON, 0, "", ""},
      { { XC*13, GENY(3.9), XC*7,  0}, Text,       TEXTID_RR,          ON, 0, "", ""},
      { { XC*1,  GENY(5.0), 0,     0}, PushButton, PUSHID_OK,          ON, 0, "", ""},
      { { XC*13, GENY(5.0), 0,     0}, PushButton, PUSHID_KUKAN,       ON, 0, "", ""},
      { { XC*26, GENY(5.0), 0,     0}, PushButton, PUSHID_END,         ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	プッシュボタン（ﾃｰﾌﾞﾙ）
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
	"ﾃｰﾌﾞﾙ"
};


/*----------------------------------------------------------------------+
**  テキスト（ﾃｰﾌﾞﾙ）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_TABLE =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    255,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    TEXT_NOCONCAT,
    "",
    "delPointNew.tblName"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（ﾃｰﾌﾞﾙ）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TABLE_KUKAN = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TABLE_KUKAN,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"区間"
};


/*----------------------------------------------------------------------+
**  テキスト（ﾃｰﾌﾞﾙ）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_TABLE_KUKAN =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    255,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    TEXT_NOCONCAT,
    "",
    "delPointNew.kukanName"
};

/*----------------------------------------------------------------------+
**  テキスト（半径）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_RR =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    5,
    "%f",
    "%f",
    "",
    "",
    NOMASK,
    TEXT_NOCONCAT,
    "×の長さ[m]",
    "delPointNew.rr"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_OK = {
	NOT_DEFAULT_BUTTON,
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
**	プッシュボタン（区間）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_KUKAN = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_KUKAN,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"区間"
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