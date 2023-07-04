#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*----------------------------------------------------------------------*/
/*-     MFCCHK.MC                                                       */
/*----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "mfcchk.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE        *fp;
MSDialog    *db_p;
MSElement   ele;
BeFileName	full;
int         ii, exitStatus;
MFCCHK      mfcchk;

#ifndef Private
#define Private
#endif
/*-    フック関数   -*/
Private void    dialog_hook( DialogMessage *dmP);
Private void    push_table_hook( DialogItemMessage *dimP);
Private void    push_bat_hook( DialogItemMessage *dimP);
Private void    option_mode_hook( DialogItemMessage *dimP);
Private void    toggle_err_hook( DialogItemMessage *dimP);
Private void    errct_text_hook( DialogItemMessage *dimP);
Private void    toggle_font_hook( DialogItemMessage *dimP);
Private void    push_ok_hook( DialogItemMessage *dimP);
Private void    push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo  uHooks[] = {
   { HOOKID_MFCCHK_DIALOG, (PFDialogHook)dialog_hook     },
   { HOOKID_PUSH_TABLE,  (PFDialogHook)push_table_hook },
   { HOOKID_PUSH_BAT,    (PFDialogHook)push_bat_hook   },
   { HOOKID_OPTION_MODE, (PFDialogHook)option_mode_hook},
   { HOOKID_TOGGLE_ERR,  (PFDialogHook)toggle_err_hook },
   { HOOKID_ERRCT_TEXT,  (PFDialogHook)errct_text_hook },
   { HOOKID_TOGGLE_FONT, (PFDialogHook)toggle_font_hook},
   { HOOKID_PUSH_OK,     (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_END,    (PFDialogHook)push_end_hook   },
};

Private  bool	unload(UnloadProgramReason unloadType);
Private  void   reload(int argc, WCharCP argv[]);
Private  void   task_unload( void);

/*-----------------------------------------------------------------------
**  MAIN
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{


    RscFileHandle   Rsc;
	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];

	

/*
#if LOCK == 1
    main_lok( );
#endif
*/
	chk_kigen( );

	/*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);



    /*--- リソースファイルを開ける ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"MFCCHK のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }



    /*--- リロード関数のセット ---*/
	SystemCallback::SetReloadProgramFunction(reload);

    /*--- フック関数の使用宣言 ---*/
    mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

    /*--- C式定義初期化設定 ---*/
	sprintf(structName, "MFCCHK_INFO");
	sprintf(variableName, "mfcchk");

    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName, &mfcchk);

    /*--------- 設定値を読み込む -------------*/
	mdlFile_find(&full, L"mfcchk.tbl", L"MS_DEF", NULL);
    if( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
        fwscanf( fp, L"%ls\n", mfcchk.table);
        fwscanf( fp, L"%d\n", &mfcchk.mode);
        fwscanf( fp, L"%ls\n", mfcchk.bat);
        fwscanf( fp, L"%d\n", &mfcchk.lv);
        fwscanf( fp, L"%d\n", &mfcchk.co);
        fwscanf( fp, L"%d\n", &mfcchk.lc);
        fwscanf( fp, L"%d\n", &mfcchk.wt);
        fwscanf( fp, L"%d\n", &mfcchk.err);
        fwscanf( fp, L"%d\n", &mfcchk.zen);
        fwscanf( fp, L"%d\n", &mfcchk.han);
        fwscanf( fp, L"%d\n", &mfcchk.font);
        fwscanf( fp, L"%d\n", &mfcchk.space);
        fwscanf( fp, L"%lf\n", &mfcchk.size);
		fclose(fp);
    }
    else{
		swprintf(mfcchk.table, L"c:\\\0");
        mfcchk.mode     = 1;
		swprintf(mfcchk.bat, L"c:\\\0");
        mfcchk.lv       = 1;
        mfcchk.co       = 0;
        mfcchk.lc       = 0;
        mfcchk.wt       = 0;
        mfcchk.err      = 1;
        mfcchk.zen      = 0;
        mfcchk.han      = 3;
        mfcchk.font     = 0;
        mfcchk.space    = 0;
        mfcchk.size     = 1;
    }
    

    /*--- ダイアログボックスオープン ---*/
    db_p = mdlDialog_open( NULL, DIALOGID_MFCCHK);
    if( db_p == NULL){
        mdlOutput_error( L"MFCCHK ダイアログボックスが開きません ");
    }

    /*--- １ファイルの場合 ---*/
    if( mfcchk.mode == 0){
        mdlDialog_itemSetEnabledState( db_p,  4, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p,  5, FALSE, FALSE);
    }

    /*--- エラーフラグ設定がオフの場合 ---*/
    if( mfcchk.err != -1){
        mdlDialog_itemSetEnabledState( db_p,  8, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p,  9, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 12, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 15, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 16, FALSE, FALSE);
        mdlDialog_itemHide( db_p, 10, FALSE);
    }

    /*--- フォント設定がオフの場合 ---*/
    if( mfcchk.font != -1){
        mdlDialog_itemSetEnabledState( db_p, 19, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 20, FALSE, FALSE);
    }

}

