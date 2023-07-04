#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-	        delPointNew.MC				-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../CommonCE/naka8.cpp"
//#include <lok.mc> 
#include "../CommonCE/check_kigen8.cpp"
#include "delPointNew.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT


FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;

ULong           renPos[100];
Dpoint3d        staP, endP;
int				num, exitStatus;
int             renNum, renNv;
Dpoint3d        renPoi[10000],cenPoi[10000];
DELPOINTNEW   	delPointNew;
MSElementDescr  *Tdscr;
UInt32          Tpos,Tflg;
DgnModelRefP	modelRefP, outModelRefP;


/*
typedef struct {
	UInt32		lv, co, wt, gg;
	int         st;
} ELMSYM;
*/
/*-	テーブル設定	-*/
typedef struct {
    int     lv, co, wt, st;
} TABLEINFO;
int         tblInum, cenInum;
TABLEINFO   tblI[100], cenI[100], kuI;

/*-	最初に選択したライン	-*/
typedef struct {
    ULong   Pos;
	UInt32     lv, co, wt;
	int        st;
} LINEINFO;
LINEINFO  linI;

#ifndef Private
#define Private
#endif
/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_table_kukan_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_kukan_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_MAIN_DIALOG,     (PFDialogHook)dialog_hook          },
   { HOOKID_PUSH_TABLE,	     (PFDialogHook)push_table_hook      },
   { HOOKID_PUSH_TABLE_KUKAN,(PFDialogHook)push_table_kukan_hook},
   { HOOKID_PUSH_OK,	     (PFDialogHook)push_ok_hook         },
   { HOOKID_PUSH_KUKAN,	     (PFDialogHook)push_kukan_hook      },
   { HOOKID_PUSH_END,	     (PFDialogHook)push_end_hook        },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int	    read_table( WChar *tblName);
Private int	    read_table_kukan( WChar *tblName);
Private void    startModify( void);
Private	void	get_point( Dpoint3d *pt, int view);
Private void    cleanFunc(void);
Private	int 	syori( void);
Private void	scan_ele( int *num, ULong *Pos, UInt32 lv, UInt32 co, int st, UInt32 wt);
Private void	syori_kukan( void);
Private void	scan_ele_kukan( int *num, ULong *Pos, int type);
Private int     lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber);
Private void	lineString_add_zokusei( MSElement *oele, UInt32 *lv, UInt32 *co, int *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);
Private int     elmSymbChk( MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr *eld);
Private int  	linkage_extract( LinkData *linkageP, LINKSET *linkage);
Private void    create_batu( double angle, Dpoint3d *Poi, DgnDrawMode drawMode);
Private void    create_batu_2(double angle, Dpoint3d *Poi, DgnDrawMode drawMode);
int get_closest_point_a(int *lnv, Dpoint3d *lPoi, Dpoint3d *orig, Dpoint3d *nearPoi, int *segnum, double *leng);
/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	//CExprResult     result;
	//CExprValue      value;
	//#pragma		    resourceID	DELPOINTNEW_ID
	const WChar     REFNM[20] = L"delPointNew_Ref.dgn";
	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];

	DgnModelP           modelP;
	ModelRefIteratorP	iterator;
	DgnFileP            fileP;
	WChar               sBuf[512];
	BeFileName          full;
	int                 cnta;
	WString             cfgVal;
	Int32               ret;
	/*
	#if LOCK == 1
		main_lok( );
	#endif
	*/
	chk_kigen();



	/*--- リソースファイルのオープン -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"DELPOINTNEW のリソースファイルが開きません");
		mdlSystem_exit(exitStatus, 1);
		return;
	}
	/*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	/*--- フック関数の使用宣言 -----------*/
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	/*--- C式定義初期設定 -------------*/
	sprintf(structName, "DELPOINTNEW_INFO");
	sprintf(variableName, "delPointNew");
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName, &delPointNew);

	/*--- テーブルファイルを読み込む -------------*/
	mdlFile_find(&full, L"delPointNew.tbl", L"MS_DEF", NULL);
	if ((fp = _wfopen(full.GetWCharCP(), L"r")) != NULL) {
		fwscanf(fp, L"%ls\n", delPointNew.tblName);
		fwscanf(fp, L"%ls\n", delPointNew.kukanName);
		fwscanf(fp, L"%lf\n", &delPointNew.rr);
		fclose(fp);
	}
	else {
		swprintf(delPointNew.tblName, L"c:\\\0");
	}

    mdlFile_find(&full, REFNM, L"MS_DGNOUT", NULL);
	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		mdlRefFile_getStringParameters(sBuf, sizeof(sBuf), REFERENCE_ATTACHNAME, modelRefP);

		if (wmemcmp(sBuf, REFNM, wcslen(REFNM)) == 0) {
			mdlRefFile_detach(modelRefP);
			break;
		}
	}

	ret = mdlFile_find(&full, REFNM, L"MS_DGNOUT", NULL);
	swprintf(sBuf,L"%ls", full.GetWCharCP());
	if (ret == 0) {
		mdlFile_delete(full.GetWCharCP());
	}
	if (mdlFile_find(&full, L"2D Metric Design.dgn", L"MS_SEEDFILES", NULL) == SUCCESS) {

		mdlFile_copy(sBuf, full.GetWCharCP());
	}
	else {
		mdlOutput_error(L"seed ファイルがありません");
		mdlSystem_exit(exitStatus, 1);
		return;
	}
	//この辺を一度流しておかないとなぜか最初のアタッチの時遅くなる
	DgnPlatform::DgnFileFormatType  pFormat;
	bool            pThreeD;
	mdlWorkDgn_openFile(&modelRefP, &pFormat, &pThreeD, full.GetWCharCP(), NULL, false);
	mdlWorkDgn_saveChanges(modelRefP);
	mdlRefFile_attach(&outModelRefP, full.GetWCharCP(), NULL, NULL, NULL, NULL, 10, NULL, 0, NULL, REF_FILE_LEVEL_DISPLAY_DEFAULT, TRUE, TRUE);
	mdlRefFile_detach(outModelRefP);
	mdlWorkDgn_closeFile(modelRefP);
