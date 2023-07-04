#define		VER		7
/*-----------------------------------------------------------------------*/
/*-	        TXCELL.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>	// WChar�p

// DLL��ID
#define  DLLAPP_TXCELL			    1
#define  DLLAPP_TXCELL_TXCE			2
#define  DLLAPP_TXCELL_FILE			3

/*-	�^�X�N�h�c			�@�@-*/
#define		TD_TXCELL		L"TXCELL"
#define		TD_TXCE			L"TXCE"
#define		TD_FILE			L"FILE"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j	�@�@-*/
#define		TXCELL_ID			100
#define		TXCE_ID				101
#define		FILE_ID				103


/*-	�_�C�A���O�{�b�N�X�h�c		�@�@-*/
#define		DIALOGID_TXCELL			200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_TXCELL_DIALOG	200


/*-	�e�L�X�g�h�c		�@�@-*/
#define		TEXTID_IDX			600
#define		TEXTID_SEED			608
#define		TEXTID_FILE			601
#define		TEXTID_FONT			602
#define		TEXTID_CELLIB		609
#define		TEXTID_CELL			603
#define		TEXTID_LV			604
#define		TEXTID_CO			605
#define		TEXTID_LC			606
#define		TEXTID_WT			607


/*-	�v�b�V���{�^���h�c		�@�@-*/
#define		PUSHID_IDX			303
#define		PUSHID_SEED			304
#define		PUSHID_FILE			300
#define		PUSHID_CELLIB		305
#define		PUSHID_OK			301
#define		PUSHID_END			302

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_PUSH_IDX		303
#define		HOOKID_PUSH_SEED	304
#define		HOOKID_PUSH_FILE	300
#define		HOOKID_PUSH_CELLIB	305
#define		HOOKID_PUSH_OK		301
#define		HOOKID_PUSH_END		302


/*-	�I�v�V�����{�^���h�c		�@�@-*/
#define		OPTIONID_FORMAT			400
#define		OPTIONID_PNAME			403
#define		OPTIONID_KUGIRI			404
#define		OPTIONID_SCALE			401
#define		OPTIONID_TXPOS			402

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_OPTION_FORMAT	400
#define		HOOKID_OPTION_PNAME		401


/*-	�g�O���{�^���h�c		�@�@-*/
#define		TOGGLEID_IDX			500

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_TOGGLE_IDX		500


typedef struct	TXSET_INFO{
   int		nn;
   int		tog_idx;
   WChar		idx[256];
   WChar		seed[256];
   int		form;
   int		pname;
   int		kugiri;
   int		txpos;
   int		font;
   int		scale;
   WChar	    cellib[256];
   WChar	    cell[10];
#if VER == 8
    UInt	lv, co, wt;
#else
   UInt32	lv, co, wt;
#endif
    int		lc;
    WChar	file[128];
}	TXSET;

