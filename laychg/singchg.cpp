/*-----------------------------------------------------------------------*/
/*-		ライン、セル、テキストのレイヤーなどを変える					-*/
/*-			SINGCHG.MC													-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>

#include "../CommonCE/naka8.cpp"
#include "laychg.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE                    *fp, *fp2;
LAYSET                  *layset;
CExprResult             result;
CExprValue              value;
RscFileHandle           Rsc;
MSDialog                *db_p;

WChar                    full[80], buf[128], file[MAXFILELENGTH], filename[128], ins[256], logname[MAXDIRLENGTH];
ExtScanList             scanList;
int                     just, fileNum, Error, getSize, type, linkCount, chgnum;
UInt32					ggnum;
ULong                   filePos, filePos2, celfilePos, filendpos, cPos, complexPos;
ULong 				    cellPos;
MSElementDescr          *elmDP, *elmDP2, *NelmDP, *DelmDP;
MSElement               ele, ele2, elece, cele, elm;
DatabaseLink            *dblink;
int                     ii, jj, ce, len, lo, exitStatus, numelems;
Dpoint3d                scale, origin, center;
WChar                    string[256];
RotMatrix               rMatrix;
TextSize                tileSize;
WChar                   wcs[64];
WChar                    tp[2];
long                    quat[4];
long                    l, s, t;            /*- テーブル中のＬ、Ｓ、Ｔの数 -*/
int                     lc;     /*- 読み込んだ要素のレイヤー -*/
UInt32					lv, co, wt;
int                     clc; /*- チェックサークルのレイヤー -*/
UInt32					clv, cco, cwt;
int                     lcn; /*- 新要素のレイヤー -*/
UInt32					lvn, con, wtn;
WChar                   MSWcell[MAX_CELLNAME_LENGTH];
WChar                    cell[MAX_CELLNAME_BYTES];
int                     moji;
LevelId 					tmpolv, tmpnlv;
int 					status;
WChar 					bufstr[256];

/*-	プロトタイプ	-*/
bool   unload(UnloadProgramReason unloadType);
void	kensaku( void);
void	kensaku_line( void);
void	kensaku_cell( void);
void	kensaku_text( void);
void	create_circle( void);
int cnv_level_name2IDb(LevelId *lvID, WChar *wlvName);
void appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt);
/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

WChar   devP[64], dirP[256], nameP[256];
	
    /*--- リソースファイルのオープン -------------*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error(L"LAYのリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
    }

    /*--- 広域変数の取得 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&layset",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"広域変数が取得できません");
    }
    layset = (LAYSET*)value.val.valPointer;

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    lo = 0;


	/*	画層名->画層ID*/

	status = cnv_level_name2IDb(&tmpolv, layset->olvstr);
	status = cnv_level_name2IDb(&tmpnlv, layset->nlvstr);
	
	layset->olv = tmpolv;
	layset->nlv = tmpnlv;
    /*--- バッチファイル ON の場合 ---*/
    if ( layset->tbat == -1){
        fp2 = _wfopen( layset->bfile, L"r");
        ii = 0;
        while( fwscanf( fp2, L"%ls\n", filename) != EOF){
            if ( memcmp( &filename[0], ";", 1) == 0){
                continue;
            }
            mdlSystem_newDesignFile( filename);

            /*--- ログファイル名 ---*/
            mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
            swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
            fp = _wfopen( logname, L"w");

            kensaku( );

            fclose( fp);
        }
        fclose( fp2);
    }
	
    /*--- バッチファイル OFF の場合 ---*/
    else{
        /*--- ログファイル名 ---*/
        mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
        swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
        fp = _wfopen( logname, L"w");
		
        kensaku( );

        fclose( fp);
    }

    mdlView_updateSingle( 0);

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
**	検索
-----------------------------------------------------------------------*/
void	kensaku( )
{
	BeFileName	filePath, Cfile;
	
	WChar buf[256];
	
   /*--- EOF の ファイルポジション ---*/
    filendpos =mdlElement_getFilePos( FILEPOS_EOF, NULL);

    chgnum = 0;
    fwprintf( fp, L"%ls\n", tcb->dgnfilenm);

    /*--- セルライブラリのアタッチ ---*/
	Cfile.SetName(layset->cfile);
    mdlCell_attachLibrary( filePath, &Cfile, NULL, TRUE);
	
    /*--- ライン、シェイプ、サークルの場合 ---*/
    if ( layset->type == 1){
        kensaku_line( );
    }
	mdlCell_getLibraryName(filePath);
    /*--- セルの場合 ---*/
    if ( layset->type == 0){
		mdlCell_getLibraryName(filePath);
        if ( wcslen( filePath.GetWCharCP()) == 0){
            mdlDialog_openInfoBox( L"セルライブラリにアタッチして下さい");
            mdlSystem_exit( exitStatus, 1);
        }
        kensaku_cell( );
    }

    /*--- 文字の場合 ---*/
    if ( layset->type == 2 || layset->type == 3){
        kensaku_text( );
    }

    if ( chgnum == 0){
        fwprintf( fp, L"  変更なし\n");
    }
}


