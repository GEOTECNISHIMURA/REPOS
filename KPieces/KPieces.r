/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:d:\mdl\KPieces\KPieces.r
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|                                                                       |
|                                                                       |
|									|
|									|
|									|
|									|
|									|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include "KPiecestxt.h"
#include "KPieces.h"

DllMdlApp DLLAPP_KPIECES =
{
    L"KPIECES", L"KPieces"          // taskid, dllName
}

/*----------------------------------------------------------------------+
|                                                                       |
|   Main Dialog Dialog Box                                        |
|                                                                       |
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_MainDialog =
    {
    DIALOGATTR_AUTOUNLOADAPP,
    49.17*XC, 23.33*YC,
    NOHELP, MHELP, NOHOOK, NOPARENTID,
    TXT_DBOX_MAINDIALOG,
    {
{{ 34.00*XC, 1.00*YC,12.00*XC,       0}, PushButton, PUSHBTNID_Workspace, ON, 0, "", ""},
{{ 3.00*XC, 20.00*YC,12.00*XC,       2.08*YC}, PushButton, PUSHBTNID_s, ON, 0, "", ""},
{{21.50*XC, 20.00*YC,12.00*XC,       2.08*YC}, PushButton, PUSHBTNID_CANCEL, ON, 0, "", ""},
{{ 10.00*XC, 3.00*YC,18.50*XC,      1.50*YC}, OptionButton, OPTIONBUTTONID_SYORI, ON, 0, "", ""},
{{10.00*XC, 1.00*YC,       18.50*XC,        1.50*YC}, OptionButton, OPTIONBUTTONID_JIKO, ON, 0, "", ""},
{{11.00*XC, 7.00*YC,       0,      1.50*YC}, OptionButton, OPTIONBUTTONID_JUST, ON, 0, "", ""},
{{11.33*XC,9.17*YC,       0,       1.50*YC}, OptionButton, OPTIONBUTTONID_TJUST, ON, 0, "", ""},
{{33.83*XC, 6.17*YC,       0,       0}, ToggleButton, TOGGLEID_TBLCHK, ON, 0, "", ""},
{{33.50*XC,9.50*YC,       0,       0}, ToggleButton, TOGGLEID_BAT, ON, 0, "", ""},
{{ 6.67*XC,14.67*YC,17.00*XC, 1.50*YC}, OptionButton, OPTIONBUTTONID_viewIndep, ON, 0, "", ""},
{{ 3.00*XC, 5.50*YC,26.50*XC, 6.33*YC}, GroupBox, 0, ON, 0, TXT_GBOX_MOJI, ""},
{{ 3.00*XC,13.00*YC,26.50*XC, 5.92*YC}, GroupBox, 0, ON, 0, TXT_GBOX_VIEWINDEPEN, ""},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   実行 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_s =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Practice, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_s
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   キャンセル PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_CANCEL =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Button_StandardAction, ACTIONBUTTON_CANCEL, NOCMD, LCMD,
    "",
    TXT_PBTN_CANCEL
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   処理 OptionButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_SYORI =
    {
    NOSYNONYM, NOHELP, LHELP,
    HOOKITEMID_Syori, OPTNBTNATTR_NEWSTYLE,
    TXT_OBTN_SYORI,
    "kpiecesGlobals.syori",
    {
    {NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, TXT_OBTN_SYORI1},
    {NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, TXT_OBTN_SYORI2},
    {NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, TXT_OBTN_henko},
    }
    };

/*----------------------------------------------------------------------+
|                                                                       |
|   ｵﾌﾟｼｮﾝ(~O): OptionButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_JIKO =
    {
    NOSYNONYM, NOHELP, LHELP,
    HOOKITEMID_Jiko, OPTNBTNATTR_NEWSTYLE,
    TXT_OBTN_JIKO,
    "kpiecesGlobals.jiko",
    {
    {NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, TXT_OBTN_JIKO1},
    {NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, TXT_OBTN_JIKO2},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   文字位置 OptionButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_JUST =
    {
    NOSYNONYM, NOHELP, LHELP,
    NOHOOK, OPTNBTNATTR_NEWSTYLE,
    TXT_OBTN_JUST,
    "kpiecesGlobals.just",
    {
    {NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, TXT_OBTN_Just0},
    {NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, TXT_OBTN_Just1},
    {NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, TXT_OBTN_Just2},
    {NOTYPE, NOICON, NOCMD, LCMD, 6, NOMASK, ON, TXT_OBTN_Just6},
    {NOTYPE, NOICON, NOCMD, LCMD, 7, NOMASK, ON, TXT_OBTN_Just7},
    {NOTYPE, NOICON, NOCMD, LCMD, 8, NOMASK, ON, TXT_OBTN_Just8},
    {NOTYPE, NOICON, NOCMD, LCMD, 12, NOMASK, ON, TXT_OBTN_Just12},
    {NOTYPE, NOICON, NOCMD, LCMD, 13, NOMASK, ON, TXT_OBTN_Just13},
    {NOTYPE, NOICON, NOCMD, LCMD, 14, NOMASK, ON, TXT_OBTN_Just14},
    {NOTYPE, NOICON, NOCMD, LCMD, 4294967295, NOMASK, ON, TXT_OBTN_somama},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   縦書　文字位置 OptionButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_TJUST =
    {
    NOSYNONYM, NOHELP, LHELP,
    NOHOOK, OPTNBTNATTR_NEWSTYLE,
    TXT_OBTN_TJust,
    "kpiecesGlobals.tjust",
    {
    {NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, TXT_OBTN_Just0},
    {NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, TXT_OBTN_Just1},
    {NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, TXT_OBTN_Just2},
    {NOTYPE, NOICON, NOCMD, LCMD, 6, NOMASK, ON, TXT_OBTN_Just6},
    {NOTYPE, NOICON, NOCMD, LCMD, 7, NOMASK, ON, TXT_OBTN_Just7},
    {NOTYPE, NOICON, NOCMD, LCMD, 8, NOMASK, ON, TXT_OBTN_Just8},
    {NOTYPE, NOICON, NOCMD, LCMD, 12, NOMASK, ON, TXT_OBTN_Just12},
    {NOTYPE, NOICON, NOCMD, LCMD, 13, NOMASK, ON, TXT_OBTN_Just13},
    {NOTYPE, NOICON, NOCMD, LCMD, 14, NOMASK, ON, TXT_OBTN_Just14},
    {NOTYPE, NOICON, NOCMD, LCMD, 4294967295, NOMASK, ON, TXT_OBTN_sonomama},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   作業環境 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_Workspace =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Workspace, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_Workspace
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   作業環境 Dialog Box                                        |
|                                                                       |
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_Workspace =
    {
    DIALOGATTR_GROWABLE,
    56.83*XC, 36.50*YC,
    NOHELP, MHELP, NOHOOK, NOPARENTID,
    TXT_DBOX_Workspace,
    {
{{ 4.67*XC, 1.50*YC,       0,       0}, ToggleButton, TOGGLEID_Backup, ON, 0, "", ""},
{{ 4.67*XC, 3.50*YC,       0,       0}, ToggleButton, TOGGLEID_CompressDesign, ON, 0, "", ""},
{{ 3.33*XC, 6.58*YC,51.00*XC,9.75*YC}, GroupBox, 0, ON, 0, TXT_GBOX_SyoriT, ""},
{{ 3.33*XC,18.42*YC,51.00*XC, 6.17*YC}, GroupBox, 0, ON, 0, TXT_GBOX_AS, ""},
{{ 8.00*XC, 8.33*YC,       0,       0}, ToggleButton, TOGGLEID_LevelsChk, OFF, 0, "", ""},
{{11.67*XC, 8.08*YC,18.33*XC,       1.83*YC}, PushButton, PUSHBTNID_Levels, OFF, 0, "", ""},
{{ 8.00*XC, 10.83*YC,       0,       0}, ToggleButton, TOGGLEID_Ftchk, ON, 0, "", ""},
{{17.67*XC, 10.67*YC, 6.67*XC,       0}, Text, TEXTID_FT, ON, 0, "", ""},
{{15.33*XC, 13.67*YC,33.17*XC,       0}, Text, TEXTID_TBL, ON, 0, "", ""},
{{35.50*XC, 11.17*YC,12.17*XC,       1.75*YC}, PushButton, PUSHBTNID_TBL, ON, 0, "", ""},
{{14.83*XC,21.25*YC,33.17*XC,       0}, Text, TEXTID_IDX, ON, 0, "", ""},
{{34.00*XC,19.42*YC,12.67*XC, 1.75*YC}, PushButton, PUSHBTNID_FILE, ON, 0, "", ""},
{{34.33*XC, 1.50*YC,       0,       0}, ToggleButton, TOGGLEID_LOG, ON, 0, "", ""},
{{35.50*XC,28.92*YC,12.00*XC,       0}, PushButton, PUSHBTNID_LogB, ON, 0, "", ""},
{{14.83*XC,31.00*YC,33.17*XC,       0}, Text, TEXTID_Logname, ON, 0, "", ""},
{{ 3.67*XC,27.67*YC,50.00*XC, 7.08*YC}, GroupBox, 0, ON, 0, TXT_GBOX_LogG, ""},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   書体 Text Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_FT =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK,
    NOARG, 20, "%d", "%d",
    "", "", NOMASK,
    TEXT_NOCONCAT,
    TXT_DTXT_FT,
    "kpiecesGlobals.ft"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|    ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_Ftchk =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    "",
    "kpiecesGlobals.ftchk"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   Levels PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_Levels =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Levels, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_Levels
    };
/*----------------------------------------------------------------------+
|                                                                       |
|    ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_LevelsChk =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    "",
    "kpiecesGlobals.levelschk"
    };

/*----------------------------------------------------------------------+
|                                                                       |
|   参照 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_TBL =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_FileTbl, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_TBL
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   テーブル名 Text Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_TBL =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK,
    NOARG, 255, "%s", "%s",
    "", "", NOMASK,
    TEXT_NOCONCAT,
    TXT_DTXT_TBL,
    "kpiecesGlobals.tblname"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   ＴＢＬ使用 ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_TBLCHK =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    TXT_TBTN_Tblchk,
    "kpiecesGlobals.tbli"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   ＩＤＸ Text Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_IDX =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK,
    NOARG, 255, "%s", "%s",
    "", "", NOMASK,
    TEXT_NOCONCAT,
    TXT_DTXT_IDX,
    "kpiecesGlobals.idxname"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   参 照 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_FILE =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_FileIdx, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_FILE
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   ＢＡＴ処理 ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_BAT =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, HOOKITEMID_CkBat, NOARG,
    NOMASK, NOINVERT,
    TXT_TBTN_BAT,
    "kpiecesGlobals.idxggle"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   Levels Dialog Box                                        |
|                                                                       |
+----------------------------------------------------------------------*/
/*DialogBoxRsc DIALOGID_Levels =
    {
    DIALOGATTR_DEFAULT,
    30.17*XC, 19.92*YC,
    NOHELP, MHELP, NOHOOK, NOPARENTID,
    TXT_DBOX_Levels,
    {
{{ 2.67*XC, 1.50*YC,       0,       0}, LevelMap, LEVELMAPID_Levels, ON, 0, "", ""},
{{ 9.00*XC,16.00*YC,12.00*XC,       0}, PushButton, PUSHBTNID_LevelsOk, ON, 0, "", ""},
    }
    };*/
