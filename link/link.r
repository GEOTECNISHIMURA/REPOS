/*-----------------------------------------------------------------------*/
/*-	�����N					        -*/
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
**	�_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_LINK = {
   DIALOGATTR_SINKABLE,
   27 * XC,
   GENY(16),
   NOHELP,
   MHELP,
   HOOKID_LINK_DIALOG,
   NOPARENTID,
   "�����N",
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
**	�v�b�V���{�^���i���C���[�e�[�u���j
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
	"���C���[�e�[�u��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i���C���[�e�[�u���j
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
**	�I�v�V�����{�^���A�C�e���i��ށj
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
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�o�b�`"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�P�t�@�C��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "1�v�f(�ő�MSLINK+1�ǉ�)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "1�v�f(�ݒ肵�Ēǉ�)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "1�v�f(����MSLINK)"},
       { NOTYPE, NOICON, NOCMD, LCMD, 5, NOMASK, ON, "̧�ٖ���MSLINK�ŗ^����"},
    }
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j
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
	"�o�b�`�t�@�C��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�o�b�`�j
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
**	�e�L�X�g�A�C�e���i�ް�������j
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
	"�ް������",
	"linkset.dbsource"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�e�[�u�����j
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
	"ð��ٖ�",
	"linkset.table"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���iMSLINK�j
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
**	�g�O���{�^���i�}�ʔԍ��j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_ZUMEN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_ZUMEN, NOARG, NOMASK, NOINVERT,
    "�}�ʔԍ�",
    "linkset.zumen"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�����j
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
       { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "�R��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "�S��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 5, NOMASK, ON, "�T��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 6, NOMASK, ON, "�U��"},
    }
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���i�����f�[�^�폜�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_DEL_DB = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG, NOMASK, NOINVERT,
    "���������ް��폜",
    "linkset.tdel"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����j
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
	"����"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����j
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
	"����"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j
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
	"�I��"
};
