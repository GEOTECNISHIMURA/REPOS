/*-----------------------------------------------------------------------*/
/*-	        NEW.MC														-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <mdl.fdf>

#include <mdl.h>
#include <mdlio.h>
#include <dlogitem.h>
#include <cexpr.h>
#include <userfnc.h>
#include <cmdlist.h>
#include <dlogbox.h>
#include <dlogids.h>
#include <dlogman.fdf>
#include <mselems.h>
#include <mdlerrs.h>
#include <keys.h>
#include <msdefs.h>
#include <msinputq.h>
#include <system.h>
#include <math.h>
#include <string.h>
#include <scanner.h>
#include <tcb.h>
#include <naka.h>
#include "conter.h"

FILE			*fp;
RscFileHandle   Rsc;
DialogBox       *db_p;
MSBsplineCurve	curve;
MSElementDescr	*elmDP1, *elmDP2;
Dpoint3d		catPoi[10000];
int				num;
double			time1, time2;
CONTER			*con;

/*-	フック関数		-*/

/*-	プロトタイプ	-*/
Private int		unload( int);
Private void	setFunction( void);
Private void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode);
Private void	input_point( Dpoint3d *inpoint);
Private void	display( int drawMode);
Private void	bspln_add( void);
Private void	bspln_undo( void);
Private void	cleanup( void);

Private void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double tolerance);
Private void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
main( )
{
#pragma			resourceID	NEW_ID
CExprResult     result;
CExprValue      value;

   /*--------- アンロード関数セット ---------------*/
   mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);

   /*--------- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox("広域変数取得できません");
      return( SUCCESS);
   }
   con = value.val.valPointer;

   num = 0;   time1 = 0;
   setFunction( );

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
**	setFunction
-----------------------------------------------------------------------*/
Private void	setFunction( )
{
   if ( num > 0){
      mdlState_setFunction( STATE_COMPLEX_DYNAMICS, complexDynamics);
   }
   mdlState_setFunction( STATE_DATAPOINT,       input_point);
   mdlState_setFunction( STATE_RESET,           bspln_undo);
   mdlState_setFunction( STATE_COMMAND_CLEANUP, cleanup);
   mdlOutput_command( "ダブルクリックで終了");
   mdlOutput_prompt( "入力/戻る");
}


/*-----------------------------------------------------------------------
**	動的表示
-----------------------------------------------------------------------*/
Private void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
   num++;

   /*--- ２点目以降の場合は、B-スプラインを表示する ---*/
   if ( num >= 2){
      /*--- 表示 ---*/
      display( TEMPDRAW);
   }
   num--;
}


/*-----------------------------------------------------------------------
**	座標の入力
-----------------------------------------------------------------------*/
Private void	input_point( Dpoint3d *inpoint)
{
   /*--- MicroStation 起動後の経過時間 ---*/
   time2 = mdlSystem_elapsedTime( );

   /*--- シングルクリックの場合 ---*/
   if ( time2 - time1 >= 0.2){
      /*--- 座標の保存 ---*/
      memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
      num++;

      /*--- 動的表示を可能にする ---*/
      if ( num == 1){
         setFunction( );
      }

      /*--- 時刻の保存 ---*/
      time1 = time2;
   }

   /*--- ダブルクリックの場合 ---*/
   else{
      /*--- B-スプラインの配置 ---*/
      bspln_add( );
   }
}


/*-----------------------------------------------------------------------
**	B-スプラインの配置
-----------------------------------------------------------------------*/
Private void	bspln_add( )
{
MSElement	ele;
Dpoint3d 	*lPoi;
int			ii, remainNv;

   /*--- 2点目の前にダブルクリックした場合 ---*/
   num--;
   if ( num == 1){
      num++;
      /*--- 表示を消す ---*/
      display( ERASE);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2点目以降にダブルクリックした場合 ---*/
   else if ( num >= 2){
      /*--- いったん表示を消す ---*/
      mdlElmdscr_display( elmDP2, 0, ERASE);

      /*--- 高さの設定 ---*/
      for( ii=0; ii<num; ii++){
         catPoi[ii].z = 1000 * con->zz;
      }

/*--- B-スプラインの場合 ---*/
if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- 要素記述子用メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP2);
      mdlElmdscr_freeAll( &elmDP1);

      /*--- メモリの割り当て ---*/
      lPoi = malloc( 10000 * sizeof( Dpoint3d));

      /*--- 変換後の各点の座標を計算 ---*/
      computePointsByTolerance( &lPoi, &remainNv, &curve, 1000 * con->genkou);

      lineString_add( NULL, NULL, NULL, NULL, NULL, lPoi, &remainNv);

      /*--- メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);
      free( lPoi);
}
/*--- ラインの場合 ---*/
else{
      lineString_add( NULL, NULL, NULL, NULL, NULL, catPoi, &num);
}
   }

   /*--- メモリの開放 ---*/
   mdlBspline_freeCurve( &curve);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);

   num = 0;
   mdlState_clear( );
   setFunction( );
}


/*-----------------------------------------------------------------------
**	クリーンアップ
-----------------------------------------------------------------------*/
Private void	cleanup( )
{
   /*--- 修正部分の表示の消去 ---*/
   num++;

   /*--- B-スプライン使用の場合 ---*/
   if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- 直前の表示を消す ---*/
      mdlElmdscr_display( elmDP1, 0, ERASE);

      /*--- B-スプライン用メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);

      /*--- 要素記述子用メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }

   /*--- B-スプライン使用しない場合 ---*/
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &num, ERASE);
   }

   num = 0;
}


/*-----------------------------------------------------------------------
**	表示
-----------------------------------------------------------------------*/
Private void	display( int drawMode)
{
MSElement	ele, u;
Dpoint3d	*tmpPoi;
int			tmpnv, ii;

   /*--- B-スプラインの場合 ---*/
   if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

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
      memcpy( tmpPoi, catPoi, num * sizeof( Dpoint3d));
      tmpnv  = num;

      while( 1){
         if ( tmpnv > 101){
            mdlLineString_create( &ele, NULL, tmpPoi, 101);
			mdlElmdscr_appendElement( elmDP1, &ele);

            for( ii=0; ii<tmpnv-100; ii++){
               memcpy( &tmpPoi[ii], &tmpPoi[ii+100], sizeof(Dpoint3d));
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
**	戻る
-----------------------------------------------------------------------*/
Private void	bspln_undo( )
{
   /*--- 2点目入力の前 ---*/
   if ( num == 1){
      /*--- 表示を消す ---*/
      display( TEMPERASE);

      num--;
      mdlState_clear( );
      setFunction( );
   }

   /*--- 2点目以降 ---*/
   if ( num >= 2){
      /*--- 表示を消す ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &num, ERASE);

      /*--- １点戻す ---*/
      num--;
   }
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
