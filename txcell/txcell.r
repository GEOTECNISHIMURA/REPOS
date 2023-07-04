/*-----------------------------------------------------------------------*/
/*-	        TXCELL.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include	"txcell.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_TXCELL =
{
    L"TXCELL", L"txcell"          // taskid, dllName
}

/*-----------------------------------------------------------------------
**	�_�C�A���O�{�b�N�X
-----------------------------------------------------------------------*/
DialogBoxRsc		DIALOGID_TXCELL = {
   DIALOGATTR_SINKABLE,
   25 * XC,
   GENY(25.2),
   NOHELP,
   MHELP,
   HOOKID_TXCELL_DIALOG,
   NOPARENTID,
   "��_�W�J", 
   {
      { { XC*1,   GENY(1),    0,     0}, ToggleButton, TOGGLEID_IDX,    ON, 0, "", ""},
      { { XC*4,   GENY(0.7),  0,     0}, PushButton,   PUSHID_IDX,      ON, 0, "", ""},
      { { XC*1,   GENY(2.2),  XC*24, 0}, Text,         TEXTID_IDX,      ON, 0, "", ""},
      { { XC*0.5, GENY(3.5),  0,     0}, PushButton,   PUSHID_SEED,     ON, 0, "", ""},
      { { XC*1,   GENY(5),    XC*24, 0}, Text,         TEXTID_SEED,     ON, 0, "", ""},
      { { XC*0.5, GENY(6.3),  0,     0}, PushButton,   PUSHID_FILE,     ON, 0, "", ""},
      { { XC*1,   GENY(7.8),  XC*24, 0}, Text,         TEXTID_FILE,     ON, 0, "", ""},
      { { XC*14,  GENY(9.3),  0,     0}, OptionButton, OPTIONID_FORMAT, ON, 0, "", ""},
      { { XC*14,  GENY(10.3), 0,     0}, OptionButton, OPTIONID_PNAME,  ON, 0, "", ""},
      { { XC*14,  GENY(11.3), 0,     0}, OptionButton, OPTIONID_KUGIRI, ON, 0, "", ""},
      { { XC*10,  GENY(12.6), 0,     0}, OptionButton, OPTIONID_TXPOS,  ON, 0, "", ""},
      { { XC*10,  GENY(13.6), XC*5,  0}, Text,         TEXTID_FONT,     ON, 0, "", ""},
      { { XC*0.5, GENY(14.9), 0,     0}, PushButton,   PUSHID_CELLIB,   ON, 0, "", ""},
      { { XC*1,   GENY(16.4), XC*24, 0}, Text,         TEXTID_CELLIB,   ON, 0, "", ""},
      { { XC*10,  GENY(17.5), 0,     0}, OptionButton, OPTIONID_SCALE,  ON, 0, "", ""},
      { { XC*10,  GENY(18.5), XC*8,  0}, Text,         TEXTID_CELL,     ON, 0, "", ""},
      { { XC*10,  GENY(19.7), XC*4,  0}, Text,         TEXTID_LV,       ON, 0, "", ""},
      { { XC*10,  GENY(20.7), XC*5,  0}, Text,         TEXTID_CO,       ON, 0, "", ""},
      { { XC*10,  GENY(21.7), XC*3,  0}, Text,         TEXTID_LC,       ON, 0, "", ""},
      { { XC*10,  GENY(22.7), XC*4,  0}, Text,         TEXTID_WT,       ON, 0, "", ""},
      { { XC*1,   GENY(23.7), 0,     0}, PushButton,   PUSHID_OK,       ON, 0, "", ""},
      { { XC*17,  GENY(23.7), 0,     0}, PushButton,   PUSHID_END,      ON, 0, "", ""},
   }
};


