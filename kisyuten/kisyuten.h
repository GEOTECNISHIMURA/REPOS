/*-----------------------------------------------------------------------*/
/*-	        KISYUTEN.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_KISYUTEN			1

/*-	�^�X�N�h�c			�@�@			-*/
#define		TD_MAIN						"KISYUTEN"

/*-	�v���O�}�h�c�i���\�[�X�h�c�j�@		-*/
#define		MAIN_ID						100   /*- ���C��   	-*/


/*-	�_�C�A���O�{�b�N�X�h�c		�@�@	-*/
#define		DIALOGID_MAIN				200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_MAIN_DIALOG			200


/*-	�g�O���{�^���h�c		�@�@		-*/
#define		TOGGLEID_IDX				300

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_TOGGLE_IDX			300


/*-	�v�b�V���{�^���h�c		�@�@		-*/
#define		PUSHID_IDX					400
#define		PUSHID_TABLE				401
#define		PUSHID_CELLIB				404
#define		PUSHID_OK					402
#define		PUSHID_END					403

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_PUSH_IDX				400
#define		HOOKID_PUSH_TABLE			401
#define		HOOKID_PUSH_CELLIB			404
#define		HOOKID_PUSH_OK				402
#define		HOOKID_PUSH_END				403


/*-	�e�L�X�g�A�C�e���h�c		�@�@	-*/
#define		TEXTID_IDX					500
#define		TEXTID_TABLE				501
#define		TEXTID_CELLIB				504
#define		TEXTID_KITEN				502
#define		TEXTID_SYUTEN				503
#define		TEXTID_LV					505
#define		TEXTID_CO					506
#define		TEXTID_WT					507
#define		TEXTID_ST					508


/*-	�I�v�V�����{�^���h�c		�@�@		-*/
#define		OPTIONID_RENZOKU			600


/*-	���x���h�c		�@�@		-*/
#define		LABELID_CELL				700


typedef struct	KISYUTEN_INFO {
	int		tog_idx, renzoku;
	WChar	idx[256], table[256];
	WChar   cellib[256];
	WChar	kiten[7], syuten[7];
	UInt32  lv, co, wt;
	int     st;
} KISYUTEN;
