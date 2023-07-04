/*-----------------------------------------------------------------------*/
/*-		DELHUKUIN.R							-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"delHukuin.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELHUKUIN =
{
    L"DELHUKUIN", L"DelHukuin"          // taskid, dllName
}
/*------------------------------------------------------------------------
**	DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DELHUKUIN = {
   DIALOGATTR_SINKABLE,
   31 * XC,
   GENY(7.8),
   NOHELP,
   MHELP,
   HOOKID_DELHUKUIN_DIALOG,
   NOPARENTID,
   "delHukuin",
   {
      { { XC*1,  GENY(1.0), 0,     0}, ToggleButton, TOGGLEID_IDX,     ON, 0, "", ""},
      { { XC*4,  GENY(0.7), 0,     0}, PushButton,   PUSHID_IDX,       ON, 0, "", ""},
      { { XC*11, GENY(1.0), XC*20, 0}, Text,         TEXTID_IDX,       ON, 0, "", ""},
      { { XC*11, GENY(2.2), 0,     0}, Label,        LABELID_LAYER,    ON, 0, "��w  �F  ���� ����", ""},
      { { XC*11, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_LV, ON, 0, "", ""},
      { { XC*15, GENY(3.2), XC*5,  0}, Text,         TEXTID_HUKUIN_CO, ON, 0, "", ""},
      { { XC*20, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_ST, ON, 0, "", ""},
      { { XC*24, GENY(3.2), XC*4,  0}, Text,         TEXTID_HUKUIN_WT, ON, 0, "", ""},
      { { XC*11, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_LV,  ON, 0, "", ""},
      { { XC*15, GENY(4.2), XC*5,  0}, Text,         TEXTID_KUKAN_CO,  ON, 0, "", ""},
      { { XC*20, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_ST,  ON, 0, "", ""},
      { { XC*24, GENY(4.2), XC*4,  0}, Text,         TEXTID_KUKAN_WT,  ON, 0, "", ""},
      { { XC*11, GENY(5.3), XC*7,  0}, Text,         TEXTID_KYOYOUTI,  ON, 0, "", ""},
      { { XC*1,  GENY(6.3), 0,     0}, PushButton,   PUSHID_OK,        ON, 0, "", ""},
      { { XC*23, GENY(6.3), 0,     0}, PushButton,   PUSHID_END,       ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���iIDX�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, NOARG, NOMASK, NOINVERT,
    "",
    "delHukuin.tog_idx"
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
    254,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.idxName"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i����_��w�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_LV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "����",
    "delHukuin.hu_lv"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i����_�F�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_CO =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_co"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i����_����j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_ST =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_st"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i����_�����j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_HUKUIN_WT =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.hu_wt"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i��Ԋ���_��w�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_LV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "��Ԋ�",
    "delHukuin.ku_lv"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i��Ԋ���_�F�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_CO =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_co"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i��Ԋ���_����j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_ST =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    1,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_st"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i��Ԋ���_�����j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KUKAN_WT =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "delHukuin.ku_wt"
};


/*----------------------------------------------------------------------+
**  �e�L�X�g�i���e�l�j
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_KYOYOUTI =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELP,
    NOHOOK, NOARG,
    5,
    "%f",
    "%f",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "���e�l[m]",
    "delHukuin.kyoyouti"
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