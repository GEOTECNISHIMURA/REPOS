/*-----------------------------------------------------------------------*/
/*-	１ファイル全てリンクさせる		-*/
/*-									-*/
/*-	        ALL.MC					-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

FILE			*fp;
MSElementDescr	*elmDP, *elmDP2;
MSElement		ele;
MS_sqlda		sqlda;
DatabaseLink	*link;
ULong			cPos[100000], Pos, mslink;
WChar			buf[128], info[128], sql[128], ins[128], ccell[100][7];
WChar			cdbf[500][10], ldbf[500][10], tdbf[500][10];
WChar			cell[7];
WChar			tp[10], filename[128], file[128], full[128], zumen[128], ms[128], *aa, *token;
int				ii, jj, cc, ll, tt, cnum, lnum, tnum, elenum, renban, zumennum;
int				len, type, flag, linknum, exitStatus;
UInt32		    clv[500], cco[500], cwt[500];
UInt32			llv[500], lco[500], lwt[500];
UInt32			tlv[500], tco[500], twt[500];
int             clc[500], llc[500], tlc[500];
UInt32          lv, co, wt;
int				lc;
LINKSET			*linkset;

#ifndef Private
#define Private
#endif


/*-	プロトタイプ	-*/
bool unload(UnloadProgramReason unloadType);
Private  void	kensaku_cell( void);
Private  void	kensaku_line( void);
Private  void	kensaku_complex( void);
Private  void	kensaku_text( void);
Private  void	link_judge_cell( void);
Private  void	link_judge_line( void);
Private  void	link_judge_text( void);
Private  void	link_cell( void);
Private  void	link_line( void);
Private  void	link_text( void);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

	BeFileName		cellib;
	CExprResult	result;
	CExprValue	value;
