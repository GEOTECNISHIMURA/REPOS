/*-----------------------------------------------------------------------*/
/*-	ファイル名をMSLINKで与える		-*/
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
#include "../CommonCE/naka8.cpp"
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

FILE			*fp;
MSElement		ele;
int				exitStatus;
LINKSET			*linkset;

#ifndef Private
#define Private
#endif

/*-	プロトタイプ	-*/
bool unload(UnloadProgramReason unloadType);
Private  void	syori( void);
Private  void	kensaku_cell( int *cnum, ULong *cPos);
Private  void	kensaku_line( int *lnum, ULong *lPos);
Private  void	kensaku_text( int *tnum, ULong *tPos);
Private  void	link_element( int num, ULong *Pos, WChar *nameP);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult		result;
CExprValue		value;
RscFileHandle   Rsc;
WChar			info[256], filename[256];

   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"ALLのリソースファイルが開きません");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- アンロード関数セット ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error( L"広域変数が取得できません");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- バッチファイル読み込み ---*/
   if ( ( fp = _wfopen( linkset->bat,L"r")) != NULL){
      while( fscanf( fp, "%ls\n", filename) != EOF){
         if ( mdlSystem_newDesignFile( filename) == SUCCESS){
            syori( );
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

   mdlSystem_exit( exitStatus, 1);
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	syori
-----------------------------------------------------------------------*/
Private	void	syori( )
{
WChar		nameP[256];
int			cnum, lnum, tnum;
ULong		*cPos, *lPos, *tPos;

   /*--- 複製モード ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- データベースと接続 ---*/
   mdlDB_activeDatabase( linkset->dbsource);

   cPos = (ULong*)calloc( 100000, sizeof( ULong));
   lPos = (ULong*)calloc( 100000, sizeof( ULong));
   tPos = (ULong*)calloc( 100000, sizeof( ULong));

   /*--- ファイル名 ---*/
   mdlFile_parseName( tcb->dgnfilenm, NULL, NULL, nameP, NULL);

   /*--- セル ---*/
   cnum = 0;
   kensaku_cell( &cnum, cPos);
   link_element( cnum, cPos, nameP);

   /*--- ライン・ラインストリング・シェイプ ---*/
   lnum = 0;
   kensaku_line( &lnum, lPos);
   link_element( lnum, lPos, nameP);

   /*--- テキスト ---*/
   tnum = 0;
   kensaku_text( &tnum, tPos);
   link_element( tnum, tPos, nameP);

   free( tPos);   free( lPos);   free( cPos);
}


/*-----------------------------------------------------------------------
**	セルの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_cell( int *cnum, ULong *cPos)
{
ExtScanList	scanList;
ULong		filePos;
int			type, i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- スキャンリストの初期化 ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;

   /*--- スキャン実行 ---------------*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
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
		  if (type == 2) {
			  cPos[*cnum] = filePos;
			  *cnum = *cnum + 1;
		  }
      }
   }
}


/*-----------------------------------------------------------------------
**	ライン、ラインストリング、シェイプの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_line( int *lnum, ULong *lPos)
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
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
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
		  lPos[*lnum] = filePos;
		  *lnum = *lnum + 1;
	  }
   }
}


/*-----------------------------------------------------------------------
**	テキストの検索
-----------------------------------------------------------------------*/
Private	void	kensaku_text( int *tnum, ULong *tPos)
{
	ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- スキャンリストの初期化 ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[1] |= (unsigned short)TMSK1_TEXT;

   /*--------- スキャン実行 ---------------*/
   filePos = 0;
   if (mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS) {
	   mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
	   mdlSystem_exit(exitStatus, 1);
   }

   while (TRUE) {
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
	   if (Error == 10) { break; }
	   if (Error == 65) { mdlDialog_openInfoBox(L"無効なファイル番号"); }
	   if (Error == 66) { mdlDialog_openInfoBox(L"EOFエラー"); }
	   if (Error == 67) { mdlDialog_openInfoBox(L"スキャンリストに誤り"); }
	   if (Error == 68) { mdlDialog_openInfoBox(L"無効な要素"); }
	   if (Error >= 65) { mdlSystem_exit(exitStatus, 1); }
	   for (i = 0; i < numAddr; i++)
	   {
		   if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		   tPos[*tnum] = filePos;
		   *tnum = *tnum + 1;
	   }
   }
}


/*-----------------------------------------------------------------------
**	リンクする
-----------------------------------------------------------------------*/
Private	void	link_element( int num, ULong *Pos, WChar *nameP)
{
WChar			ins[256];
int				ii;

   /*--- データベースに追加 ---*/
   swprintf( ins, L"insert into %ls (MSLINK) values (%ld)"
               , linkset->table, _wtol( nameP));
   if ( process_SQL( ins) == ERROR){ mdlSystem_exit( exitStatus, 1);}

   /*--- 要素にMSLINKを付加 ---*/
   for( ii=0; ii<num; ii++){
      attach_mslink_filePos( linkset->table, _wtol( nameP), &Pos[ii]);
   }
}
