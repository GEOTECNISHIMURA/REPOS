/*-----------------------------------------------------------------------*/
/*-	        YA.MC							-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "houkou.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE		*fp;
RscFileHandle   Rsc;
MSDialog	*db_p;
WChar		full[128];
MSElement       ele;
MSElementDescr  *elmDP1, *elmDP2;
ULong		firstPos, secondPos, Pos;
DPoint3d	isP1, isP2;         /* セル同士を結ぶ線とセルの外形との交点 */
DPoint3d	orig, rbP[2], linPoi[2], yarPoi[2], yalPoi[2];
double		angle, length, maxlength;
int		type, index,  exitStatus;
DgnModelRef     *modelRef;
UInt32		lv1, co1,  wt1;
int         lc1;
HOUSET      	*houset;

#ifndef Private
#define Private
#endif
/*-	プロトタイプ	-*/
bool unload(UnloadProgramReason unloadType);
Private void	cell_first( void);
Private void	cell_first_select( Dpoint3d *inpoint,  int view);
Private void	ya( void);
Private void	ya_dynamic( Dpoint3d *inpoint,  int view);
Private void	ya_place( Dpoint3d *inpoint,  int view);
//Private void	line( void);

Private void	reset( void);
Private void	end( void);
Private void	elemType( void);



/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult     result;
CExprValue      value;

   /*--------- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"CUTのリソースファイルが開きません");
      return;
   }

   /*--------- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&houset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR ) != SUCCESS){
      mdlOutput_error(L"広域変数が取得できません");
      return;
   }
   houset = (HOUSET*)value.val.valPointer;

   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   cell_first( );

   return;
}



/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}



/*-----------------------------------------------------------------------
**	第１セル
-----------------------------------------------------------------------*/
Private void	cell_first( )
{
   /*---------- リセットボタン、データボタンを押した時 ----------*/
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

   /*------ セル中の楕円要素を読み込む ------*/
   firstPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
   type = mdlElement_getType( dgnBuf);

   /*------ セルを選んだ場合 ------*/
   if ( type == 2){

      /*------ セルの基準点座標を取得 ------*/
      mdlCell_extract( &orig, NULL, NULL, NULL, NULL, NULL, dgnBuf);
      Pos = firstPos;
      mdlElement_read( dgnBuf, modelRef, Pos);

      /*------ レイヤーがアクティブの場合 ------*/
      if ( houset->layer == 0){
		  ActiveParams::GetValue(lv1, ACTIVEPARAM_LEVEL);
		  ActiveParams::GetValue(co1, ACTIVEPARAM_COLOR);
		  ActiveParams::GetValue(wt1, ACTIVEPARAM_LINEWEIGHT);
		  ActiveParams::GetValue(lc1, ACTIVEPARAM_LINESTYLE);
      }

      /*------ レイヤーがセルと同じ場合 ------*/
      if ( houset->layer == 1){
         mdlElement_getProperties( &lv1, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, dgnBuf);
         mdlElement_getSymbology( &co1, &wt1, &lc1, dgnBuf);
      }

      /*------ レイヤーを設定する場合 ------*/
      if ( houset->layer == 2){
         lv1 = houset->lv;
         co1 = houset->co;
         lc1 = houset->lc;
         wt1 = houset->wt;
      }

      ya( );
   }

   /*------ セル以外を選んでしまった場合 ------*/
   else{
      mdlOutput_prompt(L"Ｒボタンを押して下さい");
      mdlOutput_error(L"この要素は選べません");
   }
}



/*-----------------------------------------------------------------------
**	矢印
-----------------------------------------------------------------------*/
Private void	ya( )
{
   /*---------- リセットボタン、データボタンを押した時 ----------*/
   StateCallback::SetDataPointFunction((StateFunc_DataPoint)ya_place);
   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)ya_dynamic, FALSE);
}



/*-----------------------------------------------------------------------
**	方向線のラバーバンド
-----------------------------------------------------------------------*/
Private void	ya_dynamic( Dpoint3d *inpoint,  int view)
{
   memcpy( &rbP[0], &orig, sizeof(Dpoint3d) );
   memcpy( &rbP[1], inpoint, sizeof(Dpoint3d) );

   mdlLineString_create( dgnBuf, NULL, rbP, 2);
}



