#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-		リンク			           	-*/
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

/*-	フック関数		-*/
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

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);

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
/*
#if LOCK == 1
main_lok( );
#endif
*/
	chk_kigen( );


   /*--------- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"LINKのリソースファイルが開きません");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


   /*--------- フック関数の使用宣言 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--------- C式定義初期設定 -------------*/
   sprintf(structName, "LINKSET_INFO");
   sprintf(variableName, "linkset");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX |
                                        VISIBILITY_CALCULATOR, 0, TRUE);

   mdlDialog_publishComplexVariable(setP, structName, variableName, &linkset);

   /*--- 設定 ---*/
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
   

   /*-------- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_LINK);
   if ( db_p == NULL){
      mdlOutput_error(L"LINKダイアログボックスが開けません");
   }

   /*-------- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_LINK);
   if ( db_p == NULL){
      mdlOutput_error(L"LINKダイアログボックスが開けません");
   }

   /*-------- バッチの場合 --------*/
   if ( linkset.kind == 0){
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- １ファイルの場合 --------*/
   if ( linkset.kind == 1){
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- １要素（最大MSLINK＋１追加）の場合 --------*/
   if ( linkset.kind == 2){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
   }

   /*-------- １要素（設定して追加）の場合 --------*/
   if ( linkset.kind == 3){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
   }

   /*-------- １要素（既存MSLINK）の場合 --------*/
   if ( linkset.kind == 4){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
   }

   /*-------- １要素（ファイル名をMSLINKで与える）の場合 --------*/
   if ( linkset.kind == 5){
      mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
   }

   /*--- 図面番号出力しない場合 --------*/
   if ( linkset.zumen != -1){
      mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
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
**	プログラムアンロード
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
**	ダイアログフック関数
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
**	プッシュボタン（レイヤーテーブル）　フック関数
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
**	オプションボタン（種類）　フック関数
-----------------------------------------------------------------------*/
Private void	option_kind_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
        /*--- バッチの場合 ---*/
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

         /*--- １ファイルの場合 ---*/
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

         /*--- １要素（最大MSLINK＋１追加）の場合 ---*/
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

         /*--- １要素（設定して追加）の場合 ---*/
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

         /*--- １要素（既存MSLINK）の場合 ---*/
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

         /*--- １要素（ファイル名をMSLINKで与える）の場合 ---*/
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
**	プッシュボタン（バッチ）　フック関数
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
**	トグルボタンボタン（図面番号）　フック関数
-----------------------------------------------------------------------*/
Private void	toggle_zumen_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- 図面番号出力の場合 --------*/
         if ( linkset.zumen == -1){
            mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
         }

         /*--- 図面番号出力しない場合 --------*/
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
**	プッシュボタン(解除）　フック関数
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
**	プッシュボタン（結合）　フック関数
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{
    BeFileName		full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- 設定保存 ---*/
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

         /*--- 複製モード ---*/
         mdlDB_activeLinkageMode( L"duplicate");

         /*--- データベースと接続できない場合 ---*/
//         if ( mdlDB_activeDatabase( linkset.dbsource) != SUCCESS){
//            mdlDialog_openInfoBox( L"データベースと接続できません");
//         }
		 /*--- データベースと接続できない場合 ---*/
         if (mdlDB_changeDatabase(DATABASESERVERID_ODBC, linkset.dbsource) != SUCCESS){
            mdlDialog_openInfoBox( L"データベースと接続できません");
         }


         /*--- データベースと接続できた場合 ---*/
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
**	プッシュボタン(終了）　フック関数
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