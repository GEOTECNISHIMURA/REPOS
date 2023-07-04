/*-----------------------------------------------------------------------*/
/*-		edasen.r						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"edasen.h"

DllMdlApp DLLAPP_EDASEN =
{
    L"EDASEN", L"Edasen"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_MAIN = {
   DIALOGATTR_SINKABLE,
   34 * XC,
   GENY(7.9),
   NOHELP,
   MHELP,
   HOOKID_MAIN_DIALOG,
   NOPARENTID,
   "�}���ڑ�",
   {
      { { XC*1,  GENY(0.7), 0,     0}, PushButton, PUSHID_TABLE,  ON, 0, "", ""},
      { { XC*10, GENY(1.1), XC*24, 0}, Text,       TEXTID_TABLE,  ON, 0, "", ""},
      { { XC*8,  GENY(2.3), 0,     0}, Label,      LABELID_LAYER, ON, 0, "��w    �F      ����     ����      ���a[m]", ""},
      { { XC*8,  GENY(3.3), XC*4,  0}, Text,       TEXTID_EDA_LV, ON, 0, "", ""},
      { { XC*12, GENY(3.3), XC*5,  0}, Text,       TEXTID_EDA_CO, ON, 0, "", ""},
      { { XC*18, GENY(3.3), XC*3,  0}, Text,       TEXTID_EDA_ST, ON, 0, "", ""},
      { { XC*21, GENY(3.3), XC*4,  0}, Text,       TEXTID_EDA_WT, ON, 0, "", ""},
      { { XC*8,  GENY(4.3), XC*4,  0}, Text,       TEXTID_ERR_LV, ON, 0, "", ""},
      { { XC*12, GENY(4.3), XC*5,  0}, Text,       TEXTID_ERR_CO, ON, 0, "", ""},
      { { XC*18, GENY(4.3), XC*3,  0}, Text,       TEXTID_ERR_ST, ON, 0, "", ""},
      { { XC*21, GENY(4.3), XC*4,  0}, Text,       TEXTID_ERR_WT, ON, 0, "", ""},
      { { XC*26, GENY(4.3), XC*5,  0}, Text,       TEXTID_ERR_RR, ON, 0, "", ""},
      { { XC*21, GENY(5.4), XC*6,  0}, Text,       TEXTID_LENG,   ON, 0, "", ""},
      { { XC*1,  GENY(6.4), 0,     0}, PushButton, PUSHID_OK,     ON, 0, "", ""},
      { { XC*26, GENY(6.4), 0,     0}, PushButton, PUSHID_END,    ON, 0, "", ""},
   }
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
    "edasen.tblName"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�}����w�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_EDA_LV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�}��",
	"edasen.eda_lv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�}���F�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_EDA_CO = {
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
	"edasen.eda_co"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�}������j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_EDA_ST = {
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
	"edasen.eda_st"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�}�������j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_EDA_WT = {
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
	"edasen.eda_wt"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�G���[��w�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ERR_LV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�G���[",
	"edasen.err_lv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�G���[�F�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ERR_CO = {
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
	"edasen.err_co"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�G���[����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ERR_ST = {
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
	"edasen.err_st"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�G���[�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ERR_WT = {
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
	"edasen.err_wt"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�G���[���a�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_ERR_RR = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%f",
	"%f",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"edasen.err_rr"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LENG = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	4,
	"%f",
	"%f",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�{���Ǝ}���̋���[m]",
	"edasen.leng"
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