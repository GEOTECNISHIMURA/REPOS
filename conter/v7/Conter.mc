#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*----------------------------------------------------------------------+
|	conter.mc
+----------------------------------------------------------------------*/
#include <mdl.fdf>
#include <mdl.h>
#include <cexpr.h>
#include <global.h>
#include <mselems.h>
#include <dlogitem.h>
#include <dlogids.h>
#include <cmdlist.h>
#include <dlogman.fdf>
#include <mdlbspln.h>
#include <rscdefs.h>
#include <tcb.h>
#include <userfnc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <lok.mc>
#include <check_kigen7.mc>
#include "conter.h"

#define NEW			0
#define MODIFY		1
#define ONETANI		2

FILE			*fp;
DialogBox       *db_p;
MSElement		ele;
char			full[256];
int				kind;
int				exitStatus;
CONTER	    	con;

/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	option_mode_hook( DialogItemMessage *dimP);
Private void	push_incre_hook( DialogItemMessage *dimP);
Private void	push_decre_hook( DialogItemMessage *dimP);
Private void	toggle_bspln_hook( DialogItemMessage *dimP);
Private void	push_set_hook( DialogItemMessage *dimP);
Private void	push_klv_hook( DialogItemMessage *dimP);
Private void	push_slv_hook( DialogItemMessage *dimP);
Private void	push_otlv_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

static DialogHookInfo uHooks[] = 
{
    { HOOKID_CONTER_DIALOG,	 	  dialog_hook      },
    { HOOKID_OPTION_CONTER_MODE,  option_mode_hook },
    { HOOKID_PUSH_CONTER_INCRE,	  push_incre_hook  },
    { HOOKID_PUSH_CONTER_DECRE,	  push_decre_hook  },
    { HOOKID_TOGGLE_CONTER_BSPLN, toggle_bspln_hook},
    { HOOKID_PUSH_CONTER_SETZ,    push_set_hook    },
    { HOOKID_PUSH_CONTER_KLV,     push_klv_hook    },
    { HOOKID_PUSH_CONTER_SLV,     push_slv_hook    },
    { HOOKID_PUSH_CONTER_OTLV,    push_otlv_hook   },
    { HOOKID_PUSH_CONTER_OK,      push_ok_hook     },
    { HOOKID_PUSH_CONTER_END,     push_end_hook    },
};

/*-	�v���g�^�C�v	-*/
Private void	program_unload( void);
Private void    startPrimitive( void);
Private void    set_z( Dpoint3d	*inpoint);
Private void    elemType( void);
Private void    startModify( void);
Private void    get_level( void);


/*----------------------------------------------------------------------+
| name		main							|
+----------------------------------------------------------------------*/
main( )
{
char	     	*setP;
RscFileHandle   rscFileH;
int				ii;
#pragma			resourceID	CONTER_ID

#if LOCK == 1
main_lok( );
#endif
#if LOCK == 0
	chk_kigen( );
#endif

	/*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
    mdlResource_openFile( &rscFileH, NULL, 0);

    /*--- �t�b�N�֐��̎g�p�錾 ---*/
    mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

	/*--- C ����`�����ݒ� ---*/
    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
    mdlDialog_publishComplexVariable( setP, "CONTER_INFO", "con", &con);

   /*--- �ݒ� ---*/
   mdlFile_find( full, "conter.tbl", "MS_MDL", NULL);
   if ( ( fp = fopen( full,"r")) != NULL){
      fscanf( fp,"%d\n",  &con.mode);
      fscanf( fp,"%lf\n", &con.zz);
      fscanf( fp,"%lf\n", &con.pitch);
      fscanf( fp,"%d\n",  &con.bspln);
      fscanf( fp,"%lf\n", &con.genkou);
      fscanf( fp,"%d\n",  &con.klv);
      fscanf( fp,"%d\n",  &con.slv);
      fscanf( fp,"%d\n",  &con.otlv);
      fscanf( fp,"%d\n",  &con.hani);
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
   fclose( fp);

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

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�S�ďI�� 
-----------------------------------------------------------------------*/
Private	void	program_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_NEW) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_MODIF) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_ONETANI) == 0){ }
}


/*------------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
------------------------------------------------------------------------*/
Private void   dialog_hook( DialogMessage *dmP)
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
                                   mdlSystem_getCurrTaskID( ), TRUE);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*----------------------------------------------------------------------+
** 	�I�v�V�����{�^���i���e�j
+----------------------------------------------------------------------*/
Private void	option_mode_hook( DialogItemMessage *dimP)
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
Private void	push_incre_hook( DialogItemMessage *dimP)
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
Private void	push_decre_hook( DialogItemMessage *dimP)
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
Private void	toggle_bspln_hook( DialogItemMessage *dimP)
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
Private void	push_set_hook( DialogItemMessage *dimP)
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
Private void     startPrimitive( void)
{
   mdlState_startPrimitive( set_z, startPrimitive, NULL, NULL);
}


/*----------------------------------------------------------------------+
** 		�����̐ݒ�
+----------------------------------------------------------------------*/
Private void     set_z( Dpoint3d *inpoint)
{
   con.zz = inpoint->z/1000;
   mdlDialog_itemsSynch( db_p);
}


/*----------------------------------------------------------------------+
** 		�v�f�^�C�v�̐ݒ�
+----------------------------------------------------------------------*/
Private void     elemType( void)
{
static int searchType[] = { BSPLINE_CURVE_ELM, BSPLINE_KNOT_ELM, BSPLINE_POLE_ELM, BSPLINE_WEIGHT_ELM};

    mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);
}


/*----------------------------------------------------------------------+
** 	�v�b�V���{�^���i�v�Ȑݒ�j
+----------------------------------------------------------------------*/
Private void	push_klv_hook( DialogItemMessage *dimP)
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
Private void	push_slv_hook( DialogItemMessage *dimP)
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
Private void	push_otlv_hook( DialogItemMessage *dimP)
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
Private void    startModify( )
{
   mdlState_startModifyCommand( startModify, get_level, NULL, NULL, NULL, NULL, NULL, TRUE, 0);

   mdlLocate_init( );
   mdlLocate_allowLocked( );
}


/*-----------------------------------------------------------------------
**	���x���ݒ�
-----------------------------------------------------------------------*/
Private void	get_level( void)
{
ULong		filePos;
int			fileNum;

   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &fileNum);
   mdlElement_read( &ele, fileNum, filePos);

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
Private void	push_ok_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- �ݒ� ---*/
         fp = fopen( full, "w");
            fprintf( fp,"%d\n",  con.mode);
            fprintf( fp,"%lf\n", con.zz);
            fprintf( fp,"%lf\n", con.pitch);
            fprintf( fp,"%d\n",  con.bspln);
            fprintf( fp,"%lf\n", con.genkou);
            fprintf( fp,"%d\n",  con.klv);
            fprintf( fp,"%d\n",  con.slv);
            fprintf( fp,"%d\n",  con.otlv);
            fprintf( fp,"%d\n",  con.hani);
         fclose( fp);

		 /*--- �V�K ---*/
         if ( con.mode == NEW){
            mdlSystem_loadMdlProgram( TD_CONTER, TD_NEW, "");
         }

		 /*--- �C�� ---*/
         else if ( con.mode == MODIFY){
            mdlSystem_loadMdlProgram( TD_CONTER, TD_MODIF, "");
         }

		 /*--- �����E�J���� ---*/
         else{
            mdlSystem_loadMdlProgram( TD_CONTER, TD_ONETANI, "");
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
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );
         mdlSystem_exit( exitStatus, 1);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}
