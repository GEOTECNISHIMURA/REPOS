#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-		基準点配置			                        -*/
/*-													-*/
/*-	        TXCELL.MC								-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>

#include <stdio.h>
#include "../CommonCE/check_kigen8.cpp"
#include "txcell.h"
FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;

TXSET			txset;

#ifndef Private
#define Private
#endif

/*-	フック関数		-*/
Private void	txcell_dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_seed_hook( DialogItemMessage *dimP);
Private void	option_format_hook( DialogItemMessage *dimP);
Private void	option_pname_hook( DialogItemMessage *dimP);
Private void	push_file_hook( DialogItemMessage *dimP);
Private void	push_cellib_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_TXCELL_DIALOG,	(PFDialogHook)txcell_dialog_hook},
	{ HOOKID_TOGGLE_IDX,    (PFDialogHook)toggle_idx_hook   },
	{ HOOKID_PUSH_IDX,      (PFDialogHook)push_idx_hook     },
	{ HOOKID_PUSH_SEED,     (PFDialogHook)push_seed_hook    },
	{ HOOKID_OPTION_FORMAT, (PFDialogHook)option_format_hook},
	{ HOOKID_OPTION_PNAME,	(PFDialogHook)option_pname_hook },
	{ HOOKID_PUSH_FILE,		(PFDialogHook)push_file_hook    },
	{ HOOKID_PUSH_CELLIB,	(PFDialogHook)push_cellib_hook  },
	{ HOOKID_PUSH_OK,		(PFDialogHook)push_ok_hook      },
	{ HOOKID_PUSH_END,		(PFDialogHook)push_end_hook     },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private void	program_unload( void);



/*-----------------------------------------------------------------------
**	メイン
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



	chk_kigen( );

   /*---- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"TXCELLのリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期設定 -------------*/
   sprintf(structName, "TXSET_INFO");
   sprintf(variableName, "txset");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX |
                                        VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &txset);

   /*---- 設定値の読み込み -------------*/
   mdlFile_find( &full, L"txcell.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen( full, L"r")) != NULL){
      fwscanf( fp, L"%d\n", &txset.tog_idx);
      fwscanf( fp, L"%ls\n", txset.idx);
      fwscanf( fp, L"%ls\n", txset.seed);
      fwscanf( fp, L"%ls\n", txset.file);
      fwscanf( fp, L"%d\n", &txset.form);
      fwscanf( fp, L"%d\n", &txset.pname);
      fwscanf( fp, L"%d\n", &txset.kugiri);
      fwscanf( fp, L"%d\n", &txset.txpos);
      fwscanf( fp, L"%d\n", &txset.font);
      fwscanf( fp, L"%d\n", &txset.scale);
      fwscanf( fp, L"%ls\n", txset.cellib);
      fwscanf( fp, L"%ls\n", txset.cell);
      fwscanf( fp, L"%d\n", &txset.lv);
      fwscanf( fp, L"%d\n", &txset.co);
      fwscanf( fp, L"%d\n", &txset.lc);
      fwscanf( fp, L"%d\n", &txset.wt);
      fwscanf( fp, L"%ls\n", txset.file);
	  fclose(fp);
   }
   else{
      txset.tog_idx   = 0;
	  swprintf(txset.idx, L"c:\\\0");
	  swprintf(txset.seed, L"c:\\\0");
	  swprintf(txset.file, L"c:\\\0");
      txset.form      = 0;
      txset.pname     = 0;
      txset.kugiri    = 0;
      txset.font      = 0;
      txset.txpos     = 0;
      txset.scale     = 0;
	  swprintf(txset.cellib, L"c:\\\0");
	  swprintf(txset.cell, L"c:\\\0");
      txset.lv        = 1;
      txset.co        = 0;
      txset.lc        = 0;
      txset.wt        = 0;
	  swprintf(txset.file, L"c:\\\0");
   }
   

   /*--- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_TXCELL);
   if ( db_p == NULL){
      mdlOutput_error(L"TXCELLダイアログボックスが開けません");
   }

   /*--- バッチの場合 ---*/
   if ( txset.tog_idx == -1){
      mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
   }

   /*--- バッチでない場合 ---*/
   else{
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
   }

   /*--- SIMA ---*/
   if ( txset.form == 2){
      mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
   }

   /*--- 数学座標・測地座標 ---*/
   else{
      /*--- 点名なし ---*/
      if ( txset.pname == 1){
         mdlDialog_itemSetEnabledState( db_p, 10, FALSE, FALSE);
         mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
      }
   }

}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	program_unload
-----------------------------------------------------------------------*/
Private	void	program_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_TXCE) == 0){ }
}


