//#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-		削除				                -*/
/*-											-*/
/*-	        DELELE.MC						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/check_kigen8.cpp"
#include "delele.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;


FILE		*fp;
RscFileHandle   Rsc;
MSDialog       *db_p;
BeFileName  fname;
int			exitStatus;
DELSET_INFO   	delset;

#ifndef Private
#define Private
#endif


/*-    フック関数		-*/
Private void	delele_dialog_hook( DialogMessage *dmP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	option_mode_hook( DialogItemMessage *dimP);
Private void	push_bat_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_DELELE_DIALOG, (PFDialogHook)delele_dialog_hook},
   { HOOKID_PUSH_TABLE,	   (PFDialogHook)push_table_hook   },
   { HOOKID_OPTION_MODE,   (PFDialogHook)option_mode_hook  },
   { HOOKID_PUSH_BAT,	   (PFDialogHook)push_bat_hook     },
   { HOOKID_PUSH_OK,	   (PFDialogHook)push_ok_hook      },
   { HOOKID_PUSH_END,	   (PFDialogHook)push_end_hook     },
};


/*-	プロトタイプ	-*/

bool unload(UnloadProgramReason unloadType);

/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
//CExprResult     result;
//CExprValue      value;
SymbolSet		*setP;
//#pragma		resourceID	DELELE_ID



	chk_kigen();


   /*--------- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY ) != SUCCESS ){
      mdlOutput_error(L"deleleのリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- アンロード関数セット ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--------- フック関数の使用宣言 -----------*/
   mdlDialog_hookPublish(sizeof(uHooks)/sizeof(DialogHookInfo),uHooks);

   /*--------- C式定義初期設定 -------------*/
   setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, "DELSET_INFO", "delset", &delset);

    /*--------- 設定値を読み込む -------------*/
    
    
	mdlFile_find(&fname, L"delset.tbl", L"MS_DEF", NULL);
	if ((fp = _wfopen(fname.GetWCharCP(), L"r")) != NULL) {
		fwscanf(fp, L"%s\n", delset.table);
		fwscanf(fp, L"%d\n", &delset.mode);
		fwscanf(fp, L"%s\n", delset.bat);
		fclose(fp);
	}
    else{
        wcscpy( delset.table, L"c:\\");
        delset.mode     = 1;
		wcscpy( delset.bat, L"c:\\");
    }
   

   /*-------- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_DELELE);
   if ( db_p == NULL){
      mdlOutput_error(L"DELELEダイアログボックスが開けません");
   }

    /*--- １ファイルの場合 ---*/
    if ( delset.mode == 0){
        mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
        mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
    }
	
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{

	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**	ダイアログフック関数
-----------------------------------------------------------------------*/
Private	void	delele_dialog_hook( DialogMessage *dmP)
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
         if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){
         }
         if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){
         }
	 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブル）　フック関数
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP )
{
   dimP->msgUnderstood = TRUE;
   int ret;
   switch( dimP->messageType){
      case DITEM_MESSAGE_CREATE:
	 break;

      case DITEM_MESSAGE_BUTTON:
		  ret = mdlSystem_unloadMdlProgram(TD_FILE);
         if ( ret == 0){
         }
         if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){
         }

         delset.nn = 1;
         ret = mdlSystem_loadMdlProgram( TD_DELELE, TD_FILE, L"");
         mdlDialog_itemSynch( db_p, 1);
		 ret = mdlSystem_unloadMdlProgram(TD_FILE);
         break;

      default:
	 dimP->msgUnderstood = FALSE;
	 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（種類）　フック関数
-----------------------------------------------------------------------*/
Private void	option_mode_hook( DialogItemMessage *dimP )
{
   dimP->msgUnderstood = TRUE;

   switch( dimP->messageType){
      case DITEM_MESSAGE_CREATE:
	 break;

      case DITEM_MESSAGE_STATECHANGED:
         if ( delset.mode == 0){
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
         }
         if ( delset.mode == 1){
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
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
Private void	push_bat_hook( DialogItemMessage *dimP )
{
   dimP->msgUnderstood = TRUE;
   int ret;
   switch( dimP->messageType){
      case DITEM_MESSAGE_CREATE:
	 break;

      case DITEM_MESSAGE_BUTTON:
		  ret = mdlSystem_unloadMdlProgram(TD_FILE);
         if ( ret == 0){
         }
         if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){
         }

         delset.nn = 2;
         mdlSystem_loadMdlProgram( TD_DELELE, TD_FILE,L"");
         mdlDialog_itemSynch( db_p, 4);
         break;

      default:
	 dimP->msgUnderstood = FALSE;
	 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）　フック関数
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP )
{
	int ret;
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_CREATE:
	    break;

        case DITEM_MESSAGE_BUTTON:
            if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){
            }
            if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){
            }

			
			
            /*--------- テーブルファイルに書き込む -------------*/
			mdlFile_find(&fname, L"delset.tbl", L"MS_DEF", NULL);
			if ((fp = _wfopen(fname.GetWCharCP(), L"w")) != NULL) {
				fwprintf(fp, L"%s\n", delset.table);
				fwprintf(fp, L"%d\n", delset.mode);
				fwprintf(fp, L"%s\n", delset.bat);
				fclose(fp);
			}

			//swprintf(delset.table, L"C:\\Users\\nishimura\\source\\repos\\delele\\test.tbl");
            if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel,
                 L"ファイルの圧縮はしましたか？\n本当に実行しますか？", MessageBoxIconType::Question                    /*!< Question Mark */
				) == ACTIONBUTTON_OK){
                mdlSystem_loadMdlProgram( TD_DELELE, TD_DEL,L"");
				//ret = mdlSystem_unloadMdlProgram(TD_DEL);
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
Private void	push_end_hook( DialogItemMessage *dimP )
{
   dimP->msgUnderstood = TRUE;

   switch( dimP->messageType){
      case DITEM_MESSAGE_CREATE:
	 break;

      case DITEM_MESSAGE_BUTTON:
         if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){
         }
         if ( mdlSystem_unloadMdlProgram( TD_DEL) == 0){
         }
         mdlSystem_exit( exitStatus, 1);
         break;

      default:
	 dimP->msgUnderstood = FALSE;
	 break;
   }
}
