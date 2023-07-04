/*-----------------------------------------------------------------------*/
/*-	        POLLINK.H		ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
// DLL��ID
#define  DLLAPP_POLLINK			1

/*-	�^�X�N�h�c			                -*/
#define		TD_POLLINK					    "POLLINK"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j	    -*/
#define		POLLINK_ID					    100


/*-	�_�C�A���O�{�b�N�X�h�c		    	-*/
#define		DIALOGID_POLLINK			    200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c    -*/
#define		HOOKID_POLLINK_DIALOG		    200


/*-	�v�b�V���{�^���h�c		    		-*/
#define		PUSHID_POLLINK_IDX			    300
#define		PUSHID_POLLINK_TBL			    302
#define		PUSHID_POLLINK_OK			    303
#define		PUSHID_POLLINK_SELECT			305
#define		PUSHID_POLLINK_END			    304

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_PUSH_POLLINK_IDX		    300
#define		HOOKID_PUSH_POLLINK_TBL		    302
#define		HOOKID_PUSH_POLLINK_OK		    303
#define		HOOKID_PUSH_POLLINK_SELECT	    305
#define		HOOKID_PUSH_POLLINK_END		    304


/*-	�e�L�X�g�h�c		        		-*/
#define		TEXTID_POLLINK_DBSOURCE	        402
#define		TEXTID_POLLINK_IDX	            400
#define		TEXTID_POLLINK_TBL	            401
#define		TEXTID_POLLINK_TOLERANCE	    403

#define		TEXTID_POLLINK_ERRLV	        410
#define		TEXTID_POLLINK_ERRCO	        411
#define		TEXTID_POLLINK_ERRST	        412
#define		TEXTID_POLLINK_ERRWT	        413


/*-	�g�O���{�^���h�c		        	-*/
#define		TOGGLEID_POLLINK_IDX		    500
#define		TOGGLEID_POLLINK_ALL		    501

/*-	�_�C�A���O�{�b�N�X�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_TOGGLE_POLLINK_IDX	    500
#define		HOOKID_TOGGLE_POLLINK_ALL	    501


/*-	���x���h�c		                	-*/
#define		LABELID_ERR					    600


typedef struct	POLLINK_INFO {
    WChar	dbsource[256];
    int		tog_idx, tog_all, tolerance;
    WChar	idxName[256];
    WChar	tblName[256];
	UInt32  err_lv, err_co, err_wt;
    int	    err_st;
}   POLLINK;

