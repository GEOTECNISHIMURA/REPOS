/*-----------------------------------------------------------------------*/
/*-         MAIN.MC                                                     -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../CommonCE/naka8.cpp"

#include "kansuu.h"
#include "ptn.h"


FILE                *fp_save, *fp_mlpu, *fp_log, *fp_list;
RscFileHandle       Rsc;
int                 count, ii, bat_ct, bat_now, group01;
UInt32              ggNum;
WChar                filename[128], info[256], logfile[256], file[64], battxt[64];

UInt32 		CO, WT, LV;
int         exitStatus, LC, TYP;

UInt32				PTLV[1000], PTCO[1000], PTWT[1000];
int                 PTLC[1000], ptnct, delct, kk, numelems;
UInt32				DELLV[1000], DELCO[1000], DELWT[1000];
int                 DELLC[1000];
UInt32				ORGLV[1000], ORGCO[1000], ORGWT[1000];
int                 ORGLC[1000];
UInt32				CHGLV[1000], CHGCO[1000], CHGWT[1000];
int                 CHGLC[1000];
UInt32				glv[10000], gco[10000], gwt[10000];
int                 glc[10000];
UInt32				afterCo[1000], afterWt[1000];
int                 afterLc[1000];
WChar                PTCEL[1000][7], cellname[100000][7], DEL[1000][5];
double              PTSCALE[1000], scale[100000];
ULong               Position[100000], EndPos, percent, celPos[100000];
bool			    clPos;
MSDialog            *dbP;

Dpoint3d            origin, userOrigin, setOrigin, maxOrigin, minOrigin, points[101], cirPoi;
int                 numEdfields, numVerts;
TextElementJustification just;
TextEDField         edFields;
WChar                string[300];
RotMatrix           rMatrix;
TextParamWide       textstyleInfo;
TextSizeParam       tileSize;
int                 textByte, textMoj;
double              rad, Matrix, angle, hankei, wrka, wrkb;

UShort              vertical;
WChar                stringb[3];
WChar                stringmoj[300];
//MSWideWChar          stringwb[3];
//MSWideWChar          stringwmoj[300];

TextSizeParam       WtextSize;
TextParamWide       WtextParamWide;
TextEDParam         WedPatam;
TextParam           textParam;
TextParamWide       textParamWide;

PTN                 *ptn;

//MSWWChar				MSWcell[MAX_CELLNAME_LENGTH];
ULong				elemAddr[1];
ULong				ssendpos;

/*- プロトタイプ    -*/
bool	unload(UnloadProgramReason unloadType);
void    syori(void);
int     checkstart(void);
void    scanStart(void);
void    pattern(void);
void    cel_del(void);
void    divide(void);
void    set_JustOrigin(void);

void    fenceStart(void);
void    fenceScan(void);
int     fence_ptn(void);
void    chg_ptn(void);
void    chg_layer(void);
int     del_layer(void);
int     fence_divide(void);
int     Text_ExtractShape_a(MSElementP elmP, DPoint3d *points);
void    _GetCenter_a(Dpoint3d *mPoi, Dpoint3d *lPoi1, Dpoint3d *lPoi2);

/*-----------------------------------------------------------------------
**  メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;
    WChar            devP[64], dirP[64], nameP[64];

    /*--- リソースファイルのオープン ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"MAIN のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- アンロード関数セット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


    /*--- 広域変数の取得 ---*/
    if( mdlCExpression_getValue( &value, &result, "&ptn",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"広域変数が取得できません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    ptn = (PTN*)value.val.valPointer;

    /*--- バッチ処理の場合 ---*/
    if( ptn->mode == 1){
        bat_ct = 0;
        bat_now = 0;

        /*--- バッチ処理数の取得 ---*/
        if( ( fp_save = _wfopen( ptn->bat,L"r")) != NULL){
            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                bat_ct++;
            }
			fclose(fp_save);
        }


        if( ( fp_save = _wfopen( ptn->bat,L"r")) != NULL){
            mdlFile_parseName( ptn->bat, devP, dirP, nameP, NULL);
            swprintf( logfile, L"%ls:%ls%ls.log", devP, dirP, nameP);
            if( ( fp_log = _wfopen( logfile, L"w")) == NULL){
                mdlOutput_error( L"出力ファイルを開けません");
				mdlSystem_exit(exitStatus, 1);
				fclose(fp_save);
				return;
            }
            fwprintf( fp_log, L"デザインファイル名（フルパス）                           処理数");
            fwprintf( fp_log, L"\n");
            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                /*--- 処理 ---*/
                if( mdlSystem_newDesignFile( filename) == SUCCESS){
                    bat_now++;
                    syori( );
                    fwprintf( fp_log, L"%-55s %6d\n", tcb->dgnfilenm, ptn->ct);
                }
            }
			fclose(fp_save);
			fclose(fp_log);
        }
        else{
            swprintf( info, L"%ls が見つかりません", ptn->bat);
            mdlDialog_openInfoBox( info);
        }
    }

    /*--- 1ファイルの場合 ---*/
    else{
        syori( );
    }
    mdlSystem_exit( exitStatus, 1);
    return;
}


