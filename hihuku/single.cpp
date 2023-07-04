/*-----------------------------------------------------------------------*/
/*-	被覆記号を描くプログラム	-*/
/*-								-*/
/*-	        HIHUKU.MC			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "hihuku.h"

FILE			*fp;
RscFileHandle   Rsc;
MSElementDescr	*cellDP;
MSElement		ele;
int				exitStatus;
int				num;
int				tnv, bnv;
Dpoint3d		tPoi[10000], bPoi[10000];
int				tnum, bnum;
ULong			tPos[100], bPos[100], celPos[10000];
HIHUKU			*hihuku;

#ifndef Private
#define Private
#endif
/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private  void   startPrimitive( void);
Private  void   input_point( Dpoint3d *inpoint);
Private  int	scan_element_select( int *nv, Dpoint3d *Poi, ULong *Pos, Dpoint3d *inpoint, int hilv, int hico, int hilc, int hiwt);
Private  void	scan_element_all( int *num, ULong *Pos, int hilv, int hico, int hilc, int hiwt);
Private  void   create_hihuku( Dpoint3d *inpoint);
Private  void   create_hihuku_multi( double	xPitch, double yPitch, double hiLeng, int vernum);
Private  void   create_hihuku_single( Dpoint3d *inpoint, double xPitch, double yPitch, double hiLeng, int vernum);
Private  void   reset( void);


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

   /*---リソースファイルのオープン ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"HIHUKU のリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   /*--- 広域変数の取得 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&hihuku",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox( L"広域変数が取得できません");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }
   hihuku = (HIHUKU*)value.val.valPointer;

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   num = 0;
   startPrimitive( );

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
**	startPrimitive
-----------------------------------------------------------------------*/
Private  void   startPrimitive( void)
{
   /*--- 上線選択 ---*/
   if ( num == 0){
      mdlState_startPrimitive( (StateFunc_DataPoint)input_point, (StateFunc_Reset)startPrimitive, NULL, NULL);
      mdlOutput_error( L"上線を選択して下さい");
   }

   /*--- 下線選択 ---*/
   if ( hihuku->tog!=-1 && num==1){
      mdlState_startPrimitive((StateFunc_DataPoint)input_point, (StateFunc_Reset)reset, NULL, NULL);
      mdlOutput_error( L"下線を選択して下さい");
   }

   /*--- 承認/拒否 ---*/
   if ( ( hihuku->tog==-1 && num==1) || num==2){
      mdlState_startPrimitive((StateFunc_DataPoint)create_hihuku, (StateFunc_Reset)reset, NULL, NULL);
      mdlOutput_error( L"承認/拒否");
   }
}


/*-----------------------------------------------------------------------
**	選択した時
-----------------------------------------------------------------------*/
Private  void   input_point( Dpoint3d *inpoint)
{
ULong		*Pos;

int			hinum,dislc;
UInt32      disco, diswt;
   /*--- 選択要素を検索（上線） ---*/
   if ( num == 0){
      if ( scan_element_select( &tnv, tPoi, &tPos[0], inpoint, hihuku->tlv, hihuku->tco, hihuku->tlc, hihuku->twt) == ERROR){
         mdlDialog_openInfoBox( L"上線を選び直して下さい");
         startPrimitive( );
      }
      else{
         /*--- 選択要素と同レイヤーの要素を検索 ---*/
         Pos = (ULong*)malloc( 10000 * sizeof( ULong));
         scan_element_all( &hinum, Pos, hihuku->tlv, hihuku->tco, hihuku->tlc, hihuku->twt);

         /*--- 選択要素の連続を検索 ---*/
         tnum = 1;
         get_renzoku( hinum, Pos, &tnv, tPoi, &tnum, tPos);
         free( Pos);

         disco = 0;   dislc = 0;   diswt = 5;
         lineString_display( &disco, &dislc, &diswt, tPoi, &tnv, DRAW_MODE_Normal);
         num++;
         startPrimitive( );
      }
   }

   /*--- 選択要素を検索（下線） ---*/
   else{
      if ( scan_element_select( &bnv, bPoi, &bPos[0], inpoint, hihuku->blv, hihuku->bco, hihuku->blc, hihuku->bwt) == ERROR){
         mdlDialog_openInfoBox( L"下線を選び直して下さい");
         startPrimitive( );
      }
      else{
         /*--- 選択要素と同レイヤーの要素を検索 ---*/
         Pos = (ULong*)malloc( 10000 * sizeof( ULong));
         scan_element_all( &hinum, Pos, hihuku->blv, hihuku->bco, hihuku->blc, hihuku->bwt);

         /*--- 選択要素の連続を検索 ---*/
         bnum = 1;
         get_renzoku( hinum, Pos, &bnv, bPoi, &bnum, bPos);
         free( Pos);

         disco = 0;   dislc = 0;   diswt = 5;
         lineString_display( &disco, &dislc, &diswt, bPoi, &bnv, DRAW_MODE_Normal);
         num++;
         startPrimitive( );
      }
   }
}


