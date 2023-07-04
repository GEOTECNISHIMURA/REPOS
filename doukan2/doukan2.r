/*-----------------------------------------------------------------------*/
/*-		DOUKAN2.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"doukan2.h"
DllMdlApp DLLAPP_DOUKAN2 =
 
{
    L"DOUKAN2", L"Doukan2"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DOUKAN2 = {
   DIALOGATTR_SINKABLE,
   36 * XC,
   GENY(6.7),
   NOHELP,
   MHELP,
   HOOKID_DOUKAN2_DIALOG,
   NOPARENTID,
   "道管2",
   {
      { { XC*1,  GENY(1.0), 0,     0}, ToggleButton, TOGGLEID_IDX,   ON, 0, "", ""},
      { { XC*4,  GENY(1.0), XC*26, 0}, Text,         TEXTID_IDX,     ON, 0, "", ""},
      { { XC*30, GENY(0.6), 0,     0}, PushButton,   PUSHID_IDX,     ON, 0, "", ""},
      { { XC*4,  GENY(2.4), XC*26, 0}, Text,         TEXTID_TBL,     ON, 0, "", ""},
      { { XC*30, GENY(2.2), 0,     0}, PushButton,   PUSHID_TBL,     ON, 0, "", ""},
      { { XC*1,  GENY(3.7), 0,     0}, PushButton,   PUSHID_65,      ON, 0, "", ""},
      { { XC*13, GENY(3.7), 0,     0}, PushButton,   PUSHID_TEST,    ON, 0, "", ""},
      { { XC*25, GENY(3.7), 0,     0}, PushButton,   PUSHID_BUNKATU, ON, 0, "", ""},
      { { XC*1,  GENY(5.2), XC*10, 0}, PushButton,   PUSHID_GG,      ON, 0, "", ""},
      { { XC*12, GENY(5.2), XC*15, 0}, PushButton,   PUSHID_KANRI ,  ON, 0, "", ""},
      { { XC*28, GENY(5.2), 0,     0}, PushButton,   PUSHID_END,     ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	トグルボタン（IDX）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, 
    NOARG, NOMASK, NOINVERT,
    "",
    "doukan2.tog_idx"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（IDX）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_IDX = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_IDX,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"IDX"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（IDX）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_IDX = {
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
	"doukan2.idxName"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（TBL）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TBL = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TBL,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"TBL"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（TBL）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TBL = {
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
	"doukan2.tblName"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（65点以上）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_65 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_65,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"65点以上"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（64点以内で分割可能かテスト）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TEST = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TEST,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"分割単独"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（分割）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_BUNKATU = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_BUNKATU,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"分割all"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（GG検査）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_GG = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_GG,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"GG検査"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（管理番号検査）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_KANRI = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_KANRI,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"管理番号検査"
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
