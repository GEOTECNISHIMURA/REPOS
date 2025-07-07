/*-----------------------------------------------------------------------*/
/*-	        PTN.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_PTN			1
#define  DLLAPP_PTN_MAIN	2
#define  DLLAPP_PTN_FILE  	3

/*-	�^�X�N�h�c			                -*/
#define     TD_PTN                  L"PTN"
#define     TD_FILE                 L"FILE"
#define     TD_MAIN                 L"MAIN"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j        -*/
#define     PTN_ID                  100
#define     FILE_ID                 101
#define     MAIN_ID                 102


/*-	�_�C�A���O�{�b�N�X�h�c              -*/
#define     DIALOGID_PTN            200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c    -*/
#define     HOOKID_PTN_DIALOG       200


/*-	�v�b�V���{�^���h�c                  -*/
#define     PUSHID_TABLE            402
#define     PUSHID_BAT              403
#define     PUSHID_CEL              404
#define     PUSHID_OK               400
#define     PUSHID_END              401

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c    -*/
#define     HOOKID_PUSH_TABLE       402
#define     HOOKID_PUSH_BAT         403
#define     HOOKID_PUSH_CEL         404
#define     HOOKID_TOGGLE_PTLV      600
#define     HOOKID_PUSH_OK          400
#define     HOOKID_PUSH_END         401
#define     HOOKID_OPTION_MODE      700


/*-	�e�L�X�g�A�C�e���h�c                -*/
#define     TEXTID_TABLE            500
#define     TEXTID_BAT              501
#define     TEXTID_CEL              502
#define     TEXTID_LV               510


/*- �g�O���{�^���h�c                    -*/
#define     TOGGLEID_PTLV           600
#define     TOGGLEID_FENCE          601
#define     TOGGLEID_BUNKATU        602

/*- �I�v�V�����{�^���h�c                -*/
#define		OPTIONID_MODE           700
#define		OPTIONID_OPT            701


typedef struct	PTN_INFO {
	WChar		table[128];
	int			mode, opt;
	WChar		bat[128];
	WChar		cel[128];
	int			ptlv, fence, bunkatu;
	UInt32      lv;
	int			nn, ct;
}	PTN;
