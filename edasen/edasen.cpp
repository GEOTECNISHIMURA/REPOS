#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-	        EDASEN.MC				-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "edasen.h"


USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p, *db_p2;
BeFileName		full;
double          max_hukuin, min_hukuin;
ULong           max_mslink, min_mslink, maxPos[5], minPos[5], LPos[4], polPos[10];
int				maxNum, minNum, LNum, polNum, LFile[4], polFile[4];
int				exitStatus;
EDASEN     	    edasen;


/*
typedef struct {
	UInt32	lv, co, wt;
	int     st, gg;
} ELMSYM;
*/
/*-	テーブル設定	-*/
typedef struct {
	UInt32  lv[20], co[20], wt[20];
    int     st[20];
} TABLEINFO;
int         tblInum;
TABLEINFO   tblI;

#ifndef Private
#define Private
#endif

/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_MAIN_DIALOG, (PFDialogHook)dialog_hook    },
   { HOOKID_PUSH_TABLE,	 (PFDialogHook)push_table_hook},
   { HOOKID_PUSH_OK,	 (PFDialogHook)push_ok_hook   },
   { HOOKID_PUSH_END,	 (PFDialogHook)push_end_hook  },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int	    read_table( WChar *tblName);
Private	void 	syori( void);
Private	void 	syori_rosen( int kunum, ULong *roPos, ULong *roMslink);
Private void	scan_ele( int *num, ULong *Pos, ULong *mslink);
Private int     elmSymbChk( MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr *eld);
Private void	delete_flag( int *del, int chknum, ULong *chkPos, int rennum, ULong *renPos);
int             dbLinkFunc(UShort* attributeData, void* params);

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
	char            structName[128];
	char            variableName[128];
	SymbolSet		*setP;
	int ret;
