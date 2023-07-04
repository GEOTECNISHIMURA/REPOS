/*-----------------------------------------------------------------------*/
/*-     DNGLCHK.R                                                       -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "dnglchk.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DNGLCHK =
{
    L"DNGLCHK", L"Dnglchk"          // taskid, dllName
}
/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_DNGLCHK = {
   DIALOGATTR_SINKABLE,
   21 * XC,
   GENY(15.3),
   NOHELP,
   MHELP,
   HOOKID_DNGLCHK_DIALOG,
   NOPARENTID,
   "ﾀﾞﾝｸﾞﾙck Ver.1.3",
   {
      { {XC*0.5, GENY(0.8),  0,     0}, PushButton,  PUSHID_TABLE,  ON, 0, "", ""},
      { {XC*1,   GENY(2.3),  XC*19, 0}, Text,        TEXTID_TABLE,  ON, 0, "", ""},
      { {XC*6,   GENY(3.8),  0,     0}, OptionButton,OPTIONID_KIND, ON, 0, "", ""},
      { {XC*9,   GENY(5.3),  XC*8,  0}, Text,        TEXTID_SIZE,   ON, 0, "", ""},
      { {XC*1,   GENY(6.55),0,      0}, Label,       LABELID_CHK,   ON, ALIGN_LEFT, "     ﾀﾞﾝｸﾞﾙ                折返し", "" },
      { {XC*7,   GENY(7.55), XC*4,  0}, Text,        TEXTID_LV1,    ON, 0, "", ""},
      { {XC*7,   GENY(8.55), XC*5,  0}, Text,        TEXTID_CO1,    ON, 0, "", ""},
      { {XC*7,   GENY(9.55), XC*3,  0}, Text,        TEXTID_LC1,    ON, 0, "", ""},
      { {XC*7,   GENY(10.55), XC*4,  0}, Text,        TEXTID_WT1,    ON, 0, "", ""},
      { {XC*15,  GENY(7.55), XC*4,  0}, Text,        TEXTID_LV2,    ON, 0, "", ""},
      { {XC*15,  GENY(8.55), XC*5,  0}, Text,        TEXTID_CO2,    ON, 0, "", ""},
      { {XC*15,  GENY(9.55), XC*3,  0}, Text,        TEXTID_LC2,    ON, 0, "", ""},
      { {XC*15,  GENY(10.55), XC*4,  0}, Text,        TEXTID_WT2,    ON, 0, "", ""},
      { {XC*9,   GENY(11.8), XC*9,  0}, Text,        TEXTID_ANGLE,  ON, 0, "", ""},
      { {XC*1.5, GENY(13.3), 0,     0}, PushButton,  PUSHID_OK,     ON, 0, "", ""},
      { {XC*12.5,GENY(13.3), 0,     0}, PushButton,  PUSHID_END,    ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**  プッシュボタン（レイヤーテーブル選択）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_TABLE = {
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
**  テキストアイテム　（レイヤーテーブル）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TABLE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    128,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "dnglchk.table"
};


/*-----------------------------------------------------------------------
**  オプションボタンアイテム（方法）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_KIND = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    NOHOOK,
    NOARG,
    "方法",
    "dnglchk.kind",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "単一要素" },
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "連続要素"   },
    }
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（エラーサイズ）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_SIZE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    6,
    "%lf",
    "%lf",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "ｴﾗｰｻｲｽﾞ",
    "dnglchk.size"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（画層）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LV1 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "1",
    "63",
    NOMASK,
    NOCONCAT,
    "画層",
    "dnglchk.lv1"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（色）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO1 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "色",
    "dnglchk.co1"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線種）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LC1 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "0",
    "7",
    NOMASK,
    NOCONCAT,
    "線種",
    "dnglchk.lc1"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_WT1 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "0",
    "31",
    NOMASK,
    NOCONCAT,
    "線幅",
    "dnglchk.wt1"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（画層）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LV2 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "1",
    "63",
    NOMASK,
    NOCONCAT,
    "",
    "dnglchk.lv2"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（色）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO2 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "",
    "dnglchk.co2"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線種）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LC2 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "0",
    "7",
    NOMASK,
    NOCONCAT,
    "",
    "dnglchk.lc2"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_WT2 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "0",
    "31",
    NOMASK,
    NOCONCAT,
    "",
    "dnglchk.wt2"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線分角度）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_ANGLE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    7,
    "%lf",
    "%lf",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "角度",
    "dnglchk.angle"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_OK = {
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
**  プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_END = {
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