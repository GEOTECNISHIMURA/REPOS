/*-----------------------------------------------------------------------*/
/*-         FFREF.H         ͯ�ް̧��                                   -*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>


#define  DLLAPP_FFREF			1
#define  DLLAPP_FFREF_FF		2

/*- �^�X�N�h�c                      �@�@-*/
#define     TD_FFREF                L"FFREF"
#define     TD_FF                   L"FF"


/*- �v���O�}�h�c�i���\�[�X�h�c�j    �@�@-*/
#define     FFREF_ID                100
#define     FILE_ID                 101
#define     FF_ID                   102


/*- �_�C�A���O�{�b�N�X�h�c          �@�@-*/
#define     DIALOGID_FFREF          200

/*- �_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define     HOOKID_FFREF_DIALOG     200


/*- �v�b�V���{�^���h�c              �@�@-*/
#define     PUSHID_LAYER2           404
#define     PUSHID_LAYER1           405
#define     PUSHID_OK               400
#define     PUSHID_END              401

/*- �_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define     HOOKID_PUSH_LAYER1      404
#define     HOOKID_PUSH_LAYER2      405
#define     HOOKID_PUSH_OK          400
#define     HOOKID_PUSH_END         401


/*- �e�L�X�g�A�C�e���h�c            �@�@-*/
#define     TEXTID_LV               510
#define     TEXTID_CO               511
#define     TEXTID_LC               512
#define     TEXTID_WT               513
#define     TEXTID_NOLV             514
#define     TEXTID_NOCO             515
#define     TEXTID_NOLC             516
#define     TEXTID_NOWT             517


/*- �I�v�V�����{�^���h�c            �@�@-*/
#define     OPTIONID_MODE           600

/*- �g�O���{�^���h�c                �@�@-*/
#define     TOGGLEID_RDL            700


typedef struct  FFREF_INFO {
    WChar        table[128];
    int         mode;
    WChar        bat[128];
	UInt32      lv, co, wt, nolv, noco, nowt;
    int         lc;
    int         nolc;
    int         nn;
    int         rdl;
}   FFREF;
