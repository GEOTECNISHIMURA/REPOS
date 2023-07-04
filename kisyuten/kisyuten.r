/*-----------------------------------------------------------------------*/
/*-		kisyuten.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"kisyuten.h"
DllMdlApp DLLAPP_KISYUTEN =
{
    L"KISYUTEN", L"Kisyuten"          // taskid, dllName
}


/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_MAIN = {
   DIALOGATTR_SINKABLE,
   34 * XC,
   GENY(11.2),
   NOHELP,
   MHELP,
   HOOKID_MAIN_DIALOG,
   NOPARENTID,
   "�N�I�_�L������",
   {
      { { XC*1,  GENY(1.0), 0,     0}, ToggleButton, TOGGLEID_IDX,      ON, 0, "", ""},
      { { XC*4,  GENY(1.0), XC*23, 0}, Text,         TEXTID_IDX,        ON, 0, "", ""},
      { { XC*27, GENY(0.7), XC*7,  0}, PushButton,   PUSHID_IDX,        ON, 0, "", ""},
      { { XC*4,  GENY(2.5), XC*23, 0}, Text,         TEXTID_TABLE,      ON, 0, "", ""},
      { { XC*27, GENY(2.2), XC*7,  0}, PushButton,   PUSHID_TABLE,      ON, 0, "", ""},
      { { XC*4,  GENY(4.0), XC*23, 0}, Text,         TEXTID_CELLIB,     ON, 0, "", ""},
      { { XC*27, GENY(3.7), XC*7,  0}, PushButton,   PUSHID_CELLIB,     ON, 0, "", ""},
      { { XC*8,  GENY(5.4), XC*8,  0}, Text,         TEXTID_KITEN,      ON, 0, "", ""},
      { { XC*24, GENY(5.4), XC*8,  0}, Text,         TEXTID_SYUTEN,     ON, 0, "", ""},
      { { XC*12, GENY(6.7), 0,     0}, Label,        LABELID_CELL,      ON, 0, "��w  �F   ���� ����", ""},
      { { XC*12, GENY(7.7), XC*4,  0}, Text,         TEXTID_LV,         ON, 0, "", ""},
      { { XC*17, GENY(7.7), XC*5,  0}, Text,         TEXTID_CO,         ON, 0, "", ""},
      { { XC*23, GENY(7.7), XC*4,  0}, Text,         TEXTID_WT,         ON, 0, "", ""},
      { { XC*28, GENY(7.7), XC*3,  0}, Text,         TEXTID_ST,         ON, 0, "", ""},
      { { XC*14, GENY(8.7), 0,     0}, OptionButton, OPTIONID_RENZOKU,  ON, 0, "", ""},
      { { XC*1,  GENY(9.7), 0,     0}, PushButton,   PUSHID_OK,         ON, 0, "", ""},
      { { XC*25, GENY(9.7), 0,     0}, PushButton,   PUSHID_END,        ON, 0, "", ""},
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
    "kisyuten.tog_idx"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���iIDX�t�@�C���j
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
	"kisyuten.idx"
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
**	�e�L�X�g�A�C�e���i�e�[�u���t�@�C���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TABLE = {
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
	"kisyuten.table"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�e�[�u���j
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


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z�����C�u�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CELLIB = {
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
	"kisyuten.cellib"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�Z�����C�u�����j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CELLIB = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CELLIB,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"CELL"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�N�_�Z���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_KITEN = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�N�_��",
	"kisyuten.kiten"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�I�_�Z���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_SYUTEN = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�I�_��",
	"kisyuten.syuten"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z����w�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�N�I�_��",
	"kisyuten.lv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z���F�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"kisyuten.co"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z�������j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_WT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"kisyuten.wt"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z������j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ST = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	1,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"kisyuten.st"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�A���j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_RENZOKU = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"�N�I�_�̈ʒu",
	"kisyuten.renzoku",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�P�Ɨv�f�̗��["},
       { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "�P�Ȃ���̗��["},
    }
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
