/*-----------------------------------------------------------------------*/
/*-     FFREF.R                                                         -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "ffref.h"

DllMdlApp DLLAPP_FFREF =

{
    L"FFREF", L"Ffref"          // taskid, dllName
}
/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_FFREF = {
    DIALOGATTR_SINKABLE,
    22 * XC,
    GENY(11.3),
    NOHELP,
    MHELP,
    HOOKID_FFREF_DIALOG,
    NOPARENTID,
    "参照をFF",
    {
        { { XC*7,   GENY(1.0),  0,     0}, OptionButton,OPTIONID_MODE, ON, 0, "",     ""},
        { { XC*7.5, GENY(2.3),  XC*14, 0}, Label,       0,             ON, 0, "図郭", ""},
        { { XC*16.5,GENY(2.3),  XC*14, 0}, Label,       0,             ON, 0, "図番", ""},
        { { XC*7,   GENY(3.3),  XC*5,  0}, Text,        TEXTID_LV,     ON, 0, "",     ""},
        { { XC*7,   GENY(4.3),  XC*5,  0}, Text,        TEXTID_CO,     ON, 0, "",     ""},
        { { XC*7,   GENY(5.3),  XC*5,  0}, Text,        TEXTID_LC,     ON, 0, "",     ""},
        { { XC*7,   GENY(6.3),  XC*5,  0}, Text,        TEXTID_WT,     ON, 0, "",     ""},
        { { XC*16,  GENY(3.3),  XC*5,  0}, Text,        TEXTID_NOLV,   ON, 0, "",     ""},
        { { XC*16,  GENY(4.3),  XC*5,  0}, Text,        TEXTID_NOCO,   ON, 0, "",     ""},
        { { XC*16,  GENY(5.3),  XC*5,  0}, Text,        TEXTID_NOLC,   ON, 0, "",     ""},
        { { XC*16,  GENY(6.3),  XC*5,  0}, Text,        TEXTID_NOWT,   ON, 0, "",     ""},
        { { XC*5,   GENY(7.3),  0,     0}, PushButton,  PUSHID_LAYER1, ON, 0, "",     ""},
        { { XC*14,  GENY(7.3),  0,     0}, PushButton,  PUSHID_LAYER2, ON, 0, "",     ""},
        { { XC*2,   GENY(8.8),  0,     0}, ToggleButton,TOGGLEID_RDL,  ON, 0, "",     ""},
        { { XC*1,   GENY(9.8),  0,     0}, PushButton,  PUSHID_OK,     ON, 0, "",     ""},
        { { XC*14,  GENY(9.8),  0,     0}, PushButton,  PUSHID_END,    ON, 0, "",     ""},
    }
};


/*-----------------------------------------------------------------------
**  オプションボタンアイテム（モード）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_MODE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    NOHOOK, NOARG,
    " 処理",
    "ffref.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "全要素"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "セルのみ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "ポリゴン"},
        }
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図郭画層）
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
    "ffref.lv"
};

/*-----------------------------------------------------------------------
**  テキストアイテム　（図郭色）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO = {
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
    "ffref.co"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図郭線種）
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
    "ffref.lc"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図郭線幅）
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
    "ffref.wt"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（図郭レイヤー）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_LAYER1 = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_LAYER1,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "ﾚｲﾔｰ"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図番画層）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOLV = {
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
    "ffref.nolv"
};

/*-----------------------------------------------------------------------
**  テキストアイテム　（図番色）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOCO = {
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
    "ffref.noco"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図番線種）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOLC = {
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
    "ffref.nolc"
};


/*-----------------------------------------------------------------------
**  テキストアイテム　（図番線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOWT = {
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
    "ffref.nowt"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（図番レイヤー）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_LAYER2 = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_LAYER2,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "ﾚｲﾔｰ"
};


/*-----------------------------------------------------------------------
**  トグルアイテム　（エラーフラグ）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_RDL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "拡張子は<.rdl>",
    "ffref.rdl"
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