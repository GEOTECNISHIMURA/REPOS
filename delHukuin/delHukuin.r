/*-----------------------------------------------------------------------*/
/*-		DELHUKUIN.R							-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"delHukuin.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELHUKUIN =
{
    L"DELHUKUIN", L"DelHukuin"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DELHUKUIN = {
   DIALOGATTR_SINKABLE,
   31 * XC,
   GENY(7.8),
   NOHELP,
   MHELP,
   HOOKID_DELHUKUIN_DIALOG,
   NOPARENTID,
   "delHukuin",
   {
      { { XC*1,  GENY(1.0), 0,     0}, ToggleButton, TOGGLEID_IDX,     ON, 0, "", ""},
      { { XC*4,  GENY(0.7), 0,     0}, PushButton,   PUSHID_IDX,       ON, 0, "", ""},
      { { XC*11, GENY(1.0), XC*20, 0}, Text,         TEXTID_IDX,       ON, 0, "", ""},
      { { XC*11, GENY(2.2), 0,     0}, Label,        LABELID_LAYER,    ON, 0, "画層  色  線種 線幅", ""},
      { { XC*11, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_LV, ON, 0, "", ""},
      { { XC*15, GENY(3.2), XC*5,  0}, Text,         TEXTID_HUKUIN_CO, ON, 0, "", ""},
      { { XC*20, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_ST, ON, 0, "", ""},
      { { XC*24, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_WT, ON, 0, "", ""},
      { { XC*11, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_LV,  ON, 0, "", ""},
      { { XC*15, GENY(4.2), XC*5,  0}, Text,         TEXTID_KUKAN_CO,  ON, 0, "", ""},
      { { XC*20, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_ST,  ON, 0, "", ""},
      { { XC*24, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_WT,  ON, 0, "", ""},
      { { XC*11, GENY(5.3), XC*7,  0}, Text,         TEXTID_KYOYOUTI,  ON, 0, "", ""},
      { { XC*1,  GENY(6.3), 0,     0}, PushButton,   PUSHID_OK,        ON, 0, "", ""},
      { { XC*23, GENY(6.3), 0,     0}, PushButton,   PUSHID_END,       ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	トグルボタン（IDX）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, NOARG, NOMASK, NOINVERT,
    "",
    "delHukuin.tog_idx"
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


/*----------------------------------------------------------------------+
**  テキスト（IDX）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_IDX =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    254,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.idxName"
};


/*----------------------------------------------------------------------+
**  テキスト（幅員_画層）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_LV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "幅員",
    "delHukuin.hu_lv"
};


/*----------------------------------------------------------------------+
**  テキスト（幅員_色）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_CO =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_co"
};


/*----------------------------------------------------------------------+
**  テキスト（幅員_線種）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_ST =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_st"
};


/*----------------------------------------------------------------------+
**  テキスト（幅員_線幅）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_WT =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_wt"
};


/*----------------------------------------------------------------------+
**  テキスト（区間割線_画層）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_LV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "区間割",
    "delHukuin.ku_lv"
};


/*----------------------------------------------------------------------+
**  テキスト（区間割線_色）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_CO =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_co"
};


/*----------------------------------------------------------------------+
**  テキスト（区間割線_線種）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_ST =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_st"
};


/*----------------------------------------------------------------------+
**  テキスト（区間割線_線幅）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_WT =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_wt"
};


/*----------------------------------------------------------------------+
**  テキスト（許容値）
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KYOYOUTI =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    5,
    "%f",
    "%f",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "許容値[m]",
    "delHukuin.kyoyouti"
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