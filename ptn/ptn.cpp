#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*----------------------------------------------------------------------*/
/*-		PTN.MC						*/
/*----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <string.h>
#include <math.h>

/*#include <lok.mc>*/
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "ptn.h"

FILE        *fp;
MSDialog    *db_p;
MSElement   ele;
BeFileName	full;
int         ii, exitStatus;

PTN         ptn;

/*-    フック関数   -*/
void	dialog_hook( DialogMessage *dmP);
void	push_table_hook( DialogItemMessage *dimP);
void	push_bat_hook( DialogItemMessage *dimP);
void	push_cel_hook( DialogItemMessage *dimP);
void	option_mode_hook( DialogItemMessage *dimP);
void	toggle_ptlv_hook( DialogItemMessage *dimP);
void	push_ok_hook( DialogItemMessage *dimP);
void	push_end_hook( DialogItemMessage *dimP);

DialogHookInfo	uHooks[] = {
   { HOOKID_PTN_DIALOG,  (PFDialogHook)dialog_hook     },
   { HOOKID_PUSH_TABLE,  (PFDialogHook)push_table_hook },
   { HOOKID_PUSH_BAT,    (PFDialogHook)push_bat_hook   },
   { HOOKID_PUSH_CEL,    (PFDialogHook)push_cel_hook   },
   { HOOKID_OPTION_MODE, (PFDialogHook)option_mode_hook},
   { HOOKID_TOGGLE_PTLV, (PFDialogHook)toggle_ptlv_hook},
   { HOOKID_PUSH_OK,     (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_END,    (PFDialogHook)push_end_hook   },
};

bool	unload(UnloadProgramReason unloadType);
void	reload( int);
void	task_unload(void);



/*-----------------------------------------------------------------------
**	MAIN
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

//	chk_kigen();

    /*--- リソースファイルを開ける ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"PTN のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

	/*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


    /*--- リロード関数のセット ---*/
    /*mdlSystem_setFunction( SYSTEM_RELOAD_PROGRAM, reload);*/

    /*--- フック関数の使用宣言 ---*/
    mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

    /*--- C式定義初期化設定 ---*/
	sprintf(structName, "PTN_INFO");
	sprintf(variableName, "ptn");


    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
    mdlDialog_publishComplexVariable( setP, structName, variableName, &ptn);

    /*--------- 設定値を読み込む -------------*/
    mdlFile_find( &full, L"ptn.tbl", L"MS_DEF", NULL);
    if( ( fp = _wfopen( full.GetWCharCP(), L"r")) != NULL){
        fwscanf( fp, L"%ls\n", ptn.table);
        fwscanf( fp, L"%d\n", &ptn.mode);
        fwscanf( fp, L"%ls\n", ptn.bat);
        fwscanf( fp, L"%ls\n", ptn.cel);
        fwscanf( fp, L"%d\n", &ptn.ptlv);
        fwscanf( fp, L"%d\n", &ptn.lv);
        fwscanf( fp, L"%d\n", &ptn.opt);
        fwscanf( fp, L"%d\n", &ptn.fence);
        fwscanf( fp, L"%d\n", &ptn.bunkatu);
		fclose(fp);
    }
    else{
		swprintf(ptn.table, L"c:\\\0");
        ptn.mode     = 1;
		swprintf(ptn.bat, L"c:\\\0");
		swprintf(ptn.cel, L"c:\\\0");
        ptn.ptlv     = 1;
        ptn.lv       = 1;
        ptn.opt      = 1;
        ptn.fence    = 0;
        ptn.bunkatu  = 0;
    }


    /*--- ダイアログボックスオープン ---*/
    db_p = mdlDialog_open( NULL, DIALOGID_PTN);
    if( db_p == NULL){
        mdlOutput_error( L"PTN ダイアログボックスが開きません ");
    }

    /*--- １ファイルの場合 ---*/
    if( ptn.mode == 0){
        mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
    }

    if( ptn.mode == 1){
        mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
    }

    /*--- １ファイルの場合 ---*/
    if( ptn.ptlv != -1){
        mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
    }

    return;
}


/*------------------------------------------------------------------------
**	アンロード関数
------------------------------------------------------------------------*/
bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*------------------------------------------------------------------------
**	リロード関数
------------------------------------------------------------------------*/
void   reload( int  reloadType)
{
    mdlDialog_itemsSynch( db_p );
}


/*------------------------------------------------------------------------
**	task_unload
------------------------------------------------------------------------*/
void   task_unload( )
{
    if( mdlSystem_unloadMdlProgram( TD_FILE) == 0){ }
    if( mdlSystem_unloadMdlProgram( TD_MAIN) == 0){ }
}



