/*-----------------------------------------------------------------------*/
/*-		attach8.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"attach.h"

DllMdlApp DLLAPP_ATTACH =
 
{
    L"ATTACH", L"Attach"          // taskid, dllName
}

/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_ATTACH8 = {
   DIALOGATTR_SINKABLE,
   28 * XC,
   GENY(33.5),
   NOHELP,
   MHELP,
   HOOKID_ATTACH8_DIALOG,
   NOPARENTID,
   "参照ﾌｧｲﾙをｱﾀｯﾁ Ver.2",
   {
      { { XC*6, GENY(0.8),  0,     0}, OptionButton, OPTIONID_MODE,        ON, 0, "", ""},
      { { XC*15,GENY(0.8),  0,     0}, ToggleButton, TOGGLEID_DETACH,      ON, 0, "", ""},
      { { XC*1, GENY(2.0),  0,     0}, PushButton,   PUSHID_IDX,           ON, 0, "", ""},
      { { XC*1, GENY(3.5),  XC*26, 0}, Text,         TEXTID_IDX,           ON, 0, "", ""},
      { { XC*1, GENY(5.3),  0,     0}, ToggleButton, TOGGLEID_REF1,        ON, 0, "", ""},
      { { XC*4, GENY(5.0),  0,     0}, PushButton,   PUSHID_REF1,          ON, 0, "", ""},
      { { XC*14,GENY(5.3),  0,     0}, ToggleButton, TOGGLEID_REF1_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(6.5),  XC*26, 0}, Text,         TEXTID_REF1,          ON, 0, "", ""},
      { { XC*1, GENY(8.0),  0,     0}, ToggleButton, TOGGLEID_REF2,        ON, 0, "", ""},
      { { XC*4, GENY(7.7),  0,     0}, PushButton,   PUSHID_REF2,          ON, 0, "", ""},
      { { XC*14,GENY(8.0),  0,     0}, ToggleButton, TOGGLEID_REF2_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(9.2),  XC*26, 0}, Text,         TEXTID_REF2,          ON, 0, "", ""},
      { { XC*1, GENY(10.7), 0,     0}, ToggleButton, TOGGLEID_REF3,        ON, 0, "", ""},
      { { XC*4, GENY(10.4), 0,     0}, PushButton,   PUSHID_REF3,          ON, 0, "", ""},
      { { XC*14,GENY(10.7), 0,     0}, ToggleButton, TOGGLEID_REF3_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(11.9), XC*26, 0}, Text,         TEXTID_REF3,          ON, 0, "", ""},
      { { XC*1, GENY(13.4), 0,     0}, ToggleButton, TOGGLEID_REF4,        ON, 0, "", ""},
      { { XC*4, GENY(13.1), 0,     0}, PushButton,   PUSHID_REF4,          ON, 0, "", ""},
      { { XC*14,GENY(13.4), 0,     0}, ToggleButton, TOGGLEID_REF4_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(14.6), XC*26, 0}, Text,         TEXTID_REF4,          ON, 0, "", ""},
      { { XC*1, GENY(16.1), 0,     0}, ToggleButton, TOGGLEID_REF5,        ON, 0, "", ""},
      { { XC*4, GENY(15.8), 0,     0}, PushButton,   PUSHID_REF5,          ON, 0, "", ""},
      { { XC*14,GENY(16.1), 0,     0}, ToggleButton, TOGGLEID_REF5_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(17.3), XC*26, 0}, Text,         TEXTID_REF5,          ON, 0, "", ""},
      { { XC*1, GENY(18.8), 0,     0}, ToggleButton, TOGGLEID_REF6,        ON, 0, "", ""},
      { { XC*4, GENY(18.5), 0,     0}, PushButton,   PUSHID_REF6,          ON, 0, "", ""},
      { { XC*14,GENY(18.8), 0,     0}, ToggleButton, TOGGLEID_REF6_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(20.0), XC*26, 0}, Text,         TEXTID_REF6,          ON, 0, "", ""},
      { { XC*1, GENY(21.5), 0,     0}, ToggleButton, TOGGLEID_REF7,        ON, 0, "", ""},
      { { XC*4, GENY(21.2), 0,     0}, PushButton,   PUSHID_REF7,          ON, 0, "", ""},
      { { XC*14,GENY(21.5), 0,     0}, ToggleButton, TOGGLEID_REF7_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(22.7), XC*26, 0}, Text,         TEXTID_REF7,          ON, 0, "", ""},
      { { XC*1, GENY(24.2), 0,     0}, ToggleButton, TOGGLEID_REF8,        ON, 0, "", ""},
      { { XC*4, GENY(23.9), 0,     0}, PushButton,   PUSHID_REF8,          ON, 0, "", ""},
      { { XC*14,GENY(24.2), 0,     0}, ToggleButton, TOGGLEID_REF8_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(25.4), XC*26, 0}, Text,         TEXTID_REF8,          ON, 0, "", ""},
      { { XC*1, GENY(26.9), 0,     0}, ToggleButton, TOGGLEID_REF9,        ON, 0, "", ""},
      { { XC*4, GENY(26.6), 0,     0}, PushButton,   PUSHID_REF9,          ON, 0, "", ""},
      { { XC*14,GENY(26.9), 0,     0}, ToggleButton, TOGGLEID_REF9_KOTEI,  ON, 0, "", ""},
      { { XC*1, GENY(28.1), XC*26, 0}, Text,         TEXTID_REF9,          ON, 0, "", ""},
      { { XC*1, GENY(29.6), 0,     0}, ToggleButton, TOGGLEID_REF10,       ON, 0, "", ""},
      { { XC*4, GENY(29.3), 0,     0}, PushButton,   PUSHID_REF10,         ON, 0, "", ""},
      { { XC*14,GENY(29.6), 0,     0}, ToggleButton, TOGGLEID_REF10_KOTEI, ON, 0, "", ""},
      { { XC*1, GENY(30.8), XC*26, 0}, Text,         TEXTID_REF10,         ON, 0, "", ""},
      { { XC*1, GENY(32.0), 0,     0}, PushButton,   PUSHID_OK,            ON, 0, "", ""},
      { { XC*21,GENY(32.0), 0,     0}, PushButton,   PUSHID_END,           ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（処理）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_MODE,
	NOARG,
	"処理",
	"attach8.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "1ﾌｧｲﾙ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "バッチ"},
        }
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照デタッチ）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_DETACH = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "参照ﾃﾞﾀｯﾁ",
    "attach8.tog_detach"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）
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
	"バッチ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（バッチ）
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
	"attach8.idxName"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照01）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF1 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF1, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[0]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照01）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF1_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[0]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照01）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF1 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF1,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照01"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照01）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF1 = {
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
	"attach8.refName[0]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照02）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF2 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF2, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[1]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照02）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF2_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[1]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照02）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF2 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF2,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照02"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照02）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF2 = {
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
	"attach8.refName[1]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照03）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF3 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF3, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[2]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照03）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF3_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[2]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照03）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF3 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF3,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照03"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照03）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF3 = {
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
	"attach8.refName[2]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照04）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF4 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF4, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[3]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照04）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF4_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[3]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照04）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF4 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF4,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照04"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照04）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF4 = {
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
	"attach8.refName[3]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照05）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF5 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF5, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[4]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照05）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF5_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[4]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照05）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF5 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF5,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照05"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照05）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF5 = {
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
	"attach8.refName[4]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照06）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF6 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF6, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[5]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照06）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF6_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[5]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照06）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF6 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF6,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照06"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照06）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF6 = {
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
	"attach8.refName[5]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照07）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF7 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF7, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[6]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照07）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF7_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[6]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照07）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF7 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF7,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照07"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照07）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF7 = {
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
	"attach8.refName[6]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照08）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF8 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF8, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[7]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照08）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF8_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[7]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照08）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF8 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF8,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照08"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照08）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF8 = {
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
	"attach8.refName[7]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照09）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF9 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF9, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[8]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照09）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF9_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[8]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照09）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF9 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF9,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照09"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照09）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF9 = {
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
	"attach8.refName[8]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照10）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF10 = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_REF10, 
    NOARG, NOMASK, NOINVERT,
    "",
    "attach8.tog[9]"
};


/*-----------------------------------------------------------------------
**	トグルボタン（参照10）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_REF10_KOTEI = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, 
    NOARG, NOMASK, NOINVERT,
    "dgn名",
    "attach8.tog_kotei[9]"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（参照10）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_REF10 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_REF10,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"参照10"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（参照10）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_REF10 = {
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
	"attach8.refName[9]"
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