/*------------------------------------------------------------------------
**  アンロード関数
------------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}

/*------------------------------------------------------------------------
**  リロード関数
------------------------------------------------------------------------*/
Private  void  reload(int argc, WCharCP argv[])
{
    mdlDialog_itemsSynch( db_p );
}

/*------------------------------------------------------------------------
**  task_unload
------------------------------------------------------------------------*/
Private  void  task_unload( )
{
    if( mdlSystem_unloadMdlProgram( TD_FILE) == 0){ }
    if( mdlSystem_unloadMdlProgram( TD_CHK) == 0){ }
}

/*------------------------------------------------------------------------
**  ダイアログフック関数
------------------------------------------------------------------------*/
Private  void  dialog_hook( DialogMessage *dmP)
{
    DialogBoxRsc    *dbr_P;

    dmP->msgUnderstood = TRUE;
    switch( dmP->messageType){
        case DIALOG_MESSAGE_CREATE:
            dbr_P = dmP->u.create.dialogBoxRP;
            dmP->u.create.interests.updates   = TRUE;
            dmP->u.create.interests.mouses    = TRUE;
            dmP->u.create.interests.dialogFocuses = TRUE;
            dmP->u.create.interests.itemFocuses   = TRUE;
         break;

        case DIALOG_MESSAGE_DESTROY:
            task_unload( );
            mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
            break;

        default:
            dmP->msgUnderstood = FALSE;
            break;
    }
}

/*-----------------------------------------------------------------------
**  プッシュボタン（テーブル）　フック関数
-----------------------------------------------------------------------*/
Private void  push_table_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            mfcchk.nn = 1;
            mdlSystem_loadMdlProgram( TD_MFCCHK, TD_FILE,L"");
            mdlDialog_itemsSynch( db_p);

            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*-----------------------------------------------------------------------
**  プッシュボタン（バッチ）　フック関数
-----------------------------------------------------------------------*/
Private  void  push_bat_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            mfcchk.nn = 2;
            mdlSystem_loadMdlProgram( TD_MFCCHK, TD_FILE,L"");
            mdlDialog_itemsSynch( db_p);

            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*-----------------------------------------------------------------------