/*-----------------------------------------------------------------------
|  モジュール名：kensaku_line   スキャン（ライン、シェイプ、サークル）  |
-----------------------------------------------------------------------*/
void	kensaku_line( )
{
    Dpoint3d	lPoi[5000];
    int			lnv;
	int 		scanwords;
	ULong		elemAddr[1];
	int			i;
    fileNum = 0;
    numelems = 0;


/*----------------------------------------------------------------------+
| 検索条件を設定 				                        |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| スキャンリストの初期化等 			                        |
+----------------------------------------------------------------------*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

/*----------------------------------------------------------------------+
| scanListの各メンバーに以下をセットする。 			        |
+----------------------------------------------------------------------*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;*/
    /*scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;

/*----------------------------------------------------------------------+
| 検索対象の決定。		 				        |
+----------------------------------------------------------------------*/
    if ( layset->type == 1){
    	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
        scanList.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING | TMSK0_CMPLX_STRING | TMSK0_CMPLX_SHAPE
    	                     | TMSK0_SHAPE | TMSK0_ELLIPSE;
    }
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
/*----------------------------------------------------------------------+
| ｽｷｬﾝﾘｽﾄの読み込み。 				                        |
+----------------------------------------------------------------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
	filePos   = 0L;
    scanwords = sizeof(elemAddr)/sizeof( short);
/*----------------------------------------------------------------------+
| ｽｷｬﾝ実行。 					                        |
+----------------------------------------------------------------------*/
	
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        /*if ( Error == 10){ break;}*/
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read( &ele, MASTERFILE, elemAddr[0]);

/*----------------------------------------------------------------------+
| 要素にｼﾝﾎﾞﾛｼﾞを与える。 				                |
+----------------------------------------------------------------------*/

        type = mdlElement_getType( &ele);

        /*--- 旧ラインのレイヤーを取得 ---*/
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

		
    	while (TRUE){
        /*--- 旧要素のレイヤーが設定値と違う場合 ---*/
	        if ( lv != layset->olv){ break;}
	        if ( co != layset->oco){ break;}
	        if ( lc != layset->olc){ break;}
	        if ( wt != layset->owt){ break;}

	    	filePos2 = elemAddr[0];
	        memcpy( &ele2, &ele, sizeof( MSElement));

	        /*--- ライン・ラインストリング・シェイプ ---*/
	        if ( type == 3 || type == 4 || type == 6 ){
	            mdlLinear_extract( lPoi, &lnv, &ele, MASTERFILE);
	            center.x = ( lPoi[0].x + lPoi[1].x)/2;
	            center.y = ( lPoi[0].y + lPoi[1].y)/2;
	            center.z = ( lPoi[0].z + lPoi[1].z)/2;
	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
	        }

	        /*--- サークル ---*/
	        if ( type == 15){
	            if (mdlModelRef_is3D(MASTERFILE)){
	                center.x = (double)ele2.ellipse_3d.origin.x;
	                center.y = (double)ele2.ellipse_3d.origin.y;
	                center.z = (double)ele2.ellipse_3d.origin.z;
	            }
	            else{

	                center.x = (double)ele2.ellipse_2d.origin.x;
	                center.y = (double)ele2.ellipse_2d.origin.y;
	                center.z = 0;
	            }
	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
	        }

	        /*--- 新要素のレイヤー ---*/
	        lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;
			
	        if( type == 12 || type == 14){
				
	            mdlElmdscr_read( &elmDP, filePos2, MASTERFILE, FALSE, NULL);
 				mdlElmdscr_duplicate(&NelmDP, elmDP);				
 //   			memcpy(&NelmDP, &elmDP, sizeof(elmDP));
				mdlElmdscr_setProperties( &NelmDP, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	            mdlElmdscr_setSymbology ( &NelmDP, &con,  &lcn, &wtn, NULL);
	            mdlElmdscr_rewrite( NelmDP, elmDP, filePos2);
 	            mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
	            mdlElmdscr_freeAll( &NelmDP);
	            mdlElmdscr_freeAll( &elmDP);

	            mdlElement_read( &elm, MASTERFILE, filePos2);
	            mdlLinear_extract( lPoi, &lnv, &elm, MASTERFILE);
	            center.x = ( lPoi[0].x + lPoi[1].x)/2;
	            center.y = ( lPoi[0].y + lPoi[1].y)/2;
	            center.z = ( lPoi[0].z + lPoi[1].z)/2;

	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);

	        }
	        else{
	        	
	            mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	            mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	            mdlElement_rewrite( &ele2, &ele, filePos2);
	            mdlElement_display( &ele2, DRAW_MODE_Normal);
	        }
    		break;
    	}
    if ( Error == 10){ break;}
    }

}