/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
bool	unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
void    syori(void)
{
    WChar   buf[256], error[256];
	BeFileName fullName, cellName;

    ptnct = 0;
    delct = 0;
    count = 0;

    /*--- 進行状況表示ウィンドウのオープン ---*/
    dbP = mdlDialog_completionBarOpen( L"進行状況");

    /*--- デザインの圧縮 ---*/
//    mdlSystem_compressDgnFile( );

    /*--- セルライブラリのアタッチ ---*/
	cellName.SetName(WString(ptn->cel));
    mdlCell_attachLibrary( fullName, &cellName, NULL, TRUE);

    /*--- ＭＬＰＵテーブルのオープン ---*/
    if( ( fp_mlpu = _wfopen( ptn->table, L"r")) == NULL){
        mdlOutput_error( L"ＭＬＰＵテーブルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    ii = 0;
    while( fgetws( buf, 256, fp_mlpu) != NULL){

        /*--- ";"を読み飛ばす ---*/
        if( wmemcmp( buf, L";", 1) == 0)
           continue;
        else if( wmemcmp( buf, L"#DEL", 4) == 0 ){
            swscanf( buf, L"%ls %d %d %d %d\n", DEL[delct], &DELLV[delct], &DELCO[delct],
                    &DELWT[delct], &DELLC[delct]);
            delct++;
        }
        else if( wmemcmp( buf, L"#CHG", 4) == 0 ){
        }
        else{

            swscanf( buf, L"%d %d %d %d %ls %lf %d %d %d\n", &PTLV[ptnct], &PTCO[ptnct], &PTWT[ptnct],
                    &PTLC[ptnct], PTCEL[ptnct], &PTSCALE[ptnct], &afterCo[ptnct],
                    &afterWt[ptnct], &afterLc[ptnct]);

            /*clPos = mdlCell_getFilePosInLibrary( PTCEL[ptnct]);*/
        	
        		                /*--- 新セルのセルライブラリ中のポジションを取得 ---*/

	        clPos = mdlCell_existsInLibrary(PTCEL[ptnct]);
            if( clPos == FALSE){
                swprintf( error, L"セルライブラリ中に[%ls]が存在しません。\n処理を続行しますか？", PTCEL[ptnct]);
                if( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel, error, MessageBoxIconType::Warning) == ACTIONBUTTON_CANCEL){
                    mdlSystem_exit( exitStatus, 1);
					fclose(fp_mlpu);
					return;
                }
                /*break;*/
            }
            ptnct++;
        }
    }
    checkstart( );
    ptn->ct = count;

    fclose( fp_mlpu);

    /*--- 進行状況表示ウィンドウのクローズ ---*/
	mdlDialog_completionBarClose( dbP);

    /*--- ビュー１の再表示 ---*/
    mdlView_updateSingle( 0);

    /*--- 処理要素数の表示 ---*/
    mdlOutput_printf( MSG_ERROR, L"%d Elements Processed ...", ptn->ct);
}

/*-----------------------------------------------------------------------
**  checkstart
-----------------------------------------------------------------------*/
int checkstart(void)
{
    RscFileHandle        rfHandle;

    /*--- 枠で処理する場合 ---*/
    if( ptn->fence == -1){
        fenceStart( );
        fenceScan( );
    }

    /*--- 全て処理する場合 ---*/
    else{
        scanStart( );
    	
        pattern( );
    	
        cel_del( );
    	
        /*--- 漢字分割ＯＮの場合 ---*/
        if( ptn->bunkatu == -1){
            divide( );
        }
    }
    mdlResource_openFile( &rfHandle, NULL, FALSE);

    return( SUCCESS);
}

/*----------------------------------------------------------------------+
|  モジュール名：scanStart                                              |
+----------------------------------------------------------------------*/
void scanStart ( )
{
    int    Error;
    int    ii, jj, flag1;
	DgnElementClass elmclass, linptn;
	UInt	group;
    ULong  filePos;
	UInt32 readPos;
    ULong  points;

	int 	scanwords;

    MSElement       buf;
    ExtScanList     scanList;
    MSElement       ele;
    MSElementDescr  *NelmDP, *ptnDP, *elmDP, *baseDP;

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
    /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype    = NESTCELL | ELEMTYPE;
    scanList.extendedType = FILEPOS;

    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｰﾌﾟ */
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* 対象はｺﾝﾌﾟﾚｯｸｽﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* 対象はｺﾝﾌﾟﾚｯｸｽｼｪｲﾌﾟ */
	scanwords = sizeof(elemAddr)/sizeof( short);
    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    filePos = 0;   numelems = 0;   count = 0;   ggNum = 0;   elmclass = DgnElementClass::Primary;

	
    /*--- 一番後のファイルポジションを記憶 ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
	ssendpos = EndPos;

    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > EndPos){ break;}
        if( Error == 10 && elemAddr[0] == 0){ break;}
        if( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }
		mdlElement_read( &buf, MASTERFILE, elemAddr[0]);
        /*--- 複合に含まれる要素を飛ばす ---*/
        /*if( numelems != 0){
            numelems--;
            continue;
        }*/

        /*word = mdlElement_igdsSize( &buf, 0);*/

        /*--- 進行状況表示（ 0％～50％ ） ---*/
    	percent =(ULong)(( double)( elemAddr[0] / ( double)EndPos * 50));

        if( ptn->mode == 0){
            wcscpy( battxt, L"進行状況");
        }
        else if( ptn->mode == 1){
            swprintf( battxt, L"進行状況               (%d/%d)", bat_now, bat_ct);
        }
        mdlDialog_completionBarUpdate( dbP, battxt, percent);
		mdlWindow_flush(dbP);
		mdlSystem_pumpMessages();

        TYP = mdlElement_getType( &buf);
        mdlElement_getSymbology( &CO, &WT, &LC, &buf);
        mdlElement_getProperties( &LV, &group, &elmclass, NULL, NULL, NULL, NULL, NULL, &buf);

        flag1 = 0;
        if( group > ggNum){
            ggNum = group;
        }

        /*--- 複合要素に含まれる要素数を取得 ---*/
        if( TYP == 12 || TYP == 14){
            numelems = buf.complex_string.componentCount;
        }
    		

    	
        /*--- mlpuと一致するレイヤーの要素のファイルポジションを記憶する ---*/

        if( ptnct > 0){
            for( ii = 0; ii <= ptnct; ii++){
            	 
                if( ( PTLV[ii] == LV) && ( PTCO[ii] == CO) && ( PTWT[ii] == WT) &&
                    ( PTLC[ii] == LC) && ( elmclass == DgnElementClass::Primary || elmclass == DgnElementClass::Construction)){
                    wcscpy( cellname[count], PTCEL[ii]);
                    scale[count] = PTSCALE[ii];
                    /*word = mdlElement_igdsSize( &buf, 0 );*/
                    	
                    Position[count] = elemAddr[0];
                    count++;
                }
            }
        }
    	if ( Error == 10){ break;}
    } // WHILE END

    elmclass = DgnElementClass::Primary;
    linptn = DgnElementClass::LinearPatterned;
    group01 = ggNum + 1;

    for( ii = 0; ii < count; ii++){
        ggNum = ggNum + 1;

        /*--- グループ番号ごとにレイヤーを記憶 ---*/
        mdlElement_read( &ele, MASTERFILE, Position[ii]);
        get_layer( &glv[ggNum], &gco[ggNum], &glc[ggNum], &gwt[ggNum], NULL, &ele);

        mdlElmdscr_read( &elmDP, Position[ii], MASTERFILE, TRUE, &readPos);
        mdlElmdscr_duplicate( &baseDP, elmDP);

        /*--- ３次元の場合Ｚ値を０にする ---*/
        if( mdlModelRef_is3D(MASTERFILE)){
            TYP = mdlElement_getType( &baseDP->el);
            if( TYP == 3){
                baseDP->el.line_3d.start.z = 0;
                baseDP->el.line_3d.end.z = 0;
            }
            if( TYP == 4 || TYP == 6){
                points = baseDP->el.line_string_3d.numverts;
                for( jj = 0; jj < points; jj++){
                    baseDP->el.line_string_3d.vertice[jj].z = 0;
                }
            }
        }

        /*--- パターン処理 ---*/
        mdlPattern_linear( &ptnDP, baseDP, 0, NULL, cellname[ii], scale[ii], 0);
        mdlElmdscr_setProperties( &ptnDP, NULL, &ggNum, NULL, NULL, NULL, NULL, NULL, NULL);
        mdlElmdscr_add( ptnDP);

        /*--- 「標準設定」の場合 ---*/
        if( ptn->opt == 0){
            mdlElmdscr_duplicate( &NelmDP, elmDP);
            mdlElmdscr_setProperties( &NelmDP, NULL,  &ggNum, &linptn, NULL, NULL, NULL, NULL, NULL);
            mdlElmdscr_rewrite( NelmDP, elmDP, Position[ii]);
            mdlElmdscr_freeAll( &NelmDP);
        }

        /*--- 「消去、パターン」、「消去、本図」の場合 ---*/
        else if( ptn->opt == 1 || ptn->opt == 2){
            mdlElmdscr_undoableDelete( elmDP, Position[ii], TRUE);
        }

        /*--- 「本図、本図」の場合 ---*/
        else if( ptn->opt == 3){
            mdlElmdscr_duplicate( &NelmDP, elmDP);
            mdlElmdscr_setProperties( &NelmDP, NULL,  &ggNum, &elmclass, NULL, NULL, NULL, NULL, NULL);
            mdlElmdscr_rewrite( NelmDP, elmDP, Position[ii]);
            mdlElmdscr_freeAll( &NelmDP);
        }
        mdlElmdscr_display( ptnDP, MASTERFILE, DRAW_MODE_Normal);
        mdlElmdscr_freeAll( &ptnDP);
        mdlElmdscr_freeAll( &elmDP);
        mdlElmdscr_freeAll( &baseDP);
    }
}