/*
	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);

	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		modelRefP = mdlModelRefIterator_getFirst(iterator);
		mdlModelRef_getFileName(modelRefP,sBuf,512);

		modelP = modelRefP->GetDgnModelP();
		if (modelP != NULL) {

			fileP = modelRefP->GetDgnFileP();
			ret = wmemcmp(fileP->GetFileName().GetWCharCP(), full.GetWCharCP(), size_t( wcslen(fileP->GetFileName().GetWCharCP())));
			if (ret==0){
				mdlRefFile_detach(modelRefP);
				break;
			}
		}
	}
*/   
	/*--- ダイアログボックスオープン --------*/
	db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
	if ( db_p == NULL){
		mdlOutput_error( L"DELPOINTNEW ダイアログボックスが開けません");
	}

	Tpos = NULL;

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

         mdlFile_parseName( delPointNew.tblName, devP, dirP, NULL, NULL);
		 swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
		 filePath.SetName( delPointNew.tblName);
	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(delPointNew.tblName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（区間テーブル）フック関数
-----------------------------------------------------------------------*/
Private void	push_table_kukan_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
		 swprintf(filter, L"*.tbl");
		 swprintf(title, L"テーブルファイルを選択");

         mdlFile_parseName( delPointNew.kukanName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
		 filePath.SetName( delPointNew.kukanName);
	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(delPointNew.kukanName, L"%ls", filePath.GetWCharCP());
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
      swprintf( info, L"レイヤテーブルファイル %s が読めません ", tblName);
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
                      , &tblI[tblInum].lv, &tblI[tblInum].co, &tblI[tblInum].st, &tblI[tblInum].wt);
         tblInum++;
      }
   }
   fclose( fp_tbl);

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	テーブルファイルの読み込み
-----------------------------------------------------------------------*/
Private  int	read_table_kukan( WChar *tblName)
{
FILE	*fp_tbl;
WChar	s_buf[256], info[256];
int     cnt;

   /*--- レイヤテーブルファイルを開く -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"レイヤテーブルファイル %s が読めません ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      cnt     = 0;
      cenInum = 0;
      while( 1){
         fgetws( s_buf, 255, fp_tbl);
         if ( feof( fp_tbl) || ferror( fp_tbl)){ break;}
         if ( 0 == wcsncmp( s_buf, L";", 1)){  continue;}
         if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

         if ( cnt == 0){
            swscanf( s_buf, L"%d %d %d %d"
                         , &kuI.lv, &kuI.co, &kuI.st, &kuI.wt);
         }
         else{
            swscanf( s_buf, L"%d %d %d %d"
                         , &cenI[cenInum].lv, &cenI[cenInum].co, &cenI[cenInum].st, &cenI[cenInum].wt);
            cenInum++;
         }
         cnt++;
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
BeFileName      full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- 設定値保存 ---*/
         mdlFile_find( &full, L"delPointNew.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%ls\n", delPointNew.tblName);
            fwprintf( fp, L"%ls\n", delPointNew.kukanName);
            fwprintf( fp, L"%lf\n", delPointNew.rr);
         fclose( fp);

         /*--- テーブルファイルを読み込む ---*/
         if ( read_table( delPointNew.tblName) == ERROR){
            break;
         }
		
         num = 0;
         startModify( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	要素選択
------------------------------------------------------------------------*/
Private  void   startModify( void)
{
static	int	searchType[] = { LINE_ELM, LINE_STRING_ELM};

   /*--- MASTERFILEの設定要素タイプのみ選択可能 ---*/
   mdlLocate_noElemNoLocked( );
   mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);

   mdlState_startModifyCommand((StateFunc_Reset)startModify,
	   (MdlFunc_Accept)get_point, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);

   /*----- 選択要素を承認するか拒否するかを判定する ------*/
   LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)lineCheck);

   /*----- 別コマンド発行時の要素記述子クリア ------*/
   StateCallback::SetCommandCleanupFunction((StateFunc_CommandCleanup)cleanFunc);

   /*----- ファイルの先頭から検索 -----*/
   mdlLocate_init( );

   if ( num == 0){
      mdlOutput_error( L"1点目をスナップしてください");

   }
   else{
      mdlOutput_error( L"2点目をスナップしてください");
   }
}


