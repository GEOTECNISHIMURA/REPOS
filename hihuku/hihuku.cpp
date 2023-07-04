#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-	被覆発生プログラム	                -*/
/*-										-*/
/*-	        HIHUKU.MC					-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "../CommonCE/check_kigen8.cpp"
#include "hihuku.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT


FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
MSElementDescr	*elmDP1;
MSElement		ele;
int	        	exitStatus;
int	        	flag;
HIHUKU   		hihuku;

#ifndef Private
#define Private
#endif

/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	push_tlayer_hook( DialogItemMessage *dimP);
Private void	push_blayer_hook( DialogItemMessage *dimP);
Private void	push_klayer_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_HIHUKU_DIALOG,	(PFDialogHook)dialog_hook     },
    { HOOKID_PUSH_TLAYER,	(PFDialogHook)push_tlayer_hook},
    { HOOKID_PUSH_BLAYER,	(PFDialogHook)push_blayer_hook},
    { HOOKID_PUSH_KLAYER,	(PFDialogHook)push_klayer_hook},
    { HOOKID_PUSH_OK,	 	(PFDialogHook)push_ok_hook    },
	{ HOOKID_PUSH_END,	 	(PFDialogHook)push_end_hook   },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private void	program_unload( void);
Private void	startModify( void);
Private void	get_layer( void);


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

   /*--- リソースファイルのオープン ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"HIHUKU のリソースファイルが開きません");
	  mdlSystem_exit(exitStatus, 1);
      return;
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 ---*/
   mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期設定 ---*/
   sprintf(structName, "HIHUKU_INFO");
   sprintf(variableName, "hihuku");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX |
                                        VISIBILITY_CALCULATOR, 0, TRUE);

   mdlDialog_publishComplexVariable(setP, structName, variableName, &hihuku);

   /*--- テーブルファイルオープン ---*/
   mdlFile_find( &full, L"hihuku.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen( full.GetWCharCP(),L"r")) != NULL){
      fwscanf( fp, L"%d\n", &hihuku.scale);
      fwscanf( fp, L"%ls\n", hihuku.cell);
      fwscanf( fp, L"%d\n", &hihuku.tog);
      fwscanf( fp, L"%d\n", &hihuku.tlv);
      fwscanf( fp, L"%d\n", &hihuku.tco);
      fwscanf( fp, L"%d\n", &hihuku.tlc);
      fwscanf( fp, L"%d\n", &hihuku.twt);
      fwscanf( fp, L"%d\n", &hihuku.blv);
      fwscanf( fp, L"%d\n", &hihuku.bco);
      fwscanf( fp, L"%d\n", &hihuku.blc);
      fwscanf( fp, L"%d\n", &hihuku.bwt);
      fwscanf( fp, L"%d\n", &hihuku.klv);
      fwscanf( fp, L"%d\n", &hihuku.kco);
      fwscanf( fp, L"%d\n", &hihuku.klc);
      fwscanf( fp, L"%d\n", &hihuku.kwt);
	  fclose(fp);
   }
   else{
      hihuku.scale   = 1000;
	  swprintf(hihuku.cell, L"c:\\\0");
      hihuku.tog     = 0;
      hihuku.tlv     = 32;
      hihuku.tco     = 0;
      hihuku.tlc     = 0;
      hihuku.twt     = 0;
      hihuku.blv     = 32;
      hihuku.bco     = 1;
      hihuku.blc     = 0;
      hihuku.bwt     = 0;
      hihuku.klv     = 32;
      hihuku.kco     = 2;
      hihuku.klc     = 0;
      hihuku.kwt     = 0;
   }
   

   /*--- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_HIHUKU);
   if ( db_p == NULL){
      mdlOutput_error( L"HIHUKU ダイアログボックスが開けません");
   }

   return;
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	タスクの終了
-----------------------------------------------------------------------*/
Private	void	program_unload( void)
{
   if ( mdlSystem_unloadMdlProgram( TD_SINGLE) == 0){ }
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
**	プッシュボタン（上線レイヤー）  フック関数
-----------------------------------------------------------------------*/
Private void	push_tlayer_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
       case DITEM_MESSAGE_BUTTON:
           flag = 1;
           startModify( );
           break;

       default:
	      dimP->msgUnderstood = FALSE;
	      break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（下線レイヤー）  フック関数
-----------------------------------------------------------------------*/
Private void	push_blayer_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
       case DITEM_MESSAGE_BUTTON:
           flag = 2;
           startModify( );
           break;

       default:
	      dimP->msgUnderstood = FALSE;
	      break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（記号レイヤー）  フック関数
-----------------------------------------------------------------------*/
Private void	push_klayer_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
       case DITEM_MESSAGE_BUTTON:
           flag = 3;
           startModify( );
           break;

       default:
	      dimP->msgUnderstood = FALSE;
	      break;
   }
}


