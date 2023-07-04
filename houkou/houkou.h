#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_HOUKOU			1
#define  DLLAPP_HOUKOU_CUT      2
#define  DLLAPP_HOUKOU_UNCUT    3
#define  DLLAPP_HOUKOU_YA       4

/*-	�^�X�N�h�c			�@�@-*/
#define		TD_HOUKOU		L"HOUKOU"
#define		TD_UNCUT		L"UNCUT"
#define		TD_CUT			L"CUT"
#define		TD_YA			L"YA"
#define		TD_SITYU		L"SITYU"

/*-	�v���O�}�h�c�i���\�[�X�h�c�j	�@�@-*/
#define		HOUKOU_ID		100
#define		UNCUT_ID		105
#define		CUT_ID			110
#define		YA_ID			111
#define		SITYU_ID		112

/*-	�_�C�A���O�{�b�N�X�h�c		�@�@-*/
#define		DIALOGID_HOUKOU			300

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_HOUKOU_DIALOG	400

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_OPTION_CUT		500
#define		HOOKID_OPTION_LAYER		501

#define		HOOKID_PUSH_OK			510
#define		HOOKID_PUSH_END			511

/*-	�e�L�X�g�h�c		�@�@-*/
#define		TEXTID_SCALE		700
#define		TEXTID_LV			701
#define		TEXTID_CO			702
#define		TEXTID_LC			703
#define		TEXTID_WT			704
#define		TEXTID_CELL			705

/*-	�v�b�V���{�^���h�c		�@�@-*/
#define		PUSHID_OK			800
#define		PUSHID_END			801

/*-	�I�v�V�����{�^���h�c		�@�@-*/
#define		OPTIONID_CUT		900
#define		OPTIONID_LAYER		901
#define		OPTIONID_CELL		902


typedef struct	HOUSET_INFO {
    int		cut;        /*- 0:����; 1:���΂Ȃ��@2:���@3:�x��-*/
    int		layer;      /*- 0:�J�����g�@1:�Z���Ɠ���;  2:�ݒ肷�� -*/
    long	scale;
    int		lv, co, lc, wt;
    WChar	cell[7];
    bool		zukei;
}	HOUSET;
