/*-----------------------------------------------------------------------*/
/*-	        HOUKOU.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"houkou.h"	
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_HOUKOU =
{
    TD_HOUKOU, L"houkou"          // taskid, dllName
}

/*----------------------------------------------------------------------+
|    Local defines							|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_HOUKOU =
{
    DIALOGATTR_SINKABLE,
    21 * XC,
    GENY(11.5),
    NOHELP,
    MHELP,
    HOOKID_HOUKOU_DIALOG,
    NOPARENTID,
    "方向線",
    {
       { { XC*6,    GENY(1),   XC*15.5,0}, OptionButton, OPTIONID_CUT,   ON, 0, "", ""},
       { { XC*6,    GENY(2.5), 0,      0}, OptionButton, OPTIONID_LAYER, ON, 0, "", ""},
       { { XC*6,    GENY(4),   XC*7,   0}, Text,         TEXTID_SCALE,   ON, 0, "", ""},
       { { XC*6,    GENY(6),   XC*5,   0}, Text,         TEXTID_LV,      ON, 0, "", ""},
       { { XC*6,    GENY(7),   XC*5,   0}, Text,         TEXTID_CO,      ON, 0, "", ""},
       { { XC*6,    GENY(8),   XC*5,   0}, Text,         TEXTID_LC,      ON, 0, "", ""},
       { { XC*6,    GENY(9),   XC*5,   0}, Text,         TEXTID_WT,      ON, 0, "", ""},
       { { XC*1,    GENY(10),  0,      0}, PushButton,   PUSHID_OK,      ON, 0, "", ""},
       { { XC*13,   GENY(10),  0,      0}, PushButton,   PUSHID_END,     ON, 0, "", ""},
       { { XC*6,    GENY(5),   XC*8,   0}, Text,         TEXTID_CELL,    ON, 0, "", ""},
       { { XC*14.5, GENY(5),   0,      0}, OptionButton, OPTIONID_CELL,  ON, 0, "", ""},
    }
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（方向線を 結ぶ/結ばない）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CUT = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_CUT,
	NOARG,
	"種類",
	"houset.cut",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "結ぶ"         },
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "結ばない(ﾗｲﾝ)"},
        { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "結ばない(ｾﾙ)" },
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "矢印"         },
        { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "支柱"         },
    }
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（セルと同じレイヤーかどうか）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LAYER = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LAYER,
	NOARG,
	"ﾚｲﾔｰ",
	"houset.layer",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "ｱｸﾃｨﾌﾞ"      },
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "電柱ｾﾙと同じ"},
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "設定する"    },
    }
};


/*-----------------------------------------------------------------------
**	テキストアイテム（縮尺）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_SCALE = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	5,
	"%ld",
	"%ld",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"縮尺",
	"houset.scale"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（レベル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LV = {
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
	"houset.lv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（カラー）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"0",
	"255",
	NOMASK,
	NOCONCAT,
	"色",
	"houset.co"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（スタイル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LC = {
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
	"houset.lc"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（ウエイト）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_WT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"線幅",
	"houset.wt"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_OK = {
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


/*-----------------------------------------------------------------------
**	テキストアイテム（セル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CELL = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"ｾﾙ名",
	"houset.cell"
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（図形／点）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CELL = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"",
	"houset.zukei",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "図形"},
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "点"  },
    }
};
