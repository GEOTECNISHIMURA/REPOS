/*-----------------------------------------------------------------------*/
/*-		DOUKAN2.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"doukan2.h"
DllMdlApp DLLAPP_DOUKAN2 =
 
{
    L"DOUKAN2", L"Doukan2"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DOUKAN2 = {
   DIALOGATTR_SINKABLE,
   36 * XC,
   GENY(6.7),
   NOHELP,
   MHELP,
   HOOKID_DOUKAN2_DIALOG,
   NOPARENTID,
   "����2",
   {
      { { XC*1,  GENY(1.0), 0,     0}, ToggleButton, TOGGLEID_IDX,   ON, 0, "", ""},
      { { XC*4,  GENY(1.0), XC*26, 0}, Text,         TEXTID_IDX,     ON, 0, "", ""},
      { { XC*30, GENY(0.6), 0,     0}, PushButton,   PUSHID_IDX,     ON, 0, "", ""},
      { { XC*4,  GENY(2.4), XC*26, 0}, Text,         TEXTID_TBL,     ON, 0, "", ""},
      { { XC*30, GENY(2.2), 0,     0}, PushButton,   PUSHID_TBL,     ON, 0, "", ""},
      { { XC*1,  GENY(3.7), 0,     0}, PushButton,   PUSHID_65,      ON, 0, "", ""},
      { { XC*13, GENY(3.7), 0,     0}, PushButton,   PUSHID_TEST,    ON, 0, "", ""},
      { { XC*25, GENY(3.7), 0,     0}, PushButton,   PUSHID_BUNKATU, ON, 0, "", ""},
      { { XC*1,  GENY(5.2), XC*10, 0}, PushButton,   PUSHID_GG,      ON, 0, "", ""},
      { { XC*12, GENY(5.2), XC*15, 0}, PushButton,   PUSHID_KANRI ,  ON, 0, "", ""},
      { { XC*28, GENY(5.2), 0,     0}, PushButton,   PUSHID_END,     ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���iIDX�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, 
    NOARG, NOMASK, NOINVERT,
    "",
    "doukan2.tog_idx"
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


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���iIDX�j
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
	"doukan2.idxName"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���iTBL�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TBL = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TBL,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"TBL"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���iTBL�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TBL = {
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
	"doukan2.tblName"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i65�_�ȏ�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_65 = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_65,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"65�_�ȏ�"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i64�_�ȓ��ŕ����\���e�X�g�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_TEST = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_TEST,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�����P��"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_BUNKATU = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_BUNKATU,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"����all"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���iGG�����j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_GG = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_GG,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"GG����"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�Ǘ��ԍ������j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_KANRI = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_KANRI,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�Ǘ��ԍ�����"
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
