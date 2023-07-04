/*-----------------------------------------------------------------------*/
/*-	    プログラム 					-*/
/*-									-*/
/*-	        LINE3TO4.MC				-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>

#include <ctype.h>
#include <string.h>

#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "line3to4.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog       *db_p;
int				exitStatus;
LINE3TO4   		line3to4;

typedef struct {
    int   lv, co, st, wt;
} TABLEINFO;
int         tblInum;
TABLEINFO   tblI[100];

#ifndef Private
#define Private
#endif

/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_MAIN_DIALOG, (PFDialogHook)dialog_hook    },
   { HOOKID_TOGGLE_IDX,	 (PFDialogHook)toggle_idx_hook},
   { HOOKID_PUSH_IDX,	 (PFDialogHook)push_idx_hook  },
   { HOOKID_PUSH_TABLE,	 (PFDialogHook)push_table_hook},
   { HOOKID_PUSH_OK,	 (PFDialogHook)push_ok_hook   },
   { HOOKID_PUSH_END,	 (PFDialogHook)push_end_hook  },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int	    read_table( WChar *tblName);
Private void	scan_ele( void);

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


   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"LINE3TO4のリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期設定 -------------*/
   sprintf(structName, "LINE3TO4_INFO");
   sprintf(variableName, "line3to4");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &line3to4);

   /*--- テーブルファイルを読み込む -------------*/
   mdlFile_find( &full, L"line3to4.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &line3to4.tog_idx);
      fwscanf( fp, L"%ls\n", line3to4.idxName);
      fwscanf( fp, L"%ls\n", line3to4.tblName);
	  fclose(fp);
   }
   else{
      line3to4.tog_idx    = 0;
	  swprintf(line3to4.idxName, L"c:\\\0");
	  swprintf(line3to4.tblName, L"c:\\\0");
   }


   /*--- ダイアログボックスオープン --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
   if ( db_p == NULL){
      mdlOutput_error( L"LINE3TO4 ダイアログボックスが開けません");
   }

   /*--- １ファイルの場合 ---*/
   if ( line3to4.tog_idx != -1){
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
	 	 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	トグルボタン（IDX）フック関数
-----------------------------------------------------------------------*/
Private void	toggle_idx_hook( DialogItemMessage *dimP)
{


   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- １ファイルの場合 ---*/
         if ( line3to4.tog_idx != -1){
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
         }

         /*--- バッチの場合 ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 1, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（IDX）フック関数
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title,  L"テーブルファイルを選択");

         mdlFile_parseName( line3to4.idxName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf( line3to4.idxName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブル）フック関数
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.tbl");
         swprintf( title,  L"テーブルファイルを選択");

         mdlFile_parseName( line3to4.tblName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(line3to4.tblName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	テーブルファイルの読み込み
-----------------------------------------------------------------------*/
Private  int	read_table( WChar *tblName)
{
FILE	*fp_tbl;
WChar	s_buf[512], info[256];

   /*--- レイヤテーブルファイルを開く -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"レイヤテーブルファイル %ls が読めません ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      tblInum = 0;
      while( 1){
         fgetws( s_buf, 511, fp_tbl);
         if ( feof( fp_tbl) || ferror( fp_tbl)){ break;}
         if ( 0 == wcsncmp( s_buf, L";", 1)){  continue;}
         if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

         swscanf( s_buf, L"%d %d %d %d"
                      , &tblI[tblInum].lv, &tblI[tblInum].co, &tblI[tblInum].st, &tblI[tblInum].wt);
         tblInum++;
      }
   }
   fclose( fp_tbl);

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）フック関数
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{
WChar    *tok, s_buf[512], info[512];
BeFileName		full;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- 設定値保存 ---*/
         mdlFile_find( &full, L"line3to4.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full.GetWCharCP(), L"w");
            fwprintf( fp, L"%d\n", line3to4.tog_idx);
            fwprintf( fp, L"%ls\n", line3to4.idxName);
            fwprintf( fp, L"%ls\n", line3to4.tblName);
         fclose( fp);

         /*--- テーブルファイルを読み込む ---*/
         if ( read_table( line3to4.tblName) == ERROR){
            break;
         }

         /*--- １ファイルの場合 ---*/
         if ( line3to4.tog_idx != -1){
            scan_ele( );
         }

         /*--- バッチの場合 ---*/
         else{
            /*----- 入力ファイルリストを開く -----*/
	        if ( NULL == ( fp = _wfopen( line3to4.idxName, L"r"))){
		       swprintf( info, L"idxファイル %ls が読めません", line3to4.idxName);
               mdlDialog_openInfoBox( info);
		       break;
            }

            /*----- 入力ファイルリストを読む -----*/
            while( 1){
		       fgetws( s_buf, 512, fp);
               if ( feof( fp) || ferror( fp)){ break;}

               if ( 0 == wcsncmp( s_buf, L";", 1)){ continue;}
               tok = wcstok( s_buf, L"\n");

               /*----- dgnを開く -----*/
               if ( mdlSystem_newDesignFile( s_buf) == SUCCESS){
                  scan_ele( );
               }
            }
            fclose( fp);
         }
         mdlDialog_openInfoBox( L"終了しました");
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	結線の検索
-----------------------------------------------------------------------*/
Private  void	scan_ele( )
{
ExtScanList	scanlist;
MSElement	oele, nele;
ULong		eofpos, filepos, elemAddr[256];
int			ii, jj, st, lnv, scanwords, status, numAddr;
UInt        lv, co, wt;
Dpoint3d	lPoi[101];

   /*--- スキャン設定 -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* 表示可能要素指定	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE;

   /*--- スキャン開始 -----*/
   mdlScan_initialize( MASTERFILE, &scanlist);
   filepos   = 0L;
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElement_read( &oele, MASTERFILE, elemAddr[ii]);
         get_layer( &lv, &co, &st, &wt, NULL, &oele);

         for( jj=0; jj<tblInum; jj++){
            if ( ( lv == tblI[jj].lv || tblI[jj].lv == -1)
              && ( co == tblI[jj].co || tblI[jj].co == -1)
              && ( st == tblI[jj].st || tblI[jj].st == -1)
              && ( wt == tblI[jj].wt || tblI[jj].wt == -1)){
               mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);
               mdlLineString_create( &nele, &oele, lPoi, lnv);

               mdlElement_rewrite( &nele, &oele, elemAddr[ii]);
               break;
            }
         }
      }
      if ( status != BUFF_FULL){ break;}
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン(終了）フック関数
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}
