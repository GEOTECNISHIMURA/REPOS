#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-		�����N			           	-*/
/*-									-*/
/*-	        LINK.MC					-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/check_kigen8.cpp"

#include "link.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;
WChar			keyin[128];
LINKSET			linkset;

#ifndef Private
#define Private
#endif

/*-	�t�b�N�֐�		-*/
Private void	link_dialog_hook( DialogMessage *dmP);
Private void	push_layer_hook( DialogItemMessage *dimP);
Private void	option_kind_hook( DialogItemMessage *dimP);
Private void	push_bat_hook( DialogItemMessage *dimP);
Private void	toggle_zumen_hook( DialogItemMessage *dimP);
Private void	push_del_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_LINK_DIALOG,     (PFDialogHook)link_dialog_hook },
	{ HOOKID_PUSH_LINK_LAYER, (PFDialogHook)push_layer_hook  },
	{ HOOKID_OPTION_KIND,     (PFDialogHook)option_kind_hook },
	{ HOOKID_PUSH_LINK_BAT,   (PFDialogHook)push_bat_hook    },
	{ HOOKID_TOGGLE_ZUMEN,    (PFDialogHook)toggle_zumen_hook},
	{ HOOKID_PUSH_LINK_DEL,   (PFDialogHook)push_del_hook    },
	{ HOOKID_PUSH_LINK_OK,    (PFDialogHook)push_ok_hook     },
	{ HOOKID_PUSH_LINK_END,   (PFDialogHook)push_end_hook    },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);

/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	char            structName[128];
	char            variableName[128];
	SymbolSet		*setP;
	BeFileName		full, cellib;
