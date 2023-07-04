/*-----------------------------------------------------------------------*/
/*-	        USER.H		ͯ�ް̧��										-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_LSTOSHAP			    1
#define  DLLAPP_LSTOSHAP_FILE			2
#define  DLLAPP_LSTOSHAP_CNV 			3
#define  DLLAPP_LSTOSHAP_SINGLE			4

/*-	�^�X�NID			�@�@-*/
#define		TD_MAIN				L"LSTOSHAP"
#define		TD_FILE				L"LSTOSHAP_FILE"
#define		TD_CNV				L"LSTOSHAP_CNV"
#define		TD_SINGLE       	L"LSTOSHAP_SINGLE"

/*-	�v���O�}ID�i���\�[�XID�j	-*/
#define		LSTOSHAP_ID			100
#define		FILE_ID				101
#define		CNV_ID				102
#define		SINGLE_ID			103


/*-	�_�C�A���O�{�b�N�XID		�@�@-*/
#define		DIALOGID_LSTOSHAP		200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐�ID�@�@-*/
#define		HOOKID_LSTOSHAP_DIALOG	200


/*-	�v�b�V���{�^��ID		�@�@-*/
#define		PUSHID_LSTOSHAP_BAT		302
#define		PUSHID_LSTOSHAP_TABLE	303
#define		PUSHID_LSTOSHAP_OK		300
#define		PUSHID_LSTOSHAP_END		301

/*-	�_�C�A���O�A�C�e���t�b�N�֐�ID�@�@-*/
#define		HOOKID_PUSH_LSTOSHAP_BAT	302
#define		HOOKID_PUSH_LSTOSHAP_TABLE	303
#define		HOOKID_PUSH_LSTOSHAP_OK		300
#define		HOOKID_PUSH_LSTOSHAP_END	301


/*-	�e�L�X�gID		�@�@			-*/
#define		TEXTID_LSTOSHAP_BAT			400
#define		TEXTID_LSTOSHAP_TABLE		401


/*-	�g�O���{�^��ID		�@�@		-*/
#define		TOGGLEID_LSTOSHAP_BAT		500

/*-	�_�C�A���O�A�C�e���t�b�N�֐�ID�@-*/
#define		HOOKID_TOGGLE_LSTOSHAP_BAT	500


/*-	�I�v�V�����{�^��ID		�@�@	-*/
#define		OPTIONID_LSTOSHAP_TYPE		600
#define		OPTIONID_LSTOSHAP_MODE		601

/*-	�_�C�A���O�A�C�e���t�b�N�֐�ID�@�@-*/
#define		HOOKID_OPTION_LSTOSHAP_TYPE	600
#define		HOOKID_OPTION_LSTOSHAP_MODE	601


/*- ���W�I�{�^��ID 					-*/
#define		RADIOBUTTONLISTID_DIR		700
#define		RADIOBUTTONID_RIGHT			701
#define		RADIOBUTTONID_LEFT			702


typedef struct	GLOBAL_INFO {
	int		option, mode;
	int		dir;
	int		tog;
	WChar	bat[128];
	WChar	table[128];
    int		nn;
}	GLOBAL;

