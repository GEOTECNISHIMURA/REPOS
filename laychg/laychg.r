/*-----------------------------------------------------------------------*/
/*- �Z���A���C���A�e�L�X�g�̏�������                                    -*/
/*-                                                                     -*/
/*-         LAYCHG.R                                                    -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "laychg.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LAYCHG =
{
    L"LAYCHG", L"Laychg"          // taskid, dllName
}
/*-----------------------------------------------------------------------
**  �_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_LAYCHG = {
   DIALOGATTR_SINKABLE,
   47 * XC,
   GENY(15.75),
   NOHELP,
   MHELP,
   HOOKID_LAYCHG_DIALOG,
   NOPARENTID,
   "���C���[�`�F���W", 
   {
     { { XC*1,  GENY(4.0),  0,     0}, ToggleButton, TOGGLEID_TBL,  ON, 0, "", ""},
     { { XC*4,  GENY(3.6),  0,     0}, PushButton,   PUSHID_TBL,    ON, 0, "", ""},
     { { XC*16, GENY(4.0),  XC*25, 0}, Text,         TEXTID_TBL,    ON, 0, "", ""},
     { { XC*8,  GENY(5.4),  0,     0}, OptionButton, OPTIONID_TYPE, ON, 0, "", ""},
     { { XC*6,  GENY(6.9),  0,     0}, Label,        LABELID_OLD,   ON, ALIGN_LEFT, "���v�f", "" },
     { { XC*6,  GENY(7.9),  XC*8,  0}, Text,         TEXTID_OCELL,  ON, 0, "", ""},
     { { XC*6,  GENY(8.9),  XC*12, 0}, ComboBox,     COMBOID_OLV   ,ON, 0, "", ""},
     { { XC*6,  GENY(9.9),  XC*5,  0}, Text,         TEXTID_OCO,    ON, 0, "", ""},
     { { XC*6,  GENY(10.9), XC*3,  0}, Text,         TEXTID_OLC,    ON, 0, "", ""},
     { { XC*6,  GENY(11.9), XC*4,  0}, Text,         TEXTID_OWT,    ON, 0, "", ""},
     { { XC*18, GENY(6.9),  0,     0}, Label,        LABELID_NEW,   ON, ALIGN_LEFT, "�V�v�f", "" },
     { { XC*18, GENY(7.9),  XC*8,  0}, Text,         TEXTID_NCELL,  ON, 0, "", ""},
     { { XC*18, GENY(8.9),  XC*12, 0}, ComboBox,     COMBOID_NLV,   ON, 0, "", ""},
     { { XC*18, GENY(9.9),  XC*5,  0}, Text,         TEXTID_NCO,    ON, 0, "", ""},
     { { XC*18, GENY(10.9), XC*3,  0}, Text,         TEXTID_NLC,    ON, 0, "", ""},
     { { XC*18, GENY(11.9), XC*4,  0}, Text,         TEXTID_NWT,    ON, 0, "", ""},
     { { XC*30, GENY(7.9),  0,     0}, ToggleButton, TOGGLEID_SCALE,ON, 0, "", ""},
     { { XC*30, GENY(8.9),  0,     0}, ToggleButton, TOGGLEID_ANGLE,ON, 0, "", ""},
     { { XC*30, GENY(9.9),  0,     0}, ToggleButton, TOGGLEID_TH,   ON, 0, "", ""},
     { { XC*30, GENY(10.9), 0,     0}, ToggleButton, TOGGLEID_TW,   ON, 0, "", ""},
     { { XC*30, GENY(11.9), 0,     0}, ToggleButton, TOGGLEID_FONT, ON, 0, "", ""},
     { { XC*39, GENY(7.9),  XC*6,  0}, Text,         TEXTID_SCALE,  ON, 0, "", ""},
     { { XC*39, GENY(8.9),  XC*8,  0}, Text,         TEXTID_ANGLE,  ON, 0, "", ""},
     { { XC*39, GENY(9.9),  XC*7,  0}, Text,         TEXTID_TH,     ON, 0, "", ""},
     { { XC*39, GENY(10.9), XC*7,  0}, Text,         TEXTID_TW,     ON, 0, "", ""},
     { { XC*39, GENY(11.9), XC*5,  0}, Text,         TEXTID_FONT,   ON, 0, "", ""},
     { { XC*28, GENY(5.4),  0,     0}, ToggleButton, TOGGLEID_CHECK,ON, 0, "", ""},
     { { XC*1,  GENY(2.55), 0,     0}, ToggleButton, TOGGLEID_BAT,  ON, 0, "", ""},
     { { XC*4,  GENY(2.15), 0,     0}, PushButton,   PUSHID_BAT,    ON, 0, "", ""},
     { { XC*16, GENY(2.55), XC*25, 0}, Text,         TEXTID_BAT,    ON, 0, "", ""},
     { { XC*30, GENY(6.9),  0,     0}, ToggleButton, TOGGLEID_CELLTYPE, ON, 0, "", ""},
     { { XC*39, GENY(6.9),  0,     0}, OptionButton, OPTIONID_CELLTYPE, ON, 0, "", ""},
     { { XC*1,  GENY(14.25),0,     0}, PushButton,   PUSHID_OK,     ON, 0, "", ""},
     { { XC*39, GENY(14.25),0,     0}, PushButton,   PUSHID_END,    ON, 0, "", ""},
     { { XC*4,  GENY(0.5),  0,     0}, PushButton,   PUSHID_CEL,    ON, 0, "", ""},
     { { XC*16, GENY(1.1),  XC*25, 0}, Text,         TEXTID_CEL,    ON, 0, "", ""},
     { { XC*6,  GENY(12.9), XC*10, 0}, Text,         TEXTID_OSTR,   ON, 0, "", ""},
     { { XC*18, GENY(12.9), XC*10, 0}, Text,         TEXTID_NSTR,   ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_BAT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_BAT,
    NOARG, NOMASK, NOINVERT,
    "",
    "layset.tbat"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_BAT = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_BAT,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "�o�b�`"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�o�b�`�t�@�C�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_BAT = {
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
    "layset.bfile"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�e�[�u���j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_TBL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_TBL,
    NOARG, NOMASK, NOINVERT,
    "",
    "layset.ttbl"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�e�[�u���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_TBL = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_TBL,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "�e�[�u��"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�e�[�u���t�@�C�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TBL = {
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
    "layset.file"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�Z���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_CEL = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_CEL,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "��ײ����"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�Z�����C�u�������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CEL = {
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
    "layset.cfile"
};


/*-----------------------------------------------------------------------
**  �I�v�V�����{�^���A�C�e���i�ϊ��v�f�̑I���j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_TYPE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    HOOKID_OPTION_TYPE,
    NOARG,
    "�^�C�v",
    "layset.type",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "ײ�"},
       { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "÷��"},
       { NOTYPE, NOICON, NOCMD, LCMD, 3, NOMASK, ON, "÷��(�����ɂ��)"},
    }
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i���Z�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OCELL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, 
    NOHOOK, NOARG,
    100,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "�Z��",
    "layset.ocell"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�����x���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OLV = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    511,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "��w",
    "layset.olvstr"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i���J���[�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OCO = {
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
    "layset.oco"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OLC = {
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
    "layset.olc"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OWT = {
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
    "layset.owt"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_OSTR = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    100,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "����",
    "layset.ostr"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V�Z�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NCELL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    7,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.ncell"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V���x���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NLV = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    511,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.nlvstr"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V�J���[�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NCO = {
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
    "layset.nco"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NLC = {
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
    "layset.nlc"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NWT = {
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
    "layset.nwt"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NSTR = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    100,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.nstr"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i���فj
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_SCALE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_SCALE,
    NOARG, NOMASK, NOINVERT,
    "����",
    "layset.tscale"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�p�x�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_ANGLE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_ANGLE,
    NOARG, NOMASK, NOINVERT,
    "�p�x",
    "layset.tangle"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�������j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_TH = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_TH,
    NOARG, NOMASK, NOINVERT,
    "����",
    "layset.tth"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�������j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_TW = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_TW,
    NOARG, NOMASK, NOINVERT,
    "��",
    "layset.ttw"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i̫�āj
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_FONT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_FONT,
    NOARG, NOMASK, NOINVERT,
    "̫��",
    "layset.tfont"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�V�Z���X�P�[���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_SCALE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    4,
    "%lf",
    "%lf",
    "0",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.scale"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�p�x�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_ANGLE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    6,
    "%lf",
    "%lf",
    "0",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.angle"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i���������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TH = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    5,
    "%lf",
    "%lf",
    "0",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.th"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i�������j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TW = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    5,
    "%lf",
    "%lf",
    "0",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.tw"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���i̫�āj
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_FONT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "layset.font"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�`�F�b�N�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CHECK = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "�`�F�b�N",
    "layset.check"
};


/*-----------------------------------------------------------------------
**  �g�O���{�^���i�Z���^�C�v�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_CELLTYPE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD, HOOKID_TOGGLE_CELLTYPE,
    NOARG, NOMASK, NOINVERT,
    "������",
    "layset.tcelltype"
};


/*-----------------------------------------------------------------------
**  �I�v�V�����{�^���A�C�e���i�Z���^�C�v�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_CELLTYPE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    HOOKID_OPTION_TYPE,
    NOARG,
    "",
    "layset.ocelltype",
        {
           { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�}�`"},
           { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�_"},
        }
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i���s�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_OK = {
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
**  �v�b�V���{�^���i�I���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_END = {
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
**  �R���{�{�b�N�X���j���[�i����w�j
-----------------------------------------------------------------------*/
DItem_ComboBoxRsc COMBOID_OLV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
    NOHOOK, NOARG,
    256,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    0, 
    15,-2,
    0, 0,
    COMBOATTR_FULLWIDTH | COMBOATTR_READONLY/*COMBOATTR_DRAWPREFIXICON*/,
    "��w",
    "layset.olvstr",
    {
       { XC*40,256, ALIGN_LEFT | LISTCOLATTR_EDITABLE, ""},
    }
}
/*-----------------------------------------------------------------------
**  �R���{�{�b�N�X���j���[�i�V��w�j
-----------------------------------------------------------------------*/
DItem_ComboBoxRsc COMBOID_NLV =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
    NOHOOK, NOARG,
    256,
    "%s",
    "%s",
    "",
    "",
    NOMASK,
    0, 
    15, -2,
    0, 0,
    COMBOATTR_FULLWIDTH | COMBOATTR_READONLY | COMBOATTR_DRAWPREFIXICON,
    "",
    "layset.nlvstr",
    {
       { XC*40,256, ALIGN_LEFT, ""},
    }
}
