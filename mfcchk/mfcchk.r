/*-----------------------------------------------------------------------*/
/*-     MFCCHK.R                                                        -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include "mfcchk.h"
/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_MFCCHK =
{
    L"MFCCHK", L"Mfcchk"          // taskid, dllName
}

/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc        DIALOGID_MFCCHK = {
   DIALOGATTR_SINKABLE,
   25 * XC,
   GENY(19.9),
   NOHELP,
   MHELP,
   HOOKID_MFCCHK_DIALOG,
   NOPARENTID,
   "�l�e�b����   Ver.1.8",
   {
      { {XC*0.5, GENY(0.8),  0,     0}, PushButton,  PUSHID_TABLE,         ON, 0, "",             ""},
      { {XC*1,   GENY(2.3),  XC*24, 0}, Text,        TEXTID_TABLE,         ON, 0, "",             ""},
      { {XC*1,   GENY(3.8),  0,     0}, Label,       0,                    ON, 0, "����",         ""},
      { {XC*6.5, GENY(3.8),  0,     0}, OptionButton,OPTIONID_MODE,        ON, 0, "",             ""},
      { {XC*0.5, GENY(5),    0,     0}, PushButton,  PUSHID_BAT,           ON, 0, "",             ""},
      { {XC*1,   GENY(6.5),  XC*24, 0}, Text,        TEXTID_BAT,           ON, 0, "",             ""},
      { {0.5*XC, GENY(7.7),  24*XC,7.5*YC},GroupBox, 0,                    ON, 0, "",             ""},
      { {XC*1.5, GENY(8.2),  0,     0}, ToggleButton,TOGGLEID_ERR,         ON, 0, "",             ""},
      { {XC*7,   GENY(9.2),  XC*5,  0}, Text,        TEXTID_LV,            ON, 0, "",             ""},
      { {XC*7,   GENY(10.2), XC*5,  0}, Text,        TEXTID_CO,            ON, 0, "",             ""},
      { {XC*12.5,GENY(10.2), 0,     0}, ColorPicker, COLORPICKERID_ELEMCO, ON, 0, "",             ""},
      { {XC*7,   GENY(11.2), XC*5,  0}, Text,        TEXTID_LC,            ON, 0, "",             ""},
      { {XC*7,   GENY(12.2), XC*5,  0}, Text,        TEXTID_WT,            ON, 0, "",             ""},
      { {XC*15,  GENY(9.2),  XC*14, 0}, Label,       0,                    ON, 0, "�G���[��",     ""},
      { {XC*16,  GENY(10.2), XC*8,  0}, Text,        TEXTID_CT,            ON, 0, "",             ""},
      { {XC*15,  GENY(11.2), XC*14, 0}, Label,       0,                    ON, 0, "�װ����",      ""},
      { {XC*16,  GENY(12.2), XC*8,  0}, Text,        TEXTID_SIZE,          ON, 0, "",             ""},
      { {0.5*XC, GENY(14.5), 24*XC, 3.5*YC}, GroupBox, 0,                  ON, 0, "�e�L�X�g�ݒ�", ""},
      { {XC*1.5, GENY(15),   0,     0}, ToggleButton,TOGGLEID_FONT,        ON, 0, "",             ""},
      { {XC*19,  GENY(15),   XC*5,  0}, Text,        TEXTID_ZEN,           ON, 0, "",             ""},
      { {XC*19,  GENY(16),   XC*5,  0}, Text,        TEXTID_HAN,           ON, 0, "",             ""},
      { {XC*1.5, GENY(16),   0,     0}, ToggleButton,TOGGLEID_SPACE,       ON, 0, "",             ""},
      { {XC*2,   GENY(17.75),0,     0}, PushButton,  PUSHID_OK,            ON, 0, "",             ""},
      { {XC*16,  GENY(17.75),0,     0}, PushButton,  PUSHID_END,           ON, 0, "",             ""},
   }
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i���C���[�e�[�u���I���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHID_TABLE = {
    NOT_DEFAULT_BUTTON,
    NOHELP,
    LHELPCMD,
    HOOKID_PUSH_TABLE,
    ACTIONBUTTON_APPLY,
    NOCMD,
    LCMD,
    "",
    "MFC�e�[�u��"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i���C���[�e�[�u���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_TABLE = {
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
    "mfcchk.table"
};


/*-----------------------------------------------------------------------
**  �I�v�V�����{�^���A�C�e���i���[�h�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc   OPTIONID_MODE = {
    NOSYNONYM,
    NOHELP,
    MHELP,
    HOOKID_OPTION_MODE,
    NOARG,
    "",
    "mfcchk.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "1̧��"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�o�b�`"},
        }
};


/*-----------------------------------------------------------------------
**  �v�b�V���{�^���i�o�b�`�t�@�C���I���j
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
**  �e�L�X�g�A�C�e���@�i�o�b�`�t�@�C���j
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
    "mfcchk.bat"
};


/*-----------------------------------------------------------------------
**  �g�O���A�C�e���@�i�G���[�t���O�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_ERR = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_ERR,
    NOARG, NOMASK, NOINVERT,
    "�G���[�t���O",
    "mfcchk.err"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i��w�j
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
    "mfcchk.lv"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�F�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CO = {
    NOCMD, LCMD, SYNONYMID_ELECO, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "0",
    "254",
    NOMASK,
    NOCONCAT,
    "�F",
    "mfcchk.co"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i����j
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
    "mfcchk.lc"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�����j
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
    "mfcchk.wt"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�G���[���j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_CT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_ERRCT_TEXT,
    NOARG,
    5,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "mfcchk.ct"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�G���[�T�C�Y�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_SIZE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK,
    NOARG,
    6,
    "%.3lf",
    "%lf",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "",
    "mfcchk.size"
};


/*-----------------------------------------------------------------------
**  �J���[�s�b�J�[�A�C�e���@�i�J���[�p���b�g�j
-----------------------------------------------------------------------*/
DItem_ColorPickerRsc    COLORPICKERID_ELEMCO = {
    NOCMD, LCMD, SYNONYMID_ELECO, NOHELP, LHELPCMD,
    NOHOOK,
    NOARG,
    TEXTID_CO,
    NOMASK,
    "",
    "mfcchk.co"
};


/*-----------------------------------------------------------------------
**  �V�m�j�����\�[�X�@�i�J���[�e�L�X�g�ƃJ���[�p���b�g�̘A���j
-----------------------------------------------------------------------*/
DItem_SynonymsRsc SYNONYMID_ELECO = 
    {
        {
            { Text,          TEXTID_CO},
            { ColorPicker,   COLORPICKERID_ELEMCO},
        }
    };


/*-----------------------------------------------------------------------
**  �g�O���A�C�e���@�i�t�H���g�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_FONT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKID_TOGGLE_FONT,
    NOARG, NOMASK, NOINVERT,
    "�t�H���g",
    "mfcchk.font"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i�S�p�t�H���g�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_ZEN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "�S�p",
    "mfcchk.zen"
};


/*-----------------------------------------------------------------------
**  �e�L�X�g�A�C�e���@�i���p�t�H���g�j
-----------------------------------------------------------------------*/
DItem_TextRsc   TEXTID_HAN = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    NOHOOK, NOARG,
    3,
    "%d",
    "%d",
    "",
    "",
    NOMASK,
    NOCONCAT,
    "���p",
    "mfcchk.han"
};


/*-----------------------------------------------------------------------
**  �g�O���A�C�e���@�i�X�y�[�X�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_SPACE = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK,
    NOARG, NOMASK, NOINVERT,
    "�X�y�[�X",
    "mfcchk.space"
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