/*-----------------------------------------------------------------------*/
/*-	リンク					        -*/
/*-									-*/
/*-	LINK.R							-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"link.h"

/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LINK =
{
    L"LINK", L"Link"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	ダイアログボックス
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_LINK = {
   DIALOGATTR_SINKABLE,
   27 * XC,
   GENY(16),
   NOHELP,
   MHELP,
   HOOKID_LINK_DIALOG,
   NOPARENTID,
   "リンク",
   {
     { { XC*0.5, GENY(2.2),  0,     0}, PushButton,  PUSHID_LINK_LAYER, ON, 0, "", ""},
     { { XC*1,   GENY(3.7),  XC*26, 0}, Text,        TEXTID_LAYER,      ON, 0, "", ""},
     { { XC*1 ,  GENY(0.8),  XC*26, 0}, OptionButton,OPTIONID_KIND,     ON, 0, "", ""},
     { { XC*0.5, GENY(5),    0,     0}, PushButton,  PUSHID_LINK_BAT,   ON, 0, "", ""},
     { { XC*1,   GENY(6.5),  XC*26, 0}, Text,        TEXTID_BAT,        ON, 0, "", ""},
     { { XC*11,  GENY(8),    XC*16, 0}, Text,        TEXTID_DBSOURCE,   ON, 0, "", ""},
     { { XC*11,  GENY(9),    XC*16, 0}, Text,        TEXTID_TABLE,      ON, 0, "", ""},
     { { XC*11,  GENY(10),   XC*12, 0}, Text,        TEXTID_MSLINK,     ON, 0, "", ""},
     { { XC*1,   GENY(11.7), 0,     0}, ToggleButton,TOGGLEID_ZUMEN,    ON, 0, "", ""},
     { { XC*16,  GENY(11.7), 0,     0}, OptionButton,OPTIONID_KETA,     ON, 0, "", ""},
     { { XC*0,   GENY(13),   0,     0}, PushButton,  PUSHID_LINK_DEL,   ON, 0, "", ""},
     { { XC*8,   GENY(13.3), 0,     0}, ToggleButton,TOGGLEID_DEL_DB,   ON, 0, "", ""},
     { { XC*0,   GENY(14.5), 0,     0}, PushButton,  PUSHID_LINK_OK,    ON, 0, "", ""},
     { { XC*20,  GENY(14.5), 0,     0}, PushButton,  PUSHID_LINK_END,   ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	プッシュボタン（レイヤーテーブル）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LINK_LAYER = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LINK_LAYER,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"レイヤーテーブル"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（レイヤーテーブル）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LAYER = {
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
	"linkset.layer"
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（種類）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_KIND = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_KIND,
	NOARG,
	"",
	"linkset.kind",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "バッチ"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "１ファイル"},
       { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "1要素(最大MSLINK+1追加)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "1要素(設定して追加)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "1要素(既存MSLINK)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 5, NOMASK, ON, "ﾌｧｲﾙ名をMSLINKで与える"},
    }
};


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LINK_BAT = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LINK_BAT,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"バッチファイル"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（バッチ）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_BAT = {
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
	"linkset.bat"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（ﾃﾞｰﾀｿｰｽ名）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_DBSOURCE = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"ﾃﾞｰﾀｿｰｽ名",
	"linkset.dbsource"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（テーブル名）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TABLE = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"ﾃｰﾌﾞﾙ名",
	"linkset.table"
};


/*-----------------------------------------------------------------------
**	テキストアイテム（MSLINK）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_MSLINK = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	10,
	"%ld",
	"%ld",
	"1",
	"",
	NOMASK,
	NOCONCAT,
	"MSLINK",
	"linkset.mslink"
};


/*-----------------------------------------------------------------------
**	トグルボタン（図面番号）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_ZUMEN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_ZUMEN, NOARG, NOMASK, NOINVERT,
    "図面番号",
    "linkset.zumen"
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム（桁数）
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_KETA = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"",
	"linkset.keta",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "３桁"},
       { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "４桁"},
       { NOTYPE, NOICON, NOCMD, LCMD, 5, NOMASK, ON, "５桁"},
       { NOTYPE, NOICON, NOCMD, LCMD, 6, NOMASK, ON, "６桁"},
    }
};


/*-----------------------------------------------------------------------
**	トグルボタン（結合データ削除）
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_DEL_DB = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG, NOMASK, NOINVERT,
    "解除時にﾃﾞｰﾀ削除",
    "linkset.tdel"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（解除）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LINK_DEL = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LINK_DEL,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"解除"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（結合）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LINK_OK = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LINK_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"結合"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LINK_END = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LINK_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"終了"
};
