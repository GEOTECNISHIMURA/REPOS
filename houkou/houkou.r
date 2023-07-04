/*-----------------------------------------------------------------------*/
/*-	        HOUKOU.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"houkou.h"	
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_HOUKOU =
{
    TD_HOUKOU, L"houkou"          // taskid, dllName
}

/*----------------------------------------------------------------------+
|    Local defines							|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_HOUKOU =
{
    DIALOGATTR_SINKABLE,
    21 * XC,
    GENY(11.5),
    NOHELP,
    MHELP,
    HOOKID_HOUKOU_DIALOG,
    NOPARENTID,
    "������",
    {
       { { XC*6,    GENY(1),   XC*15.5,0}, OptionButton, OPTIONID_CUT,   ON, 0, "", ""},
       { { XC*6,    GENY(2.5), 0,      0}, OptionButton, OPTIONID_LAYER, ON, 0, "", ""},
       { { XC*6,    GENY(4),   XC*7,   0}, Text,         TEXTID_SCALE,   ON, 0, "", ""},
       { { XC*6,    GENY(6),   XC*5,   0}, Text,         TEXTID_LV,      ON, 0, "", ""},
       { { XC*6,    GENY(7),   XC*5,   0}, Text,         TEXTID_CO,      ON, 0, "", ""},
       { { XC*6,    GENY(8),   XC*5,   0}, Text,         TEXTID_LC,      ON, 0, "", ""},
       { { XC*6,    GENY(9),   XC*5,   0}, Text,         TEXTID_WT,      ON, 0, "", ""},
       { { XC*1,    GENY(10),  0,      0}, PushButton,   PUSHID_OK,      ON, 0, "", ""},
       { { XC*13,   GENY(10),  0,      0}, PushButton,   PUSHID_END,     ON, 0, "", ""},
       { { XC*6,    GENY(5),   XC*8,   0}, Text,         TEXTID_CELL,    ON, 0, "", ""},
       { { XC*14.5, GENY(5),   0,      0}, OptionButton, OPTIONID_CELL,  ON, 0, "", ""},
    }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�������� ����/���΂Ȃ��j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CUT = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_CUT,
	NOARG,
	"���",
	"houset.cut",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "����"         },
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "���΂Ȃ�(ײ�)"},
        { NOTYPE, NOICON, NOCMD, LCMD, 4, NOMASK, ON, "���΂Ȃ�(��)" },
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "���"         },
        { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "�x��"         },
    }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�Z���Ɠ������C���[���ǂ����j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LAYER = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LAYER,
	NOARG,
	"ڲ԰",
	"houset.layer",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "��è��"      },
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�d���قƓ���"},
        { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "�ݒ肷��"    },
    }
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�k�ځj
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_SCALE = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	5,
	"%ld",
	"%ld",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�k��",
	"houset.scale"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i���x���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"��w",
	"houset.lv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�J���[�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"0",
	"255",
	NOMASK,
	NOCONCAT,
	"�F",
	"houset.co"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�X�^�C���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LC = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	1,
	"%d",
	"%d",
	"0",
	"7",
	NOMASK,
	NOCONCAT,
	"����",
	"houset.lc"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�E�G�C�g�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_WT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"����",
	"houset.wt"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_OK = {
	DEFAULT_BUTTON,
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


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CELL = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	6,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"�ٖ�",
	"houset.cell"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�}�`�^�_�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_CELL = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"",
	"houset.zukei",
    {
        { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�}�`"},
        { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�_"  },
    }
};