/*-----------------------------------------------------------------------
**	選択要素の検索
-----------------------------------------------------------------------*/
Private  int   scan_element_select( int *nv, Dpoint3d *Poi, ULong *Pos, Dpoint3d *inpoint, int hilv, int hico, int hilc, int hiwt)
{
ExtScanList	scanList;
DSegment3d	seg;
Dpoint3d	nearPoi;
ULong		filePos;
int			getSize, Error;
int			type, lc;
UInt32	    lv, co, wt;
ULong		elemAddr[1];

   /*--- スキャンリストの初期化 ---*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;

   /*--- スキャン実行 ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
	  return( ERROR);
   }

   while( TRUE){
      Error = mdlScan_file( elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
	  if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return(ERROR); }


      /*--- 複合連結ヘッダは無視 ---*/
	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      type = mdlElement_getType( &ele);
      if ( type == 12){ continue;}

      get_layer( &lv, &co, &lc, &wt, NULL, &ele);

      /*--- 被覆の場合 ---*/
      if ( lv==hilv && co==hico && lc==hilc && wt==hiwt){
         /*--- スキャンしたライン上で、押さえた点から最短距離の点 ---*/
         memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
         mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

         /*--- 最短距離の点が1mm以下の場合は、スキャンしたラインを押さえたものとする ---*/
         if ( get_leng( &nearPoi, inpoint) < 1){
            mdlLinear_extract( Poi, nv, &ele, MASTERFILE);
			*Pos = elemAddr[0];
            return( SUCCESS);
         }
      }
   }

   return( ERROR);
}


