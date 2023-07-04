#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-		シェイプ←→ラインストリング           			-*/
/*-			SHAPTOLS.MC													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../CommonCE/check_kigen8.cpp"
#include "lstoshap.h"

FILE                    *fp;
RscFileHandle           Rsc;
MSDialog                *db_p;
int                     exitStatus;
GLOBAL                  glo;

#ifndef Private
#define Private
#endif

/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_bat_hook( DialogItemMessage *dimP);
Private void	push_bat_hook( DialogItemMessage *dimP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	option_type_hook( DialogItemMessage *dimP);
Private void	option_mode_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_LSTOSHAP_DIALOG,      (PFDialogHook)dialog_hook     },
   { HOOKID_TOGGLE_LSTOSHAP_BAT,  (PFDialogHook)toggle_bat_hook },
   { HOOKID_PUSH_LSTOSHAP_BAT,    (PFDialogHook)push_bat_hook   },
   { HOOKID_PUSH_LSTOSHAP_TABLE,  (PFDialogHook)push_table_hook },
   { HOOKID_OPTION_LSTOSHAP_TYPE, (PFDialogHook)option_type_hook},
   { HOOKID_OPTION_LSTOSHAP_MODE, (PFDialogHook)option_mode_hook},
   { HOOKID_PUSH_LSTOSHAP_OK,     (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_LSTOSHAP_END,    (PFDialogHook)push_end_hook   },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private  void	program_unload( void);

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
        mdlOutput_error(L"LSTOSHAP のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- フック関数の使用宣言 -----------*/
    mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

    /*--------- C式定義初期設定 -------------*/
	sprintf(structName, "GLOBAL_INFO");
	sprintf(variableName, "glo");
    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);

	mdlDialog_publishComplexVariable(setP, structName, variableName, &glo);

    /*--- 設定値を読む ---*/
    mdlFile_find( &full, L"lstoshap.tbl", L"MS_DEF", NULL);
    if ( ( fp = _wfopen(full.GetWCharCP(),L"r")) != NULL){
        fwscanf( fp,L"%d\n", &glo.mode);
        fwscanf( fp,L"%d\n", &glo.option);
        fwscanf( fp,L"%d\n", &glo.dir);
        fwscanf( fp,L"%d\n", &glo.tog);
        fwscanf( fp,L"%ls\n", glo.bat);
        fwscanf( fp,L"%ls\n", glo.table);
		fclose(fp);
    }
    else{
        glo.mode     = 0;
        glo.option   = 0;
        glo.dir      = 1;
        glo.tog      = 0;
		swprintf(glo.bat, L"c:\\\0");
		swprintf(glo.table, L"c:\\\0");

    }


    /*-------- ダイアログボックスオープン --------*/
    db_p = mdlDialog_open( NULL, DIALOGID_LSTOSHAP);
    if ( db_p == NULL){
        mdlOutput_error(L"LSTOSHAP ダイアログボックスが開けません");
    }

    /*-------  バッチが OFF の場合 --------*/
    if ( glo.tog != -1){
        mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
    }

    /*-------  処理がシングルの場合 --------*/
    if ( glo.mode == 0){
        mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
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
**	全て終了 
-----------------------------------------------------------------------*/
Private	void	program_unload( )
{
    if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){
    }
    if ( mdlSystem_unloadMdlProgram( TD_CNV) == 0){
    }
}


/*-----------------------------------------------------------------------
**	ダイアログフック関数
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
**	トグルボタン（バッチ）　フック関数
-----------------------------------------------------------------------*/
Private void	toggle_bat_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            program_unload( );

            /*-------  バッチが ON の場合 --------*/
            if ( glo.tog == -1){
                mdlDialog_itemSetEnabledState( db_p, 1, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
            }

            /*-------  バッチが OFF の場合 --------*/
            else{
                mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
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
            glo.nn = 1;
            mdlSystem_loadMdlProgram( TD_MAIN, TD_FILE,L"");
            mdlDialog_itemsSynch( db_p);
			mdlSystem_unloadMdlProgram(TD_FILE);
            break;

        default:
	    dimP->msgUnderstood = FALSE;
	    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブル）　フック関数
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            program_unload( );
            glo.nn = 2;
            mdlSystem_loadMdlProgram( TD_MAIN, TD_FILE,L"");
            mdlDialog_itemsSynch( db_p);
			mdlSystem_unloadMdlProgram(TD_FILE);
            break;

        default:
	    dimP->msgUnderstood = FALSE;
	    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（タイプ）　フック関数
-----------------------------------------------------------------------*/
Private void	option_type_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( glo.option == 1){
                mdlDialog_itemSetEnabledState( db_p, 7, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 8, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 7, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 8, TRUE, FALSE);
            }
            break;

        default:
	    dimP->msgUnderstood = FALSE;
	    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（モード）　フック関数
-----------------------------------------------------------------------*/
Private void	option_mode_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( glo.mode == 0){
                mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
                glo.tog = 0;
                mdlDialog_itemsSynch( db_p);
                mdlDialog_itemSetEnabledState( db_p, 0, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 0, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
            }
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

            /*--- 設定値を保存 ---*/
            mdlFile_find( &full, L"lstoshap.tbl", L"MS_DEF", NULL);
            fp = _wfopen(full.GetWCharCP(),L"w");
                fwprintf( fp,L"%d\n", glo.mode);
                fwprintf( fp,L"%d\n", glo.option);
                fwprintf( fp,L"%d\n", glo.dir);
                fwprintf( fp,L"%d\n", glo.tog);
                fwprintf( fp,L"%ls\n", glo.bat);
                fwprintf( fp,L"%ls\n", glo.table);
            fclose( fp);

            /*--- テーブルで処理 ---*/
            if ( glo.mode == 1){
                /*--- 実行確認 ---*/
                if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel,
                                               L"本当に実行しますか？", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
                    mdlSystem_loadMdlProgram( TD_MAIN, TD_CNV,L"");
                }
            }
            else if ( glo.mode == 0){
                mdlSystem_unloadMdlProgram( TD_SINGLE);
                mdlSystem_loadMdlProgram( TD_MAIN, TD_SINGLE,L"");
            }
            break;

        default:
	    dimP->msgUnderstood = FALSE;
	    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）　フック関数
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            program_unload( );
            mdlSystem_unloadMdlProgram( TD_SINGLE);
            mdlSystem_exit( exitStatus, 1);
            break;

        default:
	    dimP->msgUnderstood = FALSE;
	    break;
    }
}