/*-----------------------------------------------------------------------
**	�g�O���{�^���iIDX�j
-----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_IDX = {
    NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
    HOOKID_TOGGLE_IDX, 
    NOARG, NOMASK, NOINVERT,
    "",
    "txset.tog_idx"
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
	"IDX̧�ّI��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���iIDX�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_IDX = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"txset.idx"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�V�[�h�j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_SEED = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_SEED,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"����̧�ّI��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�V�[�h�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_SEED = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"txset.seed"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�t�@�C���j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_FILE = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_FILE,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"��_̧�ّI��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�t�@�C�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_FILE = {
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
	"txset.file"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�t�H�[�}�b�g�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_FORMAT = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_FORMAT,
	NOARG,
	"̫�ϯ�",
	"txset.form",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "���w���W"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "���n���W"},
       { NOTYPE, NOICON, NOCMD, LCMD, 2, NOMASK, ON, "SIMA"},
    }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�_���j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_PNAME = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	HOOKID_OPTION_PNAME,
	NOARG,
	"�_��",
	"txset.pname",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "����"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�Ȃ�"},
    }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i��؂�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_KUGIRI = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"��؂�L��",
	"txset.kugiri",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�X�y�[�X"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�J���}"},
    }
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�e�L�X�g�ʒu�j
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_TXPOS = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"÷�Ĉʒu",
	"txset.txpos",
    {
       { NOTYPE, NOICON, NOCMD, LCMD, 0, NOMASK, ON, "�Z���̉E"},
       { NOTYPE, NOICON, NOCMD, LCMD, 1, NOMASK, ON, "�Z����_��"},
    }
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�t�H���g�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_FONT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"̫��",
	"txset.font"
};


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�Z�����C�u�����j
-----------------------------------------------------------------------*/
DItem_PushButtonRsc	PUSHID_CELLIB = {
	NOT_DEFAULT_BUTTON,
	NOHELP,
	LHELPCMD,
	HOOKID_PUSH_CELLIB,
	ACTIONBUTTON_APPLY,
	NOCMD,
	LCMD,
	"",
	"��ײ���ؑI��"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z�����C�u�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CELLIB = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	255,
	"%s",
	"%s",
	"",
	"",
	NOMASK,
	NOCONCAT,
	"",
	"txset.cellib"
};


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���A�C�e���i�k�ځj,
-----------------------------------------------------------------------*/
DItem_OptionButtonRsc	OPTIONID_SCALE = {
	NOSYNONYM,
	NOHELP,
	MHELP,
	NOHOOK,
	NOARG,
	"�k��",
	"txset.scale",
    {
       { NOTYPE, NOICON, NOCMD, LCMD,    5, NOMASK, ON, "1/5"},
       { NOTYPE, NOICON, NOCMD, LCMD,   10, NOMASK, ON, "1/10"},
       { NOTYPE, NOICON, NOCMD, LCMD,   20, NOMASK, ON, "1/20"},
       { NOTYPE, NOICON, NOCMD, LCMD,   50, NOMASK, ON, "1/50"},
       { NOTYPE, NOICON, NOCMD, LCMD,  100, NOMASK, ON, "1/100"},
       { NOTYPE, NOICON, NOCMD, LCMD,  200, NOMASK, ON, "1/200"},
	   { NOTYPE, NOICON, NOCMD, LCMD,  250, NOMASK, ON, "1/250"},
	   { NOTYPE, NOICON, NOCMD, LCMD,  500, NOMASK, ON, "1/500"},
	   { NOTYPE, NOICON, NOCMD, LCMD, 1000, NOMASK, ON, "1/1000"},
	   { NOTYPE, NOICON, NOCMD, LCMD, 2500, NOMASK, ON, "1/2500"},
    }
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�Z�����j
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
	"�Z����",
	"txset.cell"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i��w�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LV = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"1",
	"63",
	NOMASK,
	NOCONCAT,
	"��w",
	"txset.lv"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�F�j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_CO = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	4,
	"%d",
	"%d",
	"0",
	"255",
	NOMASK,
	NOCONCAT,
	"�F",
	"txset.co"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_LC = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	2,
	"%d",
	"%d",
	"0",
	"7",
	NOMASK,
	NOCONCAT,
	"����",
	"txset.lc"
};


/*-----------------------------------------------------------------------
**	�e�L�X�g�A�C�e���i�����j
-----------------------------------------------------------------------*/
DItem_TextRsc	TEXTID_WT = {
	NOCMD, LCMD, NOSYNONYM, NOHELP, LHELPCMD,
	NOHOOK, NOARG,
	3,
	"%d",
	"%d",
	"0",
	"15",
	NOMASK,
	NOCONCAT,
	"����",
	"txset.wt"
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