/*-----------------------------------------------------------------------
**	startModify
-----------------------------------------------------------------------*/
Private void    startModify( )
{
   mdlState_startModifyCommand( (StateFunc_Reset)startModify, (MdlFunc_Accept)get_layer, NULL, NULL, NULL, NULL, NULL, TRUE, 0);

   mdlLocate_init( );
   mdlLocate_allowLocked( );
}


/*-----------------------------------------------------------------------
**	レイヤー取得
-----------------------------------------------------------------------*/
Private void	get_layer( void)
{
ULong		filePos;
int			type;
WChar		cellname[7];
DgnModelRef *modelRef;


   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
   mdlElement_read( &ele, modelRef, filePos);
   type = mdlElement_getType( &ele);
   mdlElmdscr_read( &elmDP1, filePos, modelRef, 0, NULL);

   /*--- セルヘッダの場合 ---*/
   if ( type == 2){
      mdlCell_extract( NULL, NULL, NULL, NULL, cellname,MAX_CELLNAME_LENGTH, &ele);
   }

   /*--- 上線 ---*/
   if ( flag == 1){
      mdlElement_getProperties( &hihuku.tlv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
      mdlElement_getSymbology( &hihuku.tco, &hihuku.twt, &hihuku.tlc, &ele);
   }

   /*--- 下線 ---*/
   if ( flag == 2){
      mdlElement_getProperties( &hihuku.blv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
      mdlElement_getSymbology( &hihuku.bco, &hihuku.bwt, &hihuku.blc, &ele);
   }

   /*--- 記号 ---*/
   if ( flag == 3){
      if ( type == 2){
		 mdlElement_getProperties(&hihuku.klv, NULL, NULL, NULL, NULL,
			  NULL, NULL, NULL, &elmDP1->h.firstElem->el);
         mdlElement_getSymbology( &hihuku.kco, &hihuku.kwt, &hihuku.klc, &ele);
         wcscpy( hihuku.cell, cellname);
      }
      else{
         mdlDialog_openInfoBox( L"セルを選択して下さい");
      }
   }

   mdlElmdscr_display( elmDP1, MASTERFILE, DRAW_MODE_Normal);
   mdlElmdscr_freeAll( &elmDP1);
   mdlDialog_itemsSynch( db_p);

   startModify( );
}


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）  フック関数
-----------------------------------------------------------------------*/
Private void    push_ok_hook( DialogItemMessage *dimP)
{
	BeFileName		full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         program_unload( );

         /*--- テーブルファイルに出力 ---*/
         mdlFile_find( &full, L"hihuku.tbl", L"MS_DEF", NULL);
         fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp, L"%d\n", hihuku.scale);
            fwprintf( fp, L"%ls\n", hihuku.cell);
            fwprintf( fp, L"%d\n", hihuku.tog);
            fwprintf( fp, L"%d\n", hihuku.tlv);
            fwprintf( fp, L"%d\n", hihuku.tco);
            fwprintf( fp, L"%d\n", hihuku.tlc);
            fwprintf( fp, L"%d\n", hihuku.twt);
            fwprintf( fp, L"%d\n", hihuku.blv);
            fwprintf( fp, L"%d\n", hihuku.bco);
            fwprintf( fp, L"%d\n", hihuku.blc);
            fwprintf( fp, L"%d\n", hihuku.bwt);
            fwprintf( fp, L"%d\n", hihuku.klv);
            fwprintf( fp, L"%d\n", hihuku.kco);
            fwprintf( fp, L"%d\n", hihuku.klc);
            fwprintf( fp, L"%d\n", hihuku.kwt);
         fclose( fp);

         /*--- セルが存在しない場合 ---*/
         if (mdlCell_existsInLibrary( hihuku.cell) != TRUE){
            mdlDialog_openInfoBox( L"このセルはセルライブラリに存在しません");
         }

         /*--- セルが存在する場合 ---*/
         else{
            mdlSystem_loadMdlProgram( TD_HIHUKU, TD_SINGLE, L"");
         }
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（終了）  フック関数
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
       case DITEM_MESSAGE_BUTTON:
           program_unload( );
           mdlSystem_exit( exitStatus, 1);
           break;

       default:
	      dimP->msgUnderstood = FALSE;
	      break;
   }
}