/*-----------------------------------------------------------------------
**	選択要素と同レイヤーの要素を検索
-----------------------------------------------------------------------*/
Private  void	scan_element_all( int *posnum, ULong *Pos, int hilv, int hico, int hilc, int hiwt)
{
ExtScanList	scanList;
ULong		filePos;
int			getSize, Error;
int			type, lc;
UInt32	    lv, co, wt;
ULong		elemAddr[1];

   /*--- スキャンリストの初期化 ---*/
   mdlScan_initScanlist( &scanList);

   /*--- 検索範囲の設定 ---*/
   mdlScan_noRangeCheck( &scanList);


   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;

   /*--- スキャン実行 ---*/
   *posnum = 0;  filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"スキャンリストが受け入れられませんでした。");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   while( TRUE){
      Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
	  if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return; }


      /*--- 複合連結ヘッダは無視 ---*/
	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      type = mdlElement_getType( &ele);
      if ( type == 12){ continue;}

      get_layer( &lv, &co, &lc, &wt, NULL, &ele);

      /*--- 選択した要素と同レイヤーの場合 ---*/
      if ( lv==hilv && co==hico && lc==hilc && wt==hiwt){
		  Pos[*posnum] = elemAddr[0];
         *posnum = *posnum + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	被覆記号発生
-----------------------------------------------------------------------*/
Private  void   create_hihuku( Dpoint3d *inpoint)
{
double		xPitch, yPitch, hiLeng;
int			vernum, ii, olc;
UInt32      olv, oco, owt;

   /*--- 元のアクティブレイヤーを取得 ---*/
   ActiveParams::GetValue(olv, ACTIVEPARAM_LEVEL);
   ActiveParams::GetValue(oco, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(owt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::GetValue(olc, ACTIVEPARAM_LINESTYLE);
   /*--- 被覆記号のレイヤーにする ---*/
   ActiveParams::SetValue(hihuku->klv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(hihuku->kco, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(hihuku->kwt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::SetValue(hihuku->klc, ACTIVEPARAM_LINESTYLE);
   /*--- 上線の延長 ---*/
   hiLeng = 0;
   for( ii=0; ii<tnv-1; ii++){
      hiLeng += get_leng( &tPoi[ii+1], &tPoi[ii]);
   }

   /*--- 垂線の間隔 ---*/
   if ( hihuku->scale ==   500){ xPitch = yPitch = 2120/2;}
   if ( hihuku->scale ==  1000){ xPitch = yPitch = 4240/2;}
   if ( hihuku->scale ==  2500){ xPitch =  5000/2;   yPitch = 3750;}
   if ( hihuku->scale == 10000){ xPitch = 10000/2;   yPitch = 5000;}

   /*--- 垂線の本数 ---*/
   vernum = hiLeng / xPitch;

   if ( hihuku->tog != -1){ create_hihuku_multi( xPitch, yPitch, hiLeng, vernum);}
   else{                    create_hihuku_single( inpoint, xPitch, yPitch, hiLeng, vernum);}

   /*--- 元のアクティブレイヤーに戻す ---*/
   ActiveParams::SetValue(olv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(oco, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(owt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::SetValue(olc, ACTIVEPARAM_LINESTYLE);

   reset( );
}


/*-----------------------------------------------------------------------
**	被覆記号（複数列）発生
-----------------------------------------------------------------------*/
Private  void   create_hihuku_multi( double	xPitch, double yPitch, double hiLeng, int vernum)
{
Dpoint3d	tmpPoi[2], sPoi[8], verPoi[2], kouPoi[50], ssPoi[8], scale, orig;
int			flag, ii, jj, kk, iti, knum, vernv, snv, kounum, minnum, celnum;
double		verLeng, rad, minleng, leng;

   vernv = 2;
   scale.x = scale.y = scale.z = 1;
   snv = 5;

   /*--- 垂線 ---*/
   celnum = 0;
   for( ii=1; ii<vernum; ii++){
      /*--- 垂線の上線側の点 ---*/
      get_point_along_line( ii*xPitch, tnv, tPoi, &iti, &verPoi[0]);
      verPoi[0].z = 0;

      /*--- 上線の方向 ---*/
      rad = atan2( tPoi[iti+1].y - tPoi[iti].y, tPoi[iti+1].x - tPoi[iti].x);

      /*--- 仮の垂線 ---*/
      tmpPoi[0].x = verPoi[0].x + 100000 * cos( rad + fc_piover2);
      tmpPoi[0].y = verPoi[0].y + 100000 * sin( rad + fc_piover2);
      tmpPoi[0].z = 0;
      tmpPoi[1].x = verPoi[0].x - 100000 * cos( rad + fc_piover2);
      tmpPoi[1].y = verPoi[0].y - 100000 * sin( rad + fc_piover2);
      tmpPoi[1].z = 0;

      /*--- 仮の垂線と下線との交点 ---*/
      kounum = judge_kouten_all( &vernv, tmpPoi, &bnv, bPoi, kouPoi);
      if ( kounum == 0){ continue;}

      /*--- 仮の垂線と下線との交点のうち上線に一番近いもの ---*/
      minleng = 100000;
      for( jj=0; jj<kounum; jj++){
         leng = get_leng( &verPoi[0], &kouPoi[jj]);
         if ( leng < minleng){
            minleng = leng;
            minnum = jj;
         }
      }

      /*--- 垂線の下線側の点 ---*/
      memcpy( &verPoi[1], &kouPoi[minnum], sizeof( Dpoint3d));
//lineString_add( NULL, &ii, NULL, NULL, NULL, verPoi, &vernv);

      /*--- 垂線の延長 ---*/
      verLeng = get_leng( &verPoi[0], &verPoi[1]);

      /*--- 短い場合は無視 ---*/
      if (  ( hihuku->scale==  500 && verLeng<=1210)
         || ( hihuku->scale== 1000 && verLeng<=2420)
         || ( hihuku->scale==10000 && verLeng<=5000)){
         continue;
      }

      /*--- 被覆記号の個数 ---*/
      knum = verLeng / yPitch;

      /*--- 垂線の方向 ---*/
      rad = atan2( verPoi[1].y-verPoi[0].y, verPoi[1].x-verPoi[0].x);

      /*--- 上線から下線へ引く垂線の間隔 ---*/
      for( jj=0; jj<knum; jj++){
         if ( ii%2 == jj%2){ continue;}

         orig.x = verPoi[0].x + ( jj+1) * yPitch * cos( rad);
         orig.y = verPoi[0].y + ( jj+1) * yPitch * sin( rad);

         /*--- セルヘッダの範囲を取得 ---*/
         mdlCell_getElmDscr( &cellDP, NULL, &orig, &scale, NULL, NULL, NULL, 0, 0,NULL, hihuku->cell,NULL);
         mdlCell_extract( NULL, sPoi, NULL, NULL, NULL, NULL, &cellDP->el);
         mdlElmdscr_freeAll( &cellDP);

         /*--- セルと上線との交点がある場合は無視 ---*/
         if ( judge_kouten_all( &tnv, tPoi, &snv, sPoi, kouPoi) > 0){ continue;}

         /*--- セルと下線との交点がある場合は無視 ---*/
         if ( judge_kouten_all( &bnv, bPoi, &snv, sPoi, kouPoi) > 0){ continue;}

         /*--- 以前に配置したセルとの交点があれば無視 ---*/
         flag = 0;
         for( kk=0; kk<celnum; kk++){
            mdlElement_read( &ele, MASTERFILE, celPos[kk]);
            mdlCell_extract( NULL, ssPoi, NULL, NULL, NULL, NULL, &ele);
            if ( judge_kouten_all( &snv, sPoi, &snv, ssPoi, kouPoi) > 0){ 
               flag++;
               continue;
            }
         }
         if ( flag > 0){ continue;}

         /*--- 下線と重ならない場合はセル配置 ---*/
         celPos[celnum] = mdlCell_placeCell( &orig, &scale, NULL, NULL, NULL, 0, TRUE, hihuku->klv, 0, hihuku->cell,NULL);
         celnum++;
      }
   }
}


/*-----------------------------------------------------------------------
**	被覆記号（１列のみ）発生
-----------------------------------------------------------------------*/
Private void create_hihuku_single(Dpoint3d *inpoint, double xPitch, double yPitch, double hiLeng, int vernum)
{
	Dpoint3d	scale, verPoi[2], sPoi[8], ssPoi[8], kouPoi[50];
	int			ii, jj, dir, iti, snv, vernv, celnum, flag;
	double		rad;

	scale.x = scale.y = scale.z = 1;
	snv = 5;   vernv = 2;

	/*--- 承認ボタンを押した位置が上線の左右どちら側か ---*/
	if (judge_point_lr_lstring(inpoint, tnv, tPoi) == 1) { dir = 1; }
	else { dir = -1; }

	/*--- 垂線 ---*/
	celnum = 0;
	for (ii = 1; ii <= vernum; ii++) {
		if (ii % 2 == 0) { continue; }

		/*--- 垂線の上線側の点 ---*/
		get_point_along_line(ii*xPitch, tnv, tPoi, &iti, &verPoi[0]);
		verPoi[0].z = 0;

		/*--- 上線の方向 ---*/
		rad = atan2(tPoi[iti + 1].y - tPoi[iti].y, tPoi[iti + 1].x - tPoi[iti].x);

		/*--- セルの位置 ---*/
		verPoi[1].x = verPoi[0].x + yPitch * cos(rad + dir * fc_piover2);
		verPoi[1].y = verPoi[0].y + yPitch * sin(rad + dir * fc_piover2);
		verPoi[1].z = 0;
		//lineString_add( NULL, &ii, NULL, NULL, NULL, verPoi, &vernv);

			  /*--- セルヘッダの範囲を取得 ---*/
		mdlCell_getElmDscr(&cellDP, NULL, &verPoi[1], &scale, NULL, NULL, NULL, 0, 0, NULL, hihuku->cell, NULL);
		mdlCell_extract(NULL, sPoi, NULL, NULL, NULL, NULL, &cellDP->el);
		mdlElmdscr_freeAll(&cellDP);

		/*--- セルと上線との交点がある場合は無視 ---*/
		if (judge_kouten_all(&tnv, tPoi, &snv, sPoi, kouPoi) > 0) { continue; }

		/*--- 以前に配置したセルとの交点があれば無視 ---*/
		flag = 0;
		for (jj = 0; jj < celnum; jj++) {
			mdlElement_read(&ele, MASTERFILE, celPos[jj]);
			mdlCell_extract(NULL, ssPoi, NULL, NULL, NULL, NULL, &ele);
			if (judge_kouten_all(&snv, sPoi, &snv, ssPoi, kouPoi) > 0) {
				flag++;
				continue;
			}
		}
		if (flag > 0) { continue; }

		/*--- セル配置 ---*/
		celPos[celnum] = mdlCell_placeCell(&verPoi[1], &scale, NULL, NULL, NULL, 0, TRUE, hihuku->klv, 0, hihuku->cell, NULL);
		celnum++;
	}

}


/*-----------------------------------------------------------------------
**	reset
-----------------------------------------------------------------------*/
Private  void   reset( void)
{
UInt32		co,  wt;
int         lc,ii;

   /*--- 上線選択後の場合 ---*/
   for( ii=0; ii<tnum; ii++){
      /*--- 太線消去 ---*/
      mdlElement_read( &ele, MASTERFILE, tPos[ii]);
      co = 0;   lc = 0;   wt = 5;
      set_layer( NULL, &co, &lc, &wt, NULL, &ele);
      mdlElement_display( &ele, DRAW_MODE_Erase);

      /*--- 再表示 ---*/
      mdlElement_read( &ele, MASTERFILE, tPos[ii]);
      mdlElement_display( &ele, DRAW_MODE_Normal);
   }

   /*--- 下線選択後の場合 ---*/
   if ( num == 2){
      for( ii=0; ii<bnum; ii++){
         /*--- 太線消去 ---*/
         mdlElement_read( &ele, MASTERFILE, bPos[ii]);
         co = 0;   lc = 0;   wt = 5;
         set_layer( NULL, &co, &lc, &wt, NULL, &ele);
         mdlElement_display( &ele, DRAW_MODE_Erase);

         /*--- 再表示 ---*/
         mdlElement_read( &ele, MASTERFILE, bPos[ii]);
         mdlElement_display( &ele, DRAW_MODE_Normal);
      }
   }

   num = 0;
   startPrimitive( );
}