/*----------------------------------------------------------------------+
|  モジュール名：pattern                                                |
+----------------------------------------------------------------------*/
void pattern ( )
{

    int    Error;
    int    ii, flag1;
	DgnElementClass elmclass, honzu;
    ULong  filePos, ptnPos;
	UInt32 group;
	int 	scanwords;

    MSElement       buf;
    ExtScanList        scanList;
    MSElement       ele, ele2;
    MSElementDescr  *elmDP;

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
    /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | ELEMTYPE;
    scanList.extendedType = FILEPOS;

    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｰﾌﾟ */
    scanList.typmask[0] |= TMSK0_TEXT_NODE;   /* 対象はﾃｷｽﾄﾉｰﾄﾞ */
    scanList.typmask[0] |= TMSK0_ELLIPSE;     /* 対象はｻｰｸﾙ */
    scanList.typmask[0] |= TMSK0_ARC;         /* 対象はｱｰｸ */
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* 対象はｺﾝﾌﾟﾚｯｸｽﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* 対象はｺﾝﾌﾟﾚｯｸｽｼｪｲﾌﾟ */
    scanList.typmask[1] |= TMSK1_TEXT;        /* 対象はﾃｷｽﾄ */

    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    filePos = 0;
    numelems = 0;
	scanwords = sizeof(elemAddr)/sizeof( short);
    /*--- 一番後のファイルポジションを記憶 ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > EndPos){ break;}
        if( Error == 10 && elemAddr[0] == 0){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }
		mdlElement_read( &buf, MASTERFILE, elemAddr[0]);
    	mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
       
        /*--- 複合に含まれる要素を飛ばす ---*/
        /*if( numelems != 0){
            numelems--;
            continue;
        }*/

        /*word = mdlElement_igdsSize( &buf, 0);*/

        /*--- 進行状況表示（ 50％～95％ ） ---*/
        percent = (ULong)(( double)( 50 + ( elemAddr[0] / ( double)EndPos * 45)));

        if( ptn->mode == 0){
            wcscpy( battxt, L"進行状況");
        }
        else if( ptn->mode == 1){
            swprintf( battxt, L"進行状況               (%d/%d)", bat_now, bat_ct);
        }
        mdlDialog_completionBarUpdate( dbP, battxt, percent);
		mdlWindow_flush(dbP);
		mdlSystem_pumpMessages();

        TYP = mdlElement_getType( &buf);
        mdlElement_getSymbology( &CO, &WT, &LC, &buf);
        mdlElement_getProperties( &LV, &group, &elmclass, NULL, NULL, NULL, NULL, NULL, &buf);

        flag1 = 0;

        if( TYP == 12 || TYP ==14){
            numelems = buf.complex_string.componentCount;
        }

    	if ( TYP == 17 && ssendpos <= elemAddr[0]){
    		mdlElement_undoableDelete( &ele, elemAddr[0], TRUE);
    		
    	}
    	
        /*word = mdlElement_igdsSize( &buf, 0);*/
        ptnPos = elemAddr[0];
    	

        /*--- パターン要素のみレイヤーチェンジ ---*/
        if( elmclass == DgnElementClass::PatternComponent && group01 <= group){
        	   	
            for( ii = 0; ii <= ptnct; ii++){
                if( ( PTLV[ii] == glv[group]) && ( PTCO[ii] == gco[group]) &&
                    ( PTWT[ii] == gwt[group]) && ( PTLC[ii] == glc[group])){ 

                    honzu = DgnElementClass::Primary;
                    memcpy( &ele, &buf, sizeof( MSElement));
                    mdlElement_setSymbology( &ele, &afterCo[ii], &afterWt[ii], &afterLc[ii]);
                    if( ptn->opt == 0 || ptn->opt == 1){
                        if( ptn->ptlv == -1){
                            mdlElement_setProperties( &ele, &ptn->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        }
                        mdlElement_rewrite( &ele, &buf, ptnPos);
                        mdlElement_display( &ele, DRAW_MODE_Normal);
                    }
                    else{
                        if( ptn->ptlv == -1){
                            mdlElement_setProperties( &ele, &ptn->lv, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                        }
                        else{
                            mdlElement_setProperties ( &ele, NULL, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                        }
                        mdlElement_rewrite( &ele, &buf, ptnPos);
                        mdlElement_display( &ele, DRAW_MODE_Normal);
                        flag1++;
                    }
                }
            }
        }

        /*--- 本図に戻すときに、スナップをONにする ---*/
        if( ( ptn->opt == 2 && flag1 == 1) || ( ptn->opt == 3 && flag1 == 1)){

            /*--- ３次元の場合 ---*/
            if( mdlModelRef_is3D(MASTERFILE)){
                if( TYP == 3){
                    if( ele.line_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 4 || TYP == 6){
                    if( ele.line_string_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_string_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 16){
                    if( ele.arc_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.arc_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
            }

            /*--- ２次元の場合 ---*/
            else{
                if( TYP == 3){
                    if( ele.line_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 4 || TYP == 6){
                    if( ele.line_string_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_string_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 16){
                    if( ele.arc_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.arc_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
            }
        } // IF END

        /*--- 要素を削除する ---*/
        for( ii = 0; ii < delct; ii++){
            if( ( LV == DELLV[ii] || DELLV[ii] == -1) && ( CO == DELCO[ii] || DELCO[ii] == -1) &&
                ( LC == DELLC[ii] || DELLC[ii] == -1) && ( WT == DELWT[ii] || DELWT[ii] == -1)){
                mdlElmdscr_read( &elmDP, ptnPos, MASTERFILE, 0, NULL);
                mdlElmdscr_undoableDelete( elmDP, ptnPos, TRUE);
                mdlElmdscr_freeAll( &elmDP);
                count++;
                break;
            }
        }
    	if ( Error == 10){ break;}
    } // WHILE END
}


/*----------------------------------------------------------------------+
|  モジュール名：cel_del                                                |
+----------------------------------------------------------------------*/
void cel_del ( )
{


    int    Error;
    int    ii, jj;
	int 	scanwords;
    ULong  filePos;
	int flg;

    MSElement       buf;
    ExtScanList        scanList;
    MSElementDescr  *elmDP;

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
    /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL |  ELEMTYPE;
    scanList.extendedType = FILEPOS;

    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_CELL_HEADER; /* 対象はｾﾙ */

    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    jj = 0;    filePos = 0;    numelems = 0;
	scanwords = sizeof(elemAddr)/sizeof( short);
    /*--- 一番後のファイルポジションを記憶 ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > EndPos){ break;}
        if( Error == 10 && elemAddr[0] == 0){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }
		mdlElement_read( &buf, MASTERFILE, elemAddr[0]);


        /*--- 進行状況表示（ 95％～100％ ） ---*/
    	percent = (ULong)(( double)( 95 + ( elemAddr[0] / ( double)EndPos * 5)));

        if( ptn->mode == 0){
            wcscpy( battxt, L"進行状況");
        }
        else if( ptn->mode == 1){
            swprintf( battxt, L"進行状況               (%d/%d)", bat_now, bat_ct);
        }
        mdlDialog_completionBarUpdate( dbP, battxt, percent);
		mdlWindow_flush(dbP);
		mdlSystem_pumpMessages();

        TYP = mdlElement_getType( &buf);

        if( TYP == 2){
        	if( mdlModelRef_is3D(MASTERFILE)){
        		numelems = buf.cell_3d.componentCount;
        	}
        	else{
        		numelems = buf.cell_2d.componentCount;
        	}
        	//mdlElement_read( &buf, MASTERFILE, elemAddr[0] + 1);
			mdlElement_read(&buf, MASTERFILE, elemAddr[0]);
			//mdlElmdscr_read(&elmDP, elemAddr[0], MASTERFILE, 0, NULL);
            get_layer( &LV, &CO, &LC, &WT, NULL, &buf);
        	//以下ファイルポジション処理注意
        	if (numelems > 0){
	            for( ii = 0; ii < delct; ii++){
	                if( ( LV == DELLV[ii] || DELLV[ii] == -1) && ( CO == DELCO[ii] || DELCO[ii] == -1)
	                 && ( LC == DELLC[ii] || DELLC[ii] == -1) && ( WT == DELWT[ii] || DELWT[ii] == -1)){


	                 	celPos[jj] = elemAddr[0];
	                    jj++;
	                    break;
	                }
	            }
        	}
        }
    	if ( Error == 10){  break;}
    	
    }
    for( ii = 0; ii < jj; ii++){
        mdlElmdscr_read( &elmDP, celPos[ii], MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, celPos[ii], TRUE);
        mdlElmdscr_freeAll( &elmDP);
        count++;
    }
	
}



/*----------------------------------------------------------------------+
|  モジュール名：divide                                                 |
+----------------------------------------------------------------------*/
void divide( )
{
    ExtScanList     scanList;
    ULong           filePos, txtPos, lastPos;
    int             Error, err, moj,scanerr;
    int             ii, jj, ll, lc;
	UInt32			lv, co, wt;
	int 			scanwords;
    int             cSpace1;
    long            cSsize;
    MSElement       ele, OutellBuf;
    MSElementDescr  *elmDP;
    MSElement       buf, circle;
    WChar            celname[10];
	WString          wstr;

    lastPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | ELEMTYPE;
    scanList.extendedType = FILEPOS;


    scanList.typmask[1] |= ( unsigned short)TMSK1_TEXT; /* 対象はﾃｷｽﾄ */


    /*--------- スキャン実行 ---------------*/
     filePos = 0;
	scanwords = sizeof(elemAddr)/sizeof( short);
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }
    Error = 0;
    while( TRUE){
        scanerr = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > lastPos){ break;}
        if( scanerr == 10 && elemAddr[0] == 0){ break;}
        if( scanerr == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( scanerr == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( scanerr == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( scanerr == 68){ mdlDialog_openInfoBox( L"無効な要素");}
		if (scanerr >= 65 || Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }

		mdlElement_read( &ele, MASTERFILE, elemAddr[0]);

        txtPos = elemAddr[0];

        if( lastPos <= txtPos){ break;}

        kk = 0;
		/*----文字要素からワイドキャラクタの文字列に関する情報を抽出する*/
        mdlText_extract( NULL, NULL, NULL, NULL, NULL, &WtextParamWide, NULL, &ele);
        vertical = WtextParamWide.flags.vertical;

        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

        err = -1;

        if( mdlModelRef_is3D(MASTERFILE)){
            textByte = ele.text_3d.numchars;            // ３次元用
        }
        else{
            textByte = ele.text_2d.numchars;            // ２次元用
        }
		textByte = (textByte - 2) / 2;

        mdlText_extract( &wstr, &origin, &userOrigin, &rMatrix, &tileSize, 
                          &textstyleInfo, NULL, &ele);
		just = textstyleInfo.just;


		swprintf(string, L"%ls", wstr.GetWCharCP());
        textMoj = wcslen( string);
        if( textByte != textMoj){
            textMoj = ( textByte - 2) / 2;
        }

        /*----- 回転 Matrix から角度へ変換 -----*/
        rad = mdlRMatrix_toAngle( &rMatrix);

        /*----- 文字要素に外接する短形 -----*/
        if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin, &ele, FALSE, FALSE))){ }

        if( vertical == 0){
            angle = dkisan( points[0].x, points[0].y, points[1].x, points[1].y);
        }
        else{

            angle = dkisan( points[0].x, points[0].y, points[3].x, points[3].y);
        }

        if( textMoj > 0 && ( textstyleInfo.font == 0)){
        	
            wcscpy( stringmoj, L"");
            moj = 0;
            cSpace1 = 0;
            jj = 0;
            wcscpy( stringb, L"");
            for( ii = 0; ii < textMoj; ii++){
                if( ii == 0){
                    userOrigin.x = points[0].x;
                    userOrigin.y = points[0].y;
					//CE対応
					if (vertical != 0) {
						userOrigin.x = points[3].x;
						userOrigin.y = points[3].y;
					}
					//
                    if( vertical == 0){
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                    }
                    if( vertical == 1){
                        WtextParamWide.just = TextElementJustification::LeftTop;
                    }
                }
                else{
                    if( vertical == 0){
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                        if( cSpace1 > 0){ }
                        cSsize = 0;
                        userOrigin.x = x_kis( userOrigin.x, angle,
                                              tileSize.size.width+WtextParamWide.characterSpacing);
                        userOrigin.y = y_kis( userOrigin.y, angle,
                                              tileSize.size.width+WtextParamWide.characterSpacing);
                    }
                    else{
                        WtextParamWide.just = TextElementJustification::LeftTop;
						//CE対応
                        userOrigin.x = x_kis( userOrigin.x, angle,
							(tileSize.size.height*-1) + WtextParamWide.characterSpacing*-1);
                        //userOrigin.y = y_kis( userOrigin.y, angle,
                        //                      tileSize.size.height+WtextParamWide.characterSpacing);
						userOrigin.y = y_kis(userOrigin.y, angle, (tileSize.size.height*-1) + WtextParamWide.characterSpacing*-1);
                        //
					}
                }
                if( moj == 0){
                    setOrigin.x = userOrigin.x;
                    setOrigin.y = userOrigin.y;
                }

                /*----- １文字抽出 -----*/
                wcscpy( stringb, L"");
                if( textByte == textMoj){
                    wcsncat(stringb, ii + string, 1);
                    if( ! wcscmp( stringb, L"1") || ! wcscmp( stringb, L"１")){
                        cSpace1++ ;
                    }
                }
                else{
                    wcsncat(stringb, ii * 2 - jj + string, 1);
                    if( ! wcscmp( stringb, L"1") || ! wcscmp( stringb, L"１")){
                        cSpace1++ ;
                    }
                    if( ! wcscmp( stringb, L" ")){
                        jj++;
                    }
                    else{
                        wcscpy( stringb, L"");
                        wcsncat(stringb, ii * 2 - jj + string, 2);
                    }
                }

                /*-------- ＥＮＤ　１文字抽出 ---------------*/
                if( ! wcscmp( stringb, L" ") || ! wcscmp( stringb, L"　")){

                    /*------ 文字要素を作成 ----------*/
                    //mbstowcs(stringwb, stringb, 2);
                    WtextParamWide.flags.vertical = vertical;
                    mdlText_create( &OutellBuf, &ele, stringb, &setOrigin,
                                         NULL, NULL, &WtextParamWide, NULL,MASTERFILE);

                    if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin,
                                                                  &OutellBuf, FALSE, FALSE))){ }
                    set_JustOrigin();
                    mdlText_create( &OutellBuf, &ele, stringb,
                                        &setOrigin, NULL, NULL, &WtextParamWide, NULL,MASTERFILE);
                }

                else{
                    /*------ 文字要素を作成 ----------*/
                    //mbstowcs(stringwb, stringb, 2);
                    if( 0 == wcscmp( stringb, L"ー") || 0 == wcscmp( stringb, L"－") ||
                        0 == wcscmp( stringb, L"（") || 0 == wcscmp( stringb, L"）")){
                        if( vertical == 1 && kk == 0){
                             rad = rad - fc_piover2;
                             mdlRMatrix_fromAngle( &rMatrix, rad);
                             kk++;
                        }
                        WtextParamWide.flags.vertical = 0;
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                    }
                    else if( 0 == wcscmp( stringb, L"・")){
                        if( kk == 1){
                            rad = rad + fc_piover2;
                            mdlRMatrix_fromAngle( &rMatrix, rad);
                            kk = 0;
                        }
                        WtextParamWide.flags.vertical = 0;

                        /*--- 無名セルヘッダ ---*/
                        wcscpy( celname, L"NAKATN");
                        mdlCell_create( &buf, celname, &cirPoi, FALSE);
                        mdlElmdscr_new( &elmDP, NULL, &buf);

                        if( textstyleInfo.font == 0){
                            if( vertical == 0){
                                wrka = tileSize.size.width * 0.496;
                                wrkb = tileSize.size.height * 0.5078;
                                cirPoi.x = setOrigin.x + wrka * cos( rad);
                                cirPoi.y = setOrigin.y + wrka * sin( rad);
                                cirPoi.x = cirPoi.x + wrkb * cos( rad + fc_piover2);
                                cirPoi.y = cirPoi.y + wrkb * sin( rad + fc_piover2);

                                for( ll = 1; ll < 6; ll++){
                                    hankei = 0.08333 * tileSize.size.width * 0.2 * ll;
                                    mdlEllipse_create( &circle, NULL, &cirPoi, hankei, hankei, NULL, 0);
                                    set_layer( &lv, &co, &lc, &wt, NULL, &circle);
                                    mdlElmdscr_appendElement( elmDP, &circle);
                                }
                            }
                            else{
                                wrka = tileSize.size.width * 0.496;
                                wrkb = tileSize.size.height * 0.492;
                                cirPoi.x = setOrigin.x + wrka * cos( rad);
                                cirPoi.y = setOrigin.y + wrka * sin( rad);
                                cirPoi.x = cirPoi.x - wrkb * cos( rad + fc_piover2);
                                cirPoi.y = cirPoi.y - wrkb * sin( rad + fc_piover2);
                                for( ll = 1; ll < 6; ll++){
                                    hankei = 0.08333 * tileSize.size.width * 0.2 * ll;
                                    mdlEllipse_create( &circle, NULL, &cirPoi, hankei, hankei, NULL, 0);
                                    set_layer( &lv, &co, &lc, &wt, NULL, &circle);
                                    mdlElmdscr_appendElement( elmDP, &circle);
                                }
                            }
                        }
                        /*--- 無名セルの配置　---*/
                        mdlElmdscr_validate( elmDP ,MASTERFILE);
                        mdlCell_setRange( elmDP, ACTIVEMODEL);
                        mdlElmdscr_add( elmDP);
                        mdlElmdscr_display( elmDP, MASTERFILE, DRAW_MODE_Normal);

                        mdlElmdscr_freeAll( &elmDP);
                    }
                    else{
                        if( kk == 1){
                            rad = rad + fc_piover2;
                            mdlRMatrix_fromAngle( &rMatrix, rad);
                            kk = 0;
                        }
                        WtextParamWide.flags.vertical = vertical;
                    }
                    mdlText_create( &OutellBuf, &ele, stringb, &setOrigin,
                                         &rMatrix, NULL, &WtextParamWide, NULL, MASTERFILE);
                    if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin,
                                                                   &OutellBuf, FALSE, FALSE))){ }
                    set_JustOrigin();
                    mdlElement_add( &OutellBuf);
                }
            }
        	
            /*------ 文字要素を削除 -----------------*/
            mdlElement_undoableDelete( &ele, txtPos, TRUE);
            mdlElement_display( &OutellBuf, DRAW_MODE_Normal);
        }

    	if ( scanerr == 10){ break;}

    }
}

/*----------------------------------------------------------------------+
|  モジュール名：set_JustOrigin                                         |
+----------------------------------------------------------------------*/

void set_JustOrigin(void  )
{
    int    ii;

    if( vertical == 0){
        WtextParamWide.just = just;
    }
    if( vertical == 1){
        WtextParamWide.just = just;
    }

    switch( int(WtextParamWide.just)){
        case     0:
            if( vertical == 0){
                setOrigin.x = points[3].x;
                setOrigin.y = points[3].y;
            }
            else{
                setOrigin.x = points[0].x;
                setOrigin.y = points[0].y;
            }
            break;

        case    1:
            setOrigin.x = ( points[0].x + points[3].x) / 2;
            setOrigin.y = ( points[0].y + points[3].y) / 2;
            break;

        case    2:
            if( vertical == 0){
                setOrigin.x = points[0].x;
                setOrigin.y = points[0].y;
            }
            else{
                setOrigin.x = points[3].x;
                setOrigin.y = points[3].y;
            }
            break;

        case    6:
            if( vertical == 0){
                setOrigin.x = ( points[2].x + points[3].x) / 2;
                setOrigin.y = ( points[2].y + points[3].y) / 2;
            }
            else{
                setOrigin.x = ( points[0].x + points[1].x) / 2;
                setOrigin.y = ( points[0].y + points[1].y) / 2;
            }
            break;

        case    7:
            maxOrigin.x = minOrigin.x = points[0].x;
            maxOrigin.y = minOrigin.y = points[0].y;
            for( ii = 0; ii < 4; ii++){
                if( maxOrigin.x < points[ii].x){ maxOrigin.x = points[ii].x; }
                if( minOrigin.x > points[ii].x){ minOrigin.x = points[ii].x; }
                if( maxOrigin.y < points[ii].y){ maxOrigin.y = points[ii].y; }
                if( minOrigin.y > points[ii].y){ minOrigin.y = points[ii].y; }
            }
            setOrigin.x = ( maxOrigin.x + minOrigin.x) / 2;
            setOrigin.y = ( maxOrigin.y + minOrigin.y) / 2;
            break;

        case    8:
            if( vertical == 0){
                setOrigin.x = ( points[0].x + points[1].x) / 2;
                setOrigin.y = ( points[0].y + points[1].y) / 2;
            }
            else{
                setOrigin.x = ( points[2].x + points[3].x) / 2;
                setOrigin.y = ( points[2].y + points[3].y) / 2;
            }
            break;

        case    12:
            if( vertical == 0){
                setOrigin.x = points[2].x;
                setOrigin.y = points[2].y;
            }
            else{
                setOrigin.x = points[1].x;
                setOrigin.y = points[1].y;
            }
            break;

        case    13:
            setOrigin.x =  ( points[1].x + points[2].x) / 2;
            setOrigin.y =  ( points[1].y + points[2].y) / 2;
            break;

        case    14:
            if( vertical == 0){
                setOrigin.x = points[1].x;
                setOrigin.y = points[1].y;
            }
            else{
                setOrigin.x = points[2].x;
                setOrigin.y = points[2].y;
            }
            break;

        default:
            break;
    }
}

/*----------------------------------------------------------------------+
|  モジュール名：fenceStart                                             |
+----------------------------------------------------------------------*/
void fenceStart ( )
{


    int    Error;
	int    scanwords;

	UInt32  group;
    ULong  filePos;
	
    MSElement       buf;
    ExtScanList        scanList;

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
    /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | ELEMTYPE;
    scanList.extendedType = FILEPOS;


    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｰﾌﾟ */
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* 対象はｺﾝﾌﾟﾚｯｸｽﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* 対象はｺﾝﾌﾟﾚｯｸｽｼｪｲﾌﾟ */


    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    numelems = 0;   count = 0;   ggNum = 0;
	scanwords = sizeof(elemAddr)/sizeof( short);
	EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > EndPos){ break;}
        if( Error == 10 && elemAddr[0] == 0){ break;}
        if( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }
		mdlElement_read( &buf, MASTERFILE, elemAddr[0]);
        /*--- 複合に含まれる要素を飛ばす ---*/
        /*if( numelems != 0){
            numelems--;
            continue;
        }*/

       
        TYP = mdlElement_getType( &buf);
        mdlElement_getProperties( &LV, &group, NULL, NULL, NULL, NULL, NULL, NULL, &buf);

        if( group > ggNum){
            ggNum = group;
        }

        /*--- 複合要素に含まれる要素数を取得 ---*/
        if( TYP == 12 || TYP == 14){
            numelems = buf.complex_string.componentCount;
        }
    	if ( Error == 10){ break;}

    } // WHILE END
}


