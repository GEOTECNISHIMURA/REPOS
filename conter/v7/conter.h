/*----------------------------------------------------------------------+
|   $Workfile:   conter.h  $
+----------------------------------------------------------------------*/
#include <msdefs.h>
#include <dlogitem.h>


/*-	�^�X�N�h�c			�@�@-*/
#define		TD_CONTER		"CONTER"
#define		TD_NEW			"NEW"
#define		TD_MODIF		"MODIF"
#define		TD_ONETANI		"ONETANI"


/*-	�v���O�}�h�c�i���\�[�X�h�c�j	�@�@-*/
#define		CONTER_ID				100
#define		NEW_ID					101
#define		MODIF_ID				102
#define		ONETANI_ID				103


/*-	�_�C�A���O�{�b�N�X�h�c�@�@-*/
#define		DIALOGID_CONTER			200

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_CONTER_DIALOG	200


/*-	�v�b�V���{�^���A�C�e���h�c�@�@-*/
#define PUSHID_CONTER_INCRE    		301
#define PUSHID_CONTER_DECRE    		302
#define PUSHID_CONTER_KLV    		306
#define PUSHID_CONTER_SLV    		307
#define PUSHID_CONTER_OTLV    		308
#define PUSHID_CONTER_OK     		303
#define PUSHID_CONTER_SETZ   		304
#define PUSHID_CONTER_END     		305

/*-	�_�C�A���O�{�b�N�X�A�C�e���h�c�@�@-*/
#define HOOKID_PUSH_CONTER_INCRE    301
#define HOOKID_PUSH_CONTER_DECRE    302
#define HOOKID_PUSH_CONTER_KLV      306
#define HOOKID_PUSH_CONTER_SLV      307
#define HOOKID_PUSH_CONTER_OTLV     308
#define HOOKID_PUSH_CONTER_OK     	303
#define HOOKID_PUSH_CONTER_SETZ     304
#define HOOKID_PUSH_CONTER_END     	305


/*-	�e�L�X�g�A�C�e���h�c�@�@-*/
#define TEXTID_CONTER_ZZ    		400
#define TEXTID_CONTER_PITCH    		401
#define TEXTID_CONTER_GENKOU    	402
#define TEXTID_CONTER_KLV    		403
#define TEXTID_CONTER_SLV    		404
#define TEXTID_CONTER_OTLV    		405
#define TEXTID_CONTER_HANI    		406


/*-	�I�v�V�����{�^���A�C�e���h�c�@�@-*/
#define OPTIONID_CONTER_MODE   		500

/*-	�_�C�A���O�{�b�N�X�A�C�e���h�c�@�@-*/
#define HOOKID_OPTION_CONTER_MODE   500


/*-	�g�O���{�^���A�C�e���h�c�@�@-*/
#define TOGGLEID_CONTER_BSPLN   	600

/*-	�_�C�A���O�{�b�N�X�A�C�e���h�c�@�@-*/
#define HOOKID_TOGGLE_CONTER_BSPLN  600


typedef struct CONTER_INFO
{
   int		mode;	/* �V�K/�C�� */
   double	zz;		/* ����		 */
   double	pitch;	/* �Ԋu		 */
   int		bspln;	/* B-���ײ�	 */
   double	genkou;	/* ����		 */
   int		klv;	/* �v��		 */
   int		slv;	/* ���		 */
   int		otlv;	/* �����E�J	 */
   int		hani;	/* �͈�		 */
} CONTER;
#endif
