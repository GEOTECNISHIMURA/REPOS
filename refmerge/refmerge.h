/*-----------------------------------------------------------------------*/
/*-	        DELELE.H		ͯ�ް̧��			-*/
/*-----------------------------------------------------------------------*/

#ifndef     __deleleH__
#define     __deleleH__


#include <Mstn\MicroStation.r.h>	// WChar�p

// DLL��ID
#define  DLLAPP_REFMERGE			1

/*-	�^�X�N�h�c			�@�@-*/
#define		TD_REFMERGE		L"REFMERGE"

/*-	�_�C�A���O�{�b�N�X�t�b�N�֐��h�c�@�@-*/
#define		HOOKID_REFMERGE_DIALOG	200

/*-	�_�C�A���O�A�C�e���t�b�N�֐��h�c�@�@-*/
#define		HOOKID_PUSH_BAT		300
#define		HOOKID_PUSH_OK		301
#define		HOOKID_PUSH_END		302

#define		HOOKID_TOGGLE_BAT	320
#define		HOOKID_TOGGLE_MODE	321

/*-	�_�C�A���O�{�b�N�X�h�c		�@�@-*/
#define		DIALOGID_REFMERGE		500

/*-	�e�L�X�g�h�c		�@�@-*/
#define		TEXTID_BAT		600

/*-	�v�b�V���{�^���h�c		�@�@-*/
#define		PUSHID_BAT		700
#define		PUSHID_OK		701
#define		PUSHID_END		702

/*-	�I�v�V�����{�^���h�c		�@�@-*/
#define		TOGGLEID_BAT        800
#define		TOGGLEID_MODE		801

typedef struct	REFMERGE_INFO {
	    int tog_bat;
		int	mode;
		WChar	bat[256];
	}	REFMERGE;

#endif