RscFileHandle   Rsc;

   /*--------- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"ALLのリソースファイルが開きません");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- アンロード関数セット ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--------- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"広域変数が取得できません");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- レイヤーテーブルファイルの読み込み ---*/
   if ( ( fp = _wfopen( linkset->layer, L"r")) != NULL){
      cc = 0;  ll = 0;   tt = 0;
      while( fgetws( buf, 128, fp) != NULL){
         /*--- セルの場合 ---*/
         if ( wmemcmp( &buf[0] ,L"C", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls %ls\n"
                      , tp, &clv[cc], &cco[cc], &cwt[cc], &clc[cc], ccell[cc], cdbf[cc]);
            cc++;
         }

         /*--- ラインの場合 ---*/
         if ( wmemcmp( &buf[0] ,L"L", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls\n"
                      , tp, &llv[ll], &lco[ll], &lwt[ll], &llc[ll], ldbf[ll]);
            ll++;
         }

         /*--- テキストの場合 ---*/
         if ( wmemcmp( &buf[0] ,L"T", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls\n"
                      , tp, &tlv[tt], &tco[tt], &twt[tt], &tlc[tt], tdbf[tt]);
            tt++;
         }
      }
      cnum = cc;   lnum = ll;   tnum = tt;
   }
   else{
      swprintf( info, L"%ls が読めません", linkset->layer);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   fclose( fp);

   /*--- バッチの場合 ---*/
   if ( linkset->kind == 0){
      /*--- バッチファイル読み込み ---*/
      if ( ( fp = _wfopen( linkset->bat,L"r") ) != NULL){
         while( fwscanf( fp,L"%ls\n", filename) != EOF){
            if ( mdlSystem_newDesignFile( filename) == SUCCESS){
               wcscpy( full, filename);
               aa    = wcsrchr( full, '\\');
               token = wcstok( aa, L".");
               wcscpy( file, &aa[1]);
               len   = (int)wcslen( file);

               /*--- ファイル名の下何桁か ---*/
               wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
               zumennum = _wtoi( zumen);

               renban = 0;
               kensaku_cell( );
               link_judge_cell( );

               kensaku_line( );
               link_judge_line( );

               kensaku_text( );
               link_judge_text( );
            }
            else{
               mdlSystem_exit( exitStatus, 1);
            }
         }
      }
      else{
         swprintf( info, L"%ls が開けません", linkset->bat);
         mdlDialog_openInfoBox( info);
         mdlSystem_exit( exitStatus, 1);
      }
      fclose( fp);
   }

   /*--- １ファイルの場合 ---*/
   if ( linkset->kind == 1){
      /*--- ファイル名 ---*/
      wcscpy( filename, tcb->dgnfilenm);
      wcscpy( full, filename);
      aa    = wcsrchr( full, '\\');
      token = wcstok( aa, L".");
      wcscpy( file, &aa[1]);
      len   = (int)wcslen( file);

      /*--- ファイル名の下何桁か ---*/
      wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
      zumennum = _wtoi( zumen);

      renban = 0;
      kensaku_cell( );
      link_judge_cell( );

      kensaku_line( );
      kensaku_complex( );
      link_judge_line( );

      kensaku_text( );
      link_judge_text( );
   }

}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	セルの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_cell( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--------- スキャンリストの初期化 ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--------- 検索範囲の設定 ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;

   /*--------- スキャン実行 ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS){ continue; }
		  type = mdlElement_getType(&ele);
		  if (type == 2) {
				cPos[elenum] = filePos;
				elenum++;
		  }
	  }
   }
}


/*-----------------------------------------------------------------------
**	ライン、ラインストリング、シェイプの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_line( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- スキャンリストの初期化 ---------------*/
   mdlScan_initScanlist( &scanList);

   /*---- 検索範囲の設定 ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_SHAPE;

   /*--------- スキャン実行 ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);
		  if (type == 3) {
			  cPos[elenum] = filePos;
			  elenum++;
		  }
		  if (type == 4 || type == 6) {
			  cPos[elenum] = filePos;
			  elenum++;
		  }
	  }
   }
}


/*------------------------------------------------------------------------
**	要素検索
------------------------------------------------------------------------*/
Private  void  kensaku_complex( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- スキャンリストの初期化 ---*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索条件の設定 ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;

   /*--- スキャン実行 ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);
		  mdlElement_getSymbology(&co, &wt, &lc, &ele);
		  mdlElement_getProperties(&lv, NULL, NULL, NULL
			  , NULL, NULL, NULL, NULL, &ele);

		  if (type != 12 && type != 14) { continue; }

		  for (ii = 0; ii < lnum; ii++) {
			  if (lv == llv[ii] && co == lco[ii] && wt == lwt[ii] && lc == llc[ii]) {
				  cPos[elenum] = filePos;
				  elenum++;
				  break;
			  }
		  }
	  }
   }
}


/*-----------------------------------------------------------------------
**	テキストの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_text( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- スキャンリストの初期化 ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.typmask[1] |= (unsigned short)TMSK1_TEXT;

   /*--- スキャン実行 ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);

		  cPos[elenum] = filePos;
		  elenum++;
	  }
   }
}


/*-----------------------------------------------------------------------
**	リンクするかどうかの判定（セル）
-----------------------------------------------------------------------*/
Private	void	link_judge_cell( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"セル %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlCell_extract( NULL, NULL, NULL, NULL, cell,NULL, &ele);

      Pos = cPos[ii];
      mdlElement_read( &ele, MASTERFILE, Pos);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);
      for( jj=0; jj<cnum; jj++){
         if (  lv==clv[jj] && co==cco[jj] && wt==cwt[jj] && lc==clc[jj]
            && wcscmp( cell, ccell[jj]) == 0){
            link_cell( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	リンクするかどうかの判定（ライン）
-----------------------------------------------------------------------*/
Private	void	link_judge_line( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"ライン %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);

      for( jj=0; jj<lnum; jj++){
         if (  lv==llv[jj] && co==lco[jj] && wt==lwt[jj] && lc==llc[jj]){
            link_line( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	リンクするかどうかの判定（テキスト）
-----------------------------------------------------------------------*/
Private	void	link_judge_text( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"テキスト %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);

      for( jj=0; jj<tnum; jj++){
         if (  lv==tlv[jj] && co==tco[jj] && wt==twt[jj] && lc==tlc[jj]){
            link_text( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	セルをリンクする
-----------------------------------------------------------------------*/
Private	void	link_cell( )
{
   /*--- アクティブエンティティの作成 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- データベースとの接続確認 ---*/
   swprintf( sql,L"select * from %ls", cdbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls が開けません", cdbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- MSLINK値 ---*/
   renban++;
   if ( renban < 10){
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- データベース構造を読み込む ---*/
   if ( mdlDB_describeTable( &sqlda, cdbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"データベーステーブルが読み込めません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- SQL文（図面番号出力の場合） ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , cdbf[jj], mslink, zumennum);
   }

   /*--- SQL文（図面番号出力しない場合） ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", cdbf[jj], mslink);
   }

   /*--- データベースに追加する ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"データベースに書き込めません");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- アクティブエンティティを作成 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", cdbf[jj], mslink);

   /*--- アクティブエンティティをアタッチ ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"リンクできません");
      mdlSystem_exit( exitStatus, 1);
   }
}


/*-----------------------------------------------------------------------
**	ラインをリンクする
-----------------------------------------------------------------------*/
Private	void	link_line( )
{
   /*--- アクティブエンティティの作成 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- データベースとの接続確認 ---*/
   swprintf( sql,L"select * from %ls", ldbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls が開けません", ldbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- データベース構造を読み込む ---*/
   if ( mdlDB_describeTable( &sqlda, ldbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"データベーステーブルが読み込めません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- MSLINK値 ---*/
   renban++;
   if ( renban < 10){ 
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- SQL文（図面番号出力の場合） ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , ldbf[jj], mslink, zumennum);
   }

   /*--- SQL文（図面番号出力しない場合） ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", ldbf[jj], mslink);
   }

   /*--- データベースに追加する ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"データベースに書き込めません");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- アクティブエンティティを作成 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", ldbf[jj], mslink);

   /*--- アクティブエンティティをアタッチ ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"リンクできません");
      mdlSystem_exit( exitStatus, 1);
   }
}


/*-----------------------------------------------------------------------
**	テキストをリンクする
-----------------------------------------------------------------------*/
Private	void	link_text( )
{
   /*--- アクティブエンティティの作成 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- データベースとの接続確認 ---*/
   swprintf( sql,L"select * from %ls", tdbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls が開けません", tdbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- MSLINK値 ---*/
   renban++;
   if ( renban < 10){
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- データベース構造を読み込む ---*/
   if ( mdlDB_describeTable( &sqlda, tdbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"データベーステーブルが読み込めません");
      mdlSystem_exit( exitStatus, 1);
	
   }

   /*--- SQL文（図面番号出力の場合） ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , tdbf[jj], mslink, zumennum);
   }
   
   /*--- SQL文（図面番号出力しない場合） ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", tdbf[jj], mslink);
   }

   /*--- データベースに追加する ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"データベースに書き込めません");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- アクティブエンティティを作成 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", tdbf[jj], mslink);

   /*--- アクティブエンティティをアタッチ ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"リンクできません");
      mdlSystem_exit( exitStatus, 1);
   }
}
