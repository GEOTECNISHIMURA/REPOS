#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*----------------------------------------------------------------------+
|	conter.mc
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
//#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "conter.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

#define NEW			0
#define MODIFY		1
#define ONETANI		2

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
MSElement		ele;
int				kind;
int				exitStatus;
CONTER	    	con;

/*-	�t�b�N�֐�		-*/
void	dialog_hook( DialogMessage *dmP);
void	option_mode_hook( DialogItemMessage *dimP);
void	push_incre_hook( DialogItemMessage *dimP);
void	push_decre_hook( DialogItemMessage *dimP);
void	toggle_bspln_hook( DialogItemMessage *dimP);
void	push_set_hook( DialogItemMessage *dimP);
void	push_klv_hook( DialogItemMessage *dimP);
void	push_slv_hook( DialogItemMessage *dimP);
void	push_otlv_hook( DialogItemMessage *dimP);
void	push_ok_hook( DialogItemMessage *dimP);
void	push_end_hook( DialogItemMessage *dimP);

static DialogHookInfo uHooks[] = 
{
    { HOOKID_CONTER_DIALOG,	 	  (PFDialogHook)dialog_hook      },
    { HOOKID_OPTION_CONTER_MODE,  (PFDialogHook)option_mode_hook },
    { HOOKID_PUSH_CONTER_INCRE,	  (PFDialogHook)push_incre_hook  },
    { HOOKID_PUSH_CONTER_DECRE,	  (PFDialogHook)push_decre_hook  },
    { HOOKID_TOGGLE_CONTER_BSPLN, (PFDialogHook)toggle_bspln_hook},
    { HOOKID_PUSH_CONTER_SETZ,    (PFDialogHook)push_set_hook    },
    { HOOKID_PUSH_CONTER_KLV,     (PFDialogHook)push_klv_hook    },
    { HOOKID_PUSH_CONTER_SLV,     (PFDialogHook)push_slv_hook    },
    { HOOKID_PUSH_CONTER_OTLV,    (PFDialogHook)push_otlv_hook   },
    { HOOKID_PUSH_CONTER_OK,      (PFDialogHook)push_ok_hook     },
    { HOOKID_PUSH_CONTER_END,     (PFDialogHook)push_end_hook    },
};

/*-	�v���g�^�C�v	-*/
void	program_unload( void);
void    startPrimitive( void);
void    set_z( Dpoint3d	*inpoint);
void    elemType( void);
void    startModify( void);
void    get_level( void);


/*----------------------------------------------------------------------+
| name		main							|
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
char            structName[128];
char            variableName[128];
SymbolSet		*setP;
BeFileName		full;
int             ii;

chk_kigen();


	/*--------- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"CONTER�̃��\�[�X�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
	}

    /*--- �t�b�N�֐��̎g�p�錾 ---*/
    mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

	/*--- C ����`�����ݒ� ---*/
	sprintf(structName, "CONTER_INFO");
	sprintf(variableName, "con");
    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName,  &con);

   /*--- �ݒ� ---*/
	mdlFile_find(&full, L"conter.tbl", L"MS_DEF", NULL);
	if ((fp = _wfopen(full.GetWCharCP(), L"r")) != NULL) {
      fwscanf( fp,L"%d\n",  &con.mode);
      fwscanf( fp,L"%lf\n", &con.zz);
      fwscanf( fp,L"%lf\n", &con.pitch);
      fwscanf( fp,L"%d\n",  &con.bspln);
      fwscanf( fp,L"%lf\n", &con.genkou);
      fwscanf( fp,L"%d\n",  &con.klv);
      fwscanf( fp,L"%d\n",  &con.slv);
      fwscanf( fp,L"%d\n",  &con.otlv);
      fwscanf( fp,L"%d\n",  &con.hani);
	  fclose(fp);
   }
   else{
      con.mode   = 0;
      con.zz     = 0;
      con.pitch  = 1;
      con.bspln  = -1;
      con.genkou = 0.1;
      con.klv    = 41;
      con.slv    = 43;
      con.otlv    = 43;
      con.hani   = 10;
   }
   
   /*--- �_�C�A���O�{�b�N�X�I�[�v�� ---*/
   db_p = mdlDialog_open( NULL, DIALOGID_CONTER);

   /*--- �V�K�̏ꍇ ---*/
   if ( con.mode == 0){
      for( ii=7; ii<=13; ii++){
         mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
      }
   }

   /*--- �C���̏ꍇ ---*/
   if ( con.mode == 1){
      for( ii=1; ii<=4; ii++){
         mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
      }
      for( ii=7; ii<=13; ii++){
         mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
      }
      mdlDialog_itemSetEnabledState( db_p, 15, FALSE, FALSE);
   }

   /*--- �����E�J�̏ꍇ ---*/
   if ( con.mode == 2){
      for( ii=1; ii<=6; ii++){
         mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
      }
      mdlDialog_itemSetEnabledState( db_p, 15, FALSE, FALSE);
   }

   /*--- ���C���̏ꍇ ---*/
   if ( con.bspln != -1){
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
   }

   return;
}


