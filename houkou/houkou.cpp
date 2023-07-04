#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-		�������̃J�b�g		       	-*/
/*-									-*/
/*-	        HOUKOU.MC				-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include "../CommonCE/check_kigen8.cpp"
#include "houkou.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;
HOUSET			houset;

#ifndef Private
#define Private
#endif
/*-	�t�b�N�֐�		-*/
Private void	houkou_dialog_hook( DialogMessage *dmP);
Private void	option_cut_hook( DialogItemMessage *dimP);
Private void	option_layer_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_HOUKOU_DIALOG, (PFDialogHook)houkou_dialog_hook},
	{ HOOKID_OPTION_CUT,	(PFDialogHook)option_cut_hook   },
	{ HOOKID_OPTION_LAYER,	(PFDialogHook)option_layer_hook },
	{ HOOKID_PUSH_OK,		(PFDialogHook)push_ok_hook      },
	{ HOOKID_PUSH_END,		(PFDialogHook)push_end_hook     },
};

/*-	�v���g�^�C�v	-*/
Private bool	houkou_unload( UnloadProgramReason unloadType);
Private	void	program_unload( void);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];
	BeFileName      full;
	int             i;


	chk_kigen( );


   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"HOUKOU�̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��Z�b�g ---------------*/
   SystemCallback::SetUnloadProgramFunction(houkou_unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "HOUSET_INFO");
   sprintf(variableName, "houset");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &houset);

   /*--- �e�[�u���t�@�C���̃t���p�X�擾 -------------*/
   mdlFile_find( &full, L"houkou.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp,L"%d\n", &houset.layer);
      fwscanf( fp,L"%d\n", &houset.cut);
      fwscanf( fp,L"%ld\n",&houset.scale);
      fwscanf( fp,L"%d\n", &houset.lv);
      fwscanf( fp,L"%d\n", &houset.co);
      fwscanf( fp,L"%d\n", &houset.lc);
      fwscanf( fp,L"%d\n", &houset.wt);
      fwscanf( fp,L"%ls\n", houset.cell);
      fwscanf( fp,L"%d\n", (int*)&houset.zukei);
	  fclose(fp);
   }
   else{
      houset.layer   = 2;
      houset.cut     = 0;
      houset.scale   = 0;
      houset.lv      = 1;
      houset.co      = 0;
      houset.lc      = 0;
      houset.wt      = 0;
	  swprintf(houset.cell, L"0\0");
      houset.zukei   = FALSE;
   }
   

   /*--- �_�C�A���O�{�b�N�X�I�[�v�� -------------*/
   db_p = mdlDialog_open( NULL, DIALOGID_HOUKOU);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"HOUKOU�_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- ��ނ����ԁA���΂Ȃ��i�Z���j�̏ꍇ -----*/
   if ( houset.cut == 0 || houset.cut == 4){
      mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
   }

   /*--- ��ނ����΂Ȃ��i�Z���j�ȊO�̏ꍇ -----*/
   if ( houset.cut != 4){
      mdlDialog_itemSetEnabledState( db_p,  9, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 10, FALSE, FALSE);
   }

   /*--- ���C���[���A�N�e�B�u�A�d���̏ꍇ -----*/
   if ( houset.layer != 2){
      for( i=3; i<7; i++){
         mdlDialog_itemSetEnabledState( db_p, i, FALSE, FALSE);
      }
   }

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private  bool   houkou_unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	program_unload
-----------------------------------------------------------------------*/
Private	void	program_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_UNCUT) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_CUT) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_YA) == 0){ }
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
-----------------------------------------------------------------------*/
Private	void	houkou_dialog_hook( DialogMessage *dmP)
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
         mdlOutput_prompt( L"");

         /*--- ���o�[�o���h���N���A���� -----*/
         mdlState_clear( );

         program_unload( );
		 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^��(��ށj�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	option_cut_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- ���ԁA���΂Ȃ��i�Z���j�̏ꍇ -----*/
         if ( houset.cut == 0 || houset.cut == 4){
            mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
         }

         /*--- ���΂Ȃ��A���A�x���̏ꍇ -----*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
         }

         /*--- ���΂Ȃ��i�Z���j�̏ꍇ -----*/
         if ( houset.cut == 4){
            mdlDialog_itemSetEnabledState( db_p,  9, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 10, TRUE, FALSE);
         }

         /*--- ���΂Ȃ��i�Z���j�ȊO�̏ꍇ -----*/
         else{
            mdlDialog_itemSetEnabledState( db_p,  9, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 10, FALSE, FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^��(���C���[�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	option_layer_hook( DialogItemMessage *dimP)
{
	int  i;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- �ݒ�̏ꍇ -----*/
         if ( houset.layer == 2){
            for( i=3; i<7; i++){
               mdlDialog_itemSetEnabledState( db_p, i, TRUE, FALSE);
            }
         }

         /*--- �J�����g�A�܂��́A�d���Ɠ����̏ꍇ -----*/
         else{
            for( i=3; i<7; i++){
               mdlDialog_itemSetEnabledState( db_p, i, FALSE, FALSE);
            }
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{
	BeFileName full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         if ( houset.scale == 0){
            mdlOutput_prompt(L"�k�ڂ���͂��ĉ�����");
            break;
         }
         program_unload( );

		 
		 mdlFile_find( &full, L"houkou.tbl", L"MS_DEF", NULL);
         fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp,L"%d\n", houset.layer);
            fwprintf( fp,L"%d\n", houset.cut);
            fwprintf( fp,L"%ld\n",houset.scale);
            fwprintf( fp,L"%d\n", houset.lv);
            fwprintf( fp,L"%d\n", houset.co);
            fwprintf( fp,L"%d\n", houset.lc);
            fwprintf( fp,L"%d\n", houset.wt);
            fwprintf( fp,L"%ls\n", houset.cell);
			fwprintf( fp, L"%d\n", (int)houset.zukei);
         fclose( fp);

         if ( houset.cut == 0){
            mdlSystem_loadMdlProgram( TD_HOUKOU, TD_UNCUT, L"");
         }
         if ( houset.cut == 1 || houset.cut == 4){
            if ( houset.cut == 4){
               if ( mdlCell_existsInLibrary( houset.cell) == FALSE){
                  mdlDialog_openInfoBox( L"���̃Z���̓Z�����C�u�������ɑ��݂��܂���");
                  break;
               }
            }
            mdlSystem_loadMdlProgram( TD_HOUKOU, TD_CUT, L"");
         }
         if ( houset.cut == 2 || houset.cut == 3){
            mdlSystem_loadMdlProgram( TD_HOUKOU, TD_YA, L"");
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�I���j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         mdlOutput_prompt(L"");
         program_unload( );

         /*--- ���o�[�o���h���N���A���� -----*/
         mdlState_clear( );

         mdlInput_sendKeyin( L"mdl unload houkou", 1, INPUTQ_EOQ, NULL );
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}
