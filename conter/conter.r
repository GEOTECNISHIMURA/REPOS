/*----------------------------------------------------------------------+
|   $Workfile:   conter.r
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"conter.h"
DllMdlApp DLLAPP_CONTER =
{
    L"CONTER", L"conter"          // taskid, dllName
}

/*----------------------------------------------------------------------+
|    �_�C�A���O�{�b�N�X
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_CONTER =
{
    DIALOGATTR_DEFAULT | DIALOGATTR_NORIGHTICONS | DIALOGATTR_AUTOOPEN,
    XC * 24,
    GENY(13),
    NOHELP, MHELPTOPIC,
    HOOKID_CONTER_DIALOG, NOPARENTID,
    "����",
	{
       { { XC*6,  GENY(0.7),  0,    0     }, OptionButton, OPTIONID_CONTER_MODE, ON, 0, "", ""},
       { { XC*9,  GENY(2.2),  XC*8, 0     }, Text,         TEXTID_CONTER_ZZ,     ON, 0, "", ""},
       { { XC*9,  GENY(3.2),  XC*6, 0     }, Text,         TEXTID_CONTER_PITCH,  ON, 0, "", ""},
       { { XC*17, GENY(2),    XC*4, YC*1.5}, PushButton,   PUSHID_CONTER_INCRE,  ON, 0, "", ""},
       { { XC*17, GENY(3.1),  XC*4, YC*1.5}, PushButton,   PUSHID_CONTER_DECRE,  ON, 0, "", ""},
       { { XC*1,  GENY(4.7),  0,    0     }, ToggleButton, TOGGLEID_CONTER_BSPLN,ON, 0, "", ""},
       { { XC*9,  GENY(5.7),  XC*7, 0     }, Text,         TEXTID_CONTER_GENKOU, ON, 0, "", ""},
       { { XC*13, GENY(7.1),  XC*4, 0     }, Text,         TEXTID_CONTER_KLV,    ON, 0, "", ""},
       { { XC*13, GENY(8.2),  XC*4, 0     }, Text,         TEXTID_CONTER_SLV,    ON, 0, "", ""},
       { { XC*13, GENY(9.3),  XC*4, 0     }, Text,         TEXTID_CONTER_OTLV,   ON, 0, "", ""},
       { { XC*17, GENY(6.9),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_KLV,    ON, 0, "", ""},
       { { XC*17, GENY(8.1),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_SLV,    ON, 0, "", ""},
       { { XC*17, GENY(9.2),  XC*6, YC*1.5}, PushButton,   PUSHID_CONTER_OTLV,   ON, 0, "", ""},
       { { XC*13, GENY(10.5), XC*5, 0     }, Text,         TEXTID_CONTER_HANI,   ON, 0, "", ""},
       { { XC*0,  GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_OK,     ON, 0, "", ""},
       { { XC*8,  GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_SETZ,   ON, 0, "", ""},
       { { XC*16, GENY(11.5), XC*7, 0     }, PushButton,   PUSHID_CONTER_END,    ON, 0, "", ""},
	}
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i���e�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CONTER_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_CONTER_MODE,
	NOARG,
	"���e",
	"con.mode",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�V�K"},
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�C��"},
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "������J����"},
    }
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_ZZ = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%lf",
	"%lf",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"����[m]",
	"con.zz"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Ԋu�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_PITCH = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	4,
	"%lf",
	"%lf",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�Ԋu[m]",
	"con.pitch"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�{�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_INCRE = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_INCRE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�{"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�[�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_DECRE = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_DECRE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�|"
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���iB-�X�v���C���j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CONTER_BSPLN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_CONTER_BSPLN, NOARG, NOMASK, NOINVERT,
    "B-���ײ�",
    "con.bspln"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_GENKOU = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	5,
	"%lf",
	"%lf",
	"0.001",
	"",
	NOMASK,
	NOCONCAT,
	"����[m]",
	"con.genkou"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�v�ȁj
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_KLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"�v�ȉ�w",
	"con.klv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i��ȁj
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_SLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"��ȉ�w",
	"con.slv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�����E�J��w�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_OTLV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"������J��w",
	"con.otlv"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�v�Ȑݒ�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_KLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_KLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�ݒ�"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i��Ȑݒ�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_SLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_SLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�ݒ�"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����E�J�ݒ�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_OTLV = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_OTLV,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�ݒ�"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�͈́j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CONTER_HANI = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"1",
	"",
	NOMASK,
	NOCONCAT,
	"�C���͈�[m]",
	"con.hani"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_OK = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"���s"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����ݒ�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_SETZ = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_SETZ,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"����"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CONTER_END = {
	DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CONTER_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�I��"
};