/*----------------------------------------------------------------------+
**	クリックした点の座標取得
+----------------------------------------------------------------------*/
Private	void	get_point( Dpoint3d *pt, int view)
{
MSElement   ele, u;
int         ii, jj, lNum = 0;
double      angle, angle1, angle2;
ULong       *lPos;
UInt32      co, wt;
int         lc;

DgnPlatform::DgnFileFormatType  pFormat;
bool            pThreeD;
BeFileName      full;
int ret;

   Tflg = 0;
   /*--- 1点目 ---*/
   if ( num == 0){

      staP.x = statedata.dPointStack[0].x;
      staP.y = statedata.dPointStack[0].y;
      staP.z = 0;
	  
	  mdlCell_create(&u, NULL, NULL, FALSE);
	  ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
	  ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
	  ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);

	  mdlElement_setSymbology(&u, &co, &wt, &lc);
	  mdlElmdscr_new(&Tdscr, NULL, &u);
	  
      lPos = (ULong*)malloc( 50000 * sizeof( ULong));

      /*--- 1点目要素のレイヤ ---*/
      mdlElement_read( &ele, MASTERFILE, linI.Pos);
      get_layer( &linI.lv, &linI.co, &linI.st, &linI.wt, NULL, &ele);

      /*--- 最初に選択した要素と同レイヤの要素を検索 -----*/
      scan_ele( &lNum, lPos, linI.lv, linI.co, linI.st, linI.wt);

      /*--- 最初に選択した要素と連続する要素を検索 -----*/
      renNum = 1;
      renPos[0] = linI.Pos;
      mdlLinear_extract( renPoi, &renNv, &ele, MASTERFILE);
      get_renzoku( lNum, lPos, &renNv, renPoi, &renNum, renPos);

      /*--- 点の表示 ---*/


	  mdlFile_find(&full, L"delPointNew_Ref.dgn", L"MS_DGNOUT", NULL);
	  ret = mdlWorkDgn_openFile( &modelRefP, &pFormat, &pThreeD, full.GetWCharCP(), NULL, false);
//printf("ret1 %d\n", ret);

	  /*--- 1点目 ---*/
	  angle = atan2( renPoi[1].y - renPoi[0].y, renPoi[1].x - renPoi[0].x);
	  create_batu( angle, &renPoi[0], DRAW_MODE_Normal);

	  /*--- 中間点 ---*/
	  for( jj=1; jj<renNv-1; jj++){
	     angle1 = atan2( renPoi[jj+1].y - renPoi[jj].y,   renPoi[jj+1].x - renPoi[jj].x);
		 angle2 = atan2( renPoi[jj].y   - renPoi[jj-1].y, renPoi[jj].x   - renPoi[jj-1].x);
		 angle  = ( angle1 + angle2)/2;
		 create_batu( angle, &renPoi[jj], DRAW_MODE_Normal);
	  }

	  /*--- 最後の点 ---*/
	  angle = atan2( renPoi[renNv-1].y - renPoi[renNv-2].y, renPoi[renNv-1].x - renPoi[renNv-2].x);
	  create_batu( angle, &renPoi[renNv-1], DRAW_MODE_Normal);
      
      num++;

	  Tpos = mdlWorkDgn_write(Tdscr, -1, modelRefP);
//printf("%d\n", mdlErrno);
	  mdlWorkDgn_saveChanges(modelRefP);
      mdlRefFile_attach( &outModelRefP,full.GetWCharCP(),NULL,NULL,NULL,NULL,10,NULL,0,NULL,REF_FILE_LEVEL_DISPLAY_DEFAULT,TRUE,TRUE);
	  mdlLevel_setDisplayPriority(MASTERFILE, linI.lv,-1);
      free( lPos);
      startModify( );
	  Tflg = 1;
	  
   }

   /*--- 2点目 ---*/
   else{

      endP.x = statedata.dPointStack[0].x;
      endP.y = statedata.dPointStack[0].y;
      endP.z = 0;

      /*--- 点の削除処理 ---*/
      syori( );

	  Tflg = 1;
	  cleanFunc();
	  Tflg = 0;

      num--;
      startModify( );
   }
}


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  int   syori( )
{
LINKSET		linkage0, linkage;
MSElement   ele, oele;
int         newNv, rosen0, rosen;
int         siti, eiti, ii;
double      sleng, eleng;
Dpoint3d    *newPoi, nearPoi;

   newPoi = (Dpoint3d*)malloc( 10000 * sizeof( Dpoint3d));

   /*--- 最初に選択した要素 -----*/
   mdlElement_read( &ele, MASTERFILE, linI.Pos);

   /*--- 2点目の位置 -----*/
   if (get_closest_point_a(&renNv, renPoi, &endP, &nearPoi, &eiti, &eleng) == ERROR) {
	   free(newPoi);
	   mdlDialog_openInfoBox(L"1点目と2点目がつながっていません");
	   return(ERROR);
   }
   
   if ( get_leng( &endP, &nearPoi) >= 1){
      mdlDialog_openInfoBox( L"1点目と2点目がつながっていません");
   }

   else{
      /*--- 1点目の位置 -----*/
      if (get_closest_point_a(&renNv, renPoi, &staP, &nearPoi, &siti, &sleng) == ERROR) {
		  free(newPoi);
		  return(ERROR);
	  }

      /*--- 始点側をS 終点側をEとする -----*/
      if ( siti > eiti || ( siti == eiti && sleng > eleng)){
         exchangei( &siti,  &eiti);
         exchanged( &sleng, &eleng);
         exchanged3( &staP, &endP);
      }

      /*--- キーポイントで押さえている場合 -----*/
      if ( get_leng( &staP, &renPoi[siti+1]) == 0){
         siti++;
      }

      /*--- 1点目と2点目の間の点を省く -----*/
      /*--- 1点目以前の点 -----*/
      for( ii=0; ii<=siti; ii++){
         memcpy( &newPoi[ii], &renPoi[ii], sizeof( Dpoint3d));
      }

      /*--- 2点目以降の点 -----*/
      for( ii=eiti+1; ii<renNv; ii++){
         memcpy( &newPoi[ii+siti-eiti], &renPoi[ii], sizeof( Dpoint3d));
      }
      newNv = renNv + siti - eiti;

      /*--- 元要素のユーザデータリンク（路線番号） -----*/
      mdlElement_read( &oele, MASTERFILE, renPos[0]);
      mdlLinkage_extractFromElement( NULL, &oele, 16, RSCID_DataDef_Link, NULL
                                   , (LinkageFunc)linkage_extract, &linkage0);
      rosen0 = atoi( linkage0.rosen);

      for( ii=1; ii<renNum; ii++){
         mdlElement_read( &oele, MASTERFILE, renPos[ii]);
         mdlLinkage_extractFromElement( NULL, &oele, 16, RSCID_DataDef_Link, NULL
                                      , (LinkageFunc)linkage_extract, &linkage);
         rosen = atoi( linkage.rosen);

         /*--- 路線番号が一致しない場合 -----*/
         if ( rosen != rosen0){
            mdlDialog_openInfoBox( L"路線番号属性が一致しません");
            free( newPoi);
            return( ERROR);
         }
      }

      /*--- 元要素を削除 -----*/
      for( ii=0; ii<renNum; ii++){
         mdlElement_read( &oele, MASTERFILE, renPos[ii]);
         mdlElement_undoableDelete( &oele, renPos[ii], TRUE);
      }
	  	  
      /*--- 点を間引いて配置 -----*/
      lineString_add_zokusei( &ele, &linI.lv, &linI.co, &linI.st, &linI.wt, NULL, newPoi, &newNv);
   }

  
   free( newPoi);
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	結線の検索
-----------------------------------------------------------------------*/
Private  void	scan_ele( int *numidx, ULong *Pos, UInt32 lv, UInt32 co, int st, UInt32 wt)
{
ExtScanList	    scanlist;
MSElementDescr	*dscr;
ULong		    eofpos, filepos, elemAddr[256];
int			    ii, scanwords, status, numAddr;
ELMSYM          tmpElm;

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
   mdlScan_initialize(MASTERFILE, &scanlist);
   filepos    = 0L;
   scanwords  = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

         /*--- レイヤが一致する場合 -----*/
         if ( tmpElm.lv == lv && tmpElm.co == co
           && tmpElm.st == st && tmpElm.wt == wt){
            Pos[*numidx] = elemAddr[ii];
            *numidx      = *numidx + 1;
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( status != BUFF_FULL){ break;}
   }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（区間）フック関数
-----------------------------------------------------------------------*/
Private void	push_kukan_hook( DialogItemMessage *dimP)
{
BeFileName      full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- 設定値保存 ---*/
		 mdlFile_find(&full, L"kukan.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%ls\n", delPointNew.tblName);
            fwprintf( fp, L"%ls\n", delPointNew.kukanName);
            fwprintf( fp, L"%lf\n", delPointNew.rr);
         fclose( fp);

         /*--- テーブルファイルを読み込む ---*/
         if ( read_table_kukan( delPointNew.kukanName) == ERROR){
            break;
         }

         syori_kukan( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	処理（区間）
-----------------------------------------------------------------------*/
Private void	syori_kukan( )
{
MSElementDescrP  kuDscr, cenDscr, oldDscr;
//MSElementDescrCP  oldDscr;
MSElement       ele;
int             ii, jj, kk, ll, index, kuNum, cenNum, cenNv, delNum;
ULong           *kuPos, *cenPos;
Dpoint3d        *cenPoi1, isPnt1, isPnt2;

   kuPos  = (ULong*)malloc( 10000 * sizeof( ULong));
   cenPos = (ULong*)malloc( 10000 * sizeof( ULong));
   

   /*--- 区間割線・中心線検索 -----*/
   kuNum = cenNum = 0;
   scan_ele_kukan( &kuNum, kuPos, 1);
   scan_ele_kukan( &cenNum, cenPos, 2);

   /*--- 中心線 ---*/
   for( ii=0; ii<cenNum; ii++){
      mdlElmdscr_read( &cenDscr, cenPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &oldDscr, cenDscr);
      mdlElement_read( &ele, MASTERFILE, cenPos[ii]);
      mdlElmdscr_stroke( &cenPoi1, &cenNv, cenDscr, 0.001);
	  if ( cenNv != 0) {
		  for (jj = 0; jj < cenNv; jj++) {
			  memcpy(&cenPoi[jj], &cenPoi1[jj], sizeof(Dpoint3d));
		  }
		  //free(cenPoi1);
		  dlmSystem_mdlFree(cenPoi1);
	  }

      /*--- 区間割線 ---*/
      delNum = 0;
      for( jj=0; jj<kuNum; jj++){
         mdlElmdscr_read( &kuDscr, kuPos[jj], MASTERFILE, 0, NULL);

         if ( mdlIntersect_closestBetweenElms( &isPnt1, &isPnt2, &index, cenDscr, kuDscr, NULL, &cenPoi[0], 1) == SUCCESS){
            /*--- 中心線の各点 ---*/
            for( kk=1; kk<cenNv-1; kk++){
               if ( get_leng( &isPnt1, &cenPoi[kk]) < 5){
                  mdlElmdscr_freeAll( &cenDscr);
                  for( ll=kk; ll<cenNv-1; ll++){
                     memcpy( &cenPoi[ll], &cenPoi[ll+1], sizeof( Dpoint3d));
                  }
                  delNum++;
                  cenNv--;
                  mdlElmdscr_createFromVertices( &cenDscr, &ele, cenPoi, cenNv, FALSE, 0);
                  break;
               }
            }
         }

         mdlElmdscr_freeAll( &kuDscr);
      }
      if ( delNum > 0){
         mdlElmdscr_rewrite( cenDscr, oldDscr, cenPos[ii]);
      }

	  //free(cenPoi1);
      mdlElmdscr_freeAll( &oldDscr);
      mdlElmdscr_freeAll( &cenDscr);
   }
   free( cenPos);
   free( kuPos);
}


/*-----------------------------------------------------------------------
**	結線の検索
-----------------------------------------------------------------------*/
Private  void	scan_ele_kukan( int *numidx, ULong *Pos, int type)
{
ExtScanList	    scanlist;
MSElementDescr	*dscr;
ULong		    eofpos, filepos, elemAddr[256];
int			    ii, jj, scanwords, status, numAddr;
ELMSYM          tmpElm;

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
   mdlScan_initialize(MASTERFILE, &scanlist);
   filepos    = 0L;
   scanwords  = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

         /*--- 区間割線 -----*/
         if ( type == 1){
            /*--- レイヤが一致する場合 -----*/
            if ( tmpElm.lv == kuI.lv && tmpElm.co == kuI.co
              && tmpElm.st == kuI.st && tmpElm.wt == kuI.wt){
               Pos[*numidx] = elemAddr[ii];
               *numidx      = *numidx + 1;
            }
         }

         /*--- 中心線 -----*/
         else{
            for( jj=0; jj< cenInum; jj++){
               /*--- レイヤが一致する場合 -----*/
               if ( tmpElm.lv == cenI[jj].lv && tmpElm.co == cenI[jj].co
                 && tmpElm.st == cenI[jj].st && tmpElm.wt == cenI[jj].wt){
                  Pos[*numidx] = elemAddr[ii];
                  *numidx      = *numidx + 1;
               }
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


/*----------------------------------------------------------------------+
| 	指示要素のﾌｧｲﾙﾎﾟｼﾞｼｮﾝ、始点位置を取得				|
+----------------------------------------------------------------------*/
Private int lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber)
{
int            rtn = LOCATE_ELEMENT_REJECT, sw, ii;
UInt32          readFilePos;
MSElementDescr *edP = NULL;
ELMSYM         tmpElm;

   /*--- 選択した要素を読む -----*/
   mdlElmdscr_read( &edP, filePosition, MASTERFILE, FALSE, &readFilePos);

   /*--- 選択した要素のｼﾝﾎﾞﾛｼﾞを読む -----*/
   mdlElmdscr_operation( edP, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

   sw = 0;

   /*--- 1点目 -----*/
   if ( num == 0){
      /*--- テーブル -----*/
      for( ii=0; ii<tblInum; ii++){
         /*--- レイヤが一致する場合 -----*/
         if ( ( tblI[ii].lv == -1 || tmpElm.lv == tblI[ii].lv)
           && ( tblI[ii].co == -1 || tmpElm.co == tblI[ii].co)
           && ( tblI[ii].st == -1 || tmpElm.st == tblI[ii].st)
           && ( tblI[ii].wt == -1 || tmpElm.wt == tblI[ii].wt)){
            sw++;
            break;
         }
      }
   }

   /*--- 2点目 -----*/
   else{
      if ( tmpElm.lv == linI.lv && tmpElm.co == linI.co
        && tmpElm.st == linI.st && tmpElm.wt == linI.wt){
         sw++;
      }
   }

   /*--- 選択した要素のｼﾝﾎﾞﾛｼﾞがﾃｰﾌﾞﾙと一致しない場合（拒否） -----*/
   if ( !sw){
      mdlElmdscr_freeAll( &edP);

      /*--- 1点目 -----*/
      if ( num == 0){
         mdlDialog_openInfoBox( L"設定レイヤのラインを選んで下さい");
      }

      /*--- 2点目 -----*/
      else{
         mdlDialog_openInfoBox( L"1点目と同レイヤのラインを選んで下さい");
      }
      return( rtn);
   }

   /*--- 選択した要素のﾌｧｲﾙﾎﾟｼﾞｼｮﾝ -----*/
   if ( num == 0){
      linI.Pos = filePosition;
   }

   if ( NULL != edP)    mdlElmdscr_freeAll( &edP);

   /*--- 承認 -----*/
   rtn = LOCATE_ELEMENT_ACCEPT;
   return( rtn);
}


/*------------------------------------------------------------------------------------------------*/
/*--- 複数ラインストリングの配置（属性を保持したまま） -----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
Private void	lineString_add_zokusei( MSElement *oele, UInt32 *lv, UInt32 *co, int *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv)
{
MSElement	ele;
int		    ii, lnv;

   lnv  = *nv;
   while( 1){
      if ( lnv > 80){
         mdlLineString_create( &ele, oele, lPoi, 80);
         mdlElement_setProperties( &ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &ele, co, wt, lc);
         mdlElement_add( &ele);
         mdlElement_display( &ele, DgnDrawMode::DRAW_MODE_Normal);

         for( ii=0; ii<lnv-79; ii++){
            memcpy( &lPoi[ii], &lPoi[ii+79], sizeof( Dpoint3d));
         }
         lnv = lnv - 79;
      }
      else{
         mdlLineString_create( &ele, oele, lPoi, lnv);
         mdlElement_setProperties( &ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &ele, co, wt, lc);
         mdlElement_add( &ele);
         mdlElement_display( &ele, DgnDrawMode::DRAW_MODE_Normal);
         break;
      }
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
**	UDLINKを読む
-----------------------------------------------------------------------*/
Private int  	linkage_extract( LinkData  *linkageP, LINKSET *linkage)
{
   /*--- 路線番号 ---*/
   if ( linkageP->linkage.rosen != 0){
      strcpy( linkage->rosen, linkageP->linkage.rosen);
   }

   /*--- 区間番号 ---*/
   if ( linkageP->linkage.kukan != 0){
      linkage->kukan = linkageP->linkage.kukan;
   }

   /*--- 左延長 ---*/
   if ( linkageP->linkage.lleng != 0){
      linkage->lleng = linkageP->linkage.lleng;
   }

   /*--- 右延長 ---*/
   if ( linkageP->linkage.rleng != 0){
      linkage->rleng = linkageP->linkage.rleng;
   }

   /*--- 幅員 ---*/
   if ( linkageP->linkage.huku != 0){
      linkage->huku = linkageP->linkage.huku;
   }

   /*--- その他幅員 ---*/
   if ( linkageP->linkage.hoka != 0){
      linkage->hoka = linkageP->linkage.hoka;
   }

   /*--- 側溝が道路に付いているか、歩道に付いているか ---*/
   if ( linkageP->linkage.inout != 0){
      linkage->inout = linkageP->linkage.inout;
   }

   return( SUCCESS);
}


/*----------------------------------------------------------------------+
| 	×印の発生				|
+----------------------------------------------------------------------*/
Private void    create_batu( double angle, Dpoint3d *Poi, DgnDrawMode drawMode)
{
MSElement   ele;
MSElementDescr	*dscr;
double      leng;
Dpoint3d    batuPoi[2];

UInt32      lv,co, wt;
int         lc;


   leng         = 500 * delPointNew.rr;
   batuPoi[0].z = batuPoi[1].z = 0;

   batuPoi[0].x = Poi->x + leng * cos( angle + fc_piover4);
   batuPoi[0].y = Poi->y + leng * sin( angle + fc_piover4);
   batuPoi[1].x = Poi->x - leng * cos( angle + fc_piover4);
   batuPoi[1].y = Poi->y - leng * sin( angle + fc_piover4);
   mdlLine_create( &ele, NULL, batuPoi);
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   lv = 0;
   mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   //ele.ehdr.locked = 1;
   ele.hdr.dhdr.props.b.s = 1;
   mdlElmdscr_appendElement(Tdscr, &ele);


   /*
   ele.ehdr.locked = 0;
   ele.hdr.dhdr.props.b.s = 0;
   ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
   mdlElement_setSymbology(&ele, &co, &wt, &lc);
   lv = 3;
   mdlElement_setProperties(&ele, &lv,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   mdlElmdscr_new(&dscr, NULL, &ele);
   Tpos = mdlElmdscr_addByModelRef(dscr, modelRefP);
   printf("%d\n", mdlErrno);
   mdlElmdscr_display(dscr, modelRefP, DRAW_MODE_Normal);
   mdlElmdscr_freeAll(&dscr);
   */

   batuPoi[0].x = Poi->x + leng * cos( angle - fc_piover4);
   batuPoi[0].y = Poi->y + leng * sin( angle - fc_piover4);
   batuPoi[1].x = Poi->x - leng * cos( angle - fc_piover4);
   batuPoi[1].y = Poi->y - leng * sin( angle - fc_piover4);
   mdlLine_create( &ele, NULL, batuPoi);
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   lv = 0;
   mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   //ele.ehdr.locked = 1;
   ele.hdr.dhdr.props.b.s = 1;
   mdlElmdscr_appendElement(Tdscr, &ele);



}
/*----------------------------------------------------------------------+
| 	×印の発生				|
+----------------------------------------------------------------------*/
Private void    create_batu_2(double angle, Dpoint3d *Poi, DgnDrawMode drawMode)
{
	MSElement   ele;
	MSElementDescr	*dscr;
	double      leng;
	Dpoint3d    batuPoi[2];


	leng = 500 * delPointNew.rr;
	batuPoi[0].z = batuPoi[1].z = 0;

	batuPoi[0].x = Poi->x + leng;
	batuPoi[0].y = Poi->y + leng;
	batuPoi[1].x = Poi->x - leng;
	batuPoi[1].y = Poi->y - leng;
	mdlLine_create(&ele, NULL, batuPoi);

	msTransientElmP = mdlTransient_addElement(NULL, &ele, 0, 0xffff, drawMode, false, false, true);

	batuPoi[0].x = Poi->x - leng;
	batuPoi[0].y = Poi->y + leng;
	batuPoi[1].x = Poi->x + leng;
	batuPoi[1].y = Poi->y - leng;
	mdlLine_create(&ele, NULL, batuPoi);
	msTransientElmP = mdlTransient_addElement(NULL, &ele, 0, 0xffff, drawMode, false, false, true);


}
/*------------------------------------------------------------------------------------------------*/
/*--- ある点から1番近いライン上の点とその点のライン上での位置 　（スナップ位置に変更）----------*/
/*------------------------------------------------------------------------------------------------*/
int get_closest_point_a(int *lnv, Dpoint3d *lPoi, Dpoint3d *orig, Dpoint3d *nearPoi, int *segnum, double *leng)
{
	MSElement	ele;
	DSegment3d	seg;
	Dpoint3d	*tmpPoi, tmpnearPoi;
	int 		ii, jj, tmpnv, tmpseg;
	double		tmpleng, minleng;

	int ret;
	

	tmpnv = *lnv;
	tmpPoi = (DPoint3d*)malloc(50000 * sizeof(DPoint3d));

	/*--- 元のラインの座標を保存 ---*/
	for (ii = 0; ii < tmpnv; ii++) {
		memcpy(&tmpPoi[ii], &lPoi[ii], sizeof(Dpoint3d));
	}

	minleng = 9999999;
	tmpseg = 0;
	while (1) {

		/*--- 最短距離の点 ---*/
		mdlLineString_create(&ele, NULL, tmpPoi, tmpnv);
		memcpy(&tmpnearPoi, orig, sizeof(Dpoint3d));
		//		ret = mdlLinear_getClosestSegment(&seg, &tmpnearPoi, &ele);
		for (jj = 0; jj < tmpnv; jj++) {
			if (orig->x == lPoi[jj].x && orig->y == lPoi[jj].y) {
				ret = jj - 1;
				if (ret <= 0) { ret = 0; }
				break;
			}
		}
		if (jj >= tmpnv) {
		    free(tmpPoi);
		    return(ERROR);
	    }

		/*--- 最短距離の点の位置 ---*/
		tmpleng = sqrt(pow(orig->x - tmpnearPoi.x, 2) + pow(orig->y - tmpnearPoi.y, 2));
		if (tmpleng < minleng) {
			minleng = tmpleng;
			memcpy(nearPoi, &tmpnearPoi, sizeof(Dpoint3d));
			for (jj = 0; jj < tmpnv; jj++) {
				if (sqrt(pow(tmpPoi[jj].x - lPoi[ret].x, 2) + pow(tmpPoi[jj].y - lPoi[ret].y, 2)) < 1) {
					*segnum = tmpseg + jj;
					memcpy(nearPoi, &tmpnearPoi, sizeof(Dpoint3d));
					*leng = sqrt(pow(nearPoi->x - lPoi[ret].x, 2) + pow(nearPoi->y - lPoi[ret].y, 2));
					break;
				}
			}
		}
		break;
	}
	free(tmpPoi);
	return(SUCCESS);
}


Private void    cleanFunc(void)
{
	int ret;
	if (Tpos != NULL && Tflg == 1) {

		mdlRefFile_detach(outModelRefP);
	    ret = mdlWorkDgn_delete(Tdscr, Tpos -1, modelRefP);
//printf("ret2 %d\n", ret);
		mdlElmdscr_freeAll(&Tdscr);
		Tpos = NULL;
		mdlWorkDgn_saveChanges(modelRefP);
		mdlWorkDgn_closeFile(modelRefP);
		mdlLevel_setDisplayPriority(MASTERFILE, linI.lv, 0);

	};
}