/*----------------------------------------------------------------------+
|                                                                       |
|    LevelMap Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
/*DItem_LevelMapRsc LEVELMAPID_Levels =
    {
    NOHELP, LHELP,
    "",
    "kpiecesGlobals.levels",
    ""
    };*/
/*----------------------------------------------------------------------+
|                                                                       |
|   デザインを圧縮 ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CompressDesign =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    TXT_TBTN_CompressDesign,
    "kpiecesGlobals.CompressDesign"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   バックアップを作成 ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_Backup =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    TXT_TBTN_Backup,
    "kpiecesGlobals.backup"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   OK PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_LevelsOk =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_LevelsOk, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_LevelsOk
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   ログ名 Text Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_Logname =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK,
    NOARG, 255, "%s", "%s",
    "", "", NOMASK,
    TEXT_NOCONCAT,
    TXT_DTXT_LogName,
    "kpiecesGlobals.logname"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   参照 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_LogB =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Logbottn, NOARG, NOCMD, LCMD,
    "",
    TXT_PBTN_LogB
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   ログファイル作成 ToggleButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_LOG =
    {
    NOCMD, LCMD, NOSYNONYM,
    NOHELP, LHELP, NOHOOK, NOARG,
    NOMASK, NOINVERT,
    TXT_TBTN_Log,
    "kpiecesGlobals.log"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|    OptionButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_viewIndep =
    {
    NOSYNONYM, NOHELP, LHELP,
    NOHOOK, OPTNBTNATTR_NEWSTYLE,
    "",
    "kpiecesGlobals.viewIndep",
    {
    {NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, TXT_OBTN_viewIndep1},
    {NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, TXT_OBTN_viewIndep2},
    }
    };
