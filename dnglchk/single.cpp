/*-----------------------------------------------------------------------*/
/*-         ダングルチェック                                            -*/
/*-            SINGLE.MC                                                -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "dnglchk.h"

FILE            *fp;
RscFileHandle   Rsc;
MSElement       ele;
Dpoint3d        lPoi[5000], grPoi[5000], errPoi[10000];
ULong           linePos[1000000], compPos[100000];
int             exitStatus, lnv, linect, errct, dnglct, compct;
int             numelems, elemct;
UINT32          olv[100], oco[100], owt[100], memo;
int             olc[100];
WChar           filename[128], info[256], buf[256];
double          percent, hankei;

MSDialog        *dbP;

DNGLCHK         *dnglchk;

#ifndef Private
#define Private
#endif
/*- プロトタイプ    -*/
Private bool    unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private void    dngl_layer( void);
Private void    scan_line( void);
Private void    dongle_check( int *num, Dpoint3d *Poi, double limit);
Private void    return_check( void);


/*-----------------------------------------------------------------------
**  メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    
    CExprResult     result;
    CExprValue      value;

    /*--- リソースファイルのオープン ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"SINGLE のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- 広域変数の取得 ---*/
    if( mdlCExpression_getValue( &value, &result, "&dnglchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"広域変数が取得できません");
        mdlSystem_exit( exitStatus, 1);
		return;
    }
    dnglchk = (DNGLCHK*)value.val.valPointer;

    syori( );

}


/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void    syori( void)
{


    /*--- 進行状況表示の開始 ---*/
    dbP = mdlDialog_completionBarOpen( L"進行状況");

    dngl_layer( );
    scan_line( );

    /*--- 進行状況表示の終了 ---*/
	mdlDialog_completionBarClose(dbP);

    if( errct == 0 && dnglct == 0){
        mdlOutput_error( L"エラーはありません。");
    }
    else if( errct == 0 && dnglct != 0){
        mdlOutput_printf( MSG_ERROR, L"ダングルの数は%d個です。", dnglct);
    }
    else if( errct != 0 && dnglct == 0){
        mdlOutput_printf( MSG_ERROR, L"折り返しの数は%d個です。", errct);
    }
    else if( errct != 0 && dnglct != 0){
        mdlOutput_printf( MSG_ERROR, L"折り返しの数は%d個、ダングルの数は%d個です。", errct, dnglct);
    }
}


/*-------------------------------------------------------------------------*/
//     dngl_layer
/*-------------------------------------------------------------------------*/
Private void  dngl_layer( void)
{
    /*--- レイヤーテーブルのオープン ---*/
    if((fp = _wfopen(dnglchk->table, L"r")) == NULL){
        mdlOutput_error( L"テーブルを開けません");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    memo = 0;

    while( fgetws( buf, 256, fp) != NULL){
        /*--- ";"を読み飛ばす ---*/
        if( wmemcmp( buf, L";", 1) == 0){
            continue;
        }

        swscanf( buf, L"%d %d %d %d\n", &olv[memo], &oco[memo], &olc[memo], &owt[memo]);
        memo++;
    }

    fclose( fp);
}


/*-----------------------------------------------------------------------
**  ライン、ラインストリングの検索
-----------------------------------------------------------------------*/
Private void  scan_line( )
{
    ExtScanList     scanList;
    ULong           filePos;
    int             getSize, Error;
    int             ii, type;
    UInt32          lv, co,  wt;
	int             lc;
	ULong		    elemAddr[1];

    /*--------- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    //scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= ( unsigned short)TMSK0_LINE;
    scanList.typmask[0] |= ( unsigned short)TMSK0_LINE_STRING;
    scanList.typmask[0] |= ( unsigned short)TMSK0_CMPLX_STRING;

    /*--------- スキャン実行 ---------------*/
    filePos = 0;   linect = 0;
    dnglct   = 0;   errct   = 0;   compct = 0;
    numelems = 0;   elemct  = 0;
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        type = mdlElement_getType( &ele);
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

        /*--- 設定レイヤーと一致する要素のファイルポジションを取得 ---*/
        for( ii = 0; ii < memo; ii++){
            if( ( lv == olv[ii] || olv[ii] == -1) && ( co == oco[ii] || oco[ii] == -1) &&
                ( lc == olc[ii] || olc[ii] == -1) && ( wt == owt[ii] || owt[ii] == -1)){
                if( type == 3 || type == 4){
                    linePos[linect] = elemAddr[0];
                    linect++;
                }
            }
        }
    }

    for( ii = 0; ii < linect; ii++){
        mdlElement_read( &ele, MASTERFILE, linePos[ii]);
        mdlLinear_extract( lPoi, &lnv, &ele, MASTERFILE);
        dongle_check( &lnv, lPoi, 1);
        return_check( );

        /*--- 進行状況表示 ---*/

        percent = ( double)linePos[ii] / ( double)linePos[linect-1] * 100;
		mdlDialog_completionBarUpdate( dbP, L"進行状況", percent);
    }
}


/*-----------------------------------------------------------------------
**  ダングルの検索
-----------------------------------------------------------------------*/
Private void  dongle_check( int *num, Dpoint3d *Poi, double limit)
{
    int     ii, jj, numP;

    hankei = dnglchk->size * 1000;

    numP  = *num;
    for( ii = 0; ii < numP-1; ii++){
        for( jj = ii + 1; jj < numP; jj++){
            if ( sqrt( pow( Poi[jj].x - Poi[ii].x, 2)
                     + pow( Poi[jj].y - Poi[ii].y, 2)) < limit){
                if( ( ii == 0 && jj == numP-1)){
                    continue;
                }
                else{
                    create_errCircle( dnglchk->lv1, dnglchk->co1, dnglchk->lc1, dnglchk->wt1, hankei, &Poi[ii]);
                    dnglct++;
                }
            }
        }
    }
    *num = numP;
}


/*-----------------------------------------------------------------------
**  設定した角度内での折り返しの検索
-----------------------------------------------------------------------*/
Private void  return_check( void)
{
    Dpoint3d    linePoi[5000][2];
    double      rad, kakudo;
    int         ii;

    /*--- 連続線分を元に、線分を作成 ---*/
    for( ii = 0; ii < lnv-1; ii++){
        memcpy( &linePoi[ii][0], &lPoi[ii], sizeof( Dpoint3d));
        memcpy( &linePoi[ii][1], &lPoi[ii+1], sizeof( Dpoint3d));
    }

    for( ii = 0; ii < lnv-2; ii++){
        rad = atan2( linePoi[ii][1].y   - linePoi[ii][0].y,   linePoi[ii][1].x   - linePoi[ii][0].x)
            - atan2( linePoi[ii+1][1].y - linePoi[ii+1][0].y, linePoi[ii+1][1].x - linePoi[ii+1][0].x);
        kakudo = rad / fc_piover180;
        if( ( 180 - dnglchk->angle <= kakudo && kakudo <= 180 + dnglchk->angle) ||
           ( -1 * ( 180 + dnglchk->angle) <= kakudo && kakudo <= -1 * (180 - dnglchk->angle))){
            create_errCircle( dnglchk->lv2, dnglchk->co2, dnglchk->lc2, dnglchk->wt2, hankei, &linePoi[ii][1]);
            errct++;
        }
    }
}