/*------------------------------------------------------------------------
**	ダイアログフック関数
------------------------------------------------------------------------*/
void   dialog_hook( DialogMessage *dmP)
{
    DialogBoxRsc    *dbr_P;

    dmP->msgUnderstood = TRUE;
    switch( dmP->messageType){
        case DIALOG_MESSAGE_CREATE:
            dbr_P = dmP->u.create.dialogBoxRP;
            dmP->u.create.interests.updates	  = TRUE;
            dmP->u.create.interests.mouses    = TRUE;
            dmP->u.create.interests.dialogFocuses = TRUE;
            dmP->u.create.interests.itemFocuses	  = TRUE;
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
**	プッシュボタン（テーブル）　フック関数
-----------------------------------------------------------------------*/
void	push_table_hook( DialogItemMessage *dimP )
{

    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            ptn.nn = 1;
            mdlSystem_loadMdlProgram( TD_PTN, TD_FILE,L"");
    	/*change( fileName);*/
    	
    	
    	
            mdlDialog_itemsSynch( db_p);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチ）　フック関数
-----------------------------------------------------------------------*/
void	push_bat_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            ptn.nn = 2;
            mdlSystem_loadMdlProgram( TD_PTN, TD_FILE,L"");
    		/*change( fileName);*/
            mdlDialog_itemsSynch( db_p);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（セル）　フック関数
-----------------------------------------------------------------------*/
void	push_cel_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            ptn.nn = 3;
            mdlSystem_loadMdlProgram( TD_PTN, TD_FILE,L""); 
    		/*change( fileName);*/
            mdlDialog_itemsSynch( db_p);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*-----------------------------------------------------------------------
**	オプションボタン（モード）　フック関数
-----------------------------------------------------------------------*/
void	option_mode_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            /*--- 1ファイル ---*/
            if( ptn.mode == 0){
                mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, TRUE, FALSE);
            }

            /*--- バッチ ---*/
            if( ptn.mode == 1){
                mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
                ptn.fence = 0;
                mdlDialog_itemsSynch( db_p);
                mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*-----------------------------------------------------------------------
**	トグルアイテム（パターンレベル）　フック関数
-----------------------------------------------------------------------*/
void	toggle_ptlv_hook( DialogItemMessage *dimP )
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if( ptn.ptlv == -1){
                mdlDialog_itemSetEnabledState( db_p, 9, TRUE, FALSE);
            }

            else{
                mdlDialog_itemSetEnabledState( db_p, 9, FALSE, FALSE);
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**	プッシュボタン（実行） フック関数
------------------------------------------------------------------------*/
void   push_ok_hook( DialogItemMessage *dimP)
{
	int errstat;
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--- 設定値の保存 ---*/
            mdlFile_find( &full, L"ptn.tbl", L"MS_DEF", NULL);
            fp = _wfopen( full.GetWCharCP(), L"w");
                fwprintf( fp, L"%ls\n", ptn.table);
                fwprintf( fp, L"%d\n", ptn.mode);
                fwprintf( fp, L"%ls\n", ptn.bat);
                fwprintf( fp, L"%ls\n", ptn.cel);
                fwprintf( fp, L"%d\n", ptn.ptlv);
                fwprintf( fp, L"%d\n", ptn.lv);
                fwprintf( fp, L"%d\n", ptn.opt);
                fwprintf( fp, L"%d\n", ptn.fence);
                fwprintf( fp, L"%d\n", ptn.bunkatu);
            fclose( fp);

            if( tcb->fence == 0 && ptn.fence == -1){
                if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOK, L"          枠が未定義です。", MessageBoxIconType::Warning)
                      == ACTIONBUTTON_OK){
                    break;
                }
            }
            if( mdlParams_getLock( ACTIVELOCK_FENCECLIP) == 1 && ptn.fence == -1){
                if( mdlDialog_openMessageBox( DIALOGID_MsgBoxOK, L"     切り取りでは使用できません。", MessageBoxIconType::Warning)
                                               == ACTIONBUTTON_OK){
                    mdlSystem_exit( exitStatus, 0);
					return;
                }
            }

            if( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel,
                L"ファイルの圧縮はしましたか？\n本当に実行しますか？", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
                errstat = mdlSystem_loadMdlProgram( TD_PTN, TD_MAIN, L"");
                	
            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**	プッシュボタン（終了） フック関数
------------------------------------------------------------------------*/
void  push_end_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--- 設定値の保存 ---*/
            mdlFile_find( &full, L"ptn.tbl", L"MS_DEF", NULL);
            fp = _wfopen( full.GetWCharCP(), L"w");
                fwprintf( fp, L"%ls\n", ptn.table);
                fwprintf( fp, L"%d\n", ptn.mode);
                fwprintf( fp, L"%ls\n", ptn.bat);
                fwprintf( fp, L"%ls\n", ptn.cel);
                fwprintf( fp, L"%d\n", ptn.ptlv);
                fwprintf( fp, L"%d\n", ptn.lv);
                fwprintf( fp, L"%d\n", ptn.opt);
                fwprintf( fp, L"%d\n", ptn.fence);
                fwprintf( fp, L"%d\n", ptn.bunkatu);
            fclose( fp);

            mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}
