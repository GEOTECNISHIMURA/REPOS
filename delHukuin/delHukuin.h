/*-----------------------------------------------------------------------*/
/*-	        DELHUKUIN.H		ͯ�ް̧��				-*/
/*-----------------------------------------------------------------------*/


#include <Mstn\MicroStation.r.h>


// DLL��ID
#define  DLLAPP_DELHUKUIN			1

/*-	�^�X�N�h�c							-*/
#define		TD_DELHUKUIN			    "DELHUKUIN"

/*-	�v���O�}�h�c�i���\�[�X�h�c�j		-*/
#define		DELHUKUIN_ID			    100


/*-	�_�C�A���O�{�b�N�X�h�c				-*/
#define		DIALOGID_DELHUKUIN	   	    200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c	-*/
#define		HOOKID_DELHUKUIN_DIALOG	    200


/*-	�v�b�V���{�^���h�c					-*/
#define		PUSHID_IDX				    304
#define		PUSHID_TBL				    303
#define		PUSHID_OK				    300
#define		PUSHID_END				    301

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_PUSH_IDX			    304
#define		HOOKID_PUSH_TBL			    303
#define		HOOKID_PUSH_OK			    300
#define		HOOKID_PUSH_END			    301


/*-	�g�O���{�^���h�c					-*/
#define		TOGGLEID_IDX				401

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_TOGGLE_IDX			401


/*-	�e�L�X�g�h�c						-*/
#define		TEXTID_IDX		            630

#define		TEXTID_HUKUIN_LV		    601
#define		TEXTID_HUKUIN_CO		    602
#define		TEXTID_HUKUIN_ST		    603
#define		TEXTID_HUKUIN_WT		    604

#define		TEXTID_KUKAN_LV		        611
#define		TEXTID_KUKAN_CO		        612
#define		TEXTID_KUKAN_ST		        613
#define		TEXTID_KUKAN_WT		        614

#define		TEXTID_KYOYOUTI		        621


/*-	���x���h�c		    				-*/
#define		LABELID_LAYER			    701

typedef struct	DELHUKUIN_INFO {
    int     tog_idx;
    int     hu_lv, hu_co, hu_st, hu_wt;
    int     ku_lv, ku_co, ku_st, ku_wt;
    double  kyoyouti;
    WChar    idxName[256];
	}	DELHUKUIN;
