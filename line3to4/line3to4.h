/*-----------------------------------------------------------------------*/
/*-	        LINE3TO4.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLL��ID
#define  DLLAPP_LINE3TO4			1

/*-	�^�X�N�h�c							-*/
#define		TD_LINE3TO4  		    "LINE3TO4"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j		-*/
#define		LINE3TO4_ID  			100


/*-	�_�C�A���O�{�b�N�X�h�c				-*/
#define		DIALOGID_MAIN			200


/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	�v�b�V���{�^���h�c					-*/
#define		PUSHID_TABLE			310
#define		PUSHID_IDX		    	311
#define		PUSHID_OK				300
#define		PUSHID_END			    301


/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_PUSH_TABLE		310
#define		HOOKID_PUSH_IDX		    311
#define		HOOKID_PUSH_OK	        300
#define		HOOKID_PUSH_END	        301


/*-	�e�L�X�g�h�c					-*/
#define		TEXTID_TABLE			401
#define		TEXTID_IDX  			402

/*-	�g�O���{�^���h�c					-*/
#define		TOGGLEID_IDX			500

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_TOGGLE_IDX		500


typedef struct	LINE3TO4_INFO {
    int     tog_idx;
	WChar	idxName[512];
	WChar	tblName[512];
}	LINE3TO4;