/*----------------------------------------------------------------------+
|  モジュール名：fenceScan                                              |
+----------------------------------------------------------------------*/
void fenceScan( )
{
    count = 0;
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
    mdlState_startFenceCommand( (StateFunc_FenceContentOp)fence_ptn, NULL, NULL, NULL, NULL, NULL, FENCE_NO_CLIP);
    mdlFence_process( NULL);

    chg_ptn();
    chg_layer();

    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
    mdlState_startFenceCommand( (StateFunc_FenceContentOp)del_layer, NULL, NULL, NULL, NULL, NULL, FENCE_NO_CLIP);
    mdlFence_process( NULL);

    if( ptn->bunkatu == -1){
        mdlState_startFenceCommand( (StateFunc_FenceContentOp)fence_divide, NULL, NULL, NULL, NULL, NULL, FENCE_NO_CLIP);
        mdlFence_process( NULL);
    }
}


/*----------------------------------------------------------------------+
|  モジュール名：fence_ptn                                              |
+----------------------------------------------------------------------*/
int fence_ptn( )
{
    DgnElementClass    elmclass;
	UInt32  group;
    ULong  fencePos;

    MSElement     buf;

    fencePos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);

    /*--- 進行状況表示（ 0％～50％ ） ---*/
	percent = (ULong)(( double)( fencePos) / ( double)EndPos * 50);
    mdlDialog_completionBarUpdate( dbP, L"進行状況", percent);
	mdlWindow_flush(dbP);
	mdlSystem_pumpMessages();

    elmclass = DgnElementClass::Primary;

    mdlElement_read( &buf, MASTERFILE, fencePos);
    TYP = mdlElement_getType( &buf);
    mdlElement_getSymbology( &CO, &WT, &LC, &buf);
    mdlElement_getProperties( &LV, &group, &elmclass, NULL, NULL, NULL, NULL, NULL, &buf);

    if( group > ggNum){
        ggNum = group;
    }

    if( TYP == 12 || TYP ==14){
        numelems = buf.complex_string.componentCount;
    }
	//以下ファイルポジション処理注意
    for( ii = 0; ii <= ptnct; ii++){
        if( ( PTLV[ii] == LV) && ( PTCO[ii] == CO) && ( PTWT[ii] == WT) &&
            ( PTLC[ii] == LC) && ( elmclass == DgnElementClass::Primary || elmclass == DgnElementClass::Construction)){
            wcscpy( cellname[count], PTCEL[ii]);
            scale[count] = PTSCALE[ii];
            Position[count] = fencePos;
            count++;
        }
    }
    return( SUCCESS);
}


