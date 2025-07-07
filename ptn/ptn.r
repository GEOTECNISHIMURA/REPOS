/*-----------------------------------------------------------------------*/
/*-     PTN.R                                                           -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "ptn.h"


DllMdlApp DLLAPP_PTN =
{
    TD_PTN, L"ptn"          // taskid, dllName
}
/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_PTN = {
   DIALOGATTR_SINKABLE,
   26 * XC,
   GENY(16.7),
   NOHELP,
   MHELP,
   HOOKID_PTN_DIALOG,
   NOPARENTID,
   "ﾊﾟﾀｰﾝ発生  ",
   {
      { {XC*0.5,  GENY(0.8),  0,     0}, PushButton,   PUSHID_TABLE,      ON, 0, "",     ""},
      { {XC*1,    GENY(2.3),  XC*24, 0}, Text,         TEXTID_TABLE,      ON, 0, "",     ""},
      { {XC*6.5,  GENY(3.8),  0,     0}, OptionButton, OPTIONID_MODE,     ON, 0, "",     ""},
      { {XC*0.5,  GENY(5),    0,     0}, PushButton,   PUSHID_BAT,        ON, 0, "",     ""},
      { {XC*1,    GENY(6.5),  XC*24, 0}, Text,         TEXTID_BAT,        ON, 0, "",     ""},
      { {XC*0.5,  GENY(7.7),  0,     0}, PushButton,   PUSHID_CEL,        ON, 0, "",     ""},
      { {XC*1,    GENY(9.2),  XC*24, 0}, Text,         TEXTID_CEL,        ON, 0, "",     ""},
      { {XC*1,    GENY(3.8),  0,     0}, Label,        0,                 ON, 0, "処理", ""},
      { {XC*1,    GENY(10.7), 0,     0}, ToggleButton, TOGGLEID_PTLV,     ON, 0, "",     ""},
      { {XC*15,   GENY(10.7), XC*4,  0}, Text,         TEXTID_LV,         ON, 0, "",     ""},
      { {XC*1,    GENY(12),   0,     0}, OptionButton, OPTIONID_OPT,      ON, 0, "",     ""},
      { {XC*16,   GENY(12),   0,     0}, ToggleButton, TOGGLEID_FENCE,    ON, 0, "",     ""},
      { {XC*1,    GENY(13.4), 0,     0}, ToggleButton, TOGGLEID_BUNKATU,  ON, 0, "",     ""},
      { {XC*2.5,  GENY(14.7), 0,     0}, PushButton,   PUSHID_OK,         ON, 0, "",     ""},
      { {XC*16.5, GENY(14.7), 0,     0}, PushButton,   PUSHID_END,        ON, 0, "",     ""},
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
    "MLPUテーブル"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（レイヤーテーブル）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TABLE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    128,
    "%ls",
    "%ls",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "ptn.table"
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
    "ptn.mode",
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
    "%ls",
    "%ls",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "ptn.bat"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（セルライブラリ選択）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_CEL = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_CEL,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "セル"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（セルライブラリ）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CEL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    128,
    "%ls",
    "%ls",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "ptn.cel"
};


/*-----------------------------------------------------------------------
**  トグルアイテム　（パターンレベル）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_PTLV = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_PTLV,
    NOARG, NOMASK, NOINVERT,
    "ﾊﾟﾀｰﾝﾚﾍﾞﾙ",
    "ptn.ptlv"
};



/*-----------------------------------------------------------------------
**  テキストアイテム　（パターンレベル）
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
    "",
    "ptn.lv"
};


/*-----------------------------------------------------------------------
**  オプションボタンアイテム（処理モード）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_OPT = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    NOHOOK,
    NOARG,
    "",
    "ptn.opt",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "標準設定"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "消去、ﾊﾟﾀｰﾝ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "消去、本図"},
            { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "本図、本図"},
        }
};


/*-----------------------------------------------------------------------
**  トグルアイテム　（枠使用）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_FENCE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_PTLV,
    NOARG, NOMASK, NOINVERT,
    "枠使用",
    "ptn.fence"
};



/*-----------------------------------------------------------------------
**  トグルアイテム　（漢字分割）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_BUNKATU = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "漢字分割",
    "ptn.bunkatu"
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