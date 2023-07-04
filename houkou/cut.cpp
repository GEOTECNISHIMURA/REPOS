/*-----------------------------------------------------------------------*/
/*-	        CUT.MC							-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "houkou.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog 		*db_p;
WChar			full[128];
MSElementDescr  *elmDP1, *elmDP2, *elmDP3;
MSElement  		ele;
ULong			firstPos, secondPos, Pos;
DPoint3d		Poi[2];                      /* セルの中心 */
DPoint3d		isP1, isP2, isP3[2];         /* セル同士を結ぶ線とセルの外形との交点 */
DPoint3d		houPoi[2];                   /* 方向線 */
DPoint3d		rbP[2];                      /* ラバーバンド */
double			length, maxlength;
int				type, index, exitStatus;
DgnModelRef     *modelRef, *modelRef2;
UInt32			lv1, co1, wt1;
UInt32			lv2, co2, wt2;
int             lc1, lc2;
HOUSET      	*houset;

#ifndef Private
#define Private
#endif

/*-	プロトタイプ	-*/
Private bool	unload(UnloadProgramReason unloadType);
Private void	cell_first( void);
Private void	cell_first_select( Dpoint3d *inpoint,  int view);
Private void	dynamic( Dpoint3d *inpoint,  int view);
Private void	cell_second( void);
Private void	cell_second_select( Dpoint3d *inpoint,  int view);
Private void	line( void);

