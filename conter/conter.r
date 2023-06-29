/*----------------------------------------------------------------------+
|   $Workfile:   conter.r
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"conter.h"
DllMdlApp DLLAPP_CONTER =
{
    L"CONTER", L"conter"          // taskid, dllName
}

/*----------------------------------------------------------------------+
|    ダイアログボックス
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_CONTER =
{
    DIALOGATTR_DEFAULT | DIALOGATTR_NORIGHTICONS | DIALOGATTR_AUTOOPEN,
    XC * 24,
    GENY(13),
    NOHELP, MHELPTOPIC,
    HOOKID_CONTER_DIALOG, NOPARENTID,
    "ｺﾝﾀｰ",
	{
       { { XC*6,  GENY(0.7),  0,    0     }, OptionButton, OPTIONID_CONTER_MODE, ON, 0, "", ""},
       { { XC*9,  GENY(2.2),  XC*8, 0     }, Text,         TEXTID_CONTER_ZZ,     ON, 0, "", ""},
       { { XC*9,  GENY(3.2),  XC*6, 0     }, Text,         TEXTID_CONTER_PITCH,  ON, 0, "", ""},
       { { XC*17, GENY(2),    XC*4, YC*1.5}, PushButton,   PUSHID_CONTER_INCRE,  ON, 0, "", ""},
       { { XC*17, GENY(3.1),  XC*4, YC*1.5}, PushButton,   PUSHID_CONTER_DECRE,  ON, 0, "", ""},
       { { XC*1,  GENY(4.7),  0,    0     }, ToggleButton, TOGGLEID_CONTER_BSPLN,ON, 0, "", ""},
       { { XC*9,  GENY(5.7),  XC*7, 0     }, Text,         TEXTID_CONTER_GENKOU, ON, 0, "", ""},
       { { XC*13, GENY(7.1),  XC*4, 0     }, Text,         TEXTID_CONTER_KLV,    ON, 0, "", ""},
       { { XC*13, GENY(8.2),  XC*4, 0     }, Text,         TEXTID_CONTER_SLV,    ON, 0, "", ""},
       { { XC*13, GENY(9.3),  XC*4, 0     }, Text,         TEXTID_CONTER_OTLV,   ON, 0, "", ""},
       { { XC*17, GENY(6.9),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_KLV,    ON, 0, "", ""},
       { { XC*17, GENY(8.1),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_SLV,    ON, 0, "", ""},
       { { XC*17, GENY(9.2),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_OTLV,   ON, 0, "", ""},
       { { XC*13, GENY(10.5), XC*5, 0     }, Text,         TEXTID_CONTER_HANI,   ON, 0, "", ""},
       { { XC*0,  GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_OK,     ON, 0, "", ""},
       { { XC*8,  GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_SETZ,   ON, 0, "", ""},
       { { XC*16, GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_END,    ON, 0, "", ""},
	}
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（内容）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CONTER_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_CONTER_MODE,
	NOARG,
	"内容",
	"con.mode",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "新規"},
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "修正"},
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "尾根･谷揃え"},
    }
};


/*-----------------------------------------------------------------------
**	テキストアイテム（高さ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_ZZ = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%lf",
	"%lf",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"高さ[m]",
	"con.zz"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（間隔）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_PITCH = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	4,
	"%lf",
	"%lf",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"間隔[m]",
	"con.pitch"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（＋）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_INCRE = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_INCRE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"＋"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（ー）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_DECRE = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_DECRE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"－"
};


/*-----------------------------------------------------------------------
**	トグルボタン（B-スプライン）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CONTER_BSPLN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_CONTER_BSPLN, NOARG, NOMASK, NOINVERT,
    "B-ｽﾌﾟﾗｲﾝ",
    "con.bspln"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（弦高）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_GENKOU = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	5,
	"%lf",
	"%lf",
	"0.001",
	"",
	NOMASK,
	NOCONCAT,
	"弦高[m]",
	"con.genkou"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（計曲）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_KLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"計曲画層",
	"con.klv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（主曲）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_SLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"主曲画層",
	"con.slv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（尾根・谷画層）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_OTLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"尾根･谷画層",
	"con.otlv"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（計曲設定）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_KLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_KLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"設定"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（主曲設定）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_SLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_SLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"設定"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（尾根・谷設定）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_OTLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_OTLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"設定"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（範囲）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_HANI = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"1",
	"",
	NOMASK,
	NOCONCAT,
	"修正範囲[m]",
	"con.hani"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_OK = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"実行"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（高さ設定）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_SETZ = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_SETZ,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"高さ"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_END = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"終了"
};
