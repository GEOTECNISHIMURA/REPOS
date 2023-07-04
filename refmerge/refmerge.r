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
#include	"refmerge.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_REFMERGE =
{
    TD_REFMERGE , L"refmerge"          // taskid, dllName
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_REFMERGE = {
   DIALOGATTR_SINKABLE,
   22 * XC,
   GENY(6.5),
   NOHELP,
   MHELP,
   HOOKID_REFMERGE_DIALOG,
   NOPARENTID,
   "�Q�Ɠ���", 
   {
     { { XC*8 ,  GENY(1), 0,     0}, ToggleButton,TOGGLEID_BAT,    ON, 0, "",         ""},
     { { XC*19, GENY(1),  0,     0}, ToggleButton,TOGGLEID_MODE,   ON, 0, "",         ""},
     { {XC*1,   GENY(2),  XC*13, 0}, PushButton,  PUSHID_BAT,      ON, 0, "",         ""},
     { {XC*1,   GENY(3.5),XC*20, 0}, Text,        TEXTID_BAT,      ON, 0, "",         ""},
     { {XC*1,   GENY(5),  XC*6,  0}, PushButton,  PUSHID_OK,       ON, 0, "",         ""},
     { {XC*15,  GENY(5),  XC*6,  0}, PushButton,  PUSHID_END,      ON, 0, "",         ""},
     { {XC*1,   GENY(1),  0,     0}, Label,       0,               ON, 0, "�o�b�`����",     ""},
	 { {XC*10.5,  GENY(1),  0,     0}, Label,       0,               ON, 0, "�A�^�b�`only",     ""},
   }
};


/*--------------------------------------------------------------+
|       �g�O���{�^���i�o�b�`�j              |
+--------------------------------------------------------------*/
DItem_ToggleButtonRsc  TOGGLEID_BAT = {
    NOCMD, 
    LCMD,
    NOSYNONYM,
    NOHELP,
    LHELPCMD,
    HOOKID_TOGGLE_BAT,
    NOARG,
    NOMASK,
    NOINVERT,
    "",
    "refmerge.tog_bat"
};


/*--------------------------------------------------------------+
|       �g�O���{�^���i�o�b�`�j              |
+--------------------------------------------------------------*/
DItem_ToggleButtonRsc  TOGGLEID_MODE = {
    NOCMD, 
    LCMD,
    NOSYNONYM,
    NOHELP,
    LHELPCMD,
    HOOKID_TOGGLE_MODE,
    NOARG,
    NOMASK,
    NOINVERT,
    "",
    "refmerge.mode"
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
	"idx�t�@�C���I��"
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
	"refmerge.bat"
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