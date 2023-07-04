#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*----------------------------------------------------------------------*/
/*-     DNGLCHK.MC                                                     -*/
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include <string.h>
#include <math.h>

#include "dnglchk.h"

FILE        *fp;
MSDialog   *db_p;
MSElement   ele;
BeFileName     full;
int         ii, exitStatus;
DNGLCHK     dnglchk;

#ifndef Private
#define Private
#endif
/*-    フック関数   -*/
Private void    dialog_hook( DialogMessage *dmP);
Private void    push_table_hook( DialogItemMessage *dimP);
Private void    push_ok_hook( DialogItemMessage *dimP);
Private void    push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo  uHooks[] = {
   { HOOKID_DNGLCHK_DIALOG, (PFDialogHook)dialog_hook  },
   { HOOKID_PUSH_TABLE,  (PFDialogHook)push_table_hook },
   { HOOKID_PUSH_OK,     (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_END,    (PFDialogHook)push_end_hook   },
};

Private bool   unload(UnloadProgramReason unloadType);
Private void   task_unload( void);
//Private void   startModify( void);
//Private void   set_layer_dnglchk( void);

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



   /*--- リソースファイルを開ける ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"DNGLCHK のリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- アンロード関数セット ---------------*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 ---*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期化設定 ---*/
   sprintf(structName, "DNGLCHK_INFO");
   sprintf(variableName, "dnglchk");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &dnglchk);

   /*--------- 設定値を読み込む -------------*/
   mdlFile_find( &full, L"dnglchk.tbl", L"MS_DEF", NULL);
   if ((fp = _wfopen(full.GetWCharCP(), L"r")) != NULL) {
	  fwscanf( fp, L"%d\n", &dnglchk.lv1);
      fwscanf( fp, L"%d\n", &dnglchk.co1);
      fwscanf( fp, L"%d\n", &dnglchk.lc1);
      fwscanf( fp, L"%d\n", &dnglchk.wt1);
      fwscanf( fp, L"%d\n", &dnglchk.lv2);
      fwscanf( fp, L"%d\n", &dnglchk.co2);
      fwscanf( fp, L"%d\n", &dnglchk.lc2);
      fwscanf( fp, L"%d\n", &dnglchk.wt2);
      fwscanf( fp, L"%lf\n", &dnglchk.size);
      fwscanf( fp, L"%lf\n", &dnglchk.angle);
      fwscanf( fp, L"%ls\n", dnglchk.table);
	  fclose(fp);
   }
   else{
      dnglchk.lv1      = 1;
      dnglchk.co1      = 0;
      dnglchk.lc1      = 0;
      dnglchk.wt1      = 0;
      dnglchk.lv2      = 1;
      dnglchk.co2      = 0;
      dnglchk.lc2      = 0;
      dnglchk.wt2      = 0;
      dnglchk.size     = 1;
      dnglchk.angle    = 0;
	  swprintf(dnglchk.table, L"c:\\\0");
   }


   /*--- ダイアログボックスオープン ---*/
   db_p = mdlDialog_open( NULL, DIALOGID_DNGLCHK);
   if ( db_p == NULL){
      mdlOutput_error( L"DNGLCHK ダイアログボックスが開きません ");
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
**  task_unload
------------------------------------------------------------------------*/
Private  void   task_unload( )
{
   if( mdlSystem_unloadMdlProgram( TD_CHK) == 0){ }
   if( mdlSystem_unloadMdlProgram( TD_SINGLE) == 0){ }
}


/*------------------------------------------------------------------------
**  ダイアログフック関数
------------------------------------------------------------------------*/
Private void   dialog_hook( DialogMessage *dmP)
{
DialogBoxRsc    *dbr_P;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DIALOG_MESSAGE_CREATE:
         dbr_P = dmP->u.create.dialogBoxRP;
         dmP->u.create.interests.updates    = TRUE;
         dmP->u.create.interests.mouses     = TRUE;
         dmP->u.create.interests.dialogFocuses  = TRUE;
         dmP->u.create.interests.itemFocuses    = TRUE;
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
Private void    push_table_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;

    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            mdlSystem_loadMdlProgram( TD_DNGLCHK, TD_FILE,L"");
            mdlDialog_itemsSynch( db_p);
			mdlSystem_unloadMdlProgram(TD_FILE);
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**  プッシュボタン（実行） フック関数
------------------------------------------------------------------------*/
Private  void   push_ok_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         task_unload( );

         /*--- 設定値の保存 ---*/
         mdlFile_find( &full, L"dnglchk.tbl", L"MS_DEF", NULL);
		 fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp, L"%d\n", dnglchk.lv1);
            fwprintf( fp, L"%d\n", dnglchk.co1);
            fwprintf( fp, L"%d\n", dnglchk.lc1);
            fwprintf( fp, L"%d\n", dnglchk.wt1);
            fwprintf( fp, L"%d\n", dnglchk.lv2);
            fwprintf( fp, L"%d\n", dnglchk.co2);
            fwprintf( fp, L"%d\n", dnglchk.lc2);
            fwprintf( fp, L"%d\n", dnglchk.wt2);
            fwprintf( fp, L"%lf\n", dnglchk.size);
            fwprintf( fp, L"%lf\n", dnglchk.angle);
            fwprintf( fp, L"%ls\n", dnglchk.table);
         fclose( fp);

         if( dnglchk.kind == 0){
             mdlSystem_loadMdlProgram( TD_DNGLCHK, TD_SINGLE, L"");
         }
         else if( dnglchk.kind == 1){
             mdlSystem_loadMdlProgram( TD_DNGLCHK, TD_CHK, L"");
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
         mdlSystem_exit( exitStatus, 1);
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}
