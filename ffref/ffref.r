/*-----------------------------------------------------------------------*/
/*-     FFREF.R                                                         -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "ffref.h"

DllMdlApp DLLAPP_FFREF =

{
    L"FFREF", L"Ffref"          // taskid, dllName
}
/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_FFREF = {
    DIALOGATTR_SINKABLE,
    22 * XC,
    GENY(11.3),
    NOHELP,
    MHELP,
    HOOKID_FFREF_DIALOG,
    NOPARENTID,
    "�Q�Ƃ�FF",
    {
        { { XC*7,   GENY(1.0),  0,     0}, OptionButton,OPTIONID_MODE, ON, 0, "",     ""},
        { { XC*7.5, GENY(2.3),  XC*14, 0}, Label,       0,             ON, 0, "�}�s", ""},
        { { XC*16.5,GENY(2.3),  XC*14, 0}, Label,       0,             ON, 0, "�}��", ""},
        { { XC*7,   GENY(3.3),  XC*5,  0}, Text,        TEXTID_LV,     ON, 0, "",     ""},
        { { XC*7,   GENY(4.3),  XC*5,  0}, Text,        TEXTID_CO,     ON, 0, "",     ""},
        { { XC*7,   GENY(5.3),  XC*5,  0}, Text,        TEXTID_LC,     ON, 0, "",     ""},
        { { XC*7,   GENY(6.3),  XC*5,  0}, Text,        TEXTID_WT,     ON, 0, "",     ""},
        { { XC*16,  GENY(3.3),  XC*5,  0}, Text,        TEXTID_NOLV,   ON, 0, "",     ""},
        { { XC*16,  GENY(4.3),  XC*5,  0}, Text,        TEXTID_NOCO,   ON, 0, "",     ""},
        { { XC*16,  GENY(5.3),  XC*5,  0}, Text,        TEXTID_NOLC,   ON, 0, "",     ""},
        { { XC*16,  GENY(6.3),  XC*5,  0}, Text,        TEXTID_NOWT,   ON, 0, "",     ""},
        { { XC*5,   GENY(7.3),  0,     0}, PushButton,  PUSHID_LAYER1, ON, 0, "",     ""},
        { { XC*14,  GENY(7.3),  0,     0}, PushButton,  PUSHID_LAYER2, ON, 0, "",     ""},
        { { XC*2,   GENY(8.8),  0,     0}, ToggleButton,TOGGLEID_RDL,  ON, 0, "",     ""},
        { { XC*1,   GENY(9.8),  0,     0}, PushButton,  PUSHID_OK,     ON, 0, "",     ""},
        { { XC*14,  GENY(9.8),  0,     0}, PushButton,  PUSHID_END,    ON, 0, "",     ""},
    }
};


/*-----------------------------------------------------------------------
**  �I�v�V�����{�^���A�C�e���i���[�h�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_MODE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    NOHOOK, NOARG,
    " ����",
    "ffref.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�S�v�f"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�Z���̂�"},
            { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "�|���S��"},
        }
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�s��w�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LV = {
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
    "ffref.lv"
};

/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�s�F�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "�F",
    "ffref.co"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�s����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_LC = {
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
    "ffref.lc"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�s�����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_WT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "0",
    "31",
    NOMASK,
    NOCONCAT,
    "����",
    "ffref.wt"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�}�s���C���[�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_LAYER1 = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_LAYER1,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "ڲ԰"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�ԉ�w�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOLV = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "1",
    "63",
    NOMASK,
    NOCONCAT,
    "",
    "ffref.nolv"
};

/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�ԐF�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOCO = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "",
    "ffref.noco"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�Ԑ���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOLC = {
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
    "ffref.nolc"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�}�Ԑ����j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_NOWT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    2,
    "%d",
    "%d",
    "0",
    "31",
    NOMASK,
    NOCONCAT,
    "",
    "ffref.nowt"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�}�ԃ��C���[�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_LAYER2 = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_LAYER2,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "ڲ԰"
};


/*-----------------------------------------------------------------------
**  �g�O���A�C�e���@�i�G���[�t���O�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_RDL = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "�g���q��<.rdl>",
    "ffref.rdl"
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i���s�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_OK = {
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