/*
#if LOCK == 1
main_lok( );
#endif
*/
	chk_kigen( );


   /*--------- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"LINK�̃��\�[�X�t�@�C�����J���܂���");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


   /*--------- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--------- C����`�����ݒ� -------------*/
   sprintf(structName, "LINKSET_INFO");
   sprintf(variableName, "linkset");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX |
                                        VISIBILITY_CALCULATOR, 0, TRUE);

   mdlDialog_publishComplexVariable(setP, structName, variableName, &linkset);

   /*--- �ݒ� ---*/
   mdlFile_find( &full, L"link.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &linkset.kind);
      fwscanf( fp, L"%ld\n",&linkset.mslink);
      fwscanf( fp, L"%ls\n", linkset.layer);
      fwscanf( fp, L"%ls\n", linkset.bat);
      fwscanf( fp, L"%ls\n", linkset.dbsource);
      fwscanf( fp, L"%ls\n", linkset.table);
      fwscanf( fp, L"%d\n", &linkset.zumen);
      fwscanf( fp, L"%d\n", &linkset.keta);
      fwscanf( fp, L"%d\n", &linkset.tdel);
	  fclose(fp);
   }
   else{
      linkset.kind        = 0;
      linkset.mslink      = 1;
	  swprintf(linkset.layer, L"c:\\\0");
	  swprintf(linkset.bat, L"c:\\\0");
	  swprintf(linkset.dbsource, L"dbsource\0");
	  swprintf(linkset.table, L"tablename\0");
      linkset.zumen       = -1;
      linkset.keta        = 5;
      linkset.tdel        = -1;
   }
   

   /*-------- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_LINK);
   if ( db_p == NULL){
      mdlOutput_error(L"LINK�_�C�A���O�{�b�N�X���J���܂���");
   }

   /*-------- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_LINK);
   if ( db_p == NULL){
      mdlOutput_error(L"LINK�_�C�A���O�{�b�N�X���J���܂���");
   }

   /*-------- �o�b�`�̏ꍇ --------*/
   if ( linkset.kind == 0){
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- �P�t�@�C���̏ꍇ --------*/
   if ( linkset.kind == 1){
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- �P�v�f�i�ő�MSLINK�{�P�ǉ��j�̏ꍇ --------*/
   if ( linkset.kind == 2){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- �P�v�f�i�ݒ肵�Ēǉ��j�̏ꍇ --------*/
   if ( linkset.kind == 3){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
   }

   /*-------- �P�v�f�i����MSLINK�j�̏ꍇ --------*/
   if ( linkset.kind == 4){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
   }

   /*-------- �P�v�f�i�t�@�C������MSLINK�ŗ^����j�̏ꍇ --------*/
   if ( linkset.kind == 5){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
   }

   /*--- �}�ʔԍ��o�͂��Ȃ��ꍇ --------*/
   if ( linkset.zumen != -1){
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
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
**	�v���O�����A�����[�h
-----------------------------------------------------------------------*/
Private	void	program_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_ALL) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_SINGLE) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_NAME) == 0){ }
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
-----------------------------------------------------------------------*/
Private	void	link_dialog_hook( DialogMessage *dmP)
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
         program_unload( );
	 	 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ),INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���C���[�e�[�u���j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_layer_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );
         linkset.nn = 1;
         mdlSystem_loadMdlProgram( TD_LINK, TD_FILE, L"");
         mdlDialog_itemSynch( db_p, 1);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�I�v�V�����{�^���i��ށj�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	option_kind_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
        /*--- �o�b�`�̏ꍇ ---*/
        if ( linkset.kind == 0){
            mdlDialog_itemSetEnabledState( db_p, 0, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, TRUE, FALSE);
            if ( linkset.zumen == -1){
               mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
            }
         }

         /*--- �P�t�@�C���̏ꍇ ---*/
         if ( linkset.kind == 1){
            mdlDialog_itemSetEnabledState( db_p, 0, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, TRUE, FALSE);
            if ( linkset.zumen == -1){
               mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
            }
         }

         /*--- �P�v�f�i�ő�MSLINK�{�P�ǉ��j�̏ꍇ ---*/
         if ( linkset.kind == 2){
            mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, TRUE, FALSE);
            if ( linkset.zumen == -1){
               mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
            }
         }

         /*--- �P�v�f�i�ݒ肵�Ēǉ��j�̏ꍇ ---*/
         if ( linkset.kind == 3){
            mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, TRUE,  FALSE);
            if ( linkset.zumen == -1){
               mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
            }
         }

         /*--- �P�v�f�i����MSLINK�j�̏ꍇ ---*/
         if ( linkset.kind == 4){
            mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
         }

         /*--- �P�v�f�i�t�@�C������MSLINK�ŗ^����j�̏ꍇ ---*/
         if ( linkset.kind == 5){
            mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_bat_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );
         linkset.nn = 2;
         mdlSystem_loadMdlProgram( TD_LINK, TD_FILE, L"");
         mdlDialog_itemSynch( db_p, 4);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^���{�^���i�}�ʔԍ��j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_zumen_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �}�ʔԍ��o�͂̏ꍇ --------*/
         if ( linkset.zumen == -1){
            mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
         }

         /*--- �}�ʔԍ��o�͂��Ȃ��ꍇ --------*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�����j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_del_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         mdlSystem_loadMdlProgram( TD_LINK, TD_DEL, L"");
         break;

      default:
	 	dimP->msgUnderstood = FALSE;
	 	break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�����j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{
    BeFileName		full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- �ݒ�ۑ� ---*/
         mdlFile_find( &full, L"link.tbl", L"MS_DEF", NULL);
         fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp,L"%d\n", linkset.kind);
            fwprintf( fp,L"%ld\n",linkset.mslink);
            fwprintf( fp,L"%ls\n", linkset.layer);
            fwprintf( fp,L"%ls\n", linkset.bat);
            fwprintf( fp,L"%ls\n", linkset.dbsource);
            fwprintf( fp,L"%ls\n", linkset.table);
            fwprintf( fp,L"%d\n", linkset.zumen);
            fwprintf( fp,L"%d\n", linkset.keta);
            fwprintf( fp,L"%d\n", linkset.tdel);
         fclose( fp);

         /*--- �������[�h ---*/
         mdlDB_activeLinkageMode( L"duplicate");

         /*--- �f�[�^�x�[�X�Ɛڑ��ł��Ȃ��ꍇ ---*/
//         if ( mdlDB_activeDatabase( linkset.dbsource) != SUCCESS){
//            mdlDialog_openInfoBox( L"�f�[�^�x�[�X�Ɛڑ��ł��܂���");
//         }
		 /*--- �f�[�^�x�[�X�Ɛڑ��ł��Ȃ��ꍇ ---*/
         if (mdlDB_changeDatabase(DATABASESERVERID_ODBC, linkset.dbsource) != SUCCESS){
            mdlDialog_openInfoBox( L"�f�[�^�x�[�X�Ɛڑ��ł��܂���");
         }


         /*--- �f�[�^�x�[�X�Ɛڑ��ł����ꍇ ---*/
         else{
            if ( linkset.kind == 0 || linkset.kind == 1){
               mdlSystem_loadMdlProgram( TD_LINK, TD_ALL, L"");
            }
            else if ( linkset.kind == 5){
               mdlSystem_loadMdlProgram( TD_LINK, TD_NAME, L"");
            }
            else{
               mdlSystem_loadMdlProgram( TD_LINK, TD_SINGLE, L"");
            }
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
         program_unload( );
         mdlInput_sendKeyin( L"mdl unload link", 1, INPUTQ_EOQ, NULL);
         break;

      default:
	 	dimP->msgUnderstood = FALSE;
	 	break;
   }
}
