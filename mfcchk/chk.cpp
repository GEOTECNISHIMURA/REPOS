/*-----------------------------------------------------------------------*/
/*-         CHK.MC                                                      -*/
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
#include "mfcchk.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE                    *fp_save, *fp_mfc, *fp_err, *fp_log, *fp_list, *fp_sort;
RscFileHandle           Rsc;
int                     exitStatus, linect, txtct, cellct, memo, bat_ct, bat_now;
WChar                   filename[128], info[256], logfile[256], file[64], file_err[64], file_sort[64];
int                     LC, TYP;
UInt32                  CO, WT, LV;
int                     cell[10000][6], txt[10000][7], line[10000][6], TC, just, kijyun[10000], FT, numelems;
WChar                   cname[10000][7], tjust[3], ejust[3], str[256], battxt[64];

double                  txth[10000][2], hankei;
TextSizeParam           textSizeParam;
TextStyleInfo           tsi;
ULong                   linePos[10000], cellPos[10000], txtPos[10000];
WChar                   s[128], pp[1000][128], keep[128];
int                     max_str = 100;          /* 文字列の最大長 */
MSDialog                *dbP;
MFCCHK                  *mfcchk;

#ifndef Private
#define Private
#endif

/*- プロトタイプ    -*/
Private  bool	unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private int     checkstart( void);
Private void    sort( void);
Private void    scanStart( void);
Private void    scanStart_Cel( void);
Private void    print_err( void);
Private void    err_mem( void);

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
    WChar   devP[64], dirP[64], nameP[64];

    /*--- リソースファイルのオープン ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"DEL のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

	/*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- 広域変数の取得 ---*/
    if( mdlCExpression_getValue( &value, &result, "&mfcchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"広域変数が取得できません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }
    mfcchk = (MFCCHK*)value.val.valPointer;

    /*--- バッチ処理の場合 ---*/
    if( mfcchk->mode == 1){
        bat_ct = 0;
        bat_now = 0;

        /*--- バッチ処理数の取得 ---*/
        if( ( fp_save = _wfopen(mfcchk->bat,L"r")) != NULL){
            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                bat_ct++;
            }
			fclose(fp_save);
        }
        

        /*--- バッチファイルの記憶 ---*/
        if( ( fp_save = _wfopen( mfcchk->bat,L"r")) != NULL){
            mdlFile_parseName( mfcchk->bat, devP, dirP, nameP, NULL);
            swprintf( logfile, L"%ls:%ls%ls.log", devP, dirP, nameP);
            if( ( fp_log = _wfopen( logfile, L"w")) == NULL){
                mdlOutput_error( L"出力ファイルを開けません");
				mdlSystem_exit(exitStatus, 1);
				return;
            }
            fwprintf( fp_log, L"デザインファイル名（フルパス）                         エラー数");
            fwprintf( fp_log, L"\n");

            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                /*--- 記憶したファイルを開く ---*/
                if( mdlSystem_newDesignFile( filename) == SUCCESS){
                    bat_now++;
                    syori( );
                    fwprintf( fp_log, L"%-55s %4d\n", tcb->dgnfilenm, mfcchk->ct);
                }
            } // WHILE END
			fclose(fp_save);
			fclose(fp_log);
        }
        else{
            swprintf( info, L"%ls が見つかりません", mfcchk->bat);
            mdlDialog_openInfoBox( info);
        }

    }

    /*--- 1ファイルの場合 ---*/
    else{
        syori( );

    }

    mdlSystem_loadMdlProgram( TD_MFCCHK, TD_MFCCHK, L"");
    mdlSystem_exit( exitStatus, 1);
	return;
    
}

/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
Private  bool	unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}

