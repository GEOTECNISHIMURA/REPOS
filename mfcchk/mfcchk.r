/*-----------------------------------------------------------------------*/
/*-     MFCCHK.R                                                        -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include "mfcchk.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_MFCCHK =
{
    L"MFCCHK", L"Mfcchk"          // taskid, dllName
}

/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc        DIALOGID_MFCCHK = {
   DIALOGATTR_SINKABLE,
   25 * XC,
   GENY(19.9),
   NOHELP,
   MHELP,
   HOOKID_MFCCHK_DIALOG,
   NOPARENTID,
   "ＭＦＣ検査   Ver.1.8",
   {
      { {XC*0.5, GENY(0.8),  0,     0}, PushButton,  PUSHID_TABLE,         ON, 0, "",             ""},
      { {XC*1,   GENY(2.3),  XC*24, 0}, Text,        TEXTID_TABLE,         ON, 0, "",             ""},
      { {XC*1,   GENY(3.8),  0,     0}, Label,       0,                    ON, 0, "処理",         ""},
      { {XC*6.5, GENY(3.8),  0,     0}, OptionButton,OPTIONID_MODE,        ON, 0, "",             ""},
      { {XC*0.5, GENY(5),    0,     0}, PushButton,  PUSHID_BAT,           ON, 0, "",             ""},
      { {XC*1,   GENY(6.5),  XC*24, 0}, Text,        TEXTID_BAT,           ON, 0, "",             ""},
      { {0.5*XC, GENY(7.7),  24*XC,7.5*YC},GroupBox, 0,                    ON, 0, "",             ""},
      { {XC*1.5, GENY(8.2),  0,     0}, ToggleButton,TOGGLEID_ERR,         ON, 0, "",             ""},
      { {XC*7,   GENY(9.2),  XC*5,  0}, Text,        TEXTID_LV,            ON, 0, "",             ""},
      { {XC*7,   GENY(10.2), XC*5,  0}, Text,        TEXTID_CO,            ON, 0, "",             ""},
      { {XC*12.5,GENY(10.2), 0,     0}, ColorPicker, COLORPICKERID_ELEMCO, ON, 0, "",             ""},
      { {XC*7,   GENY(11.2), XC*5,  0}, Text,        TEXTID_LC,            ON, 0, "",             ""},
      { {XC*7,   GENY(12.2), XC*5,  0}, Text,        TEXTID_WT,            ON, 0, "",             ""},
      { {XC*15,  GENY(9.2),  XC*14, 0}, Label,       0,                    ON, 0, "エラー数",     ""},
      { {XC*16,  GENY(10.2), XC*8,  0}, Text,        TEXTID_CT,            ON, 0, "",             ""},
      { {XC*15,  GENY(11.2), XC*14, 0}, Label,       0,                    ON, 0, "ｴﾗｰｻｲｽﾞ",      ""},
      { {XC*16,  GENY(12.2), XC*8,  0}, Text,        TEXTID_SIZE,          ON, 0, "",             ""},
      { {0.5*XC, GENY(14.5), 24*XC, 3.5*YC}, GroupBox, 0,                  ON, 0, "テキスト設定", ""},
      { {XC*1.5, GENY(15),   0,     0}, ToggleButton,TOGGLEID_FONT,        ON, 0, "",             ""},
      { {XC*19,  GENY(15),   XC*5,  0}, Text,        TEXTID_ZEN,           ON, 0, "",             ""},
      { {XC*19,  GENY(16),   XC*5,  0}, Text,        TEXTID_HAN,           ON, 0, "",             ""},
      { {XC*1.5, GENY(16),   0,     0}, ToggleButton,TOGGLEID_SPACE,       ON, 0, "",             ""},
      { {XC*2,   GENY(17.75),0,     0}, PushButton,  PUSHID_OK,            ON, 0, "",             ""},
      { {XC*16,  GENY(17.75),0,     0}, PushButton,  PUSHID_END,           ON, 0, "",             ""},
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
    "MFCテーブル"
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
    "mfcchk.table"
};


/*-----------------------------------------------------------------------
**  オプションボタンアイテム（モード）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_MODE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    HOOKID_OPTION_MODE,
    NOARG,
    "",
    "mfcchk.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "1ﾌｧｲﾙ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "バッチ"},
        }
};


/*-----------------------------------------------------------------------
**  プッシュボタン（バッチファイル選択）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_BAT = {
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
**  テキストアイテム　（バッチファイル）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_BAT = {
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
    "mfcchk.bat"
};


/*-----------------------------------------------------------------------
**  トグルアイテム　（エラーフラグ）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_ERR = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_ERR,
    NOARG, NOMASK, NOINVERT,
    "エラーフラグ",
    "mfcchk.err"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（画層）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LV = {
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
    "mfcchk.lv"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（色）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO = {
    NOCMD, LCMD, SYNONYMID_ELECO, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "色",
    "mfcchk.co"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線種）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LC = {
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
    "mfcchk.lc"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_WT = {
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
    "mfcchk.wt"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（エラー数）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_ERRCT_TEXT,
    NOARG,
    5,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "mfcchk.ct"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（エラーサイズ）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_SIZE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK,
    NOARG,
    6,
    "%.3lf",
    "%lf",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "mfcchk.size"
};


/*-----------------------------------------------------------------------
**  カラーピッカーアイテム　（カラーパレット）
-----------------------------------------------------------------------*/
DItem_ColorPickerRsc    COLORPICKERID_ELEMCO = {
    NOCMD, LCMD, SYNONYMID_ELECO, NOHELP, LHELPCMD,
    NOHOOK,
    NOARG,
    TEXTID_CO,
    NOMASK,
    "",
    "mfcchk.co"
};


/*-----------------------------------------------------------------------
**  シノニムリソース　（カラーテキストとカラーパレットの連動）
-----------------------------------------------------------------------*/
DItem_SynonymsRsc SYNONYMID_ELECO = 
    {
        {
            { Text,          TEXTID_CO},
            { ColorPicker,   COLORPICKERID_ELEMCO},
        }
    };


/*-----------------------------------------------------------------------
**  トグルアイテム　（フォント）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_FONT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_FONT,
    NOARG, NOMASK, NOINVERT,
    "フォント",
    "mfcchk.font"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（全角フォント）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_ZEN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "全角",
    "mfcchk.zen"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（半角フォント）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_HAN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "半角",
    "mfcchk.han"
};


/*-----------------------------------------------------------------------
**  トグルアイテム　（スペース）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_SPACE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "スペース",
    "mfcchk.space"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_OK = {
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