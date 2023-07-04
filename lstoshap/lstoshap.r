/*-----------------------------------------------------------------------*/
/*-	        LSTOSHAP.R													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"lstoshap.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LSTOSHAP =
{
    L"LSTOSHAP", L"lstoshap"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	�_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_LSTOSHAP = {
   DIALOGATTR_SINKABLE,
   26 * XC,
   GENY(11.9),
   NOHELP,
   MHELP,
   HOOKID_LSTOSHAP_DIALOG,
   NOPARENTID,
   "ײ݁��������", 
   {
      { {XC*1,  GENY(0.9), 0,     0},ToggleButton, TOGGLEID_LSTOSHAP_BAT, ON, 0, "", ""},
      { {XC*4,  GENY(0.6), 0,     0},PushButton,   PUSHID_LSTOSHAP_BAT,   ON, 0, "", ""},
      { {XC*1,  GENY(2.1), XC*25, 0},Text,         TEXTID_LSTOSHAP_BAT,   ON, 0, "", ""},
      { {XC*0.5,GENY(3.3), 0,     0},PushButton,   PUSHID_LSTOSHAP_TABLE, ON, 0, "", ""},
      { {XC*1,  GENY(4.8), XC*25, 0},Text,         TEXTID_LSTOSHAP_TABLE, ON, 0, "", ""},
      { {XC*6,  GENY(6.25),0,     0},OptionButton, OPTIONID_LSTOSHAP_MODE,ON, 0, "", ""},
      { {XC*6,  GENY(7.5), 0,     0},OptionButton, OPTIONID_LSTOSHAP_TYPE,ON, 0, "", ""},
      { {XC*1,  GENY(8.9), 0,     0},RadioButton,  RADIOBUTTONID_RIGHT,   ON, 0, "", ""},
      { {XC*14, GENY(8.9), 0,     0},RadioButton,  RADIOBUTTONID_LEFT,    ON, 0, "", ""},
      { {XC*1,  GENY(10),  0,     0},PushButton,   PUSHID_LSTOSHAP_OK,    ON, 0, "", ""},
      { {XC*18, GENY(10),  0,     0},PushButton,   PUSHID_LSTOSHAP_END,   ON, 0, "", ""},
   }
};


/*------------------------------------------------------------------------
**	�g�O���{�^���A�C�e���i�o�b�`�j
------------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_LSTOSHAP_BAT = {
   NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
   HOOKID_TOGGLE_LSTOSHAP_BAT, 
   NOARG, NOMASK, NOINVERT,
   "",
   "glo.tog"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_BAT = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_BAT,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�o�b�`"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LSTOSHAP_BAT = {
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
	"glo.bat"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�e�[�u���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_TABLE = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_TABLE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"ڲ԰ð���"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�e�[�u���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LSTOSHAP_TABLE = {
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
	"glo.table"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i���[�h�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LSTOSHAP_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LSTOSHAP_MODE,
	NOARG,
	"����",
	"glo.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�V���O��"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�e�[�u��"},
        }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�^�C�v�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_LSTOSHAP_TYPE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_LSTOSHAP_TYPE,
	NOARG,
	"����",
	"glo.option",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "ײݽ��ݸށ������"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "����߁�ײݽ��ݸ�"},
            { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "����߁������"},
        }
};


/*-----------------------------------------------------------------------
**	���W�I�{�^���A�C�e���i���X�g�j
-----------------------------------------------------------------------*/
DItem_RadioButtonListRsc RADIOBUTTONLISTID_DIR = {
    {
    	RADIOBUTTONID_RIGHT,
    	RADIOBUTTONID_LEFT,
    }
};


/*-----------------------------------------------------------------------
**	���W�I�{�^���A�C�e���i�E���j
-----------------------------------------------------------------------*/
DItem_RadioButtonRsc RADIOBUTTONID_RIGHT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
    NOHOOK, NOARG,
    1,
    0xFFFFFFFF,
    RADIOBUTTONLISTID_DIR,
    "�E���",
    "glo.dir"
};


/*-----------------------------------------------------------------------
**	���W�I�{�^���A�C�e���i�����j
-----------------------------------------------------------------------*/
DItem_RadioButtonRsc RADIOBUTTONID_LEFT = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    2,
    0xFFFFFFFF,
    RADIOBUTTONLISTID_DIR,
    "�����",
    "glo.dir"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_OK = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_OK,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"���s"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_LSTOSHAP_END = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_LSTOSHAP_END,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"�I��"
};
