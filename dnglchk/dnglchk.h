/*-----------------------------------------------------------------------*/
/*-         DNGLCHK.H           ͯ�ް̧��                               -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

// DLL��ID
#define  DLLAPP_DNGLCHK			    1
#define  DLLAPP_DNGLCHK_FILE	    2
#define  DLLAPP_DNGLCHK_CHK			3
#define  DLLAPP_DNGLCHK_SINGLE		4


/*- �^�X�N�h�c                      �@�@-*/
#define     TD_DNGLCHK          L"DNGLCHK"
#define     TD_FILE             L"DNGLCHK_FILE"
#define     TD_CHK              L"DNGLCHK_CHK"
#define     TD_SINGLE           L"DNGLCHK_SINGLE"


/*- �v���O�}�h�c�i���\�[�X�h�c�j    �@�@-*/
#define     DNGLCHK_ID              100
#define     FILE_ID                 101
#define     CHK_ID                  102
#define     SINGLE_ID               103


/*- �_�C�A���O�{�b�N�X�h�c          �@�@-*/
#define     DIALOGID_DNGLCHK        200

/*- �_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define     HOOKID_DNGLCHK_DIALOG   200


/*- �v�b�V���{�^���h�c              �@�@-*/
#define     PUSHID_OK               400
#define     PUSHID_END              401
#define     PUSHID_TABLE            402

/*- �_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define     HOOKID_PUSH_OK          400
#define     HOOKID_PUSH_END         401
#define     HOOKID_PUSH_TABLE       402


/*- �v�b�V���{�^���h�c              �@�@-*/
#define     OPTIONID_KIND           700


/*- �e�L�X�g�A�C�e���h�c            �@�@-*/
#define     TEXTID_TABLE            501
#define     TEXTID_SIZE             502
#define     TEXTID_ANGLE            503
#define     TEXTID_LV1              510
#define     TEXTID_CO1              511
#define     TEXTID_LC1              512
#define     TEXTID_WT1              513
#define     TEXTID_LV2              514
#define     TEXTID_CO2              515
#define     TEXTID_LC2              516
#define     TEXTID_WT2              517

/*- ���x���A�C�e���h�c              �@�@-*/
#define     LABELID_CHK             600

typedef struct  DNGLCHK_INFO {
    int         lv1, co1, lc1, wt1;
    int         lv2, co2, lc2, wt2;
    double      size;
    double      angle;
    WChar        table[128];
    int         kind;
}   DNGLCHK;