/*----------------------------------------------------------------------+
|  モジュール名：chg_ptn                                                |
+----------------------------------------------------------------------*/
void chg_ptn ( )
{
	int     ii, jj;
	DgnElementClass elmclass, linptn;
    UInt32  readPos;
    ULong   points;

    MSElement       ele;
    MSElementDescr  *NelmDP, *ptnDP, *elmDP, *baseDP;

    elmclass = DgnElementClass::Primary;
    linptn = DgnElementClass::LinearPatterned;

    group01 = ggNum + 1;
    for( ii = 0; ii < count; ii++){
        ggNum = ggNum + 1;

        /*--- グループ番号ごとにレイヤーを記憶 ---*/
        mdlElement_read( &ele, MASTERFILE, Position[ii]);
        get_layer( &glv[ggNum], &gco[ggNum], &glc[ggNum], &gwt[ggNum], NULL, &ele);

        mdlElmdscr_read( &elmDP, Position[ii], MASTERFILE, TRUE, &readPos);
        mdlElmdscr_duplicate( &baseDP, elmDP);

        /*--- ３次元の場合 ---*/
        if( mdlModelRef_is3D(MASTERFILE)){
            TYP = mdlElement_getType( &baseDP->el);
            if( TYP == 3){
                baseDP->el.line_3d.start.z = 0;
                baseDP->el.line_3d.end.z = 0;
            }
            if( TYP == 4 || TYP == 6){
                points = baseDP->el.line_string_3d.numverts;
                for ( jj = 0; jj < points; jj++){
                    baseDP->el.line_string_3d.vertice[jj].z = 0;
                }
            }
        }
        mdlPattern_linear( &ptnDP, baseDP, 0, NULL, cellname[ii], 1.0, 0);
        mdlElmdscr_setProperties( &ptnDP, NULL, &ggNum, NULL, NULL, NULL, NULL, NULL, NULL);
        mdlElmdscr_add( ptnDP);
        if( ptn->opt == 0){
            mdlElmdscr_duplicate( &NelmDP, elmDP);
            mdlElmdscr_setProperties( &NelmDP, NULL,  &ggNum, &linptn, NULL, NULL, NULL, NULL, NULL);
            mdlElmdscr_rewrite( NelmDP, elmDP, Position[ii]);
            mdlElmdscr_freeAll( &NelmDP);
        }
        else if( ptn->opt == 1 || ptn->opt == 2){
            mdlElmdscr_undoableDelete( elmDP, Position[ii], TRUE);
        }
        else if( ptn->opt == 3){
            mdlElmdscr_duplicate( &NelmDP, elmDP);
            mdlElmdscr_setProperties( &NelmDP, NULL,  &ggNum, &elmclass, NULL, NULL, NULL, NULL, NULL);
            mdlElmdscr_rewrite( NelmDP, elmDP, Position[ii]);
            mdlElmdscr_freeAll( &NelmDP);
        }
        mdlElmdscr_display( ptnDP, MASTERFILE, DRAW_MODE_Normal);
        mdlElmdscr_freeAll( &ptnDP);
        mdlElmdscr_freeAll( &elmDP);
        mdlElmdscr_freeAll( &baseDP);
    }
}