/*
#if LOCK == 1
	main_lok( );
#endif
*/
	chk_kigen( );



   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"EDASEN のリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期設定 -------------*/
   sprintf(structName, "EDASEN_INFO");
   sprintf(variableName, "edasen");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &edasen);

   /*--- テーブルファイルを読み込む -------------*/
   mdlFile_find( &full, L"edasen.tbl", L"MS_DEF", NULL);
   if ( (fp = _wfopen( full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%ls\n", edasen.tblName);
      fwscanf( fp, L"%d\n", &edasen.eda_lv);
      fwscanf( fp, L"%d\n", &edasen.eda_co);
      fwscanf( fp, L"%d\n", &edasen.eda_st);
      fwscanf( fp, L"%d\n", &edasen.eda_wt);
      fwscanf( fp, L"%d\n", &edasen.err_lv);
      fwscanf( fp, L"%d\n", &edasen.err_co);
      fwscanf( fp, L"%d\n", &edasen.err_st);
      fwscanf( fp, L"%d\n", &edasen.err_wt);
      fwscanf( fp, L"%lf\n", &edasen.err_rr);
      fwscanf( fp, L"%lf\n", &edasen.leng);
	  fclose(fp);
   }
   else{
	  swprintf(edasen.tblName, L"c:\\\0");
      edasen.eda_lv     = 62;
      edasen.eda_co     = 0;
      edasen.eda_st     = 0;
      edasen.eda_wt     = 2;
      edasen.err_lv     = 63;
      edasen.err_co     = 0;
      edasen.err_st     = 0;
      edasen.err_wt     = 2;
      edasen.err_rr     = 1;
      edasen.leng       = 10;
   }
   /*
   MSElement ele;
   int           nLinks = 0;
   UShort atrib[256];
   int    length, linkCount,dbchk,dbchk2,ret ;
   int params;
   DatabaseLink  link, *linka;
   WChar dbname[256];
   params=0;
   swprintf(dbname, L" ");
   dbchk = mdlDB_activeDatabase(dbname);
   if (wcscmp(dbname, L"") == 0) {


	   dbchk = DBSERVER_NOT_ACTIVE;
   }
   ret = mdlDB_changeDatabase(DATABASESERVERID_ODBC, L"*");
   mdlElement_read(&ele, MASTERFILE, 4000787);
   mdlDB_extractLinkages(&linka, &linkCount, &ele);
   
   //dbchk2 = mdlDB_activeLinkageMode(L"duplicate");
   //
   //mdlElement_extractAttributes(&length, atrib, &ele);
   //mdlDB_elementFilter(&ele);
   //mdlLinkage_extractFromElement(0, &ele, ODBC_ID, NULL, NULL, (LinkageFunc)dbLinkFunc(atrib,&params), &nLinks);
   //ret = mdlDB_decodeLink(&link, atrib);
   */
   
   /*
   if (mdlDB_changeDatabase(DATABASESERVERID_ODBC, L"*") != SUCCESS)
   {
	   mdlDialog_openInfoBox(L"ﾃﾞｰﾀﾍﾞｰｽに接続できませんでした");
	   mdlSystem_exit(exitStatus, 1);
	   return;
   }
   */
   db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
   if ( db_p == NULL){
      mdlOutput_error( L"EDASEN ダイアログボックスが開けません");
   }
   
}


/*-----------------------------------------------------------------------
**	アンロード関数 
-----------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
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
**	プッシュボタン（テーブル）フック関数
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.tbl");
         swprintf( title,  L"テーブルファイルを選択");

         mdlFile_parseName( edasen.tblName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(edasen.tblName, L"%ls", filePath.GetWCharCP());
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
WChar	s_buf[256], info[256];


   /*--- レイヤテーブルファイルを開く -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"レイヤテーブルファイル %ls が読めません ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      tblInum = 0;
      while( 1){
         fgetws( s_buf, 255, fp_tbl);
         if ( feof( fp_tbl) || ferror( fp_tbl)){ break;}
         if ( 0 == wcsncmp( s_buf, L";", 1)){  continue;}
         if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

         swscanf( s_buf, L"%d %d %d %d"
                      , &tblI.lv[tblInum], &tblI.co[tblInum], &tblI.st[tblInum], &tblI.wt[tblInum]);
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
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- 設定値保存 ---*/
         mdlFile_find(&full, L"edasen.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%ls\n", edasen.tblName);
            fwprintf( fp, L"%d\n", edasen.eda_lv);
            fwprintf( fp, L"%d\n", edasen.eda_co);
            fwprintf( fp, L"%d\n", edasen.eda_st);
            fwprintf( fp, L"%d\n", edasen.eda_wt);
            fwprintf( fp, L"%d\n", edasen.err_lv);
            fwprintf( fp, L"%d\n", edasen.err_co);
            fwprintf( fp, L"%d\n", edasen.err_st);
            fwprintf( fp, L"%d\n", edasen.err_wt);
            fwprintf( fp, L"%lf\n", edasen.err_rr);
            fwprintf( fp, L"%lf\n", edasen.leng);
         fclose( fp);

         /*--- テーブルファイルを読み込む ---*/
         if ( read_table( edasen.tblName) == ERROR){
            break;
         }
         syori( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  void   syori( )
{
ULong   *Pos, *mslink, roMslink[1000], roPos[1000];
int     ii, jj, num, kunum;


   Pos    = (ULong*)malloc( 100000 * sizeof( ULong));
   mslink = (ULong*)malloc( 100000 * sizeof( ULong));

   /*--- 中心線（分割）レイヤ検索 -----*/
   num = 0;
   scan_ele( &num, Pos, mslink);

   /*--- MSLINK順にソート -----*/
db_p2 = mdlDialog_completionBarOpen( L"MSLINKでソート中");
   for( ii=0; ii<num-1; ii++){
mdlDialog_completionBarUpdate( db_p2, L"MSLINKでソート中", 100 * ( ii+1)/num);
      for( jj=ii+1; jj<num; jj++){
         if ( mslink[jj] < mslink[ii]){
            exchangeul( &mslink[ii], &mslink[jj]);
            exchangeul( &Pos[ii],    &Pos[jj]);
         }
      }
   }
mdlDialog_completionBarClose( db_p2);

   /*--- 区間 -----*/
db_p2 = mdlDialog_completionBarOpen( L"枝線処理中");
   kunum = 1;
   for( ii=1; ii<num; ii++){
mdlDialog_completionBarUpdate( db_p2, L"枝線処理中", 100 * ( ii+1)/num);
      /*--- 前の区間と路線が違う -----*/
      if ( mslink[ii]/10000 != mslink[ii-1]/10000){
         /*--- 前の路線を抽出 -----*/
         for( jj=0; jj<kunum; jj++){
            roMslink[jj] = mslink[ii-kunum+jj];
            roPos[jj]    = Pos[ii-kunum+jj];
         }

         /*--- 抽出した路線の処理 -----*/
         syori_rosen( kunum, roPos, roMslink);
         kunum = 0;
      }
      kunum++;
   }
mdlDialog_completionBarClose( db_p2);

   /*--- 最後の路線を抽出 -----*/
   for( jj=0; jj<kunum; jj++){
      roMslink[jj] = mslink[ii-kunum+jj];
      roPos[jj]    = Pos[ii-kunum+jj];
   }

   /*--- 抽出した路線の処理 -----*/
   syori_rosen( kunum, roPos, roMslink);

   free( mslink);
   free( Pos);
}

/*-----------------------------------------------------------------------
**	syori_rosen
-----------------------------------------------------------------------*/
Private	void 	syori_rosen( int kunum, ULong *roPos, ULong *roMslink)
{
MSElementDescr  *edP1, *edP2;
MSElement       ele, nele;
int             ii, rennv, rennum, *del;
ULong           *renPos;
Dpoint3d        *renPoi, newPoi[101], isP1[10], isP2[10];
double          angle, rr;

   renPoi = (Dpoint3d*)malloc( 10000 * sizeof( Dpoint3d));
   renPos = (ULong*)malloc( kunum * sizeof( ULong));
   del    = (int*)calloc( kunum, sizeof( ULong));
   rr     = 1000 * edasen.err_rr;

   /*--- 最初の区間 -----*/
   mdlElement_read( &ele, MASTERFILE, roPos[0]);
   mdlLinear_extract( renPoi, &rennv, &ele, MASTERFILE);
   rennum    = 1;
   renPos[0] = roPos[0];

   /*--- 本線 -----*/
   get_renzoku( kunum, roPos, &rennv, renPoi, &rennum, renPos);
   mdlElmdscr_createFromVertices( &edP1, NULL, renPoi, rennv, FALSE, 0);

   delete_flag( del, kunum, roPos, rennum, renPos);

   /*--- 区間 -----*/
   for( ii=0; ii<kunum; ii++){
      if ( del[ii] > 0){ continue;}

      mdlElement_read( &ele, MASTERFILE, roPos[ii]);
      mdlLinear_extract( renPoi, &rennv, &ele, MASTERFILE);
      rennum    = 1;
      renPos[0] = roPos[ii];

      /*--- 枝線の1区間目を始点側に10m伸ばす -----*/
      memcpy( newPoi, renPoi, rennv * sizeof( Dpoint3d));
      angle       = atan2( newPoi[1].y - newPoi[0].y, newPoi[1].x - newPoi[0].x);
      newPoi[0].x = newPoi[0].x - 1000 * edasen.leng * cos( angle);
      newPoi[0].y = newPoi[0].y - 1000 * edasen.leng * sin( angle);
      mdlElmdscr_createFromVertices( &edP2, NULL, newPoi, rennv, FALSE, 0);

      /*--- 本線と交点有 -----*/
      if ( mdlIntersect_allBetweenElms( isP1, isP2, 10, edP1, edP2, NULL, 0.001) > 0){
         /*--- 枝線を本線との交点まで伸ばす -----*/
         memcpy( &newPoi[0], &isP2[0], sizeof( Dpoint3d));
         mdlLineString_create( &nele, &ele, newPoi, rennv);
         mdlElement_rewrite( &nele, &ele, roPos[ii]);
      }
      /*--- 本線と交点無 -----*/
      else{
         create_errCircle( edasen.err_lv, edasen.err_co, edasen.err_st, edasen.err_wt, rr, &renPoi[0]);
      }
      mdlElmdscr_freeAll( &edP2);

      /*--- 処理前の枝線（1つ目の区間のみ） -----*/
      set_layer( &edasen.eda_lv, &edasen.eda_co, &edasen.eda_st, &edasen.eda_wt, NULL, &ele);
      mdlElement_add( &ele);

      /*--- 枝線 -----*/
      get_renzoku( kunum, roPos, &rennv, renPoi, &rennum, renPos);
      delete_flag( del, kunum, roPos, rennum, renPos);
   }

   mdlElmdscr_freeAll( &edP1);
   free( del);
   free( renPos);
   free( renPoi);
}


/*-----------------------------------------------------------------------
**	中心線（分割）の検索
-----------------------------------------------------------------------*/
Private  void	scan_ele( int *num, ULong *Pos, ULong *mslink)
{
DatabaseLink    *link;
ExtScanList	    scanlist;
MSElementDescr	*dscr;
MSElement       ele;
ULong		    eofpos, filepos, elemAddr[256];
int			    ii, jj, linkCount, scanwords, status, numAddr,ret;
ELMSYM          tmpElm;
int           nLinks = 0;
UShort atrib[256];
int    length;
UShort *buf;
   

   /*--- スキャン設定 -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* 表示可能要素指定	*/
   scanlist.scantype     = NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING;

   /*--- スキャン開始 -----*/
   filepos    = 0L;
   mdlScan_initialize(MASTERFILE, &scanlist);
   scanwords  = sizeof( elemAddr)/sizeof( short);

   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);
		 mdlElement_read(&ele, MASTERFILE, elemAddr[ii]);
         /*--- レイヤが一致する場合 -----*/
         for( jj=0; jj<tblInum; jj++){
			 if (tmpElm.lv == tblI.lv[jj] && tmpElm.co == tblI.co[jj]
				 && tmpElm.st == tblI.st[jj] && tmpElm.wt == tblI.wt[jj]) {

				 ret = mdlDB_extractLinkages(&link, &linkCount, &ele);
				 if (linkCount != 0) {
					 Pos[*num] = elemAddr[ii];
					 mslink[*num] = link->mslink;
					 *num = *num + 1;
			     }
				 dlmSystem_mdlFree(&link);
				 break;
			   
            }
         }
         mdlElmdscr_freeAll( &dscr);
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


/*-----------------------------------------------------------------------
**	レイヤー取得
-----------------------------------------------------------------------*/
int elmSymbChk
(
	MSElement		*el,
	void      		*param,
	int				operation,
	UInt32			offset,
	MSElementDescr	*eld
)
{
	ELMSYM *tmpElm = (ELMSYM*)param;
	mdlElement_getProperties(&tmpElm->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, el);
	mdlElement_getSymbology(&tmpElm->co, &tmpElm->wt, &tmpElm->st, el);

	return(1);
}

/*-----------------------------------------------------------------------
**	デリートフラグ
-----------------------------------------------------------------------*/
Private void	delete_flag( int *del, int chknum, ULong *chkPos, int rennum, ULong *renPos)
{
int		ii, jj;

   for( ii=0; ii<chknum; ii++){
      for( jj=0; jj<rennum; jj++){
         if ( chkPos[ii] == renPos[jj]){
            del[ii]++;
            break;
         }
      }
   }
}

int dbLinkFunc(UShort* attributeData, void* params /* anything you like */)
{
	int*          nLinks = static_cast<int*>(params);
	DatabaseLink  link;
	if (SUCCESS == mdlDB_decodeLink(&link, attributeData))
	{
		// Do something with DatabaseLink
		++*nLinks;
	}

	return 0; // continue processing links
}