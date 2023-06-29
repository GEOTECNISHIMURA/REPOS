/*-----------------------------------------------------------------------*/
/*-	        MODIF.MC		-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <mdl.fdf>

#include <mdl.h>
#include <cexpr.h>
#include <string.h>
#include <math.h>
#include <mdlio.h>
#include <userfnc.h>
#include <cmdlist.h>
#include <mselems.h>
#include <mdlerrs.h>
#include <dbdefs.h>
#include <dberrs.h>
#include <keys.h>
#include <msdefs.h>
#include <msinputq.h>
#include <scanner.h>
#include <dbdlglib.fdf>
#include <naka.h>
#include "conter.h"

#define OPEN  			0
#define CLOSE			1
#define KITEN  			0
#define SYUTEN			1
#define DOUBLE_CLICK	0
#define SETUZOKU		1
#define SAME  			0
#define DIFFERENT		1

RscFileHandle   Rsc;
MSElement       ele, ele2, ele3;
MSBsplineCurve	curve;
MSElementDescr	*elmDP1, *elmDP2;
Dpoint3d		catPoi[100000];
int				exitStatus;

int				sNum,  eNum;
int				siti,  eiti;
double			sleng, eleng;

Dpoint3d		newPoi[100000];
int	        	newnv;

ULong			linPos[100000];						/* 最初に押さえたラインと同じレイヤーの要素	*/
int				linnum;								/* 最初に押さえたラインと同じレイヤーの要素	*/

Dpoint3d		oPoi[2][100000];						/* 元のラインの座標 					*/
ULong			oPos[2][10000];						/* 元のラインのファイル位置 			*/
int	        	olv, oco, olc, owt, ggnum, onv[2], onum[2];/* 元のラインのレイヤー、点数、要素数 	*/

int 			catnv;		 						/* 修正中の点数 */
Dpoint3d		catPoi[100000], tmpPoi[100000];		/* 修正中の座標（表示用） */

double			time1, time2;						/* MicroStation起動後の時間 */

CONTER			*con;


/*-	プロトタイプ	-*/
Private int		unload( int);
Private void	startPrimitive( void);
Private void	get_conter_info( Dpoint3d *inpoint, int view);
Private int		scan_conter( Dpoint3d *inpoint, int *nv, Dpoint3d *Poi, int *num, ULong *Pos, int *iti, int kisyu);
Private void	scan_line( int olv, int oco, int olc, int owt, int *linnum, ULong *linPos);
Private void	setFunction( void);

Private void	place_conter( Dpoint3d *inpoint, int view);
Private void	conter_sakusei( int flag);

Private void	place_conter_open( int close);
Private void	place_conter_close( void);
Private void	place_conter_different( void);

Private void	bspln_add( int flag);
Private int		convertCrvFunc( MSElementUnion *header, void *params, int fileNum, MSElementDescr *edP, MSElementDescr **newEdPP);
Private void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance);
Private void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP);

Private void	exchange_syusei( void);
Private void	delete_orig( int num, ULong *Pos, int nv, Dpoint3d *Poi, int iti, Dpoint3d *start);
Private void	syusei_z( void);

Private void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode);
Private void	display( int drawMode);
Private void	bspln_undo( void);
Private void	cleanup( void);
Private void	get_hirei_z( Dpoint3d *Poi, Dpoint3d *Poi1, Dpoint3d *Poi2);
Private int		judge_conter( int lnv, Dpoint3d *lPoi);

/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
main( )
{
#pragma			resourceID	MODIF_ID
CExprResult     result;
CExprValue      value;

   /*--- リソースファイルのオープン ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox("MSLINK のリソースファイルが開きません");
      return( SUCCESS);
   }

   /*--- アンロード関数セット ---*/
   mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);

   /*--- 広域変数の取得 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox("広域変数が取得できません");
      return( SUCCESS);
   }
   con = value.val.valPointer;
   catnv = 0;   time1 = 0;

   startPrimitive( );

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
Private	int	unload( int unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	startPrimitive
-----------------------------------------------------------------------*/
Private void	startPrimitive( void)
{
   mdlState_startPrimitive( get_conter_info, startPrimitive, NULL, NULL);
   mdlOutput_error("修正を開始する位置を押さえて下さい");
}