/*----------------------------------------------------------------------+
|  モジュール名：chg_layer                                               |
+----------------------------------------------------------------------*/
void chg_layer( )
{

    int    Error;
	int    ii, flag1;
	DgnElementClass   elmclass, honzu;
	UInt32  group;
    ULong  filePos, ptnPos;
	int 	scanwords;
	

    MSElement       buf;
    ExtScanList        scanList;
    MSElement       ele, ele2;


    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
     /*scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;*/
	/*mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | ELEMTYPE;
    scanList.extendedType = FILEPOS;


    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｰﾌﾟ */
    scanList.typmask[0] |= TMSK0_ARC;         /* 対象はｱｰｸ */

    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    filePos = 0;
    numelems = 0;
	scanwords = sizeof(elemAddr)/sizeof( short);
    /*--- 一番後のファイルポジションを記憶 ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
    	if ( filePos > EndPos){ break;}
        if( Error == 10 && elemAddr[0] == 0){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1); return; }
		mdlElement_read( &buf, MASTERFILE, elemAddr[0]);
    	mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
    	


        /*--- 進行状況表示（ 50％～95％ ） ---*/
    	percent = (ULong)(( double)( 50 + ( elemAddr[0] / ( double)EndPos * 45)));
        wcscpy( battxt, L"進行状況");
        mdlDialog_completionBarUpdate( dbP, battxt, percent);
		mdlWindow_flush(dbP);
		mdlSystem_pumpMessages();

        TYP = mdlElement_getType( &buf);
        mdlElement_getSymbology( &CO, &WT, &LC, &buf);
        mdlElement_getProperties( &LV, &group, &elmclass, NULL, NULL, NULL, NULL, NULL, &buf);

        flag1 = 0;


        ptnPos = elemAddr[0];

        /*--- パターン要素のみレイヤーチェンジ ---*/
        if( elmclass == DgnElementClass::PatternComponent && group01 <= group){
            for( ii = 0; ii <= ptnct; ii++){
                if( ( PTLV[ii] == glv[group]) && ( PTCO[ii] == gco[group]) &&
                    ( PTWT[ii] == gwt[group]) && ( PTLC[ii] == glc[group])){ 

                    honzu = DgnElementClass::Primary;
                    memcpy( &ele, &buf, sizeof( MSElement));
                    mdlElement_setSymbology( &ele, &afterCo[ii], &afterWt[ii], &afterLc[ii]);
                    if( ptn->opt == 0 || ptn->opt == 1){
                        if( ptn->ptlv == -1){
                            mdlElement_setProperties( &ele, &ptn->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        }
                        mdlElement_rewrite( &ele, &buf, ptnPos);
                        mdlElement_display( &ele, DRAW_MODE_Normal);
                    }
                    else{
                        if( ptn->ptlv == -1){
                            mdlElement_setProperties( &ele, &ptn->lv, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                        }
                        else{
                            mdlElement_setProperties ( &ele, NULL, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                        }
                        mdlElement_rewrite( &ele, &buf, ptnPos);
                        mdlElement_display( &ele, DRAW_MODE_Normal);
                        flag1++;
                    }
                }
            }
        }

        /*--- 本図に戻すときに、スナップをONにする ---*/
        if( ( ptn->opt == 2 && flag1 == 1) || ( ptn->opt == 3 && flag1 == 1)){

            /*--- ３次元の場合 ---*/
            if( mdlModelRef_is3D(MASTERFILE)){
                if( TYP == 3){
                    if( ele.line_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 4 || TYP == 6){
                    if( ele.line_string_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_string_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 16){
                    if( ele.arc_3d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.arc_3d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
            }

            /*--- ２次元の場合 ---*/
            else{
                if( TYP == 3){
                    if( ele.line_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 4 || TYP == 6){
                    if( ele.line_string_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.line_string_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
                else if( TYP == 16){
                    if( ele.arc_2d.dhdr.props.b.s == 1){
                        memcpy( &ele2, &ele, sizeof( MSElement));
                        ele2.arc_2d.dhdr.props.b.s = 0;
                        mdlElement_rewrite( &ele2, &ele, ptnPos);
                    }
                }
            }
        } // IF END
    	if ( Error == 10){ break;}
    } // WHILE END
}


/*----------------------------------------------------------------------+
|  モジュール名：del_layer                                              |
+----------------------------------------------------------------------*/
int del_layer( )
{

    int    ii, jj, flag1;
	DgnElementClass  elmclass;
	UInt32  group;
    ULong  ptnPos, nextPos;

    MSElement       buf;
    MSElement       elm;
    MSElementDescr  *elmDP;

    ptnPos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);

    mdlElement_read( &buf, MASTERFILE, ptnPos);
    TYP = mdlElement_getType( &buf);
    mdlElement_getSymbology( &CO, &WT, &LC, &buf);
    mdlElement_getProperties( &LV, &group, &elmclass, NULL, NULL, NULL, NULL, NULL, &buf);

    flag1 = 0;

/*
    if( elmclass == 1){
        for( ii = 0; ii <= ptnct; ii++ ){
            if( ( PTLV[ii] == glv[group]) && ( PTCO[ii] == gco[group]) &&
                ( PTWT[ii] == gwt[group]) && ( PTLC[ii] == glc[group])){ 
                honzu = 0;
                memcpy( &ele, &buf, sizeof( MSElement));
                mdlElement_setSymbology( &ele, &afterCo[ii], &afterWt[ii], &afterLc[ii]);
                if( ptn->opt == 0 || ptn->opt == 1){
                    if( ptn->ptlv == -1){
                        mdlElement_setProperties( &ele, &ptn->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                    }
                    mdlElement_rewrite( &ele, &buf, ptnPos);
                    mdlElement_display( &ele, NORMALDRAW);
                }
                else{
                    if( ptn->ptlv == -1){
                        mdlElement_setProperties( &ele, &ptn->lv, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                    }
                    else{
                        mdlElement_setProperties( &ele, NULL, NULL, &honzu, NULL, NULL, NULL, NULL, NULL);
                    }
                    mdlElement_rewrite( &ele, &buf, ptnPos);
                    mdlElement_display( &ele, NORMALDRAW);
                    flag1++;
                }
            }
        }
    }
    if( ( ptn->opt == 2 && flag1 == 1) || ( ptn->opt == 3 && flag1 == 1)){
        if( tcb->fbfdcn.design3d != 0){
            if( TYP == 3){
                if( ele.line_3d.dhdr.props.b.s == 1){
                    memcpy( &ele2, &ele, sizeof( MSElement));
                    ele2.line_3d.dhdr.props.b.s = 0;
                    mdlElement_rewrite( &ele2, &ele, ptnPos);
                }
            }
            else if( TYP == 4 || TYP == 6){
                if( ele.line_string_3d.dhdr.props.b.s == 1){
                    memcpy( &ele2, &ele, sizeof( MSElement));
                    ele2.line_string_3d.dhdr.props.b.s = 0;
                    mdlElement_rewrite( &ele2, &ele, ptnPos);
                }
            }
        }
        else{
            if( TYP == 3){
                if( ele.line_2d.dhdr.props.b.s == 1){
                    memcpy( &ele2, &ele, sizeof( MSElement));
                    ele2.line_2d.dhdr.props.b.s = 0;
                    mdlElement_rewrite( &ele2, &ele, ptnPos);
                }
            }
            else if( TYP == 4 || TYP == 6){
                if( ele.line_string_2d.dhdr.props.b.s == 1){
                    memcpy( &ele2, &ele, sizeof( MSElement));
                    ele2.line_string_2d.dhdr.props.b.s = 0;
                    mdlElement_rewrite( &ele2, &ele, ptnPos);
                }
            }
        }
    }
*/
	//以下ファイルポジション処理注意
    if( TYP == 2){

        nextPos = ptnPos + 1;
        mdlElement_read( &elm, MASTERFILE, nextPos);
        mdlElement_getSymbology( &CO, &WT, &LC, &elm);
        mdlElement_getProperties( &LV, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &elm);
        jj = 0;
        for( ii = 0; ii < delct; ii++){
            if( ( LV == DELLV[ii] || DELLV[ii] == -1) && ( CO == DELCO[ii] || DELCO[ii] == -1) &&
                ( LC == DELLC[ii] || DELLC[ii] == -1) && ( WT == DELWT[ii] || DELWT[ii] == -1)){
                mdlElmdscr_read( &elmDP, ptnPos, MASTERFILE, 0, NULL);
                mdlElmdscr_undoableDelete( elmDP, ptnPos, TRUE);
                mdlElmdscr_freeAll( &elmDP);
                count++;
                break;
            }
        }
    }
    else{
        jj = 0;
        for( ii = 0; ii < delct; ii++){
            if( ( LV == DELLV[ii] || DELLV[ii] == -1) && ( CO == DELCO[ii] || DELCO[ii] == -1) &&
                ( LC == DELLC[ii] || DELLC[ii] == -1) && ( WT == DELWT[ii] || DELWT[ii] == -1)){
                mdlElmdscr_read( &elmDP, ptnPos, MASTERFILE, 0, NULL);
                mdlElmdscr_undoableDelete( elmDP, ptnPos, TRUE);
                mdlElmdscr_freeAll( &elmDP);
                count++;
                break;
            }
        }
    }

    return( SUCCESS);
}

/*----------------------------------------------------------------------+
|  モジュール名：fence_divide                                           |
+----------------------------------------------------------------------*/
int fence_divide( )
{

    ULong           txtPos, lastPos;
    int             Error, err, moj;
    int             ii, jj, ll, lc;
	UInt32			lv, co, wt;
    int             cSpace1;
    long            cSsize;
    MSElement       ele, OutellBuf;
    MSElementDescr  *elmDP;
    MSElement        buf, circle;
    WChar            celname[10];
	WString          wstr;

    lastPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    txtPos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);
    mdlElement_read( &ele, MASTERFILE, txtPos);
    TYP = mdlElement_getType( &ele);

//        if( lastPos <= txtPos){ break;}

    if( TYP == 17){
        kk = 0;

        mdlText_extract( NULL, NULL, NULL, NULL, &WtextSize, &WtextParamWide, NULL, &ele);
        vertical = WtextParamWide.flags.vertical;

        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

        err = -1;

        if( mdlModelRef_is3D(MASTERFILE)){
            textByte = ele.text_3d.numchars;            // ３次元用
        }
        else{
            textByte = ele.text_2d.numchars;            // 2次元用
        }
		textByte = (textByte - 2) / 2;

		mdlText_extract(&wstr, &origin, &userOrigin, &rMatrix, &tileSize,
			&textstyleInfo, NULL, &ele);
		just = textstyleInfo.just;
		swprintf(string, L"%ls", wstr.GetWCharCP());

        textMoj = wcslen( string);
        if( textByte != textMoj){
            textMoj = ( textByte - 2) / 2;
        }

        /*----- 回転 Matrix から角度へ変換 -----*/
        rad = mdlRMatrix_toAngle( &rMatrix);

        /*----- 文字要素に外接する短形 -----*/
        if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin, &ele, FALSE, FALSE))){ }

        if( vertical == 0){
            angle = dkisan( points[0].x, points[0].y, points[1].x, points[1].y);
        }
        else{
            angle = dkisan( points[0].x, points[0].y, points[3].x, points[3].y);
        }

        if( textMoj > 0 && textstyleInfo.font == 0){
            wcscpy( stringmoj, L"");
            moj = 0;
            cSpace1 = 0;
            jj = 0;
            wcscpy( stringb, L"");
            for( ii = 0; ii < textMoj; ii++){
                if( ii == 0){
                    userOrigin.x = points[0].x;
                    userOrigin.y = points[0].y;
					//CE対応
					if (vertical != 0) {
						userOrigin.x = points[3].x;
						userOrigin.y = points[3].y;
					}
                    if( vertical == 0){
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                    }
                    if( vertical == 1){
                        WtextParamWide.just = TextElementJustification::LeftTop;
                    }
                }
                else{
                    if( vertical == 0){
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                        if( cSpace1 > 0){ }
                        cSsize = 0;
                        userOrigin.x = x_kis( userOrigin.x, angle,
                                              tileSize.size.width+WtextParamWide.characterSpacing);
                        userOrigin.y = y_kis( userOrigin.y, angle,
                                              tileSize.size.width+WtextParamWide.characterSpacing);
                    }
                    else{
                        WtextParamWide.just = TextElementJustification::LeftTop;
						//CE対応
						userOrigin.x = x_kis(userOrigin.x, angle,
							(tileSize.size.height*-1) + WtextParamWide.characterSpacing*-1);
						//userOrigin.y = y_kis( userOrigin.y, angle,
						//                      tileSize.size.height+WtextParamWide.characterSpacing);
						userOrigin.y = y_kis(userOrigin.y, angle, (tileSize.size.height*-1) + WtextParamWide.characterSpacing*-1);
						//
                    }
                }
                if( moj == 0 ){
                    setOrigin.x = userOrigin.x;
                    setOrigin.y = userOrigin.y;
                }

                /*----- １文字抽出 -----*/
                wcscpy( stringb, L"");
                if( textByte == textMoj){
                    wcsncat(stringb, ii + string, 1);
                    if( ! wcscmp( stringb, L"1") || ! wcscmp( stringb, L"１")){
                        cSpace1++ ;
                    }
                }
                else{
                    wcsncat(stringb, ii * 2 - jj + string, 1);
                    if( ! wcscmp( stringb, L"1") || ! wcscmp( stringb, L"１")){
                        cSpace1++ ;
                    }
                    if( ! wcscmp( stringb, L" ")){
                        jj++;
                    }
                    else{
                        wcscpy( stringb, L"");
                        wcsncat(stringb, ii * 2 - jj + string, 2);
                    }
                }

                /*-------- ＥＮＤ　１文字抽出 ---------------*/
                if( ! wcscmp( stringb, L" ") || ! wcscmp( stringb, L"　")){

                    /*------ 文字要素を作成 ----------*/
                    //mbstowcs(stringwb, stringb, 2);
                    WtextParamWide.flags.vertical = vertical;
                    mdlText_create( &OutellBuf, &ele, stringb, &setOrigin,
                                         NULL, NULL, &WtextParamWide, NULL,MASTERFILE);

                    if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin,
                                                                  &OutellBuf, FALSE, FALSE))){ }
                    set_JustOrigin();
                    mdlText_create( &OutellBuf, &ele, stringb,
                                        &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
                }   

                else{
                    /*------ 文字要素を作成 ----------*/
                    //mbstowcs(stringwb, stringb, 2);
                    if( 0 == wcscmp( stringb, L"ー") || 0 == wcscmp( stringb, L"－") ||
                        0 == wcscmp( stringb, L"（") || 0 == wcscmp( stringb, L"）")){
                        if( vertical == 1 && kk == 0){
                             rad = rad - fc_piover2;
                             mdlRMatrix_fromAngle( &rMatrix, rad);
                             kk++;
                        }
                        WtextParamWide.flags.vertical = 0;
                        WtextParamWide.just = TextElementJustification::LeftBaseline;
                    }
                    else if( 0 == wcscmp( stringb, L"・")){
                        if( kk == 1){
                            rad = rad + fc_piover2;
                            mdlRMatrix_fromAngle( &rMatrix, rad);
                            kk = 0;
                        }
                        WtextParamWide.flags.vertical = 0;

                        /*--- 無名セルヘッダ ---*/
                        wcscpy( celname, L"NAKATN");
                        mdlCell_create( &buf, celname, &cirPoi, FALSE);
                        mdlElmdscr_new( &elmDP, NULL, &buf);

                        if( vertical == 0){
                            wrka = tileSize.size.width * 0.496;
                            wrkb = tileSize.size.height * 0.5078;
                            cirPoi.x = setOrigin.x + wrka * cos( rad);
                            cirPoi.y = setOrigin.y + wrka * sin( rad);
                            cirPoi.x = cirPoi.x + wrkb * cos( rad + fc_piover2);
                            cirPoi.y = cirPoi.y + wrkb * sin( rad + fc_piover2);


                            for( ll = 1; ll < 6; ll++){
                                hankei = 0.08333 * tileSize.size.width * 0.2 * ll;
                                mdlEllipse_create( &circle, NULL, &cirPoi, hankei, hankei, NULL, 0);
                                set_layer( &lv, &co, &lc, &wt, NULL, &circle);
                                mdlElmdscr_appendElement( elmDP, &circle);
                            }
                        }
                        else{
                            wrka = tileSize.size.width * 0.496;
                            wrkb = tileSize.size.height * 0.492;
                            cirPoi.x = setOrigin.x + wrka * cos( rad);
                            cirPoi.y = setOrigin.y + wrka * sin( rad);
                            cirPoi.x = cirPoi.x - wrkb * cos( rad + fc_piover2);
                            cirPoi.y = cirPoi.y - wrkb * sin( rad + fc_piover2);
                            for( ll = 1; ll < 6; ll++){
                                hankei = 0.08333 * tileSize.size.width * 0.2 * ll;
                                mdlEllipse_create( &circle, NULL, &cirPoi, hankei, hankei, NULL, 0);
                                set_layer( &lv, &co, &lc, &wt, NULL, &circle);
                                mdlElmdscr_appendElement( elmDP, &circle);
//                                create_errCircle( lv, co, lc, wt, hankei, &cirPoi);
                            }
                        }
                        /*--- 無名セルの配置　---*/
                        mdlElmdscr_validate( elmDP, MASTERFILE);
                        mdlCell_setRange( elmDP, ACTIVEMODEL);
                        mdlElmdscr_add( elmDP);
                        mdlElmdscr_display( elmDP, MASTERFILE, DRAW_MODE_Normal);

                        mdlElmdscr_freeAll( &elmDP);
                    }
                    else{
                        if( kk == 1){
                            rad = rad + fc_piover2;
                            mdlRMatrix_fromAngle( &rMatrix, rad);
                            kk = 0;
                        }
                        WtextParamWide.flags.vertical = vertical;
                    }
                    mdlText_create( &OutellBuf, &ele, stringb, &setOrigin,
                                        &rMatrix, NULL, &WtextParamWide, NULL, MASTERFILE);
                    if( SUCCESS != ( Error = mdlText_extractShape( points, &userOrigin,
                                                                   &OutellBuf, FALSE, FALSE))){ }
                    set_JustOrigin();
                    mdlElement_add( &OutellBuf);
                }
            }
            /*------ 文字要素を削除 -----------------*/
            mdlElement_undoableDelete( &ele, txtPos, TRUE);
            mdlElement_display( &OutellBuf, DRAW_MODE_Normal);
        }
    }
    return( SUCCESS);
}

int Text_ExtractShape_a(MSElementP elmP, DPoint3d *points)
{
	// テキスト・テキストノードの場合
	if (elmP->ehdr.type == TEXT_ELM || elmP->ehdr.type == TEXT_NODE_ELM)
	{
		// 幅、高さ、寄せ位置の取得
		EditElementHandle eh(elmP, MASTERFILE);
		TextBlockPtr textBlock = TextHandlerBase::GetFirstTextPartValue(eh);
		if (textBlock.IsValid())
		{
			DPoint3d userOrigin = textBlock->GetUserOrigin();
			DRange3d range = textBlock->GetNominalRange();
			RotMatrix rotMatrix = textBlock->GetOrientation();
			DVec3d vec;
			double angle = rotMatrix.GetRotationAngleAndVector(vec);	// vec.zのプラスマイナスで回転方向が決まる
			if (vec.z < 0) angle *= -1;

			points[0] = range.low;
			points[1] = range.low; points[1].x = range.high.x;
			points[2] = range.high;
			points[3] = range.high; points[3].x = range.low.x;
			points[4] = range.low;


			DPoint3d shift = DPoint3d::From(0, 0, 0);
			CaretPtr caret = textBlock->CreateStartCaret();
			TextElementJustification just = caret->GetCurrentParagraphCP()->GetProperties().GetJustification();
			switch (just)
			{
			case Bentley::DgnPlatform::TextElementJustification::LeftTop:
				shift = points[3];
				break;
			case Bentley::DgnPlatform::TextElementJustification::LeftMiddle:
				_GetCenter_a(&shift, &points[0], &points[3]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::LeftBaseline:
				shift = points[0];
				break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftDescender:
				//	break;
			case Bentley::DgnPlatform::TextElementJustification::CenterTop:
				_GetCenter_a(&shift, &points[2], &points[3]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::CenterMiddle:
				DPoint3d ct, cb;
				_GetCenter_a(&ct, &points[2], &points[3]);
				_GetCenter_a(&cb, &points[0], &points[1]);
				_GetCenter_a(&shift, &ct, &cb);
				break;
			case Bentley::DgnPlatform::TextElementJustification::CenterBaseline:
				_GetCenter_a(&shift, &points[0], &points[1]);
				break;
				//case Bentley::DgnPlatform::TextElementJustification::CenterDescender:
				//	break;
			case Bentley::DgnPlatform::TextElementJustification::RightTop:
				shift = points[2];
				break;
			case Bentley::DgnPlatform::TextElementJustification::RightMiddle:
				_GetCenter_a(&shift, &points[1], &points[2]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::RightBaseline:
				shift = points[1];
				break;
				//case Bentley::DgnPlatform::TextElementJustification::RightDescender:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftMarginTop:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftMarginMiddle:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftMarginBaseline:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftMarginDescender:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightMarginTop:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightMarginMiddle:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightMarginBaseline:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightMarginDescender:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftCap:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftMarginCap:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::CenterCap:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightCap:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::RightMarginCap:
				//	break;
				//case Bentley::DgnPlatform::TextElementJustification::Invalid:
				break;
			default:
				return ERROR;
				break;
			}

			// 起点を原点にする
			for (int i = 0; i < 5; i++)
				points[i].Subtract(shift);

			// 回転させる
			for (int i = 0; i < 5; i++)
				points[i].RotateXY(angle);

			// 実座標へ移動
			for (int i = 0; i < 5; i++)
				points[i].Add(userOrigin);

			// mm -> m
			mdlCurrTrans_invtransPointArray(points, points, 5);


			//test
			double angle2 = atan2(points[1].y - points[0].y, points[1].x - points[0].x);


			//test 枠を描画
			//MSElement ele;
			//mdlLineString_create(&ele, NULL, points, 5);
			////mdlElement_setSymbology(&ele, co, wt, lc);
			//mdlElement_add(&ele);

		}
		else
		{
			return(ERROR);
		}
	}
	else
	{
		return(ERROR);
	}

	return(SUCCESS);
}
/*
void set_JustOrigin(void)
{
	Dpoint3d     tmppnt;

	int	i;

	if (vertical == 0) {
			WtextParamWide.just = just;
	}
	if (vertical == 1) {

			WtextParamWide.just = just;
		//左下原点左回りを左上原点右周りにする（V7に合わせる）
		memcpy(&tmppnt, &points[0], sizeof(Dpoint3d));
		memcpy(&points[0], &points[3], sizeof(Dpoint3d));
		memcpy(&points[3], &tmppnt, sizeof(Dpoint3d));
		memcpy(&tmppnt, &points[1], sizeof(Dpoint3d));
		memcpy(&points[1], &points[2], sizeof(Dpoint3d));
		memcpy(&points[2], &tmppnt, sizeof(Dpoint3d));

	}


	switch (WtextParamWide.just)
	{
	case	TextElementJustification::LeftTop:
		if (vertical == 0) {
			setOrigin.x = points[3].x;
			setOrigin.y = points[3].y;
		}
		else {
			setOrigin.x = points[0].x;
			setOrigin.y = points[0].y;
		}
		break;
	case	TextElementJustification::LeftMiddle:
		setOrigin.x = (points[0].x + points[3].x) / 2;
		setOrigin.y = (points[0].y + points[3].y) / 2;
		break;
	case	TextElementJustification::LeftBaseline:
		if (vertical == 0) {
			setOrigin.x = points[0].x;
			setOrigin.y = points[0].y;
		}
		else {
			setOrigin.x = points[3].x;
			setOrigin.y = points[3].y;
		}
		break;
	case	TextElementJustification::CenterTop:
		if (vertical == 0) {
			setOrigin.x = (points[2].x + points[3].x) / 2;
			setOrigin.y = (points[2].y + points[3].y) / 2;
		}
		else {
			setOrigin.x = (points[0].x + points[1].x) / 2;
			setOrigin.y = (points[0].y + points[1].y) / 2;
		}
		break;
	case	TextElementJustification::CenterMiddle:
		maxOrigin.x = minOrigin.x = points[0].x;
		maxOrigin.y = minOrigin.y = points[0].y;
		for (i = 0; i < 4; i++) {
			if (maxOrigin.x < points[i].x) { maxOrigin.x = points[i].x; }
			if (minOrigin.x > points[i].x) { minOrigin.x = points[i].x; }
			if (maxOrigin.y < points[i].y) { maxOrigin.y = points[i].y; }
			if (minOrigin.y > points[i].y) { minOrigin.y = points[i].y; }
		}
		setOrigin.x = (maxOrigin.x + minOrigin.x) / 2;
		setOrigin.y = (maxOrigin.y + minOrigin.y) / 2;
		break;
	case	TextElementJustification::CenterBaseline:
		if (vertical == 0) {
			setOrigin.x = (points[0].x + points[1].x) / 2;
			setOrigin.y = (points[0].y + points[1].y) / 2;
		}
		else {
			setOrigin.x = (points[2].x + points[3].x) / 2;
			setOrigin.y = (points[2].y + points[3].y) / 2;
		}
		break;
	case	TextElementJustification::RightTop:
		if (vertical == 0) {
			setOrigin.x = points[2].x;
			setOrigin.y = points[2].y;
		}
		else {
			setOrigin.x = points[1].x;
			setOrigin.y = points[1].y;
		}
		break;
	case	TextElementJustification::RightMiddle:
		setOrigin.x = (points[1].x + points[2].x) / 2;
		setOrigin.y = (points[1].y + points[2].y) / 2;
		break;
	case	TextElementJustification::RightBaseline:
		if (vertical == 0) {
			setOrigin.x = points[1].x;
			setOrigin.y = points[1].y;
		}
		else {
			setOrigin.x = points[2].x;
			setOrigin.y = points[2].y;
		}
		break;
	default:
		break;
	}
}
*/
void _GetCenter_a(Dpoint3d *mPoi, Dpoint3d *lPoi1, Dpoint3d *lPoi2)
{
	mPoi->x = (lPoi1->x + lPoi2->x) / 2;
	mPoi->y = (lPoi1->y + lPoi2->y) / 2;
	mPoi->z = 0;
}