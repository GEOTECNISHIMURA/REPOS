/*-----------------------------------------------------------------------*/
/*-	        DELPOINTNEW.H			ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>


// DLL��ID
#define  DLLAPP_DELPOINTNEW			1


/*-	�^�X�N�h�c							-*/
#define		TD_DELPOINTNEW  	    "DELPOINTNEW"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j		-*/
#define		DELPOINTNEW_ID  	    100


/*-	�_�C�A���O�{�b�N�X�h�c				-*/
#define		DIALOGID_MAIN			200


/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	�v�b�V���{�^���h�c					-*/
#define		PUSHID_TABLE			310
#define		PUSHID_TABLE_KUKAN		311
#define		PUSHID_OK				300
#define		PUSHID_KUKAN			302
#define		PUSHID_END			    301


/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c	-*/
#define		HOOKID_PUSH_TABLE		310
#define		HOOKID_PUSH_TABLE_KUKAN	311
#define		HOOKID_PUSH_OK	        300
#define		HOOKID_PUSH_KUKAN       302
#define		HOOKID_PUSH_END	        301


/*-	�e�L�X�g�h�c					-*/
#define		TEXTID_TABLE			401
#define		TEXTID_TABLE_KUKAN		403
#define		TEXTID_RR			    402

typedef struct	DELPOINTNEW_INFO {
	WChar	tblName[512];
	WChar	kukanName[512];
    double  rr;
}	DELPOINTNEW;

#define  RSCID_DataDef_Link  	1

typedef struct LINK_INFO{
    char	rosen[6];	/* �H���ԍ�	*/
    int		kukan;		/* ��Ԕԍ�	*/
    int		lleng;		/* �������� */
    int		rleng;		/* �E������	*/
    long	huku;		/* ����		*/
    long	hoka;		/* ���̑�����	*/
    long	inout;		/* ���a�������ɕt���Ă��邩�A�ԓ��ɕt���Ă��邩	*/
} LINKSET;

struct LinkageHeader_a
{
	UInt16      wdMantissa : 8;           // mantissa words: wtf if wdExponent=0 
	UInt16      wdExponent : 4;           // exponent for words multiplier       
	UInt16      user : 1;                 // boolean: user data linkage          
	UInt16      modified : 1;             // boolean: user linkage modified      
	UInt16      remote : 1;               // boolean: remote linkage             
	UInt16      info : 1;                 // boolean: informational linkage      
	UInt16      primaryID;              // User ID number                      
};


typedef struct linkdata{
    LinkageHeader_a  linkHdr;
    LINKSET 	   linkage;
} LinkData;