/*-----------------------------------------------------------------------
**	ダイアログフック関数
-----------------------------------------------------------------------*/
Private	void	txcell_dialog_hook( DialogMessage *dmP)
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
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	トグルボタン（IDX）  フック関数
-----------------------------------------------------------------------*/
Private void	toggle_idx_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- バッチの場合 ---*/
         if ( txset.tog_idx == -1){
            mdlDialog_itemSetEnabledState( db_p, 1, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
         }

         /*--- バッチでない場合 ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE,  FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（IDX）  フック関数
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         txset.nn = 1;
         mdlSystem_loadMdlProgram( TD_TXCELL, TD_FILE, L"");

         mdlDialog_itemsSynch( db_p);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（ｼｰﾄﾞﾌｧｲﾙ選択）  フック関数
-----------------------------------------------------------------------*/
Private void	push_seed_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         txset.nn = 2;
         mdlSystem_loadMdlProgram( TD_TXCELL, TD_FILE, L"");

         mdlDialog_itemsSynch( db_p);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	オプションボタン(ﾌｫｰﾏｯﾄ）　フック関数
-----------------------------------------------------------------------*/
Private void	option_format_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- 数学座標・測地座標 ---*/
         if ( txset.form != 2){
            mdlDialog_itemSetEnabledState( db_p, 8, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);

            /*--- 点名なし ---*/
            if ( txset.pname == 1){
               mdlDialog_itemSetEnabledState( db_p, 10, FALSE, FALSE);
               mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
            }
         }

         /*--- SIMA ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p,  8, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p,  9, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 10, TRUE,  FALSE);
            mdlDialog_itemSetEnabledState( db_p, 11, TRUE,  FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	オプションボタン(点名）　フック関数
-----------------------------------------------------------------------*/
Private void	option_pname_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- 点名あり ---*/
         if ( txset.pname == 0){
            mdlDialog_itemSetEnabledState( db_p, 10, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 11, TRUE, FALSE);
         }

         /*--- 点名なし ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 10, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン(ファイル）　フック関数
-----------------------------------------------------------------------*/
Private void	push_file_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         txset.nn = 4;
         mdlSystem_loadMdlProgram( TD_TXCELL, TD_FILE, L"");

         mdlDialog_itemsSynch( db_p);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン(ｾﾙﾗｲﾌﾞﾗﾘ）　フック関数
-----------------------------------------------------------------------*/
Private void	push_cellib_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         txset.nn = 3;
         mdlSystem_loadMdlProgram( TD_TXCELL, TD_FILE, L"");

         mdlDialog_itemsSynch( db_p);
		 mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）　フック関数
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{
	BeFileName		full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- 設定値の保存 ---*/
         mdlFile_find( &full, L"txcell.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%d\n", txset.tog_idx);
            fwprintf( fp, L"%ls\n", txset.idx);
            fwprintf( fp, L"%ls\n", txset.seed);
            fwprintf( fp, L"%ls\n", txset.file);
            fwprintf( fp, L"%d\n", txset.form);
            fwprintf( fp, L"%d\n", txset.pname);
            fwprintf( fp, L"%d\n", txset.kugiri);
            fwprintf( fp, L"%d\n", txset.txpos);
            fwprintf( fp, L"%d\n", txset.font);
            fwprintf( fp, L"%d\n", txset.scale);
            fwprintf( fp, L"%ls\n", txset.cellib);
            fwprintf( fp, L"%ls\n", txset.cell);
            fwprintf( fp, L"%d\n", txset.lv);
            fwprintf( fp, L"%d\n", txset.co);
            fwprintf( fp, L"%d\n", txset.lc);
            fwprintf( fp, L"%d\n", txset.wt);
            fwprintf( fp, L"%ls\n", txset.file);
         fclose( fp);
		 
         if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel, L"本当に実行しますか？", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
            mdlSystem_loadMdlProgram( TD_TXCELL, TD_TXCE, L"");
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン(終了）　フック関数
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );
         //mdlInput_sendKeyin( L"mdl unload txcell", 1, INPUTQ_EOQ, NULL);
		 mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD,
			 mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}