**  オプションボタン（モード）　フック関数
-----------------------------------------------------------------------*/
Private void    option_mode_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            /*--- 1ファイル ---*/
            if( mfcchk.mode == 0){
                mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
            }

            /*--- バッチ ---*/
            if( mfcchk.mode == 1){
                mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 5, TRUE, FALSE);
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*-----------------------------------------------------------------------
**  トグルアイテム（エラーフラグ）　フック関数
-----------------------------------------------------------------------*/
Private  void  toggle_err_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:

            /*--- エラーフラグON ---*/
            if( mfcchk.err == -1){
                mdlDialog_itemSetEnabledState( db_p,  8, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p,  9, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 12, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 15, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 16, TRUE, FALSE);
                mdlDialog_itemShow( db_p, 10);
            }

            /*--- エラーフラグOFF ---*/
            else{
                mdlDialog_itemSetEnabledState( db_p,  8, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p,  9, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 12, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 15, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 16, FALSE, FALSE);
                mdlDialog_itemHide( db_p, 10, FALSE);
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*------------------------------------------------------------------------
**  テキストアイテム（エラー数） フック関数
------------------------------------------------------------------------*/
Private  void   errct_text_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_SETSTATE:
         mdlDialog_itemsSynch( db_p);
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}

/*-----------------------------------------------------------------------
**  トグルアイテム（エラーフラグ）　フック関数
-----------------------------------------------------------------------*/
Private  void  toggle_font_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:

            /*--- フォントON ---*/
            if( mfcchk.font == -1){
                mdlDialog_itemSetEnabledState( db_p, 19, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 20, TRUE, FALSE);
            }

            /*--- フォントOFF ---*/
            else{
                mdlDialog_itemSetEnabledState( db_p, 19, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 20, FALSE, FALSE);
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*------------------------------------------------------------------------
**  プッシュボタン（実行） フック関数
------------------------------------------------------------------------*/
Private  void  push_ok_hook( DialogItemMessage *dimP)
{

    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--- 設定値の保存 ---*/
			mdlFile_find(&full, L"mfcchk.tbl", L"MS_DEF", NULL);
            fp = _wfopen(full.GetWCharCP(), L"w");
                fwprintf( fp, L"%ls\n", mfcchk.table);
                fwprintf( fp, L"%d\n", mfcchk.mode);
                fwprintf( fp, L"%ls\n", mfcchk.bat);
                fwprintf( fp, L"%d\n", mfcchk.lv);
                fwprintf( fp, L"%d\n", mfcchk.co);
                fwprintf( fp, L"%d\n", mfcchk.lc);
                fwprintf( fp, L"%d\n", mfcchk.wt);
                fwprintf( fp, L"%d\n", mfcchk.err);
                fwprintf( fp, L"%d\n", mfcchk.zen);
                fwprintf( fp, L"%d\n", mfcchk.han);
                fwprintf( fp, L"%d\n", mfcchk.font);
                fwprintf( fp, L"%d\n", mfcchk.space);
                fwprintf( fp, L"%lf\n", mfcchk.size);
            fclose( fp);

            if( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel,
                L"ファイルの圧縮はしましたか？\n本当に実行しますか？", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
                mdlSystem_loadMdlProgram( TD_MFCCHK, TD_CHK, L"");
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}

/*------------------------------------------------------------------------
**  プッシュボタン（終了） フック関数
------------------------------------------------------------------------*/
Private  void  push_end_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--- 設定値の保存 ---*/
			mdlFile_find(&full, L"mfcchk.tbl", L"MS_DEF", NULL);
			fp = _wfopen(full.GetWCharCP(), L"w");
                fwprintf( fp, L"%ls\n", mfcchk.table);
                fwprintf( fp, L"%d\n", mfcchk.mode);
                fwprintf( fp, L"%ls\n", mfcchk.bat);
                fwprintf( fp, L"%d\n", mfcchk.lv);
                fwprintf( fp, L"%d\n", mfcchk.co);
                fwprintf( fp, L"%d\n", mfcchk.lc);
                fwprintf( fp, L"%d\n", mfcchk.wt);
                fwprintf( fp, L"%d\n", mfcchk.err);
                fwprintf( fp, L"%d\n", mfcchk.zen);
                fwprintf( fp, L"%d\n", mfcchk.han);
                fwprintf( fp, L"%d\n", mfcchk.font);
                fwprintf( fp, L"%d\n", mfcchk.space);
                fwprintf( fp, L"%lf\n", mfcchk.size);
            fclose( fp);

            mdlSystem_exit( exitStatus, 1);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}
