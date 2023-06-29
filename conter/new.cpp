/*-----------------------------------------------------------------------*/
/*-	        NEW.MC														-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
//#include "../CommonCE/naka8.cpp"
#include "conter.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
MSBsplineCurve	curve;
MSElementDescr	*elmDP1, *elmDP2;
Dpoint3d		catPoi[10000];
int				num;
double			time1, time2;
CONTER			*con;

/*-	フック関数		-*/

/*-	プロトタイプ	-*/
bool    unload(UnloadProgramReason unloadType);
void	setFunction( void);
void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode);
void	input_point( Dpoint3d *inpoint);
void	display( int drawMode);
void	bspln_add( void);
void	bspln_undo( void);
void	cleanup( void);

void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double tolerance);
void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP);
void    lineString_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);
void    lineString_display(UInt32 *co, Int32 *lc, UInt32 *wt, Dpoint3d *lPoi, int *nv, DgnDrawMode drawMode);
void    mdlElement_displayCE(MSElementP elm, DgnDrawMode drawMode);
/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult     result;
CExprValue      value;

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--------- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"広域変数取得できません");
      return;
   }
   con = (CONTER*)value.val.valPointer;

   num = 0;   time1 = 0;
   setFunction( );

   return;
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool    unload(UnloadProgramReason unloadType)

{
	mdlTransient_free(&msTransientElmP, true);
	mdlState_clear();
	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**	setFunction
-----------------------------------------------------------------------*/
void	setFunction( )
{
	mdlTransient_free(&msTransientElmP, true);
   if ( num > 0){
	   StateCallback::SetComplexDynamicsFunction((StateFunc_ComplexDynamics)complexDynamics);
   }
   StateCallback::SetDataPointFunction((StateFunc_DataPoint)input_point);
   StateCallback::SetResetFunction((StateFunc_Reset)bspln_undo);
   StateCallback::SetCommandCleanupFunction((StateFunc_CommandCleanup)cleanup);
   mdlOutput_command( L"ダブルクリックで終了");
   mdlOutput_prompt( L"入力/戻る");
}


/*-----------------------------------------------------------------------
**	動的表示
-----------------------------------------------------------------------*/
void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   inpoint[0].z = 0;
   memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
   num++;

   /*--- ２点目以降の場合は、B-スプラインを表示する ---*/
   if ( num >= 2){
      /*--- 表示 ---*/
      display(DRAW_MODE_TempDraw);
   }
   num--;
}