/*-----------------------------------------------------------------------
|  モジュール名：kensaku_cell  スキャン（セル）                         |
-----------------------------------------------------------------------*/
void	kensaku_cell( )
{
    MSElement           u;
	MSElementDescr      *TelmDP;
    ULong               celendPos;
    bool                solid;
    int                 length;
    UInt16              attrP[MAX_ATTRIBSIZE];
	int 				scanwords;
	ULong				elemAddr[1];
    double              transform2d[2][2];
    double              transform3d[3][3];
	int 				i;
	UInt32              cmpcnt;
	length = 0;
/*----------------------------------------------------------------------+
| 検索条件を設定 				                        |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| スキャンリストの初期化等 			                        |
+----------------------------------------------------------------------*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

/*----------------------------------------------------------------------+
| scanListの各メンバーに以下をセットする。 			        |
+----------------------------------------------------------------------*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;
/*----------------------------------------------------------------------+
| 検索対象の決定。		 				        |
+----------------------------------------------------------------------*/
    if ( layset->type == 0){
    	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
        scanList.typmask[0] = TMSK0_CELL_HEADER;
    }
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--- スキャン実行 ---*/
/*----------------------------------------------------------------------+
| ｽｷｬﾝﾘｽﾄの読み込み。 				                        |
+----------------------------------------------------------------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlDialog_openInfoBox(L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
	filePos   = 0L;
    scanwords = sizeof(elemAddr)/sizeof( short);
    fileNum = 0;

/*----------------------------------------------------------------------+
| ｽｷｬﾝ実行。 					                        |
+----------------------------------------------------------------------*/
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
	
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
    	mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
		/*printf("celpos %ld err %ld filePos %ld filendpos %ld\n",elemAddr[0],Error,filePos,filendpos);*/
/*----------------------------------------------------------------------+
| 要素にｼﾝﾎﾞﾛｼﾞを与える。 				                |
+----------------------------------------------------------------------*/
        type = mdlElement_getType( &ele);

        switch( type){
	    case CELL_HEADER_ELM:
        	
                ce = 1;
        		celfilePos = elemAddr[0];
        	
                memcpy( &elece, &ele, sizeof( MSElement));

                /*--- 旧セルの基準点、rMatrix、スケール、セル名を取得 ---*/
        		mdlCell_extract( &origin, NULL, &rMatrix, &scale, MSWcell, MAX_CELLNAME_LENGTH, &ele);
        	
                /*--- リンケージ（UDLINK、MSLINK）取得 ---*/
                mdlElement_extractAttributes( &length, attrP, &ele);
	    	    /*break;*/

                /*--- セルのレイヤーを取得 ---*/
				
				mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, FALSE, NULL);
				get_layer(&lv, &co, &lc, &wt, NULL, &elmDP->h.firstElem->el);
				mdlElmdscr_freeAll(&elmDP);
        		while (TRUE){
        			
	                /*--- 旧セルのレイヤーが設定値と一致しない場合 ---*/
        			
	                if ( lv != layset->olv){ break;}
	                if ( co != layset->oco){ break;}
	                if ( lc != layset->olc){ break;}
	                if ( wt != layset->owt){ break;}
	                if ( wcscmp( MSWcell, layset->ocell) != 0){ break;}
        			
	                /*--- 新セルのセルライブラリ中のポジションを取得 ---*/
					swprintf(MSWcell, L"%ls", layset->ncell);
	                cellPos = mdlCell_existsInLibrary( MSWcell);
	                /*--- セルがセルライブラリ中に存在しない場合 ---*/
	                if ( cellPos == FALSE){
	                	
	                    lo++;
	                    fwprintf( fp, L"%lsがセルライブラリ中に存在しません。\n", layset->ncell);
	                    break;
	                }

	                /*--- 新要素のレイヤー ---*/
	                lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;

	                /*--- スケール ---*/
	                if ( layset->tscale == -1){
	                    scale.x = scale.y = layset->scale;
	                    if (mdlModelRef_is3D(MASTERFILE)){ scale.z = layset->scale;}
	                    else{                       scale.z = 1;}
	                }

	                /*--- 角度の設定 ---*/
	                if ( layset->tangle == -1){
	                    mdlRMatrix_fromAngle( &rMatrix, layset->angle * fc_pi/180);
	                }

	                /*--- 新セルの配置 ---*/
					ActiveParams::GetValue( lv, ACTIVEPARAM_LEVEL);
					ActiveParams::GetValue( co, ACTIVEPARAM_COLOR);
					ActiveParams::GetValue( wt, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::GetValue( lc, ACTIVEPARAM_LINESTYLE);
					ActiveParams::SetValue( lvn, ACTIVEPARAM_LEVEL);
					ActiveParams::SetValue( con, ACTIVEPARAM_COLOR);
					ActiveParams::SetValue( wtn, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::SetValue( lcn, ACTIVEPARAM_LINESTYLE);

	                cPos = mdlCell_placeCell( &origin, &scale, FALSE, &rMatrix, NULL, ggnum, TRUE, lvn, 0, MSWcell, NULL);
					ActiveParams::SetValue( lv, ACTIVEPARAM_LEVEL);
					ActiveParams::SetValue( co, ACTIVEPARAM_COLOR);
					ActiveParams::SetValue( wt, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::SetValue( lc, ACTIVEPARAM_LINESTYLE);

	                center.x = origin.x;
	                center.y = origin.y;
	                center.z = origin.z;
	                create_circle( );
	                chgnum++;
	                fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);

	                /*--- 新セルヘッダ作成（図形セルの場合） ---*/
	                if ( layset->tcelltype == -1 && layset->ocelltype == 0){
	                    mdlCell_create( &u, MSWcell, &origin, FALSE);
	                }

	                /*--- 新セルヘッダ作成（点セルの場合） ---*/
	                if ( layset->tcelltype == -1 && layset->ocelltype == 1){
	                    mdlCell_create( &u, MSWcell, &origin, TRUE);
	                }

	                /*--- 新セルヘッダ作成（セルタイプは元のままの場合） ---*/
	                if ( layset->tcelltype == 0){
	                    /*--- 旧セルヘッダが点セルの場合 ---*/
	                    if ( mdlCell_isPointCell( &elece) == TRUE){
	                        mdlCell_create( &u, MSWcell, &origin, TRUE);
	                    }
	                    /*--- 旧セルヘッダが図形セルの場合 ---*/
	                    if ( mdlCell_isPointCell( &elece) == FALSE){
	                        mdlCell_create( &u, MSWcell, &origin, FALSE);
	                    }
	                }

	                /*--- スナップ可能にする ---*/
	                if (mdlModelRef_is3D(MASTERFILE)){ u.cell_3d.dhdr.props.b.s = 0;}
	                else{                       u.cell_2d.dhdr.props.b.s = 0;}

	                /*--- ソリッドにする ---*/
	                solid = 0;
	                mdlElement_setProperties( &u, NULL, &ggnum, NULL, NULL, NULL, NULL, NULL, &solid);

	                /*--- 色、線種、線幅の設定 ---*/
	                mdlElement_setSymbology( &u, &layset->nco, &layset->nwt, &layset->nlc);

	                mdlElement_read( &ele, MASTERFILE, cPos);
	                if (mdlModelRef_is3D(MASTERFILE)){
	                    memcpy( u.cell_3d.transform, ele.cell_3d.transform, sizeof( transform3d));
	                }
	                else{
	                    memcpy( u.cell_2d.transform, ele.cell_2d.transform, sizeof( transform2d));
	                }
	                mdlElmdscr_new( &NelmDP, NULL, &u);

	                /*--- 変換前のリンケージ（UDLINK、MSLINK）を与える ---*/
	                mdlElmdscr_appendAttributes( &NelmDP, length, attrP);

					/*--- 新セルヘッダに要素を追加 ---*/
					mdlElmdscr_read(&TelmDP, cPos, MASTERFILE, 0, NULL);
					if (mdlModelRef_is3D(MASTERFILE)) {
						cmpcnt = TelmDP->el.cell_3d.componentCount;
					}
					else {
						cmpcnt = TelmDP->el.cell_2d.componentCount;
					}
					appendElemtoDP(TelmDP, cmpcnt);

					/*--- 元のセル削除 ---*/
					mdlElmdscr_undoableDelete(TelmDP, cPos, TRUE);
					mdlElmdscr_freeAll(&TelmDP);
					
					mdlElmdscr_setSymbology(&NelmDP, &con, &lcn, &wtn, 0);

	                /*--- 新セルの配置 ---*/
					mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, 0, NULL);
	                mdlElmdscr_validate( NelmDP, 0);
	                mdlCell_setRange( NelmDP, ACTIVEMODEL);
        			mdlLinkage_deleteStringLinkage( &NelmDP->el,1,1);
					mdlElmdscr_rewrite( NelmDP, elmDP, celfilePos);
	                mdlElmdscr_display( NelmDP, MASTERFILE, DRAW_MODE_Normal);
					mdlElmdscr_freeAll(&elmDP);
	                mdlElmdscr_freeAll( &NelmDP);

		       break;
        	}
    		break;
        }
    }
}


