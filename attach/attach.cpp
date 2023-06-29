/*-----------------------------------------------------------------------*/
/*-	        ���C��														-*/
/*-	        ATTACH8.MC													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <string.h>
#include <stdio.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "attach.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE			*fp, *fp_log;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;
ATTACH8 		attach8;

#ifndef Private
#define Private
#endif


/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	option_mode_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	toggle_ref1_hook( DialogItemMessage *dimP);
Private void	toggle_ref2_hook( DialogItemMessage *dimP);
Private void	toggle_ref3_hook( DialogItemMessage *dimP);
Private void	toggle_ref4_hook( DialogItemMessage *dimP);
Private void	toggle_ref5_hook( DialogItemMessage *dimP);
Private void	toggle_ref6_hook( DialogItemMessage *dimP);
Private void	toggle_ref7_hook( DialogItemMessage *dimP);
Private void	toggle_ref8_hook( DialogItemMessage *dimP);
Private void	toggle_ref9_hook( DialogItemMessage *dimP);
Private void	toggle_ref10_hook( DialogItemMessage *dimP);
Private void	push_ref1_hook( DialogItemMessage *dimP);
Private void	push_ref2_hook( DialogItemMessage *dimP);
Private void	push_ref3_hook( DialogItemMessage *dimP);
Private void	push_ref4_hook( DialogItemMessage *dimP);
Private void	push_ref5_hook( DialogItemMessage *dimP);
Private void	push_ref6_hook( DialogItemMessage *dimP);
Private void	push_ref7_hook( DialogItemMessage *dimP);
Private void	push_ref8_hook( DialogItemMessage *dimP);
Private void	push_ref9_hook( DialogItemMessage *dimP);
Private void	push_ref10_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_ATTACH8_DIALOG, (PFDialogHook)dialog_hook      },
	{ HOOKID_OPTION_MODE,    (PFDialogHook)option_mode_hook },
	{ HOOKID_PUSH_IDX,       (PFDialogHook)push_idx_hook    },
	{ HOOKID_TOGGLE_REF1,    (PFDialogHook)toggle_ref1_hook },
	{ HOOKID_TOGGLE_REF2,    (PFDialogHook)toggle_ref2_hook },
	{ HOOKID_TOGGLE_REF3,    (PFDialogHook)toggle_ref3_hook },
	{ HOOKID_TOGGLE_REF4,    (PFDialogHook)toggle_ref4_hook },
	{ HOOKID_TOGGLE_REF5,    (PFDialogHook)toggle_ref5_hook },
	{ HOOKID_TOGGLE_REF6,    (PFDialogHook)toggle_ref6_hook },
	{ HOOKID_TOGGLE_REF7,    (PFDialogHook)toggle_ref7_hook },
	{ HOOKID_TOGGLE_REF8,    (PFDialogHook)toggle_ref8_hook },
	{ HOOKID_TOGGLE_REF9,    (PFDialogHook)toggle_ref9_hook },
	{ HOOKID_TOGGLE_REF10,   (PFDialogHook)toggle_ref10_hook},
	{ HOOKID_PUSH_REF1,      (PFDialogHook)push_ref1_hook   },
	{ HOOKID_PUSH_REF2,      (PFDialogHook)push_ref2_hook   },
	{ HOOKID_PUSH_REF3,      (PFDialogHook)push_ref3_hook   },
	{ HOOKID_PUSH_REF4,      (PFDialogHook)push_ref4_hook   },
	{ HOOKID_PUSH_REF5,      (PFDialogHook)push_ref5_hook   },
	{ HOOKID_PUSH_REF6,      (PFDialogHook)push_ref6_hook   },
	{ HOOKID_PUSH_REF7,      (PFDialogHook)push_ref7_hook   },
	{ HOOKID_PUSH_REF8,      (PFDialogHook)push_ref8_hook   },
	{ HOOKID_PUSH_REF9,      (PFDialogHook)push_ref9_hook   },
	{ HOOKID_PUSH_REF10,     (PFDialogHook)push_ref10_hook  },
	{ HOOKID_PUSH_OK,        (PFDialogHook)push_ok_hook     },
	{ HOOKID_PUSH_END,       (PFDialogHook)push_end_hook    },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int     syori( void);
Private	int		ref_attach( void);
Private	void	toggle_change( int ii);
Private	void	get_refFile( int ii);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

int			ii;
char            structName[128];
char            variableName[128];
SymbolSet		*setP;
BeFileName		full;
   chk_kigen( );

   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"ATTACH8 �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "ATTACH8_INFO");
   sprintf(variableName, "attach8");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable( setP, structName, variableName, &attach8);

   /*--- �e�[�u���t�@�C���I�[�v�� -----*/
   mdlFile_find( &full, L"attach8.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &attach8.mode);
      fwscanf( fp, L"%d\n", &attach8.tog_detach);
      fwscanf( fp, L"%ls\n", attach8.idxName);
      fwscanf( fp, L"%d\n", &attach8.tog[0]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[0]);
      fwscanf( fp, L"%ls\n", attach8.refName[0]);
      fwscanf( fp, L"%d\n", &attach8.tog[1]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[1]);
      fwscanf( fp, L"%ls\n", attach8.refName[1]);
      fwscanf( fp, L"%d\n", &attach8.tog[2]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[2]);
      fwscanf( fp, L"%ls\n", attach8.refName[2]);
      fwscanf( fp, L"%d\n", &attach8.tog[3]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[3]);
      fwscanf( fp, L"%ls\n", attach8.refName[3]);
      fwscanf( fp, L"%d\n", &attach8.tog[4]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[4]);
      fwscanf( fp, L"%ls\n", attach8.refName[4]);
      fwscanf( fp, L"%d\n", &attach8.tog[5]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[5]);
      fwscanf( fp, L"%ls\n", attach8.refName[5]);
      fwscanf( fp, L"%d\n", &attach8.tog[6]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[6]);
      fwscanf( fp, L"%ls\n", attach8.refName[6]);
      fwscanf( fp, L"%d\n", &attach8.tog[7]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[7]);
      fwscanf( fp, L"%ls\n", attach8.refName[7]);
      fwscanf( fp, L"%d\n", &attach8.tog[8]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[8]);
      fwscanf( fp, L"%ls\n", attach8.refName[8]);
      fwscanf( fp, L"%d\n", &attach8.tog[9]);
      fwscanf( fp, L"%d\n", &attach8.tog_kotei[9]);
      fwscanf( fp, L"%ls\n", attach8.refName[9]);
	  fclose(fp);
   }
   else{
      attach8.mode          = 0;
      attach8.tog_detach    = 0;
	  swprintf(attach8.idxName, L"c:\\\0");
      attach8.tog[0]        = 0;
      attach8.tog_kotei[0]  = 0;
	  swprintf(attach8.refName[0], L"c:\\\0");
      attach8.tog[1]        = 0;
      attach8.tog_kotei[1]  = 0;
	  swprintf(attach8.refName[1], L"c:\\\0");
      attach8.tog[2]        = 0;
      attach8.tog_kotei[2]  = 0;
	  swprintf(attach8.refName[2], L"c:\\\0");
      attach8.tog[3]        = 0;
      attach8.tog_kotei[3]  = 0;
	  swprintf(attach8.refName[3], L"c:\\\0");
      attach8.tog[4]        = 0;
      attach8.tog_kotei[4]  = 0;
	  swprintf(attach8.refName[4], L"c:\\\0");
      attach8.tog[5]        = 0;
      attach8.tog_kotei[5]  = 0;
	  swprintf(attach8.refName[5], L"c:\\\0");
      attach8.tog[6]        = 0;
      attach8.tog_kotei[6]  = 0;
	  swprintf(attach8.refName[6], L"c:\\\0");
      attach8.tog[7]        = 0;
      attach8.tog_kotei[7]  = 0;
	  swprintf(attach8.refName[7], L"c:\\\0");
      attach8.tog[8]        = 0;
      attach8.tog_kotei[8]  = 0;
	  swprintf(attach8.refName[8], L"c:\\\0");
      attach8.tog[9]        = 0;
      attach8.tog_kotei[9]  = 0;
	  swprintf(attach8.refName[9], L"c:\\\0");
   }


   /*--- �_�C�A���O�{�b�N�X ------------*/
   db_p = mdlDialog_open( NULL, DIALOGID_ATTACH8);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"ATTACH8 �_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �P�t�@�C���̏ꍇ ------------*/
   if ( attach8.mode == 0){
      mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
   }

   for( ii=0; ii<10; ii++){
      toggle_change( ii);
   }

}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
-----------------------------------------------------------------------*/
Private	void	dialog_hook( DialogMessage *dmP)
{
DialogBoxRsc	*dbr_P;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DIALOG_MESSAGE_CREATE:
	     dbr_P = dmP->u.create.dialogBoxRP;
	     dmP->u.create.interests.updates       = TRUE;
	     dmP->u.create.interests.mouses        = TRUE;
	     dmP->u.create.interests.dialogFocuses = TRUE;
	     dmP->u.create.interests.itemFocuses   = TRUE;
         break;

      case DIALOG_MESSAGE_DESTROY:
	     mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^��(�����j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	option_mode_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- �P�t�@�C���̏ꍇ -----*/
         if ( attach8.mode == 0){
            mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
         }

         /*--- �o�b�`�̏ꍇ -----*/
         if ( attach8.mode == 1){
            mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
         }
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(IDX�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title,  L"�o�b�`�t�@�C����I��");

         mdlFile_parseName( attach8.idxName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

         mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(attach8.idxName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��01�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref1_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 0);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��02�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref2_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 1);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��03�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref3_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 2);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��04�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref4_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 3);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��05�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref5_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 4);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��06�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref6_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 5);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��07�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref7_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 6);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��08�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref8_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 7);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��09�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref9_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 8);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^��(�Q��10�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_ref10_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         toggle_change( 9);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��01�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref1_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 0);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��02�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref2_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 1);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��03�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref3_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 2);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��04�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref4_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 3);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��05�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref5_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 4);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��06�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref6_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 5);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��07�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref7_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 6);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��08�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref8_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 7);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��09�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref9_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 8);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�Q��10�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ref10_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         get_refFile( 9);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^�� �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_ok_hook( DialogItemMessage *dimP)
{
   BeFileName		full;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �e�[�u���t�@�C���I�[�v�� -----*/
         mdlFile_find( &full, L"attach8.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%d\n", attach8.mode);
            fwprintf( fp, L"%d\n", attach8.tog_detach);
            fwprintf( fp, L"%ls\n", attach8.idxName);
            fwprintf( fp, L"%d\n", attach8.tog[0]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[0]);
            fwprintf( fp, L"%ls\n", attach8.refName[0]);
            fwprintf( fp, L"%d\n", attach8.tog[1]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[1]);
            fwprintf( fp, L"%ls\n", attach8.refName[1]);
            fwprintf( fp, L"%d\n", attach8.tog[2]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[2]);
            fwprintf( fp, L"%ls\n", attach8.refName[2]);
            fwprintf( fp, L"%d\n", attach8.tog[3]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[3]);
            fwprintf( fp, L"%ls\n", attach8.refName[3]);
            fwprintf( fp, L"%d\n", attach8.tog[4]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[4]);
            fwprintf( fp, L"%ls\n", attach8.refName[4]);
            fwprintf( fp, L"%d\n", attach8.tog[5]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[5]);
            fwprintf( fp, L"%ls\n", attach8.refName[5]);
            fwprintf( fp, L"%d\n", attach8.tog[6]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[6]);
            fwprintf( fp, L"%ls\n", attach8.refName[6]);
            fwprintf( fp, L"%d\n", attach8.tog[7]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[7]);
            fwprintf( fp, L"%ls\n", attach8.refName[7]);
            fwprintf( fp, L"%d\n", attach8.tog[8]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[8]);
            fwprintf( fp, L"%ls\n", attach8.refName[8]);
            fwprintf( fp, L"%d\n", attach8.tog[9]);
            fwprintf( fp, L"%d\n", attach8.tog_kotei[9]);
            fwprintf( fp, L"%ls\n", attach8.refName[9]);
         fclose( fp);

         syori( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  int   syori( )
{
WChar	info[256], dgnName[512];

   /*--- �P�t�@�C���̏ꍇ ---*/
   if ( attach8.mode == 0){
      ref_attach( );
   }

   /*--- �o�b�`�̏ꍇ ---*/
   else{
      /*----- ���̓t�@�C�����X�g���J�� -----*/
      if ( NULL == ( fp = _wfopen( attach8.idxName, L"r"))){
         swprintf( info, L"idx�t�@�C�� %ls ���J���܂���", attach8.idxName);
         mdlDialog_openInfoBox( info);
         return( ERROR);
      }

      /*----- ���̓t�@�C�����X�g��ǂ� -----*/
      while( 1){
	     fwscanf( fp, L"%ls", dgnName);
         if ( feof( fp) || ferror( fp)){ break;}

         if ( 0 == wcsncmp( dgnName, L";", 1)){ continue;}

         /*----- dgn���J�� -----*/
         if ( mdlSystem_newDesignFile( dgnName) == SUCCESS){
            ref_attach( );
         }
      }
      fclose( fp);
   }
   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�Q�ƃt�@�C���A�^�b�`
------------------------------------------------------------------------*/
Private  int   ref_attach( )
{
DgnModelRefP	model;
WChar			*p, logFile[512], info[256];
WChar			devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH], nameP[MAXNAMELENGTH], filename[512];
Dpoint3d		orig;
short			levels[8][4];
int				ii;

   orig.x = orig.y = orig.z = 0;
   for( ii=0; ii<8; ii++){
      levels[ii][0] = 0xFFFF;
      levels[ii][1] = 0xFFFF;
      levels[ii][2] = 0xFFFF;
      levels[ii][3] = 0x7FFF;
   }

   /*----- �t�@�C�������琔���̕����𔲂��o�� -----*/
   mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
   p = wcspbrk( nameP, L"0123456789");

   /*----- log�t�@�C���쐬 -----*/
   mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
   swprintf( logFile, L"%ls:%ls%ls.log", devP, dirP, nameP);
   if ( ( fp_log = _wfopen( logFile, L"w")) == NULL){
      swprintf( info, L"log�t�@�C�� %ls �ɏo�͂ł��܂���", logFile);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }
   fwprintf( fp_log, L"-------------------------------------------------------------------------------\n");
   fwprintf( fp_log, L"�����t�@�C�����F%ls\n", tcb->dgnfilenm);
   fwprintf( fp_log, L"-------------------------------------------------------------------------------\n");

   /*----- �Q�ƃt�@�C���S�f�^�b�` -----*/
   if ( attach8.tog_detach == -1){
      mdlRefFile_detach( REFERENCE_All_References);
   }

   for( ii=0; ii<10; ii++){
      if ( attach8.tog[ii] == -1){
         /*----- �Œ�t�@�C���̏ꍇ -----*/
         if ( attach8.tog_kotei[ii] == -1){
            swprintf( filename, L"%ls", attach8.refName[ii]);
         }
         else{
            swprintf( filename, L"%ls%ls.dgn", attach8.refName[ii], p);
         }

         /*----- �Q�ƃt�@�C���A�^�b�`�ł����ꍇ -----*/
         if ( mdlRefFile_attach( &model, filename, NULL, NULL, &orig, &orig, 1, NULL
                               , 0, NULL, REF_FILE_LEVEL_DISPLAY_DEFAULT, TRUE, TRUE) == SUCCESS){
            fwprintf( fp_log, L"�Q��0%d�F%ls\n", ii+1, filename);
         }

         /*----- �Q�ƃt�@�C���A�^�b�`�ł��Ȃ������ꍇ -----*/
         else{
            fwprintf( fp_log, L"�Q��0%d�F�A�^�b�`�o���܂���ł����B\n", ii+1);
         }
      }
   }
   fclose( fp_log);
   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^�� �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          mdlSystem_exit( exitStatus, 1);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�g�O���{�^������������
------------------------------------------------------------------------*/
Private	void	toggle_change( int ii)
{
    /*--- ON�ɂȂ����ꍇ -----*/
    if ( attach8.tog[ii] == -1){
       mdlDialog_itemSetEnabledState( db_p, 4*ii+5, TRUE, FALSE);
       mdlDialog_itemSetEnabledState( db_p, 4*ii+6, TRUE, FALSE);
       mdlDialog_itemSetEnabledState( db_p, 4*ii+7, TRUE, FALSE);
    }

    /*--- OFF�ɂȂ����ꍇ -----*/
    else{
       mdlDialog_itemSetEnabledState( db_p, 4*ii+5, FALSE, FALSE);
       mdlDialog_itemSetEnabledState( db_p, 4*ii+6, FALSE, FALSE);
       mdlDialog_itemSetEnabledState( db_p, 4*ii+7, FALSE, FALSE);
    }
    mdlDialog_itemsSynch( db_p);
}


/*------------------------------------------------------------------------
**	�Q�ƃt�@�C����I�������Ƃ�
------------------------------------------------------------------------*/
Private	void	get_refFile( int ii)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH], nameP[MAXNAMELENGTH];
BeFileName	filePath;

   swprintf( filter, L"*.dgn");
   swprintf( title,  L"�Q�ƃt�@�C����I��");

   mdlFile_parseName( attach8.refName[ii], devP, dirP, NULL, NULL);
   swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

   mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
   swprintf(attach8.refName[ii], L"%ls", filePath.GetWCharCP());
   mdlFile_parseName( attach8.refName[ii], devP, dirP, nameP, NULL);

   /*--- �Œ�łȂ��ꍇ -----*/
   if ( attach8.tog_kotei[ii] != -1){
      swprintf( attach8.refName[ii], L"%ls:%ls\%c", devP, dirP, nameP[0]);
   }
   mdlDialog_itemsSynch( db_p);
}
