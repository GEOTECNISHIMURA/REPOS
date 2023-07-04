/*-----------------------------------------------------------------------*/
/*-	        LINK.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_LINK			    1
#define  DLLAPP_LINK_SINGLE			2
#define  DLLAPP_LINK_ALL 			3
#define  DLLAPP_LINK_FILE			4
#define  DLLAPP_LINK_DEL 			5
#define  DLLAPP_LINK_NAME			6


/*-	�^�X�N�h�c			�@�@-*/
#define		TD_LINK		L"LINK"
#define		TD_SINGLE	L"LINK_SINGLE"
#define		TD_ALL		L"LINK_ALL"
#define		TD_FILE		L"LINK_FILE"
#define		TD_DEL		L"LINK_DEL"
#define		TD_NAME		L"LINK_NAME"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j	�@�@-*/
#define		LINK_ID			100
#define		SINGLE_ID		101
#define		ALL_ID			102
#define		FILE_ID			103
#define		DEL_ID			104
#define		NAME_ID			105


/*-	�_�C�A���O�{�b�N�X�h�c		�@�@-*/
#define		DIALOGID_LINK			200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_LINK_DIALOG		200


/*-	�e�L�X�g�h�c		�@�@-*/
#define		TEXTID_LAYER			300
#define		TEXTID_BAT				301
#define		TEXTID_DBSOURCE			302
#define		TEXTID_TABLE			304
#define		TEXTID_MSLINK			303


/*-	�v�b�V���{�^���h�c		�@�@-*/
#define		PUSHID_LINK_LAYER		400
#define		PUSHID_LINK_BAT			401
#define		PUSHID_LINK_OK			402
#define		PUSHID_LINK_END			403
#define		PUSHID_LINK_DEL			404

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_PUSH_LINK_LAYER	400
#define		HOOKID_PUSH_LINK_BAT	401
#define		HOOKID_PUSH_LINK_OK		402
#define		HOOKID_PUSH_LINK_END	403
#define		HOOKID_PUSH_LINK_DEL	404


/*-	�I�v�V�����{�^���h�c		�@�@-*/
#define		OPTIONID_KIND			500
#define		OPTIONID_KETA			501

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_OPTION_KIND		500


/*-	�g�O���{�^���h�c		�@�@-*/
#define		TOGGLEID_ZUMEN			600
#define		TOGGLEID_DEL_DB			601

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_TOGGLE_ZUMEN		600


typedef struct	LINKSET_INFO {
    int		nn;
    int		kind;
    ULong	mslink;
    WChar	layer[128];
    WChar	bat[128];
    WChar	dbsource[100];
    WChar	table[100];
    int		zumen;
    int		keta;
    int		tdel;
}   LINKSET;