/*-----------------------------------------------------------------------
**	スキャン（テキスト）
-----------------------------------------------------------------------*/
void	kensaku_text( )
{
TextSizeParam   txtSize;
TextParamWide   txtParamWide;
//WChar			stro[256], strn[256];
int             scanwords;
ULong           elemAddr[1];
int				i;
WString         wstr;
    fileNum = 0;
    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;
	
	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
    scanList.typmask[1] = TMSK1_TEXT;
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--------- スキャン実行 ---------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
    filePos   = 0L;
    scanwords = sizeof( elemAddr)/sizeof( short);
    while( TRUE){
        Error = mdlScan_file( elemAddr, &getSize, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
    	
        type = mdlElement_getType( &ele);

        /*--- 旧テキストのレイヤーを取得 ---*/
        get_layer( &lv, &co, &lc, &wt, &ggnum, &ele);

        /*--- 範囲内の要素を検索 ---*/
        switch( type){
	    	case TEXT_ELM:
        		while( TRUE){	
	                /*--- 旧要素のレイヤーが設定値と違う場合 ---*/
	                if ( lv != layset->olv){ break;}
	                if ( co != layset->oco){ break;}
	                if ( lc != layset->olc){ break;}
	                if ( wt != layset->owt){ break;}

	                /*--- 基準点、文字列、角度、フォント、基準点の位置、文字サイズを取得、文字間隔を取得 ---*/
	                txtSize.mode = TXT_BY_TILE_SIZE;
	                mdlText_extract( &wstr, NULL, &origin, &rMatrix, &txtSize, &txtParamWide, NULL, &ele);
					swprintf(wcs, L"%ls", wstr.GetWCharCP());

	                /*--- 旧要素の文字列が設定値と違う場合 ---*/
	                if ( layset->type == 3){
	                   if ( wcscmp( layset->ostr, wcs) != 0){
	                      break;
	                   }
	                }

	                /*word = mdlElement_igdsSize( &ele, 0);*/
	                /*filePos2 = filePos - 2 * (ULong)word;*/
	        		filePos2 = elemAddr[0];

	                /*--- 新要素のレイヤー ---*/
	                lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;

	                /*--- 新要素の文字列 ---*/
	                if ( layset->type == 3){
	                   wcscpy( wcs, layset->nstr);
	                }

	                /*------ 文字サイズの設定 ------*/
	                if ( layset->tth == -1){ txtSize.size.height = layset->th * 1000;}
	                if ( layset->ttw == -1){ txtSize.size.width  = layset->tw * 1000;}

	                /*------ 角度の設定 ------*/
	                if ( layset->tangle == -1){
	                    mdlRMatrix_fromAngle( &rMatrix, layset->angle * fc_piover180);
	                }

	                /*------ フォントの設定 ------*/
	                if ( layset->tfont == -1){
	                    txtParamWide.font = layset->font;
	                }

	                mdlText_create( &ele2, &ele, wcs, &origin, &rMatrix, &txtSize, &txtParamWide, NULL,MASTERFILE);
	                mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	                mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	                mdlElement_rewrite( &ele2, &ele, filePos2);
	                center.x = origin.x;
	                center.y = origin.y;
	                center.z = origin.z;
	                create_circle( );
	                chgnum++;
	                fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
		        	break;
        		}
	        	break;
        }
    }
}

    
/*-----------------------------------------------------------------------
**	チェック用のサークル作成
-----------------------------------------------------------------------*/
void	create_circle( )
{
    if ( layset->check == -1){
        clv = 63;   cco = 0;   clc = 0;   cwt = 2;
        mdlEllipse_create( &cele, NULL, &center, 2000, 2000, NULL, 0);
        element_add( &clv, &cco, &clc, &cwt, NULL, &cele);
    }
}

int cnv_level_name2IDb(LevelId *lvID, WChar *wlvName)
{

	StatusInt	status;

	/*----- （6-3562） -----*/
	status = mdlLevel_getIdFromName(lvID, MASTERFILE, LEVEL_NULL_ID, wlvName);

	return(status);
}
/*-----------------------------------------------------------------------
**	要素記述子の構成要素を追加する
-----------------------------------------------------------------------*/
void  appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt)
{
	while (cmpcnt != 0)
	{
		if (TelmDP->h.isHeader) {
			appendElemtoDP( TelmDP->h.firstElem, cmpcnt);
			return;
		}
		else
		{
			mdlElmdscr_appendElement(NelmDP, &TelmDP->el);
			cmpcnt--;
			appendElemtoDP(TelmDP->h.next, cmpcnt);
			return;
		}
	}
	return;
}