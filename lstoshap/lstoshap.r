/*-----------------------------------------------------------------------*/
/*-	        LSTOSHAP.R													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"lstoshap.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LSTOSHAP =
{
    L"LSTOSHAP", L"lstoshap"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	ダイアログボックス
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_LSTOSHAP = {
   DIALOGATTR_SINKABLE,
   26 * XC,
   GENY(11.9),
   NOHELP,
   MHELP,
   HOOKID_LSTOSHAP_DIALOG,
   NOPARENTID,
   "ﾗｲﾝ←→ｼｪｲﾌﾟ", 
   {
      { {XC*1,  GENY(0.9), 0,     0},ToggleButton, TOGGLEID_LSTOSHAP_BAT, ON, 0, "", ""},
      { {XC*4,  GENY(0.6), 0,     0},PushButton,   PUSHID_LSTOSHAP_BAT,   ON, 0, "", ""},
      { {XC*1,  GENY(2.1), XC*25, 0},Text,         TEXTID_LSTOSHAP_BAT,   ON, 0, "", ""},
      { {XC*0.5,GENY(3.3), 0,     0},PushButton,   PUSHID_LSTOSHAP_TABLE, ON, 0, "", ""},
      { {XC*1,  GENY(4.8), XC*25, 0},Text,         TEXTID_LSTOSHAP_TABLE, ON, 0, "", ""},
      { {XC*6,  GENY(6.25),0,     0},OptionButton, OPTIONID_LSTOSHAP_MODE,ON, 0, "", ""},
      { {XC*6,  GENY(7.5), 0,     0},OptionButton, OPTIONID_LSTOSHAP_TYPE,ON, 0, "", ""},
      { {XC*1,  GENY(8.9), 0,     0},RadioButton,  RADIOBUTTONID_RIGHT,   ON, 0, "", ""},
      { {XC*14, GENY(8.9), 0,     0},RadioButton,  RADIOBUTTONID_LEFT,    ON, 0, "", ""},
      { {XC*1,  GENY(10),  0,     0},PushButton,   PUSHID_LSTOSHAP_OK,    ON, 0, "", ""},
      { {XC*18, GENY(10),  0,     0},PushButton,   PUSHID_LSTOSHAP_END,   ON, 0, "", ""},
   }
};


/*------------------------------------------------------------------------
**	トグルボタンアイテム（バッチ）
------------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_LSTOSHAP_BAT = {
   NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
   HOOKID_TOGGLE_LSTOSHAP_BAT, 
   NOARG, NOMASK, NOINVERT,
   "",
   "glo.tog"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_BAT = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_BAT,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"バッチ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（バッチ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LSTOSHAP_BAT = {
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
	"glo.bat"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブル）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_TABLE = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_TABLE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ﾚｲﾔｰﾃｰﾌﾞﾙ"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（テーブル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LSTOSHAP_TABLE = {
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
	"glo.table"
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（モード）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LSTOSHAP_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LSTOSHAP_MODE,
	NOARG,
	"処理",
	"glo.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "シングル"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "テーブル"},
        }
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（タイプ）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LSTOSHAP_TYPE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LSTOSHAP_TYPE,
	NOARG,
	"ﾀｲﾌﾟ",
	"glo.option",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "ﾗｲﾝｽﾄﾘﾝｸﾞ→ｼｪｲﾌﾟ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "ｼｪｲﾌﾟ→ﾗｲﾝｽﾄﾘﾝｸﾞ"},
            { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "ｼｪｲﾌﾟ→ｼｪｲﾌﾟ"},
        }
};


/*-----------------------------------------------------------------------
**	ラジオボタンアイテム（リスト）
-----------------------------------------------------------------------*/
DItem_RadioButtonListRsc RADIOBUTTONLISTID_DIR = {
    {
    	RADIOBUTTONID_RIGHT,
    	RADIOBUTTONID_LEFT,
    }
};


/*-----------------------------------------------------------------------
**	ラジオボタンアイテム（右回り）
-----------------------------------------------------------------------*/
DItem_RadioButtonRsc RADIOBUTTONID_RIGHT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
    NOHOOK, NOARG,
    1,
    0xFFFFFFFF,
    RADIOBUTTONLISTID_DIR,
    "右回り",
    "glo.dir"
};


/*-----------------------------------------------------------------------
**	ラジオボタンアイテム（左回り）
-----------------------------------------------------------------------*/
DItem_RadioButtonRsc RADIOBUTTONID_LEFT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    2,
    0xFFFFFFFF,
    RADIOBUTTONLISTID_DIR,
    "左回り",
    "glo.dir"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_OK = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"実行"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_END = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"終了"
};
