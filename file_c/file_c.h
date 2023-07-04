/*-----------------------------------------------------------------------*/
/*-	        file_c.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_FILE_C			1

/*-	�^�X�N�h�c							-*/
#define		TD_FILE_C  	    "file_c"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j		-*/
#define		FILE_C_ID  	            100

/*-	�_�C�A���O�{�b�N�X�h�c				-*/
#define     DIALOGID_MAIN      	    200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	�v�b�V���{�^���h�c					-*/
#define     PUSHID_OK              	300
#define     PUSHID_END              301
#define     PUSHID_BAT              302
#define     PUSHID_CELL             303

/*-	�A�C�e���t�b�N�֐��h�c	-*/
#define     HOOKID_PUSH_OK		    300
#define     HOOKID_PUSH_END		    301
#define     HOOKID_PUSH_BAT	        302
#define     HOOKID_PUSH_CELL        303


/*-	�g�O���{�^���h�c					-*/
#define     TOGGLEID_REFFILE		401
#define     TOGGLEID_VIEWTOP 		402
#define     TOGGLEID_FILEDESIGN   	403
#define     TOGGLEID_TEXTNODES    	404
#define     TOGGLEID_LEVELS       	405
#define     TOGGLEID_CELL         	406

/*-	�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_TOGGLE_CELL		406


/*-	�e�L�X�g�A�C�e���h�c					-*/
#define     TEXTID_BAT		        501
#define     TEXTID_CELL 		    502


typedef struct FILE_C_INFO {
	int	    reffile;
	int	    viewtop;
	int	    textnodes;
	int	    levels;
	int	    tog_cell;
	WChar    cellib[512];
	int	    filedesign;
	WChar    idxname[512];
    } FILE_C;
