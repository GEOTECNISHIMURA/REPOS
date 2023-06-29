/*-----------------------------------------------------------------------*/
/*-		�폜			                        	-*/
/*-									-*/
/*-	        DELELE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"delele.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELELE =
{
    L"DELELE", L"Delele"          // taskid, dllName
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_DELELE = {
   DIALOGATTR_SINKABLE,
   22 * XC,
   GENY(10),
   NOHELP,
   MHELP,
   HOOKID_DELELE_DIALOG,
   NOPARENTID,
   "�폜 Ver.1.4", 
   {
     { {XC*1,   GENY(1),  0,     0}, PushButton,  PUSHID_TABLE, ON, 0, "",         ""},
     { {XC*1,   GENY(2.5),XC*20, 0}, Text,        TEXTID_TABLE, ON, 0, "",         ""},
     { {XC*6.5, GENY(4),  0,     0}, OptionButton,OPTIONID_MODE,ON, 0, "",         ""},
     { {XC*1,   GENY(5),  0,     0}, PushButton,  PUSHID_BAT,   ON, 0, "",         ""},
     { {XC*1,   GENY(6.5),XC*20, 0}, Text,        TEXTID_BAT,   ON, 0, "",         ""},
     { {XC*1,   GENY(8),  XC*6,  0}, PushButton,  PUSHID_OK,    ON, 0, "",         ""},
     { {XC*15,  GENY(8),  XC*6,  0}, PushButton,  PUSHID_END,   ON, 0, "",         ""},
     { {XC*1,   GENY(4),  0,     0}, Label,       0,            ON, 0, "����",     ""},
   }
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
	"�e�[�u��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���@�i�e�[�u���j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_TABLE = {
	NOCMD,
	LCMD,
	NOSYNONYM,
	NOHELP,
	LHELPCMD,
	NOHOOK,
	NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"delset.table"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i���[�h�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_MODE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_MODE,
	NOARG,
	"",
	"delset.mode",
        {
            { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "1̧��"},
            { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�o�b�`"},
        }
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_BAT = {
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
**	�e�L�X�g�A�C�e���@�i�o�b�`�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_BAT = {
	NOCMD,
	LCMD,
	NOSYNONYM,
	NOHELP,
	LHELPCMD,
	NOHOOK,
	NOARG,
	128,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"delset.bat"
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
