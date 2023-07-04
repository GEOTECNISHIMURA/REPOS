/*-----------------------------------------------------------------------*/
/*-		line3to4.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"line3to4.h"
DllMdlApp DLLAPP_LINE3TO4 =
{
    L"LINE3TO4", L"Line3to4"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_MAIN = {
   DIALOGATTR_SINKABLE,
   33 * XC,
   GENY(5.6),
   NOHELP,
   MHELP,
   HOOKID_MAIN_DIALOG,
   NOPARENTID,
   "line3to4",
   {
      { { XC*1,  GENY(1.1), 0,     0}, ToggleButton, TOGGLEID_IDX, ON, 0, "", ""},
      { { XC*4,  GENY(0.8), 0,     0}, PushButton,   PUSHID_IDX,   ON, 0, "", ""},
      { { XC*11, GENY(1.1), XC*22, 0}, Text,         TEXTID_IDX,   ON, 0, "", ""},
      { { XC*1,  GENY(2.3), 0,     0}, PushButton,   PUSHID_TABLE, ON, 0, "", ""},
      { { XC*11, GENY(2.6), XC*22, 0}, Text,         TEXTID_TABLE, ON, 0, "", ""},
      { { XC*1,  GENY(4.1), 0,     0}, PushButton,   PUSHID_OK,    ON, 0, "", ""},
      { { XC*25, GENY(4.1), 0,     0}, PushButton,   PUSHID_END,   ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���A�C�e���@�iIDX�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc   TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, 
    NOARG, NOMASK, NOINVERT,
    "",
    "line3to4.tog_idx"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���iIDX�j
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
	"IDX"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�iIDX�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_IDX =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    255,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    TEXT_NOCONCAT,
    "",
    "line3to4.idxName"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���ið��فj
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TABLE = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TABLE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ð���"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�ið��فj
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_TABLE =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    255,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    TEXT_NOCONCAT,
    "",
    "line3to4.tblName"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j
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
	"���s"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j
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
	"�I��"
};