/*-----------------------------------------------------------------------
**	矢印を作成
-----------------------------------------------------------------------*/
Private void	ya_place( Dpoint3d *inpoint,  int view)
{
   memcpy( &linPoi[1], &orig, sizeof(Dpoint3d) );
   memcpy( &linPoi[0], inpoint, sizeof(Dpoint3d) );
   memcpy( &yarPoi[0], inpoint, sizeof(Dpoint3d) );
   memcpy( &yalPoi[0], inpoint, sizeof(Dpoint3d) );

   mdlLine_create( &ele, NULL, linPoi);
   mdlElmdscr_new( &elmDP2, NULL, &ele);

   /*------ 第１セルを読み込む ------*/
   mdlElmdscr_read( &elmDP1, firstPos, modelRef, 0, NULL);
   maxlength = 0;
   elmDP1 = elmDP1->h.firstElem;
   for( ; elmDP1; elmDP1 = elmDP1->h.next){

      /*------ 第１セル内の各要素とラインとの交点 ------*/
      mdlIntersect_closestBetweenElms( &isP1, &isP2, &index, elmDP1,
		  elmDP2, NULL, &linPoi[0], 0.001);

      /*------ 第１セルの基準点から１番遠い交点を求める ------*/
      length = sqrt( pow( isP2.x - orig.x, 2) 
                   + pow( isP2.y - orig.y, 2) );
      if ( length > maxlength){
         maxlength = length;
         memcpy( &linPoi[1], &isP2, sizeof( Dpoint3d) );
      }
   }

   /*------ 方向線作成 ------*/
   mdlLine_create( &ele, NULL, linPoi);
   mdlElement_setProperties( &ele, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   mdlElement_setSymbology( &ele, &co1, &wt1, &lc1);
   mdlElement_add( &ele);
   mdlElement_display( &ele, DRAW_MODE_Normal);

   /*------ 方向線の角度 ------*/
   angle = atan2( linPoi[1].y-linPoi[0].y, linPoi[1].x-linPoi[0].x);

   /*------ 矢印作成 ------*/
   if ( houset->cut == 2){
      yarPoi[1].x = yarPoi[0].x + houset->scale * cos( angle + fc_pi/12);
      yarPoi[1].y = yarPoi[0].y + houset->scale * sin( angle + fc_pi/12);
   }
   if ( houset->cut == 3){
      yarPoi[1].x = yarPoi[0].x + 0.5 * houset->scale * cos( angle + fc_pi/2);
      yarPoi[1].y = yarPoi[0].y + 0.5 * houset->scale * sin( angle + fc_pi/2);
   }
   mdlLine_create( &ele, NULL, yarPoi);
   mdlElement_setProperties( &ele, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   mdlElement_setSymbology( &ele, &co1, &wt1, &lc1);
   mdlElement_add( &ele);
   mdlElement_display( &ele, DRAW_MODE_Normal);

   if ( houset->cut == 2){
      yalPoi[1].x = yalPoi[0].x + houset->scale * cos( angle - fc_pi/12);
      yalPoi[1].y = yalPoi[0].y + houset->scale * sin( angle - fc_pi/12);
   }
   if ( houset->cut == 3){
      yalPoi[1].x = yalPoi[0].x + 0.5 * houset->scale * cos( angle - fc_pi/2);
      yalPoi[1].y = yalPoi[0].y + 0.5 * houset->scale * sin( angle - fc_pi/2);
   }
   mdlLine_create( &ele, NULL, yalPoi);
   mdlElement_setProperties( &ele, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   mdlElement_setSymbology( &ele, &co1, &wt1, &lc1);
   mdlElement_add( &ele);
   mdlElement_display( &ele, DRAW_MODE_Normal);

   cell_first( );
}



/*-----------------------------------------------------------------------
**	リセットを押した場合
-----------------------------------------------------------------------*/
Private void	reset( )
{
   /*---------- リセットボタン、データボタンを押した時 ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)end, (MdlFunc_Accept)ya_place,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"セルを選んで下さい");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)ya_dynamic, FALSE);
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
    static int searchType[] = {CELL_HEADER_ELM};
    mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);
}