Private void	reset( void);
Private void	end( void);
Private void	elemType( void);


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

   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"CUTのリソースファイルが開きません");
      return;

   }

   /*--- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&houset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"広域変数が取得できません");
      return;
   }
   houset = (HOUSET*)value.val.valPointer;

   /*--- アンロード関数セット ---------------*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   cell_first( );

   return;
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
Private	bool	unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	第１セル
-----------------------------------------------------------------------*/
Private void	cell_first( )
{
   /*--- リセットボタン、データボタンを押した時 ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)cell_first, (MdlFunc_Accept)cell_first_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"セルを選んで下さい");
   mdlOutput_error(L"");
}


/*-----------------------------------------------------------------------
**	第１セル選択
-----------------------------------------------------------------------*/
Private void	cell_first_select( Dpoint3d *inpoint,  int view)
{
MSElementDescrP  dscrP;

   /*--- セル中の楕円要素を読み込む ------*/
   firstPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
   type = mdlElement_getType( dgnBuf);

   /*--- セルを選んだ場合 ------*/
   if ( type == 2){
      /*--- セルの基準点座標を取得 ------*/
      mdlCell_extract( &Poi[0], NULL, NULL, NULL, NULL, NULL, dgnBuf);
      Pos = firstPos;
      mdlElement_read( dgnBuf, modelRef, Pos);
	  mdlElmdscr_read( &dscrP, Pos, MASTERFILE, false, NULL);

      /*--- レイヤーがアクティブの場合 ------*/
      if ( houset->layer == 0){
		  ActiveParams::GetValue(lv1, ACTIVEPARAM_LEVEL);
		  ActiveParams::GetValue(co1, ACTIVEPARAM_COLOR);
		  ActiveParams::GetValue(wt1, ACTIVEPARAM_LINEWEIGHT);
		  ActiveParams::GetValue(lc1, ACTIVEPARAM_LINESTYLE);

      }

      /*--- レイヤーがセルと同じ場合 ------*/
      if ( houset->layer == 1){
		 mdlElement_getProperties( &lv1, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, &dscrP->h.firstElem->el);
         mdlElement_getSymbology( &co1, &wt1, &lc1, dgnBuf);
		 
      }

      /*--- レイヤーを設定する場合 ------*/
      if ( houset->layer == 2){
         lv1 = houset->lv;
         co1 = houset->co;
         lc1 = houset->lc;
         wt1 = houset->wt;
      }
	  mdlElmdscr_freeAll(&dscrP);
      cell_second( );
   }

   /*--- セル以外を選んでしまった場合 ------*/
   else{
      mdlOutput_prompt(L"Ｒボタンを押して下さい");
      mdlOutput_error(L"この要素は選べません");
   }
}


/*-----------------------------------------------------------------------
**	第２セル
-----------------------------------------------------------------------*/
Private void	cell_second( )
{
   /*--- リセットボタン、データボタンを押した時 ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)reset, (MdlFunc_Accept)cell_second_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"セルを選んで下さい");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)dynamic, FALSE);
}


/*-----------------------------------------------------------------------
**	方向線のラバーバンド
-----------------------------------------------------------------------*/
Private void	dynamic( Dpoint3d *inpoint,  int view)
{
   memcpy( &rbP[0], &Poi[0], sizeof( Dpoint3d));
   memcpy( &rbP[1], inpoint, sizeof( Dpoint3d));

   mdlLineString_create( dgnBuf, NULL, rbP, 2);
}


/*-----------------------------------------------------------------------
**	第２セル選択
-----------------------------------------------------------------------*/
Private void	cell_second_select( Dpoint3d *inpoint,  int view)
{
MSElementDescrP  dscrP;

   /*--- セル中の楕円要素を読み込む ------*/
   secondPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);

   type = mdlElement_getType( dgnBuf);

   /*--- セルを選んだ場合 ------*/
   if ( type == 2){
      /*--- セルの基準点座標を取得 ------*/
      mdlCell_extract( &Poi[1], NULL, NULL, NULL, NULL, NULL, dgnBuf);

      Pos = secondPos;
      mdlElement_read( dgnBuf, modelRef, Pos);
	  mdlElmdscr_read(&dscrP, Pos, MASTERFILE, false, NULL);

      /*--- レイヤーがアクティブの場合 ------*/
      if ( houset->layer == 0){
		 ActiveParams::GetValue(lv2, ACTIVEPARAM_LEVEL);
		 ActiveParams::GetValue(co2, ACTIVEPARAM_COLOR);
		 ActiveParams::GetValue(wt2, ACTIVEPARAM_LINEWEIGHT);
		 ActiveParams::GetValue(lc2, ACTIVEPARAM_LINESTYLE);

      }

      /*--- レイヤーがセルと同じ場合 ------*/
      if ( houset->layer == 1){
         mdlElement_getProperties( &lv2, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, &dscrP->h.firstElem->el);
         mdlElement_getSymbology( &co2, &wt2, &lc2, dgnBuf);
		 
      }

      /*--- レイヤーを設定する場合 ------*/
      if ( houset->layer == 2){
         lv2 = houset->lv;
         co2 = houset->co;
         lc2 = houset->lc;
         wt2 = houset->wt;
      }
	  mdlElmdscr_freeAll(&dscrP);
      line( );
   }

   /*--- セル以外を選んでしまった場合 ------*/
   else{
      mdlOutput_prompt(L"Ｒボタンを押して下さい");
      mdlOutput_error(L"この要素は選べません");
   }
}


/*-----------------------------------------------------------------------
**	方向線を配置
-----------------------------------------------------------------------*/
Private void	line( )
{
MSElementDescr  *cellDP, *cellDP2;
MSElementDescr  *txtNodeDP;
RotMatrix  		rMatrix;
Dpoint3d		scale;
double			rad;
int				nv = 2;
bool           solid = FALSE;


   /*--- 第１セルと第２セルを結ぶラインを作成 ------*/
   mdlLine_create( dgnBuf, NULL, Poi);
   mdlElmdscr_new( &elmDP3, NULL, dgnBuf);

   /*--- 第１セルを読み込む ------*/
   mdlElmdscr_read( &elmDP1, firstPos, modelRef, 0, NULL);
   maxlength = 0;
   elmDP1 = elmDP1->h.firstElem;
   for( ; elmDP1; elmDP1 = elmDP1->h.next){
      /*--- 第１セル内の各要素とラインとの交点 ------*/
      mdlIntersect_closestBetweenElms( &isP1, &isP3[0], &index, elmDP1,
                                       elmDP3, NULL, &Poi[0], 0.001);

      /*--- 第１セルの基準点から１番遠い交点を求める ------*/
      length = sqrt( pow( Poi[0].x - isP3[0].x, 2) 
                   + pow( Poi[0].y - isP3[0].y, 2));
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[0], &isP3[0], sizeof( Dpoint3d));
      }
   }

   /*--- 方向線がラインの場合 ------*/
   if ( houset->cut == 1){
      /*--- 第１セルから図上1mmの点の座標 ------*/
      houPoi[1].x = houPoi[0].x + houset->scale * ( houPoi[0].x - Poi[0].x)
                  / sqrt( pow( houPoi[0].x-Poi[0].x, 2) + pow( houPoi[0].y-Poi[0].y, 2));
      houPoi[1].y = houPoi[0].y + houset->scale * ( houPoi[0].y - Poi[0].y)
                  / sqrt( pow( houPoi[0].x-Poi[0].x, 2) + pow( houPoi[0].y-Poi[0].y, 2));

      /*--- 第１セルから図上1mmの方向線を作成 ------*/
      lineString_add( &lv1, &co1, &lc1, &wt1, NULL, houPoi, &nv);
   }

   /*--- 方向線がセルの場合 ------*/
   else{
      scale.x = scale.y = scale.z = 1;
      rad = atan2( Poi[1].y - Poi[0].y, Poi[1].x - Poi[0].x);
      mdlRMatrix_fromAngle( &rMatrix, rad);
      mdlCell_getElmDscr( &cellDP, &txtNodeDP, &Poi[0], &scale, NULL, &rMatrix, NULL, 0, 0, NULL, houset->cell, NULL);

      /*--- セルヘッダ ------*/
      mdlCell_create( &ele, houset->cell, &Poi[0], houset->zukei);
      mdlElement_setSymbology( &ele, &co1, &wt1, &lc1);
      mdlElement_setProperties( &ele, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &solid);
      if ( mdlModelRef_is3D( modelRef)) {
         memcpy( &ele.cell_3d.transform, &cellDP->el.cell_3d.transform, sizeof(cellDP->el.cell_3d.transform));
      }
      else{
         memcpy( &ele.cell_2d.transform, &cellDP->el.cell_2d.transform, sizeof(cellDP->el.cell_2d.transform));
      }
      mdlElmdscr_new( &cellDP2, NULL, &ele);

      /*--- ヘッダ内 ------*/
      cellDP = cellDP->h.firstElem;
      for( ; cellDP; cellDP = cellDP->h.next){
         mdlElement_setProperties( &cellDP->el, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &cellDP->el, &co1, &wt1, &lc1);

         /*--- 点セルの場合はスナップ不可能にする ------*/
         if ( houset->zukei == TRUE){
            if ( mdlModelRef_is3D( modelRef)) { cellDP->el.cell_3d.dhdr.props.b.s = 1;}
            else{                       cellDP->el.cell_2d.dhdr.props.b.s = 1;}
         }
         else{
            if ( mdlModelRef_is3D( modelRef)) { cellDP->el.cell_3d.dhdr.props.b.s = 0;}
            else{                       cellDP->el.cell_2d.dhdr.props.b.s = 0;}
         }
         mdlElmdscr_appendElement( cellDP2, &cellDP->el);
      }

      mdlElmdscr_add( cellDP2);
      mdlElmdscr_display( cellDP2, modelRef, DRAW_MODE_Normal);
      mdlElmdscr_freeAll( &cellDP);
      mdlElmdscr_freeAll( &cellDP2);
   }

   /*--- 第２セルを読み込む ------*/
   mdlElmdscr_read( &elmDP2, secondPos, modelRef2, 0, NULL);
   maxlength = 0;
   elmDP2    = elmDP2->h.firstElem;
   for( ; elmDP2; elmDP2 = elmDP2->h.next){
      /*--- 第２セル内の各要素とラインとの交点 ------*/
      mdlIntersect_closestBetweenElms( &isP2, &isP3[1], &index, elmDP2,
                                       elmDP3, NULL, &Poi[1], 0.001);

      /*--- 第２セルの基準点から１番遠い交点を求める ------*/
      length = sqrt( pow( Poi[1].x - isP3[1].x, 2) 
                   + pow( Poi[1].y - isP3[1].y, 2));
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[0], &isP3[1], sizeof( Dpoint3d));
      }
   }

   /*--- 方向線がラインの場合 ------*/
   if ( houset->cut == 1){
      /*--- 第２セルから図上1mmの点の座標 ------*/
      houPoi[1].x = houPoi[0].x + houset->scale * ( houPoi[0].x - Poi[1].x)
                  / sqrt( pow( houPoi[0].x-Poi[1].x, 2) + pow( houPoi[0].y-Poi[1].y, 2));
      houPoi[1].y = houPoi[0].y + houset->scale * ( houPoi[0].y - Poi[1].y)
                  / sqrt( pow( houPoi[0].x-Poi[1].x, 2) + pow( houPoi[0].y-Poi[1].y, 2));

      /*--- 第２セルから図上1mmの方向線を作成 ------*/
      lineString_add( &lv2, &co2, &lc2, &wt2, NULL, houPoi, &nv);
   }

   /*--- 方向線がセルの場合 ------*/
   else{
      scale.x = scale.y = scale.z = 1;
      rad = atan2( Poi[0].y - Poi[1].y, Poi[0].x - Poi[1].x);
      mdlRMatrix_fromAngle( &rMatrix, rad);
      mdlCell_getElmDscr( &cellDP, &txtNodeDP, &Poi[1], &scale,NULL, &rMatrix, NULL, 0, 0, NULL, houset->cell, NULL);

      /*--- セルヘッダ ------*/
      mdlCell_create( &ele, houset->cell, &Poi[1], houset->zukei);
      mdlElement_setSymbology( &cellDP->el, &co2, &wt2, &lc2);
      mdlElement_setProperties( &cellDP->el, NULL, NULL, NULL, NULL, NULL, NULL, &houset->zukei, NULL);
      mdlElement_setProperties( &ele, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &solid);
      if ( mdlModelRef_is3D(modelRef2)) {
         memcpy( &ele.cell_3d.transform, &cellDP->el.cell_3d.transform, sizeof( cellDP->el.cell_3d.transform));
      }
      else{
         memcpy( &ele.cell_2d.transform, &cellDP->el.cell_2d.transform, sizeof( cellDP->el.cell_3d.transform));
      }
      mdlElmdscr_new( &cellDP2, NULL, &ele);

      /*--- ヘッダ内 ------*/
      cellDP = cellDP->h.firstElem;
      for( ; cellDP; cellDP = cellDP->h.next){
         mdlElement_setProperties( &cellDP->el, &lv2, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &cellDP->el, &co2, &wt2, &lc2);

         /*--- 点セルの場合はスナップ不可能にする ------*/
         if ( houset->zukei == TRUE){
            if (mdlModelRef_is3D(modelRef2)) { cellDP->el.cell_3d.dhdr.props.b.s = 1;}
            else{                       cellDP->el.cell_2d.dhdr.props.b.s = 1;}
         }
         else{
            if (mdlModelRef_is3D(modelRef2)) { cellDP->el.cell_3d.dhdr.props.b.s = 0;}
            else{                       cellDP->el.cell_2d.dhdr.props.b.s = 0;}
         }
         mdlElmdscr_appendElement( cellDP2, &cellDP->el);
      }

      mdlElmdscr_add( cellDP2);
      mdlElmdscr_display( cellDP2, modelRef2, DRAW_MODE_Normal);
      mdlElmdscr_freeAll( &cellDP);
      mdlElmdscr_freeAll( &cellDP2);
   }

   /*--- 今回の第２セルを次回の第１セルにする ------*/
   memcpy( &Poi[0], &Poi[1], sizeof( Dpoint3d));
   lv1 = lv2;
   co1 = co2;
   lc1 = lc2;
   wt1 = wt2;
   firstPos = secondPos;
   modelRef = modelRef2;
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP3);
   mdlElmdscr_duplicate( &elmDP1, elmDP2);
   mdlElmdscr_freeAll( &elmDP2);

   cell_second( );
}


/*-----------------------------------------------------------------------
**	リセットを押した場合
-----------------------------------------------------------------------*/
Private void	reset( )
{
   /*--- リセットボタン、データボタンを押した時 ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)end, (MdlFunc_Accept)cell_second_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"セルを選んで下さい");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)dynamic, FALSE);
}


/*-----------------------------------------------------------------------
**	リセット２回で終了
-----------------------------------------------------------------------*/
Private void	end( )
{
    mdlState_clear( );
    mdlSystem_exit( exitStatus, 1);
}


/*----------------------------------------------------------------------
**		elemType						
----------------------------------------------------------------------*/
Private void     elemType( )
{
    static int searchType[] = { CELL_HEADER_ELM};
    mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);
}