/*-----------------------------------------------------------------------
**	�S�ďI�� 
-----------------------------------------------------------------------*/
	void	program_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_NEW) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_MODIF) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_ONETANI) == 0){ }
}


/*------------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
------------------------------------------------------------------------*/
void   dialog_hook( DialogMessage *dmP)
{
DialogBoxRsc	*dbr_P;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DIALOG_MESSAGE_CREATE:
         dbr_P = dmP->u.create.dialogBoxRP;
         dmP->u.create.interests.updates	= TRUE;
         dmP->u.create.interests.mouses		= TRUE;
         dmP->u.create.interests.dialogFocuses	= TRUE;
         dmP->u.create.interests.itemFocuses	= TRUE;
         break;

      case DIALOG_MESSAGE_DESTROY:
         program_unload( );
	     mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*----------------------------------------------------------------------+
** 	�I�v�V�����{�^���i���e�j
+----------------------------------------------------------------------*/
void	option_mode_hook( DialogItemMessage *dimP)
{
int		ii;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- �V�K�̏ꍇ ---*/
         if ( con.mode == 0){
            for( ii=1; ii<=5; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, TRUE,  FALSE);
            }
            if ( con.bspln == -1){
               mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            }
            for( ii=7; ii<=13; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, FALSE,  FALSE);
            }
            mdlDialog_itemSetEnabledState( db_p, 15, TRUE,  FALSE);
         }

         /*--- �C���̏ꍇ ---*/
         if ( con.mode == 1){
            for( ii=1; ii<=4; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
            }
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            if ( con.bspln == -1){
               mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            }
            for( ii=7; ii<=13; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, FALSE,  FALSE);
            }
            mdlDialog_itemSetEnabledState( db_p, 15, FALSE, FALSE);
         }

         /*--- �����E�J�̏ꍇ ---*/
         if ( con.mode == 2){
            for( ii=1; ii<=6; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
            }
            for( ii=7; ii<=13; ii++){
               mdlDialog_itemSetEnabledState( db_p, ii, TRUE,  FALSE);
            }
            mdlDialog_itemSetEnabledState( db_p, 15, FALSE,  FALSE);
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�{�j
+----------------------------------------------------------------------*/
void	push_incre_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         con.zz = con.zz + con.pitch;
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�|�j
+----------------------------------------------------------------------*/
void	push_decre_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         con.zz = con.zz - con.pitch;
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�g�O���{�^���iB-�X�v���C���j
+----------------------------------------------------------------------*/
void	toggle_bspln_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- B-�X�v���C���̏ꍇ ---*/
         if ( con.bspln == -1){
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE, FALSE);
         }

         /*--- ���C���̏ꍇ ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�����ݒ�j
+----------------------------------------------------------------------*/
void	push_set_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         startPrimitive( );
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 		startPrimitive						|
+----------------------------------------------------------------------*/
void     startPrimitive( void)
{
   mdlState_startPrimitive( (StateFunc_DataPoint)set_z, (StateFunc_Reset)startPrimitive, NULL, NULL);
}


/*----------------------------------------------------------------------+
** 		�����̐ݒ�
+----------------------------------------------------------------------*/
void     set_z( Dpoint3d *inpoint)
{
   con.zz = inpoint->z/1000;
   mdlDialog_itemsSynch( db_p);
}


/*----------------------------------------------------------------------+
** 		�v�f�^�C�v�̐ݒ�
+----------------------------------------------------------------------*/
void     elemType( void)
{
static int searchType[] = { BSPLINE_CURVE_ELM, BSPLINE_KNOT_ELM, BSPLINE_POLE_ELM, BSPLINE_WEIGHT_ELM};

    mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�v�Ȑݒ�j
+----------------------------------------------------------------------*/
void	push_klv_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         kind = 1;
         startModify( );
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i��Ȑݒ�j
+----------------------------------------------------------------------*/
void	push_slv_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         kind = 2;
         startModify( );
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�����E�J�ݒ�j
+----------------------------------------------------------------------*/
void	push_otlv_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         kind = 3;
         startModify( );
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	startModify
-----------------------------------------------------------------------*/
void    startModify( )
{
   mdlState_startModifyCommand( (StateFunc_Reset)startModify, (MdlFunc_Accept)get_level, NULL, NULL, NULL, NULL, NULL, TRUE, 0);

   mdlLocate_init( );
   mdlLocate_allowLocked( );
}


/*-----------------------------------------------------------------------
**	���x���ݒ�
-----------------------------------------------------------------------*/
void	get_level( void)
{
ULong		filePos;
DgnModelRefP modelRefP;

   filePos = mdlElement_getFilePos( FILEPOS_CURRENT,  &modelRefP);
   mdlElement_read( &ele, modelRefP, filePos);

   /*--- �v�� ---*/
   if ( kind == 1){
      mdlElement_getProperties( &con.klv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
   }

   /*--- ��� ---*/
   if ( kind == 2){
      mdlElement_getProperties( &con.slv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
   }

   /*--- �����E�J ---*/
   if ( kind == 3){
      mdlElement_getProperties( &con.otlv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
   }
   mdlDialog_itemsSynch( db_p);

   startModify( );
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i���s�j
+----------------------------------------------------------------------*/
void	push_ok_hook( DialogItemMessage *dimP)
{
BeFileName		full;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- �ݒ� ---*/
		 mdlFile_find(&full, L"conter.tbl", L"MS_DEF", NULL);
		 fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp,L"%d\n",  con.mode);
            fwprintf( fp,L"%lf\n", con.zz);
            fwprintf( fp,L"%lf\n", con.pitch);
            fwprintf( fp,L"%d\n",  con.bspln);
            fwprintf( fp,L"%lf\n", con.genkou);
            fwprintf( fp,L"%d\n",  con.klv);
            fwprintf( fp,L"%d\n",  con.slv);
            fwprintf( fp,L"%d\n",  con.otlv);
            fwprintf( fp,L"%d\n",  con.hani);
         fclose( fp);

		 /*--- �V�K ---*/
         if ( con.mode == NEW){
            mdlSystem_loadMdlProgram( TD_CONTER, TD_NEW, L"");
         }

		 /*--- �C�� ---*/
         else if ( con.mode == MODIFY){
            mdlSystem_loadMdlProgram( TD_CONTER, TD_MODIF, L"");
         }

		 /*--- �����E�J���� ---*/
         else{
            mdlSystem_loadMdlProgram( TD_CONTER, TD_ONETANI, L"");
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�I���j
+----------------------------------------------------------------------*/
void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );
         mdlSystem_exit( exitStatus, 1);
		 return;
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}