/*-----------------------------------------------------------------------
**	コンターの検索
-----------------------------------------------------------------------*/
Private void	get_conter_info( Dpoint3d *inpoint, int view)
{
   scan_conter( inpoint, &onv[0], oPoi[0], &onum[0], oPos[0], &siti, KITEN);
}


/*-----------------------------------------------------------------------
**	コンターの検索
-----------------------------------------------------------------------*/
Private int		scan_conter( Dpoint3d *inpoint, int *nv, Dpoint3d *Poi, int *num, ULong *Pos, int *iti, int kisyu)
{
Scanlist	scanList;
Dpoint3d	nearPoi;
Dvector3d	seg;
ULong		filePos;
int			flag, word, fileNum, getSize, Error;
int			ii, lv, co, lc, wt;

   if ( kisyu == KITEN){
      memcpy( &catPoi[0], inpoint, sizeof( Dpoint3d));
   }

   /*--- スキャンリストの初期化 ---*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ELEMDATA;
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   /*--- スキャン実行 ---*/
   fileNum = 0;   filePos = 0;   flag = 0;
   if ( mdlScan_initialize( fileNum, &scanList) != SUCCESS){
      mdlOutput_prompt("スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
      Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox("無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox("EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox("スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox("無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

      /*--- 終点側の場合はレイヤーチェック ---*/
      if ( kisyu == SYUTEN){
         get_layer( &lv, &co, &lc, &wt, NULL, &ele);
         if ( olv!=lv || oco!=co || olc!=lc || owt!=wt){
            continue;
         }
      }

      /*--- スキャンしたライン上で、押さえた点から最短距離の点 ---*/
      memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

      /*--- 最短距離の点が1mm以下の場合は、スキャンしたラインを押さえたものとする ---*/
      if ( get_leng( &nearPoi, inpoint) < 1){
         word = mdlElement_igdsSize( &ele, 0);
         if ( kisyu == KITEN){
            get_layer( &olv, &oco, &olc, &owt, &ggnum, &ele);
         }

         mdlLinear_extract( Poi, nv, &ele, 0);
         Pos[0] = filePos - 2 * (ULong)word;
         flag++;
         break;
      }
   }

   /*--- 要素が見つからなかった場合 ---*/
   if ( flag == 0){
      if ( kisyu == KITEN){
         mdlDialog_openInfoBox("選び直して下さい");
         startPrimitive( );
      }
      return( ERROR);
   }

   /*--- 要素が見つかった場合 ---*/
   else{
      if ( kisyu == KITEN){ catnv = 1;}
      linnum = 0;
      scan_line( olv, oco, olc, owt, &linnum, linPos);

      /*--- 押さえたラインの連続を検索 ---*/
      *num = 1;
      get_renzoku( linnum, linPos, nv, Poi, num, Pos);

      /*--- 押さえた位置 ---*/
      get_closest_point( nv, Poi, inpoint, &nearPoi, iti, &sleng);
      setFunction( );

      return( SUCCESS);
   }
}


/*-----------------------------------------------------------------------
**	押さえたラインと同レイヤーのラインを検索
------------------------------------------------------------------------*/
Private  void  scan_line( int olv, int oco, int olc, int owt, int *linnum, ULong *linPos)
{
Scanlist	scanList;
ULong		filePos;
int			word, fileNum, getSize, Error;
int			lv, co, lc, wt;

   /*--- スキャンリストの初期化 ---*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ELEMDATA;
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   /*--- 検索レベルの設定 ---*/
   if ( olv != -1){
      scanList.scantype |= (unsigned short)LEVELS;

      scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
      set_lvmask( &olv, scanList.levmask);
   }

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   /*--- スキャン実行 ---*/
   fileNum = 0;   filePos = 0;
   if ( mdlScan_initialize( fileNum, &scanList) != SUCCESS){
      mdlOutput_prompt("スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
      Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox("無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox("EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox("スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox("無効な要素");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

      get_layer( &lv, &co, &lc, &wt, NULL, &ele);
      if ( ( olv==-1 || lv==olv) && ( oco==-1 || co==oco) && ( olc==-1 || lc==olc) && ( owt==-1 || wt==owt)){
         word = mdlElement_igdsSize( &ele, 0);
         linPos[*linnum] = filePos - 2 * (ULong)word;
         *linnum = *linnum + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	setFunction
-----------------------------------------------------------------------*/
Private void	setFunction( void)
{
   mdlState_setFunction( STATE_COMPLEX_DYNAMICS, complexDynamics);
   mdlState_setFunction( STATE_DATAPOINT,        place_conter);
   mdlState_setFunction( STATE_RESET,            bspln_undo);
   mdlState_setFunction( STATE_COMMAND_CLEANUP,  cleanup);

   mdlOutput_command( "ダブルクリックで終了");
   mdlOutput_error("入力/取消");
}


/*-----------------------------------------------------------------------
**	修正するラインの２点目以降が入力された時
-----------------------------------------------------------------------*/
Private void	place_conter( Dpoint3d *inpoint, int view)
{
Dpoint3d	nearPoi;
double		leng;

   memcpy( &catPoi[catnv], inpoint, sizeof( Dpoint3d));
   catnv++;

   /*--- 入力した点が元のライン上にある場合 ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[catnv-1], &nearPoi, &eiti, &eleng);
   leng = get_leng( &catPoi[catnv-1], &nearPoi);
   if ( leng < 1){
      onv[1] = onv[0];
      memcpy( oPoi[1], oPoi[0], onv[0] * sizeof( Dpoint3d));
      onum[1] = onum[0];
      memcpy( oPos[1], oPos[0], onum[0] * sizeof( ULong));
      conter_sakusei( SAME);

      catnv = 0;
      startPrimitive( );
   }

   /*--- 入力した点が元のライン上にない場合 ---*/
   else{
      /*--- 元のラインと同レイヤーのものを検索 ---*/
      linnum = 0;
      if ( scan_conter( inpoint, &onv[1], oPoi[1], &onum[1], oPos[1], &eiti, SYUTEN) == SUCCESS){
         conter_sakusei( DIFFERENT);

         catnv = 0;
         startPrimitive( );
      }

      /*--- 入力した点が元のラインと同レイヤーのライン上にない場合 ---*/
      else{
         /*--- MicroStation 起動後の経過時間 ---*/
         time2 = mdlSystem_elapsedTime( );

         /*--- ダブルクリックの場合 ---*/
         if ( time2 - time1 < 0.2){
            catnv--;
            /*--- 修正部分の高さ ---*/
            syusei_z( );

            /*--- 元のラインのうちの短い側を削除 ---*/
            delete_orig( onum[0], oPos[0], onv[0], oPoi[0], siti, &catPoi[0]);

            /*--- B-スプラインの配置 ---*/
            bspln_add( DOUBLE_CLICK);

            catnv = 0;
            startPrimitive( );
         }
         time1 = time2;
      }
   }
}


/*-----------------------------------------------------------------------
**	コンターの作成
-----------------------------------------------------------------------*/
Private void	conter_sakusei( int flag)
{
   /*--- siti を起点側にする ---*/
   if ( flag == SAME){   exchange_syusei( );}

   /*--- 修正部の起点がどの要素上か ---*/
/*   for( ii=0; ii<onum[0]; ii++){
      mdlElement_read( &ele, 0, oPos[0][ii]);
      memcpy( &nearPoi, &catPoi[0], sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);
      if ( get_leng( &catPoi[0], &nearPoi) < 1){
         sNum = ii;
         break;
      }
   }*/

   /*--- 修正部の終点がどの要素上か ---*/
/*   for( ii=0; ii<onum[1]; ii++){
      mdlElement_read( &ele, 0, oPos[1][ii]);
      memcpy( &nearPoi, &catPoi[catnv-1], sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);
      if ( get_leng( &catPoi[catnv-1], &nearPoi) < 1){
         eNum = ii;
         break;
      }
   }*/

   /*--- 別ラインの場合 ---*/
   if ( flag == DIFFERENT){
      place_conter_different( );
   }
   else{
      /*--- 起終点一致の場合 ---*/
      if ( memcmp( &oPoi[0][0], &oPoi[0][onv[0]-1], sizeof( Dpoint3d)) == 0){
         place_conter_close( );
      }

      /*--- 起終点一致しない場合 ---*/
      else{
         place_conter_open( OPEN);
      }
   }
}


/*-----------------------------------------------------------------------
**	別ラインの場合
-----------------------------------------------------------------------*/
Private void	place_conter_different( )
{
Dpoint3d	nearPoi;
int			iti;
double		leng;

   /*--- 修正部起点の高さ ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[0], &nearPoi, &iti, &leng);
   catPoi[0].z = oPoi[0][iti].z 
               + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[0])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- 起点側ライン削除 ---*/
   delete_orig( onum[0], oPos[0], onv[0], oPoi[0], siti, &catPoi[0]);

   /*--- 修正部終点の高さ ---*/
   get_closest_point( &onv[1], oPoi[1], &catPoi[catnv-1], &nearPoi, &iti, &leng);
   catPoi[catnv-1].z = oPoi[1][iti].z 
                     + ( oPoi[1][iti+1].z - oPoi[1][iti].z)*get_leng( &oPoi[1][iti], &catPoi[catnv-1])/get_leng( &oPoi[1][iti], &oPoi[1][iti+1]);

   /*--- 終点側ライン削除 ---*/
   delete_orig( onum[1], oPos[1], onv[1], oPoi[1], eiti, &catPoi[catnv-1]);

   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	開いている場合
-----------------------------------------------------------------------*/
Private void	place_conter_open( int close)
{
Dpoint3d	nearPoi;
int			ii, iti;
double		leng;

   /*--- 削除 ---*/
   for( ii=0; ii<onum[0]; ii++){
      mdlElement_read( &ele, 0, oPos[0][ii]);
      mdlElement_undoableDelete( &ele, oPos[0][ii], ERASE);
   }

   /*--- 修正部起点の高さ ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[0], &nearPoi, &iti, &leng);
   catPoi[0].z = oPoi[0][iti].z 
               + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[0])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- 修正部分より手前 ---*/
   for( ii=0; ii<=siti; ii++){
      memcpy( &newPoi[ii], &oPoi[0][ii], sizeof( Dpoint3d));
   }
   memcpy( &newPoi[siti+1], &catPoi[0], sizeof( Dpoint3d));
   newnv = siti + 2;
   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- 修正部終点の高さ ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[catnv-1], &nearPoi, &iti, &leng);
   catPoi[catnv-1].z = oPoi[0][iti].z 
                     + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[catnv-1])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- 修正部分より後 ---*/
   memcpy( &newPoi[0], &catPoi[catnv-1], sizeof( Dpoint3d));
   for( ii=eiti+1; ii<onv[0]; ii++){
      memcpy( &newPoi[ii-eiti], &oPoi[0][ii], sizeof( Dpoint3d));
   }
   newnv = onv[0] - eiti;
   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- 修正部分 ---*/
   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	閉じている場合
-----------------------------------------------------------------------*/
Private void	place_conter_close( void)
{
Dpoint3d	Poi[101], nearPoi;
ULong		tmpPos[1000];
int			ii, nv, iti, slv, sco, slc, swt, elv, eco, elc, ewt;
double		leng1, leng2;

   /*--- 削除 ---*/
   for( ii=0; ii<onum[0]; ii++){
      mdlElement_read( &ele, 0, oPos[0][ii]);
      mdlElement_undoableDelete( &ele, oPos[0][ii], TRUE);
   }

   leng1 = 0;   leng2 = 0;
   /*--- 起終点経由しない距離 ---*/
   leng1 = get_leng( &oPoi[0][siti], &oPoi[0][siti+1]) - sleng;
   for( ii=siti+1; ii<eiti; ii++){
      leng1 = leng1 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   leng1 = leng1 + eleng;

   /*--- 起終点経由の距離 ---*/
   leng2 = get_leng( &oPoi[0][eiti], &oPoi[0][eiti+1]) - eleng;
   for( ii=eiti+1; ii<onv[0]-1; ii++){
      leng2 = leng2 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   for( ii=0; ii<siti; ii++){
      leng2 = leng2 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   leng2 = leng2 + sleng;

   /*--- 元の部分 ---*/
   /*--- 修正部分が起終点経由する場合 ---*/
   if ( leng2 < leng1){
      memcpy( &newPoi[0], &catPoi[0], sizeof( Dpoint3d));
      for( ii=siti+1; ii<=eiti; ii++){
         memcpy( &newPoi[ii - siti], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      memcpy( &newPoi[eiti - siti + 1], &catPoi[catnv-1], sizeof( Dpoint3d));
      newnv = eiti - siti + 2;
   }

   /*--- 修正部分が起終点経由しない場合 ---*/
   else{
      memcpy( &newPoi[0], &catPoi[catnv-1], sizeof( Dpoint3d));
      for( ii=eiti+1; ii<onv[0]; ii++){
         memcpy( &newPoi[ii - eiti], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      newnv = onv[0] - eiti;
      for( ii=0; ii<=siti; ii++){
         memcpy( &newPoi[newnv + ii], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      memcpy( &newPoi[newnv + siti + 1], &catPoi[0], sizeof( Dpoint3d));
      newnv = newnv + siti + 2;
   }

   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- 修正部分 ---*/
   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	B-スプラインの配置
-----------------------------------------------------------------------*/
Private void	bspln_add( int flag)
{
MSElement	ele;
Dpoint3d	*lPoi, nearPoi;
int			ii, nearnum, remainNv;
double		leng;

   /*--- ダブルクリックした場合 ---*/
   if ( flag == DOUBLE_CLICK){ catnv--;}

   /*--- 2点目以前にダブルクリックした場合 ---*/
   if ( catnv == 1){
      catnv++;
      /*--- 表示を消す ---*/
      display( ERASE);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2点目以降にダブルクリック、または、接続で終了した場合 ---*/
   else if ( catnv >= 2){
      /*--- いったん表示を消す ---*/
      display( TEMPERASE);
      mdlElmdscr_display( elmDP2, 0, ERASE);

      /*--- 高さの設定 ---*/
      for( ii=0; ii<catnv; ii++){
         catPoi[ii].z = catPoi[0].z;
      }

/*--- B-スプラインの場合 ---*/
if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- 要素記述子用メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP2);
      mdlElmdscr_freeAll( &elmDP1);

      /*--- メモリの割り当て ---*/
      lPoi = malloc( 10000 * sizeof( Dpoint3d));

      /*--- 変換後の各点の座標を計算 ---*/
      computePointsByTolerance( &lPoi, &remainNv, &curve, 1000 * con->genkou);

      /*--- 等高線でない場合 ---*/
      if ( judge_conter( onv[0], oPoi[0]) == FALSE){
         /*--- 修正部分の高さ ---*/
         for( ii=0; ii<remainNv; ii++){
            /*--- 修正部分の点からの最短距離を求める ---*/
            get_closest_point( &onv[0], oPoi[0], &lPoi[ii], &nearPoi, &nearnum, &leng);

            /*--- 比例配分により高さを求める ---*/
            if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
               && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
               lPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
            }
            else{
               get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
               lPoi[ii].z = nearPoi.z;
            }
         }
      }

      /*--- 修正部分配置 ---*/
      del_doubled3( &remainNv, lPoi, 1);
      if ( remainNv >= 2){lineString_add( &olv, &oco, &olc, &owt, &ggnum, lPoi, &remainNv);}

      /*--- メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);
      free( lPoi);
}
/*--- ラインの場合 ---*/
else{
      del_doubled3( &catnv, catPoi, 1);

      /*--- 修正部分の高さ ---*/
      for( ii=0; ii<catnv; ii++){
         /*--- 修正部分の点からの最短距離を求める ---*/
         get_closest_point( &onv[0], oPoi[0], &catPoi[ii], &nearPoi, &nearnum, &leng);

         /*--- 比例配分により高さを求める ---*/
         if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
            && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
            catPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
         }
         else{
            get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
            catPoi[ii].z = nearPoi.z;
         }
      }
      if ( catnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, catPoi, &catnv);}
}
   }

   /*--- メモリの開放 ---*/
   mdlBspline_freeCurve( &curve);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);
}


/*----------------------------------------------------------------------+
**	座標の計算
+----------------------------------------------------------------------*/
Private void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance)
{
int			bufferSize;
double      maxDeviation;
Dpoint3d    *tmpP = NULL;

   *nv = 2;   bufferSize = 33;

   /*--- 座標のメモリの割り当て ---*/
   if ( ( tmpP = malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
      return;
   }
   do{
      *nv = ( *nv - 1) * 2 + 1;
      if ( *nv > bufferSize){
         free ( tmpP);

         /*--- 座標のメモリを３倍にして割り当てる ---*/
         bufferSize = *nv * 3;
         if ( ( tmpP = malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
			return;
         }
	  }

      /*---	B-スプライン曲線から最短距離の座標を計算する ---*/
      computeMinDistToCurve( tmpP, &maxDeviation, *nv, curveP);
   }
   while( maxDeviation > tolerance);

   *lPoi = tmpP;
}


/*----------------------------------------------------------------------+
**	B-スプライン曲線から最短距離の座標を計算する
+----------------------------------------------------------------------*/
Private void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP)
{
double      deviation, step, param;
Dpoint3d    mid, *pP, *endP;

   *maxDeviationP = param = 0.0;
   step = fc_1 /( nv - 1);
   for( pP = endP = poi, endP += nv; pP < endP; pP++){
      /*--- B-スプライン曲線上の点 ---*/
      mdlBspline_evaluateCurvePoint( pP, NULL, curveP, param);
	  if ( pP != poi){
         /*--- 前の点との中点を求める ---*/
	   	 mdlVec_addPoint( &mid, pP, pP-1);
	     mdlVec_scale( &mid, &mid, 0.5);

         /*--- 点から最短距離にあるB-スプライン曲線上の点 ---*/
	     mdlBspline_minimumDistanceToCurve( &deviation, NULL, NULL, &mid, curveP);
	     if ( deviation > *maxDeviationP){
			*maxDeviationP = deviation;
         }
      }
	  param += step;
   }
}


/*-----------------------------------------------------------------------
**	修正部分の入れ替え
-----------------------------------------------------------------------*/
Private void	exchange_syusei( )
{
int		ii;

   if ( siti>eiti || ( siti==eiti && sleng > eleng)){
      exchangei( &siti, &eiti);
      exchanged( &sleng, &eleng);

      for( ii=0; ii<catnv; ii++){
         memcpy( &tmpPoi[ii], &catPoi[ii], sizeof( Dpoint3d));
      }
      for( ii=0; ii<catnv; ii++){
         memcpy( &catPoi[ii], &tmpPoi[catnv-1-ii], sizeof( Dpoint3d));
      }
   }
}


/*----------------------------------------------------------------------+
**	元のラインを削除する
+----------------------------------------------------------------------*/
Private void	delete_orig( int num, ULong *Pos, int nv, Dpoint3d *Poi, int iti, Dpoint3d *start)
{
double		leng1, leng2;
int			ii;

   /*--- 元のラインを削除 ---*/
   for( ii=0; ii<num; ii++){
      mdlElement_read( &ele, 0, Pos[ii]);
      mdlElement_undoableDelete( &ele, Pos[ii], ERASE);
   }

   leng1 = leng2 = 0;
   /*--- 押さえた点より起点側 ---*/
   for( ii=0; ii<iti; ii++){
      leng1 += get_leng( &Poi[ii], &Poi[ii+1]);
   }
   leng1 += get_leng( &Poi[iti], start);

   /*--- 押さえた点より終点側 ---*/
   leng2 += get_leng( &Poi[iti+1], start);
   for( ii=iti+1; ii<nv-1; ii++){
      leng2 += get_leng( &Poi[ii], &Poi[ii+1]);
   }
   /*--- 起点側が長い場合 ---*/
   if ( leng1 > leng2){
      memcpy( &Poi[iti+1], start, sizeof( Dpoint3d));
      nv = iti + 2;
      del_doubled3( &nv, Poi, 1);

      if ( nv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, Poi, &nv);}
   }

   /*--- 終点側が長い場合 ---*/
   else{
      for( ii=0; ii<nv; ii++){
         memcpy( &tmpPoi[ii], &Poi[ii], sizeof( Dpoint3d));
      }
      for( ii=0; ii<nv-iti-1; ii++){
         memcpy( &Poi[ii], &tmpPoi[nv-1-ii], sizeof( Dpoint3d));
      }
      memcpy( &Poi[nv - iti - 1], start, sizeof( Dpoint3d));
      nv = nv - iti;
      del_doubled3( &nv, Poi, 1);

      if ( nv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, Poi, &nv);}
   }
}


/*----------------------------------------------------------------------+
**	修正部分に高さを与える
+----------------------------------------------------------------------*/
Private void	syusei_z( void)
{
Dpoint3d	nearPoi;
int			ii, nearnum;
double		leng;

   /*--- 修正部分の高さ ---*/
   for( ii=0; ii<catnv; ii++){
      /*--- 修正部分の点からの最短距離を求める ---*/
      get_closest_point( &onv[0], oPoi[0], &catPoi[ii], &nearPoi, &nearnum, &leng);

      /*--- 比例配分により高さを求める ---*/
      if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
         && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
         catPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
      }
      else{
         get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
         catPoi[ii].z = nearPoi.z;
      }
   }
}


/*-----------------------------------------------------------------------
**	戻る
-----------------------------------------------------------------------*/
Private void	bspln_undo( )
{
   /*--- 2点目入力の前 ---*/
   if ( catnv == 1){
      /*--- 表示を消す ---*/
      display( TEMPERASE);

      mdlState_clear( );
      catnv = 0;
      startPrimitive( );
   }

   /*--- 2点目以降 ---*/
   if ( catnv >= 2){
      /*--- 表示を消す ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &catnv, ERASE);

      /*--- １点戻す ---*/
      catnv--;
   }
}


/*-----------------------------------------------------------------------
**	クリーンアップ
-----------------------------------------------------------------------*/
Private void	cleanup( )
{
   /*--- 修正部分の表示の消去 ---*/
   catnv++;

   /*--- B-スプライン使用の場合 ---*/
   if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- 直前のB-スプラインの表示を消す ---*/
      mdlElmdscr_display( elmDP1, 0, ERASE);

      /*--- B-スプライン用メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);

      /*--- 要素記述子用メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &catnv, ERASE);
   }

   catnv = 0;
   startPrimitive( );
}


/*-----------------------------------------------------------------------
**	ラバーバンド表示
-----------------------------------------------------------------------*/
Private void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   memcpy( &catPoi[catnv], inpoint, sizeof( Dpoint3d));
   catnv++;

   /*--- ２点目以降の場合は、B-スプラインを表示する ---*/
   if ( catnv >= 2){
      /*--- 表示 ---*/
      display( TEMPDRAW);
   }
   catnv--;
}


/*-----------------------------------------------------------------------
**	表示
-----------------------------------------------------------------------*/
Private void	display( int drawMode)
{
MSElement	u;
Dpoint3d	*tmpPoi;
int			ii, tmpnv;

   mdlParams_setActive( ( int *)oco, ACTIVEPARAM_COLOR);
   mdlParams_setActive( ( int *)olc, ACTIVEPARAM_LINESTYLE);
   mdlParams_setActive( ( int *)owt, ACTIVEPARAM_LINEWEIGHT);

   /*--- B-スプラインの場合 ---*/
   if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- B-スプラインの表示 ---*/
      mdlElmdscr_display( elmDP1, 0, drawMode);

      /*--- B-スプライン用メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);
   }

   /*--- B-スプラインでない場合 ---*/
   else{
      /*--- ヘッダ作成 ---*/
      mdlComplexChain_createHeader( &u, FALSE, 0);
      mdlElmdscr_new( &elmDP1, NULL, &u);

      /*--- 保存 ---*/
      tmpPoi = malloc( 10000 * sizeof( Dpoint3d));
      memcpy( tmpPoi, catPoi, catnv * sizeof( Dpoint3d));
      tmpnv  = catnv;

      while( 1){
         if ( tmpnv > 101){
            mdlLineString_create( &ele, NULL, tmpPoi, 101);
			mdlElmdscr_appendElement( elmDP1, &ele);

            for( ii=0; ii<tmpnv-100; ii++){
               memcpy( &tmpPoi[ii], &tmpPoi[ii+100], sizeof( Dpoint3d));
            }
            tmpnv = tmpnv - 100;
         }
         else{
            mdlLineString_create( &ele, NULL, tmpPoi, tmpnv);
			mdlElmdscr_appendElement( elmDP1, &ele);
            break;
         }
      }

      /*--- 表示 ---*/
      mdlElmdscr_display( elmDP1, 0, drawMode);

      /*--- メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP1);
      free( tmpPoi);
   }

   /*--- 要素記述子用メモリの開放 ---*/
   mdlElmdscr_freeAll( &elmDP1);
}


/*-----------------------------------------------------------------------
**	２点間の比例配分で高さ取得
-----------------------------------------------------------------------*/
Private void	get_hirei_z( Dpoint3d *Poi, Dpoint3d *Poi1, Dpoint3d *Poi2)
{
Dpoint3d	tmpPoi[2], nearPoi;
int			iti, nv;
double		leng;

   memcpy( &tmpPoi[0], Poi1, sizeof( Dpoint3d));
   memcpy( &tmpPoi[1], Poi2, sizeof( Dpoint3d));
   nv = 2;

   /*--- 高さを与える点から基準となる2点を結ぶ線分に引いた垂線の足 ---*/
   get_closest_point( &nv, tmpPoi, Poi, &nearPoi, &iti, &leng);

   /*--- 基準となる2点の（X, Y）が同じ場合 ---*/
   if ( get_leng( Poi1, Poi2) < 1){
      Poi->z = ( Poi1->z + Poi2->z)/2;
   }

   /*--- 基準となる2点の（X, Y）が違う場合 ---*/
   else{
      Poi->z = Poi1->z + ( Poi2->z - Poi1->z) * get_leng( &nearPoi, Poi1) / get_leng( Poi2, Poi1);
   }
}


/*-----------------------------------------------------------------------
**	等高線かどうかの判定
-----------------------------------------------------------------------*/
Private int	judge_conter( int lnv, Dpoint3d *lPoi)
{
int			ii;

   /*--- 修正部分 ---*/
   for( ii=1; ii<lnv; ii++){
      if ( lPoi[ii].z != lPoi[0].z){
         return( FALSE);
      }
   }

   return( TRUE);
}
