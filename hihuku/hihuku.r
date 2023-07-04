/*-----------------------------------------------------------------------*/
/*-	被覆発生プログラム	                    -*/
/*-											-*/
/*-	        HIHUKU.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"hihuku.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_HIHUKU =
{
    TD_HIHUKU, L"hihuku"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	ダイアログボックス
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_HIHUKU = {
   DIALOGATTR_SINKABLE,
   22 * XC,
   GENY(12.2),
   NOHELP,
   MHELP,
   HOOKID_HIHUKU_DIALOG,
   NOPARENTID,
   "被覆発生", 
   {
      { { XC*10, GENY(0.8), 0,    0}, OptionButton, OPTIONID_SCALE,     ON, 0, "", ""},
      { { XC*10, GENY(1.9), XC*8, 0}, Text,         TEXTID_HIHUKU_CELL, ON, 0, "", ""},
      { { XC*6 , GENY(2.9), 0,    0}, ToggleButton, TOGGLEID_SINGLE,    ON, 0, "", ""},
      { { XC*6,  GENY(4.2), 0,    0}, Label,        LABELID_HIHUKU,     ON, ALIGN_LEFT, "上線     下線     記号", "" },
      { { XC*6,  GENY(5.2), XC*4, 0}, Text,         TEXTID_HIHUKU_TLV,  ON, 0, "", ""},
      { { XC*6,  GENY(6.2), XC*5, 0}, Text,         TEXTID_HIHUKU_TCO,  ON, 0, "", ""},
      { { XC*6,  GENY(7.2), XC*3, 0}, Text,         TEXTID_HIHUKU_TLC,  ON, 0, "", ""},
      { { XC*6,  GENY(8.2), XC*4, 0}, Text,         TEXTID_HIHUKU_TWT,  ON, 0, "", ""},
      { { XC*11, GENY(5.2), XC*4, 0}, Text,         TEXTID_HIHUKU_BLV,  ON, 0, "", ""},
      { { XC*11, GENY(6.2), XC*5, 0}, Text,         TEXTID_HIHUKU_BCO,  ON, 0, "", ""},
      { { XC*11, GENY(7.2), XC*3, 0}, Text,         TEXTID_HIHUKU_BLC,  ON, 0, "", ""},
      { { XC*11, GENY(8.2), XC*4, 0}, Text,         TEXTID_HIHUKU_BWT,  ON, 0, "", ""},
      { { XC*16, GENY(5.2), XC*4, 0}, Text,         TEXTID_HIHUKU_KLV,  ON, 0, "", ""},
      { { XC*16, GENY(6.2), XC*5, 0}, Text,         TEXTID_HIHUKU_KCO,  ON, 0, "", ""},
      { { XC*16, GENY(7.2), XC*3, 0}, Text,         TEXTID_HIHUKU_KLC,  ON, 0, "", ""},
      { { XC*16, GENY(8.2), XC*4, 0}, Text,         TEXTID_HIHUKU_KWT,  ON, 0, "", ""},
      { { XC*5,  GENY(9.2), XC*5, 0}, PushButton,   PUSHID_TLAYER,      ON, 0, "", ""},
      { { XC*10, GENY(9.2), XC*5, 0}, PushButton,   PUSHID_BLAYER,      ON, 0, "", ""},
      { { XC*15, GENY(9.2), XC*5, 0}, PushButton,   PUSHID_KLAYER,      ON, 0, "", ""},
      { { XC*0,  GENY(10.7),0,    0}, PushButton,   PUSHID_OK,          ON, 0, "", ""},
      { { XC*15, GENY(10.7),0,    0}, PushButton,   PUSHID_END,         ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	オプションボタンアイテム
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_SCALE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"縮尺",
	"hihuku.scale",
	{
	   { NOTYPE, NOICON, NOCMD, LCMD,   500, NOMASK, ON, "1/500"},
       { NOTYPE, NOICON, NOCMD, LCMD,  1000, NOMASK, ON, "1/1000"},
       { NOTYPE, NOICON, NOCMD, LCMD,  2500, NOMASK, ON, "1/2500"},
       { NOTYPE, NOICON, NOCMD, LCMD, 10000, NOMASK, ON, "1/10000"},
	}
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（セル名）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_CELL = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    6,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"セル名",
	"hihuku.cell"
};


/*------------------------------------------------------------------------
**	トグルボタンアイテム（１列のみ）
------------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_SINGLE = {
   NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
   NOHOOK, 
   NOARG, NOMASK, NOINVERT,
   "１列のみ",
   "hihuku.tog"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（上線画層）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_TLV = {
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
	"hihuku.tlv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（上線色）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_TCO = {
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
	"hihuku.tco"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（上線線種）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_TLC = {
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
	"hihuku.tlc"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（上線線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_TWT = {
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
	"hihuku.twt"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（下線画層）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_BLV = {
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
	"hihuku.blv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（下線色）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_BCO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
	"%d",
	"%d",
	"0",
	"255",
	NOMASK,
	NOCONCAT,
	"",
	"hihuku.bco"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（下線線種）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_BLC = {
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
	"hihuku.blc"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（下線線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_BWT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"",
	"hihuku.bwt"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（記号画層）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_KLV = {
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
	"hihuku.klv"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（記号色）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_KCO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
	"%d",
	"%d",
	"0",
	"255",
	NOMASK,
	NOCONCAT,
	"",
	"hihuku.kco"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（記号線種）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_KLC = {
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
	"hihuku.klc"
};


/*-----------------------------------------------------------------------
**	テキストアイテム　（記号線幅）
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_HIHUKU_KWT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"",
	"hihuku.kwt"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（上線レイヤー）
------------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TLAYER = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TLAYER,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ﾚｲﾔ"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（下線レイヤー）
------------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_BLAYER = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_BLAYER,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ﾚｲﾔ"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（記号レイヤー）
------------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_KLAYER = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_KLAYER,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ﾚｲﾔ"
};


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）
------------------------------------------------------------------------*/
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
	DEFAULT_BUTTON,
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