/*-----------------------------------------------------------------------
**	座標の入力
-----------------------------------------------------------------------*/
void	input_point( Dpoint3d *inpoint)
{
	//mdlTransient_free(&msTransientElmP, true);	// いったん削除
	inpoint[0].z = 0;
   /*--- MicroStation 起動後の経過時間 ---*/
   //time2 = mdlSystem_elapsedTime( );
   time2 = mdlSystem_getTicks();
   /*--- シングルクリックの場合 ---*/
   if ( time2 - time1 >= 12){
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
void	bspln_add( )
{
MSElement	ele;
Dpoint3d 	*lPoi;
int			ii, remainNv;

   /*--- 2点目の前にダブルクリックした場合 ---*/
   num--;
   if ( num == 1){
      num++;
      /*--- 表示を消す ---*/
      display( DRAW_MODE_Erase);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2点目以降にダブルクリックした場合 ---*/
   else if ( num >= 2){
		/*--- いったん表示を消す ---*/
		mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Erase);

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
			lPoi = (DPoint3d*)malloc( 10000 * sizeof( Dpoint3d));

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
void	cleanup( )
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
      mdlElmdscr_display( elmDP1, MASTERFILE, DRAW_MODE_Erase);

      /*--- B-スプライン用メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);

      /*--- 要素記述子用メモリの開放 ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }

   /*--- B-スプライン使用しない場合 ---*/
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &num, DRAW_MODE_Erase);
   }

   num = 0;
}


/*-----------------------------------------------------------------------
**	表示
-----------------------------------------------------------------------*/
void	display( int drawMode)
{
MSElement	ele, u;
DPoint3d    *tmpPoi;
int			tmpnv, ii;
UInt32      lv, co, wt;
int         lc;

   /*--- B-スプラインの場合 ---*/
   if ( con->bspln == -1){
      /*--- B-スプライン（カタマラン）の構造体情報作成 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- 構造体情報から要素記述子作成 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- B-スプラインの表示 ---*/
	  mdlTransient_free(&msTransientElmP, true);
	  msTransientElmP = mdlTransient_addElemDescr(msTransientElmP, elmDP1, 0, 0xffff, drawMode, false, false, true);

      /*--- B-スプライン用メモリの開放 ---*/
      mdlBspline_freeCurve( &curve);
   }

   /*--- B-スプラインでない場合 ---*/
   else{
		ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
		ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
		ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
		ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
		mdlLineString_create( &ele, NULL, catPoi, num);
		mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		mdlElement_setSymbology(&ele, &co, &wt, &lc);

      /*--- 表示 ---*/
	  mdlTransient_free(&msTransientElmP, true);
	  msTransientElmP = mdlTransient_addElement(msTransientElmP, &ele, 0, 0xffff, drawMode, false, false, true);

   }

   /*--- 要素記述子用メモリの開放 ---*/
   mdlElmdscr_freeAll( &elmDP1);
}


/*-----------------------------------------------------------------------
**	戻る
-----------------------------------------------------------------------*/
void	bspln_undo( )
{
   /*--- 2点目入力の前 ---*/
   if ( num == 1){
      /*--- 表示を消す ---*/
      display( DRAW_MODE_Erase);

      num--;
      mdlState_clear( );
      setFunction( );
   }

   /*--- 2点目以降 ---*/
   if ( num >= 2){
      /*--- 表示を消す ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &num, DRAW_MODE_Erase);

      /*--- １点戻す ---*/
      num--;
   }
}


/*----------------------------------------------------------------------+
**	座標の計算
+----------------------------------------------------------------------*/
void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance)
{
int			bufferSize;
double      maxDeviation;
Dpoint3d    *tmpP = NULL;

   *nv = 2;   bufferSize = 33;

   /*--- 座標のメモリの割り当て ---*/
   if ( ( tmpP = (DPoint3d*)malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
      return;
   }
   do{
      *nv = ( *nv - 1) * 2 + 1;
      if ( *nv > bufferSize){
         free ( tmpP);

         /*--- 座標のメモリを３倍にして割り当てる ---*/
         bufferSize = *nv * 3;
         if ( ( tmpP = (DPoint3d*)malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
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
void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP)
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
/*------------------------------------------------------------------------------------------------*/
/*--- 複数ラインストリングの配置 -----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void lineString_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv)
{
	MSElement	ele;
	int		    ii, lnv;

	lnv = *nv;
	while (1) {
		if (lnv > 4800) {
			mdlLineString_create(&ele, NULL, lPoi, 4800);
			mdlElement_setProperties(&ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
			mdlElement_setSymbology(&ele, co, wt, lc);
			mdlElement_add(&ele);
			//mdlElement_displayCE(&ele, DgnDrawMode::DRAW_MODE_Normal);

			for (ii = 0; ii < lnv - 4799; ii++) {
				memcpy(&lPoi[ii], &lPoi[ii + 4799], sizeof(Dpoint3d));
			}
			lnv = lnv - 4799;
		}
		else {
			mdlLineString_create(&ele, NULL, lPoi, lnv);
			mdlElement_setProperties(&ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
			mdlElement_setSymbology(&ele, co, wt, lc);
			mdlElement_add(&ele);
			//mdlElement_displayCE(&ele, DgnDrawMode::DRAW_MODE_Normal);
			break;
		}
	}
}


/*------------------------------------------------------------------------------------------------*/
/*--- 複数ラインストリングの表示 -----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void lineString_display(UInt32 * co, Int32 * lc, UInt32 * wt, Dpoint3d * lPoi, int * nv, DgnDrawMode drawMode)
{
	MSElement	ele;
	MSElementDescr	*desc;
	Dpoint3d	*tmpPoi;
	int			ii, lnv;

	lnv = *nv;
	tmpPoi = (Dpoint3d*)malloc(lnv * sizeof(Dpoint3d));

	for (ii = 0; ii < lnv; ii++) {
		memcpy(&tmpPoi[ii], &lPoi[ii], sizeof(Dpoint3d));
	}

	while (1) {
		if (lnv > 5000) {
			mdlLineString_create(&ele, NULL, tmpPoi, 5000);
			mdlElement_setSymbology(&ele, co, wt, lc);
			mdlElement_displayCE(&ele, drawMode);

			for (ii = 0; ii < lnv - 4999; ii++) {
				memcpy(&tmpPoi[ii], &tmpPoi[ii + 4999], sizeof(Dpoint3d));
			}
			lnv = lnv - 4999;
		}
		else {
			mdlLineString_create(&ele, NULL, tmpPoi, lnv);
			mdlElement_setSymbology(&ele, co, wt, lc);
			mdlElement_displayCE(&ele, drawMode);
			break;
		}
	}

	free(tmpPoi);
}
/*-----------------------------------------------------------------------
**	mdlElement_display の代わり
-----------------------------------------------------------------------*/
void mdlElement_displayCE(MSElementP elm, DgnDrawMode drawMode)
{
	mdlTransient_free(&msTransientElmP, true);
	if (elm == NULL)
		return;

	msTransientElmP = mdlTransient_addElement(NULL, elm, 0, 0xffff, drawMode, false, false, true);
}