/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void  syori( void)
{
    WChar   buf[256], devP[64], dirP[64], nameP[64];

    memo = 0;   linect = 0;   txtct  = 0;   cellct = 0;

    /*--- 進行状況表示ウィンドウのオープン ---*/
    dbP = mdlDialog_completionBarOpen( L"進行状況");

    /*--- ＭＦＣテーブルのオープン ---*/
    if( ( fp_mfc = _wfopen( mfcchk->table, L"r")) == NULL){
        mdlOutput_error( L"ＭＦＣテーブルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- ソート前エラーファイル名の取得 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, NULL, NULL);
    swprintf( file_err, L"%ls:%lserr.txt", devP, dirP);

    /*--- ソート前エラーファイルのオープン ---*/
    if( ( fp_err = _wfopen( file_err, L"w")) == NULL){
        mdlOutput_error( L"エラーファイルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    while( fgetws( buf, 256, fp_mfc) != NULL){
        /*--- ";"を読み飛ばす ---*/
        if( wmemcmp( buf, L";", 1) == 0)
            continue;

        /*--- タイプ別に文字を読み込む。 ---*/
        /*--- セルの場合 ---*/
        if( wmemcmp( buf, L" 2", 2) == 0){
            swscanf( buf, L"%d %d %d %d %d %ls\n", &cell[memo][0], &cell[memo][1], &cell[memo][2],
                    &cell[memo][3], &cell[memo][4], cname[memo]);
            if( wcscmp( cname[memo], L";") == 0){
                wcscpy( cname[memo], L"");
            }
            memo++;
        }

        /*--- テキストの場合 ---*/
        else if( wmemcmp( buf, L"17", 2) == 0){
            swscanf( buf, L"%d %d %d %d %d %lf %lf %ls %d %d\n", &txt[memo][0], &txt[memo][1],
                    &txt[memo][2], &txt[memo][3], &txt[memo][4], &txth[memo][0], &txth[memo][1],
                     tjust, &txt[memo][5], &txt[memo][6]);
            TC = txt[memo][5];

            if(      wcscmp( tjust, L"LT") == 0){ kijyun[memo] =  0;} /*--- 基準点が左上の場合 ---*/
            else if( wcscmp( tjust, L"LC") == 0){ kijyun[memo] =  1;} /*--- 基準点が左中央の場合 ---*/
            else if( wcscmp( tjust, L"LB") == 0){ kijyun[memo] =  2;} /*--- 基準点が左下の場合 ---*/
            else if( wcscmp( tjust, L"CT") == 0){ kijyun[memo] =  6;} /*--- 基準点が中央上の場合 ---*/
            else if( wcscmp( tjust, L"CC") == 0){ kijyun[memo] =  7;} /*--- 基準点が中央中央の場合 ---*/
            else if( wcscmp( tjust, L"CB") == 0){ kijyun[memo] =  8;} /*--- 基準点が中央下の場合 ---*/
            else if( wcscmp( tjust, L"RT") == 0){ kijyun[memo] = 12;} /*--- 基準点が右上の場合 ---*/
            else if( wcscmp( tjust, L"RC") == 0){ kijyun[memo] = 13;} /*--- 基準点が右中央の場合 ---*/
            else if( wcscmp( tjust, L"RB") == 0){ kijyun[memo] = 14;} /*--- 基準点が右下の場合 ---*/
            else if( wcscmp( tjust, L"-1") == 0){ kijyun[memo] = -1;} /*--- 基準点が全ての場合 ---*/

            memo++;
        }

        /*--- その他の場合 ---*/
        else{
            swscanf( buf, L"%d %d %d %d %d\n", &line[memo][0], &line[memo][1],
                    &line[memo][2], &line[memo][3], &line[memo][4]);
            memo++;
        }
    } // WHILE END

    /*--- 処理開始 ---*/
    checkstart( );
    fclose( fp_err);
    mfcchk->ct = linect + txtct + cellct;

    if( mfcchk->ct == 0){
        /*--- 出力ファイル名の取得 ---*/
        mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
        swprintf( file, L"%ls:%ls%ls.err", devP, dirP, nameP);
        /*--- 出力ファイルのオープン ---*/
        if( ( fp_list = _wfopen( file, L"w")) == NULL){
            mdlOutput_error( L"出力ファイルを開けません");
			mdlSystem_exit(exitStatus, 1);
			return;
        }
        fwprintf( fp_list, L"%ls\n", tcb->dgnfilenm);
        fwprintf( fp_list, L"\nTYP   LV   CO   WT   LC  セル名   字高   字幅   基準   字数   書体  エラー\n");
        fwprintf( fp_list, L"エラーはありません。\n");
        fwprintf( fp_list, L"\n                                                          エラー総数は%d個です。\n", mfcchk->ct);

        fclose( fp_list);
    }
    else{
        sort( );
        print_err( );
    }

    fclose( fp_mfc);

    _wremove( file_err);
    _wremove( file_sort);

    /*--- 進行状況表示ウィンドウのクローズ ---*/
    mdlDialog_completionBarClose( dbP);
}

/*-----------------------------------------------------------------------
**  checkstart
-----------------------------------------------------------------------*/
Private int  checkstart( )
{
    RscFileHandle   rfHandle;

    hankei = mfcchk->size * 1000;

    scanStart( );
    scanStart_Cel( );
    mdlResource_openFile( &rfHandle, NULL, FALSE);

    return( SUCCESS);
}

/*----------------------------------------------------------------------+
|  モジュール名：scanStart                                              |
+----------------------------------------------------------------------*/
Private void scanStart( )
{
    int             getSize;
    int             Error;
    int             ii, flag1, flag2;
    ULong           filePos, complexPos,  EndPos, percent, elemAddr[1];
    double          integral, decimal;
    MSElement       buf;
    MSElement       err;
    ExtScanList     scanList;
    Dpoint3d        userOrigin;
    MSElement       ele;
    MSElementDescr  *elmDP,*NelmDP;
	TextParamWide   textParamW;
	TextParam       textParams;
	WString        wstr;
	MSTextSize      textSize;
	char           mbstr[1024];

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;
	scanList.extendedType = FILEPOS;

    /*--------- 検索対象の決定 ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｰﾌﾟ */
    scanList.typmask[0] |= TMSK0_TEXT_NODE;   /* 対象はﾃｷｽﾄﾉｰﾄﾞ */
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* 対象はｺﾝﾌﾟﾚｯｸｽﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* 対象はｺﾝﾌﾟﾚｯｸｽｼｪｲﾌﾟ */
    scanList.typmask[0] |= TMSK0_ELLIPSE;     /* 対象はｻｰｸﾙ */
    scanList.typmask[0] |= TMSK0_ARC;         /* 対象はｱｰｸ */
    scanList.typmask[1] |= TMSK1_TEXT;        /* 対象はﾃｷｽﾄ */

    /*--------- スキャン実行 ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- 各種変数の初期化 ---*/
    filePos = 0;   numelems = 0;

    /*--- 一番後のファイルポジションを記憶 ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read(&buf, MASTERFILE, elemAddr[0]);
        /*--- 進行状況表示のパーセンテージを計算 ---*/
        percent = elemAddr[0] / EndPos * 100;

        /*--- １ファイルの場合 ---*/
        if( mfcchk->mode == 0){
            wcscpy( battxt, L"進行状況");
        }

        /*--- バッチ処理の場合 ---*/
        else if( mfcchk->mode == 1){
            swprintf( battxt, L"進行状況                (%d/%d)", bat_now, bat_ct);
        }
        mdlDialog_completionBarUpdate( dbP, battxt, percent);

        TYP = mdlElement_getType( &buf);
        get_layer( &LV, &CO, &LC, &WT, NULL, &buf);

        /*--- テキストの場合 ---*/
        if( TYP == 17){
            mdlText_extract( &wstr, NULL, &userOrigin, NULL, &textSizeParam, &textParamW,  NULL, &buf);
			swprintf(str, L"%ls", wstr.GetWCharCP());
			memcpy(&textSize, &textSizeParam.size, sizeof(MSTextSize));
			just = (int)textParamW.just;
            /*--- フォントONの場合 ---*/
            if( mfcchk->font == -1){
                if( textParamW.font == mfcchk->han){       FT = 0;}  // フォントが半角設定と同じの場合
                else if( textParamW.font == mfcchk->zen){  FT = 1;}  // フォントが全角設定と同じの場合
                else{                               FT = 2;}  // フォントが設定と一致しない場合
            }

            /*--- 文字高を四捨五入する ---*/
            integral = floor( textSize.height);        /* 整数部分 */
            decimal  = textSize.height - integral;     /* 小数点以下 */

            if( decimal >= 0.5){ textSize.height = integral + 1;}
            else{                textSize.height = integral;    }

            /*--- 文字幅を四捨五入する ---*/
            integral = floor( textSize.width);        /* 整数部分 */
            decimal  = textSize.width - integral;     /* 小数点以下 */

            if( decimal >= 0.5){ textSize.width = integral + 1;}
            else{                textSize.width = integral;    }
        }

        /*--- 文字ノードの場合 ---*/
        if( TYP == 7){
            mdlTextNode_extract ( &userOrigin, NULL, &textSize, NULL, &textParams, NULL, &buf,NULL);

            /*--- 文字ノードに含まれる、文字の数を取得 ---*/
            /*--- ２次元の場合 ---*/
            if( mdlModelRef_is3D(MASTERFILE) != TRUE){
                numelems = buf.text_node_2d.componentCount;
            }

            /*--- ３次元の場合 ---*/
            else{
                numelems = buf.text_node_3d.componentCount;
            }

            /*--- フォントONの場合 ---*/
            if( mfcchk->font == -1){
                if( textParams.font == mfcchk->han){       FT = 0;}  // フォントが半角設定と同じの場合
                else if( textParams.font == mfcchk->zen){  FT = 1;}  // フォントが全角設定と同じの場合
                else{                                      FT = 2;}  // フォントが設定と一致しない場合
            }
        }

        flag1 = 0, flag2 = 0;

        /*--- MFCテーブルから読み込んだレイヤーと比較する ---*/
        for( ii = 0; ii < memo; ii++){

            /*--- テキストとセル以外の要素と一致する場合 ---*/
            if( line[ii][0] == TYP && line[ii][1] == LV && line[ii][2] == CO &&
                line[ii][3] == WT && line[ii][4] == LC){ 
                flag1++;  //一致したらフラグを立てる
            }

            /*--- テキストと一致する場合 ---*/
            else if( txt[ii][0] == TYP && txt[ii][1] == LV && txt[ii][2] == CO &&
                     txt[ii][3] == WT && txt[ii][4] == LC && txth[ii][0] == textSize.height/1000 &&
                     txth[ii][1] == textSize.width/1000 && 
                   ( kijyun[ii] == just ||  kijyun[ii] == -1) &&
                   ( ( mfcchk->font == -1 && txt[ii][6] == FT) ||  mfcchk->font != -1)){
				wcstombs(mbstr, str, 1024);
                /*--- ２次元の場合 ---*/
                if( mdlModelRef_is3D(MASTERFILE) != TRUE){
					
                    if( strlen( mbstr) == buf.text_2d.numchars){     flag2++;} // 文字が全て半角の場合フラグを立てる
                    if( strlen( mbstr)/2 == buf.text_2d.numchars/2 - 1){ flag2++;} // 文字が全て全角の場合フラグを立てる
                }

                /*--- ３次元の場合 ---*/
                else{
                    if( strlen(mbstr) == buf.text_3d.numchars){     flag2++;} // 文字が全て半角の場合フラグを立てる
                    if( strlen(mbstr) / 2 == buf.text_3d.numchars/2 - 1){ flag2++;} // 文字が全て全角の場合フラグを立てる
                }

                /*--- フォントONの場合 ---*/
                if( mfcchk->font == -1){

                    /*--- エラーのない文字列の場合 ---*/
                    if( flag2 > 0){
                        flag2 = 0;
                        /*--- 半角で設定したフォントと一致するかCK  ---*/
                        if( tsi.font == mfcchk->han){
                            /*--- ２次元の場合 ---*/
                            if( mdlModelRef_is3D(MASTERFILE) != TRUE ){
                                if( strlen(mbstr) == buf.text_2d.numchars){ flag2++;} // 文字が全て半角の場合フラグを立てる
                            }

                            /*--- ３次元の場合 ---*/
                            else{
                                if( strlen(mbstr) == buf.text_3d.numchars){ flag2++;} // 文字が全て半角の場合フラグを立てる
                            }
                        }

                        else if( tsi.font == mfcchk->zen){
                            /*--- ２次元の場合 ---*/
                            if( mdlModelRef_is3D(MASTERFILE) != TRUE ){
                                if( strlen(mbstr) / 2 == buf.text_2d.numchars/2 - 1){ flag2++;} // 文字が全て全角の場合フラグを立てる
                            }

                            /*--- ３次元の場合 ---*/
                            else{
                                if( strlen(mbstr) / 2 == buf.text_3d.numchars/2 - 1){ flag2++;} // 文字が全て全角の場合フラグを立てる
                            }
                        }
                    }
                }

                /*--- スペースONの場合 ---*/
                if( mfcchk->space == -1){
                    if( wcschr( str, L' ') != NULL){  flag2 = 0;} // 半角スペースが存在したら、フラグをリセットする
                    if( wcschr( str, L'　') != NULL){ flag2 = 0;} // 全角スペースが存在したら、フラグをリセットする
                }
            }
        } // FOR ii END

        /*--- タイプが3,4,6,12,14,15,16のどれかで、フラグが立たなかった場合 ---*/
        if( ( TYP == 3 || TYP == 4 || TYP == 6 || TYP == 12 || TYP == 14 || TYP == 15 || TYP == 16) && flag1 == 0){

            /*--- エラーログにエラー要素のレイヤーを書き込む ---*/
            fwprintf ( fp_err, L"%3d %4d %4d %4d %4d\n", TYP, LV, CO, WT, LC);

            /*--- エラーフラグONの場合、ファイルポジションを記憶 ---*/
            if( mfcchk->err == -1){
                linePos[linect] = elemAddr[0];
            }
            linect++;
        }

        /*--- テキストでフラグが立たなかった場合 ---*/
        else if( TYP == 17 && flag2 == 0){
            if( just == 0){       wcscpy( ejust, L"LT");} // 文字基準点が「  左上  」の場合、エラーログ用にコピーする
            else if( just ==  1){ wcscpy( ejust, L"LC");} // 文字基準点が「 左中央 」の場合、エラーログ用にコピーする
            else if( just ==  2){ wcscpy( ejust, L"LB");} // 文字基準点が「  左下  」の場合、エラーログ用にコピーする
            else if( just ==  6){ wcscpy( ejust, L"CT");} // 文字基準点が「 中央上 」の場合、エラーログ用にコピーする
            else if( just ==  7){ wcscpy( ejust, L"CC");} // 文字基準点が「中央中央」の場合、エラーログ用にコピーする
            else if( just ==  8){ wcscpy( ejust, L"CB");} // 文字基準点が「 中央下 」の場合、エラーログ用にコピーする
            else if( just == 12){ wcscpy( ejust, L"RT");} // 文字基準点が「  右上  」の場合、エラーログ用にコピーする
            else if( just == 13){ wcscpy( ejust, L"RC");} // 文字基準点が「 右中央 」の場合、エラーログ用にコピーする
            else if( just == 14){ wcscpy( ejust, L"RB");} // 文字基準点が「  右下  」の場合、エラーログ用にコピーする

            /*--- エラーログにエラー要素のレイヤー等を書き込む ---*/
            fwprintf( fp_err, L"%3d %4d %4d %4d %4d %14.2f %6.2f %5s %6d %6d \n", TYP, LV, CO, WT, LC,
                              textSize.height/1000, textSize.width/1000, ejust, TC, tsi.font);

            /*--- エラーフラグONの場合、ファイルポジションを記憶 ---*/
            if( mfcchk->err == -1){
                txtPos[txtct] = elemAddr[0];
            }
            txtct++;
        }

        /*--- テキストノードの場合 ---*/
        else if( TYP == 7 && flag1 == 0){
            if( textParams.just == TextElementJustification::LeftTop)            { wcscpy( ejust, L"LT");} // 文字基準点が「  左上  」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::LeftMiddle)    { wcscpy( ejust, L"LC");} // 文字基準点が「 左中央 」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::LeftBaseline)  { wcscpy( ejust, L"LB");} // 文字基準点が「  左下  」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::CenterTop)     { wcscpy( ejust, L"CT");} // 文字基準点が「 中央上 」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::CenterMiddle)  { wcscpy( ejust, L"CC");} // 文字基準点が「中央中央」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::CenterBaseline){ wcscpy( ejust, L"CB");} // 文字基準点が「 中央下 」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::RightTop)      { wcscpy( ejust, L"RT");} // 文字基準点が「  右上  」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::RightMiddle)   { wcscpy( ejust, L"RC");} // 文字基準点が「 右中央 」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする
            else if( textParams.just == TextElementJustification::RightBaseline) { wcscpy( ejust, L"RB");} // 文字基準点が「  右下  」の場合、ｴﾗｰﾛｸﾞ用にｺﾋﾟｰする

            fwprintf( fp_err, L"%3d %4d %4d %4d %4d %14.2f %6.2f %5s %6d %6d \n", TYP, LV, CO, WT, LC,
                              textSize.height/1000, textSize.width/1000, ejust, TC, textParams.font);

            if( mfcchk->err == -1){
                txtPos[txtct] = elemAddr[0];
            }
            txtct++;
        }
    } // WHILE END

    /*--- エラーフラグONの場合 ---*/
    if( mfcchk->err == -1){

        /*--- セル、テキスト以外の要素の場合 ---*/
        for( ii = 0; ii < linect; ii++){
            mdlElement_read( &buf, MASTERFILE, linePos[ii]);
            TYP = mdlElement_getType( &buf);

            /*--- 複合連結、複合図形の場合 ---*/
            if( TYP == 12 || TYP == 14){
				mdlElmdscr_read(&elmDP, linePos[ii], MASTERFILE, FALSE, NULL);
				mdlElmdscr_duplicate(&NelmDP, elmDP);
                //( &mfcchk->lv, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL, &ele);
				mdlElmdscr_setProperties(&NelmDP, &mfcchk->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				mdlElmdscr_setSymbology(&NelmDP, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL);
				mdlElmdscr_add(NelmDP);
				mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
                mdlElmdscr_freeAll( &NelmDP);
				mdlElmdscr_freeAll(&elmDP);
            }

            /*--- その他の場合 ---*/
            else{
                memcpy( &err, &buf, sizeof( MSElement));
                set_layer( &mfcchk->lv, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL, &err);
                mdlElement_add( &err);
                mdlElement_display( &err, DRAW_MODE_Normal);
            }
        } // FOR ii END

        /*--- テキストの場合 ---*/
        for( ii = 0; ii < txtct; ii++){
            mdlElement_read( &buf, MASTERFILE, txtPos[ii]);
            TYP = mdlElement_getType( &buf);
			mdlText_extract(NULL, NULL, &userOrigin, NULL, NULL, NULL, NULL, &buf);
            /*--- テキストノードの場合 ---*/
            if( TYP == 7){
                txtPos[ii] = txtPos[ii];
                mdlElement_read( &buf, MASTERFILE, txtPos[ii]);
				if (mdlModelRef_is3D(MASTERFILE)) {
					memcpy(&userOrigin, &buf.text_node_3d.origin, sizeof(Dpoint3d));
				}
				else
				{
					memcpy(&userOrigin, &buf.text_node_2d.origin, sizeof(Dpoint3d));
				}
				//mdlText_extract(NULL, &userOrigin, NULL, NULL, NULL, NULL, NULL, &buf);
            }
            
            create_errCircle( mfcchk->lv, mfcchk->co, mfcchk->lc, mfcchk->wt, hankei, &userOrigin);
            mdlElement_display( &buf, DRAW_MODE_Normal);
        } // FOR ii END
    }
}

/*----------------------------------------------------------------------+
|  モジュール名：scanStart_Cel                                          |
+----------------------------------------------------------------------*/
Private void scanStart_Cel( )
{
    int    getSize;
    int    Error;
    int    jj, flag;
    ULong  filePos;
    WChar   cellName[7];
    MSElement       buf;
	MSElementDescr	*descr;
    ExtScanList     scanList;
    Dpoint3d        origin;
	ULong				elemAddr[1];
	int 				scanwords;

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanListの各メンバーに以下をセット --------------*/
	scanList.scantype =  ELEMTYPE | ONEELEM;
	scanList.extendedType = FILEPOS;
	scanList.clasmsk = 0xFFF1;
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
    filePos = 0;
	//scanwords = sizeof(elemAddr) / sizeof(short);
    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read(&buf, MASTERFILE, elemAddr[0]);
        TYP = mdlElement_getType ( &buf);

        /*--- セルの場合 ---*/
        if( TYP == 2){
            mdlCell_extract( &origin, NULL, NULL, NULL, cellName,NULL,  &buf);
			mdlCell_extractName(cellName, 256, &buf);

			mdlElmdscr_read(&descr, elemAddr[0], MASTERFILE, 0, NULL);
			get_layer( &LV, &CO, &LC, &WT, NULL, &descr->h.firstElem->el);
			mdlElmdscr_freeAll(&descr);
			flag = 0;
			for( jj = 0; jj <= memo; jj++){
				if( ( cell[jj][1] == LV) && ( cell[jj][2] == CO) &&
					( cell[jj][3] == WT) && ( cell[jj][4] == LC) &&
					( wcscmp( cname[jj], cellName) == 0)){
					flag++;
				}
			}
			if( flag == 0){
				fwprintf( fp_err, L"  2 %4d %4d %4d %4d %7s\n", LV, CO, WT, LC, cellName);
				if( mfcchk->err == -1){

					cellPos[cellct] = elemAddr[0];
					mdlElement_read( &buf, MASTERFILE, cellPos[cellct]);
					create_errCircle( mfcchk->lv, mfcchk->co, mfcchk->lc, mfcchk->wt, hankei, &origin);
					mdlElement_display( &buf, DRAW_MODE_Normal);
				}
				cellct++;
			}
		}

    } // WHILE END
}

/*----------------------------------------------------------------------+
|  モジュール名：sort                                                   |
+----------------------------------------------------------------------*/
Private void sort ( )
{
    int   total_str = 10000;       /* 文字列の総数 */
    int   ii, jj,  mm, nn;
    WChar  devP[64], dirP[64];

    if( ( fp_err = _wfopen( file_err, L"r")) == NULL){
        mdlOutput_error( L"入力ファイルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- ソートファイル名の取得 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, NULL, NULL);
    swprintf( file_sort, L"%ls:%lssort.txt", devP, dirP);

    if( ( fp_sort = _wfopen( file_sort, L"w")) == NULL){
        mdlOutput_error( L"出力ファイルを開けません");
		mdlSystem_exit( exitStatus, 1);
		return;
    }

    ii = 0;

    /* 単語をファイルからメモリに読み込む */
    while( fgetws( s, max_str, fp_err) != NULL){
        /* 改行コードを削除 */
        jj = wcslen( s);
        if( ( *( s + jj - 1)) == '\n')
            ( *( s + jj - 1)) = '\0';
        wcscpy( pp[ii], L"");
        wcscpy( pp[ii], s);
        ii++;
    }

    /* 登録した単語の個数が予想よりも少ない場合 */
    if( ii < total_str){
        total_str = ii;
    }

    /* データをソート */
    for( mm = 0; mm < total_str - 1; mm++){
        for( nn = mm + 1; nn < total_str; nn++){
            /* データの比較, 交換 */
            if( wcscmp( pp[mm], pp[nn]) > 0) {
                wcscpy( keep, pp[nn]);
                wcscpy( pp[nn], pp[mm]);
                wcscpy( pp[mm], keep);
            }
        }
    }


    /* ソートの結果を表示 */
    for( ii = 0; ii < total_str; ii++){
        fwprintf( fp_sort, L"%ls\n", pp[ii]);
    }

    fclose( fp_err);
    fclose( fp_sort);
}

/*----------------------------------------------------------------------+
|  モジュール名：err_mem                                                |
+----------------------------------------------------------------------*/
Private void err_mem( )
{
    mdlOutput_error( L"メモリの確保に失敗しました。");
	mdlSystem_exit(exitStatus, 1);
	return;
}

/*----------------------------------------------------------------------+
|  モジュール名：print_err                                              |
+----------------------------------------------------------------------*/
Private void print_err( )
{
    int   count = 0;
    int   ii;
    WChar  before[256], now[256], sort_now[256], sort_before[256];
    WChar  devP[64], dirP[64], nameP[64];

    if( ( fp_sort = _wfopen( file_sort, L"r")) == NULL){
        mdlOutput_error( L"ソートファイルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- 出力ファイル名の取得 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
    swprintf( file, L"%ls:%ls%ls.err", devP, dirP, nameP);

    /*--- 出力ファイルのオープン ---*/
    if( ( fp_list = _wfopen( file, L"w")) == NULL){
        mdlOutput_error( L"出力ファイルを開けません");
		mdlSystem_exit( exitStatus, 1);
		return;
        
    }

    fwprintf( fp_list, L"%ls\n", tcb->dgnfilenm);
    fwprintf( fp_list, L"\nTYP   LV   CO   WT   LC  セル名   字高   字幅   基準   字数   書体  エラー\n");

    fgetws( before, max_str, fp_sort);
    ii = wcslen( before);

    if( ( *( before + ii - 1)) == '\n')
        ( *( before + ii - 1)) = '\0';
    wcscpy( sort_before, before);

    fclose( fp_sort);

    if( ( fp_sort = _wfopen( file_sort, L"r")) == NULL){
        mdlOutput_error( L"ソートファイルを開けません");
		mdlSystem_exit( exitStatus, 1);
		return;
    }

    while( fgetws( now, max_str, fp_sort) != NULL){
        ii = wcslen( now);

        if( ( *( now + ii - 1)) == '\n')
            ( *( now + ii - 1)) = '\0';
        wcscpy( sort_now, now);

        if( wcscmp( sort_before, sort_now) == 0){
            count++;
        }
        else{
            fwprintf( fp_list, L"%-67s %3d\n", sort_before, count);
            wcscpy( sort_before, sort_now);
            count = 1;
        }
    }
    fwprintf( fp_list, L"%-67s %3d\n", sort_now, count);
    fwprintf( fp_list, L"\n                                                         エラー総数は%d個です。\n", mfcchk->ct);
    fwprintf( fp_list, L"ﾀｲﾌﾟ 2:ｾﾙ                        LT：左上");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ 3:ﾗｲﾝ                       LC：左中央");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ 4:ﾗｲﾝｽﾄﾘﾝｸﾞ                 LB：左下");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ 6:ｼｪｲﾌﾟ                     CT：中央上");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ 7:ﾃｷｽﾄﾉｰﾄﾞ                  CC：中央中央");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ12:ｺﾝﾌﾟﾚｯｸｽﾁｪｲﾝ              CB：中央下");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ14:ｺﾝﾌﾟﾚｯｸｽｼｪｲﾌﾟ             RT：右上");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ15:ｻｰｸﾙ                      RC：右中央");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ16:ｱｰｸ                       RB：右下");
    fwprintf( fp_list, L"\nﾀｲﾌﾟ17:ﾃｷｽﾄ");
    fwprintf( fp_list, L"\n");

    fclose( fp_sort);
    fclose( fp_list);
}


