/*-----------------------------------------------------------------------*/
/*-	        TXCE.MC								-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	"txcell.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

FILE	   		*fp_idx, *fp_txt;
WChar	   		info[256];
int	   			exitStatus;
MSElement       ele;
long			celsize, kan;
Dpoint3d		celorg, scale, txorg;
ULong			Pos;
WChar	   		celnam[7];
int	   			len;
TextSizeParam	txtSize;
TextParamWide	txtParam;
TXSET 			*txset;

#ifndef Private
#define Private
#endif

Private	void	syori( WChar *txtName);
Private	void	suu_soku( WChar *txtName);
Private	void	sima( WChar *txtName);
Private	void	get_zahyou_ari( WChar *buf, int dim, WChar *kugiri, WChar *tx, double *mx, double *my, double *mz);
Private	void	get_zahyou_nasi( WChar *buf, int dim, WChar *kugiri, double *mx, double *my, double *mz);


/*-----------------------------------------------------------------------
**	メイン,
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult     result;
CExprValue      value;
WChar		txtName[256], dgnName[256], dev[MAXDEVICELENGTH], dir[MAXDIRLENGTH], name[MAXNAMELENGTH];

   /*--- 広域変数の取得 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&txset",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"広域変数が取得できません");
      mdlSystem_exit( exitStatus, 1);
   }
   txset = (TXSET*)value.val.valPointer;

   /*--- テキスト位置がセルの右 ---*/
   if ( txset->txpos == 0){
      kan = txset->scale;
   }

   /*--- テキスト位置がセル基準点 ---*/
   else{
      kan = 0;
   }

   /*--- バッチの場合 ---*/
   if ( txset->tog_idx == -1){
      if ( ( fp_idx = _wfopen( txset->idx, L"r")) == NULL){
         swprintf( info, L"%ls が読めません", txset->idx);
         mdlDialog_openInfoBox( info);
      }
      else{
         while( fwscanf( fp_idx, L"%ls\n", txtName) != EOF){
            /*--- ;はとばす ---*/
            if ( wmemcmp( &txtName[0], L";", 1) != 0){
               mdlFile_parseName( txtName, dev, dir, name, NULL);
               swprintf( dgnName, L"%ls:%ls%ls.dgn", dev, dir, name);

               /*--- シードファイルをコピーする ---*/
               if ( mdlFile_copy( dgnName, txset->seed) ==SUCCESS){
                  /*--- 処理 ---*/
                  if ( mdlSystem_newDesignFile( dgnName) == SUCCESS){
                     syori( txtName);
                  }
               }
            }
         }
      }
      fclose( fp_idx);
   }

   /*--- バッチでない場合 ---*/
   else{
      syori( txset->file);
   }

   mdlSystem_exit( exitStatus, 1);
}


/*-----------------------------------------------------------------------
**	syori
-----------------------------------------------------------------------*/
Private	void	syori( WChar *txtName)
{

BeFileName cellib, filepath;
UInt32 lv, co, wt;
Int32  lc;

   /*--- セルライブラリアタッチ ---*/
   cellib.SetName(txset->cellib);
   mdlCell_attachLibrary( filepath, &cellib, NULL, TRUE);

   /*--- 旧シンボロジー保存 ---*/
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
   ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);

   /*--- レベルの設定 ---*/
   ActiveParams::SetValue(txset->lv, ACTIVEPARAM_LEVEL);

   /*--- 色の設定 ---*/
   ActiveParams::SetValue( txset->co, ACTIVEPARAM_COLOR);

   /*--- 線種の設定 ---*/
   ActiveParams::SetValue(txset->lc, ACTIVEPARAM_LINESTYLE);

   /*--- 線幅の設定 ---*/
   ActiveParams::SetValue(txset->wt, ACTIVEPARAM_LINEWEIGHT);

   /*--- 数学座標・測地座標 ---*/
   if ( txset->form != 2){
      suu_soku( txtName);
   }

   /*--- SIMA ---*/
   if ( txset->form == 2){
      sima( txtName);
   }

   /*--- 旧シンボロジー設定 ---*/
   ActiveParams::SetValue(lv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(lc, ACTIVEPARAM_LINESTYLE);
   ActiveParams::SetValue(wt, ACTIVEPARAM_LINEWEIGHT);

}


/*-----------------------------------------------------------------------
**	数学座標・測地座標
-----------------------------------------------------------------------*/
Private	void	suu_soku( WChar *txtName)
{
WChar	kugiri[2], tx[20], buf[256];
double	mx, my, mz;
int		ii;
MSElementDescr  *dscr,*Ndscr;


   if ( ( fp_txt = _wfopen( txtName, L"r")) == NULL){
      mdlDialog_openInfoBox( L"基準点ファイルが読めません");
   }

   /*--- 基準点ファイルの読み込み -----*/
   else{
      if ( txset->kugiri == 0){ wcscpy( kugiri, L" ");}// スペース区切り
      else{                     wcscpy( kugiri, L",");}// カンマ区切り

      ii = 0;
      while( fgetws( buf, 128, fp_txt) != NULL){
mdlOutput_printf( MSG_ERROR, L"%d", ii);
         ii++;
         if ( wcslen( buf) <= 1){
            break;
         }

         /*--- 座標を読む（点名ありの場合） ---*/
         if ( txset->pname == 0){
            get_zahyou_ari( buf, tcb->ndices, kugiri, tx, &mx, &my, &mz);
         }

         /*--- 座標を読む（点名なしの場合） ---*/
         else{
            get_zahyou_nasi( buf, tcb->ndices, kugiri, &mx, &my, &mz);
         }

         celorg.x = mx * 1000;   celorg.y = my * 1000;   celorg.z = mz * 1000L;
         scale.x  = 1;           scale.y  = 1;           scale.z  = 1;

         /*--- セルの配置 ---*/
#if VER == 8
         mbstowcs( wcelnam, txset->cell, sizeof( txset->cell)/sizeof( MSWideChar));
//         Pos = mdlCell_placeCell( &celorg, &scale, FALSE, NULL, NULL,
//                                  0, TRUE, txset->lv, 0, wcelnam, NULL);
#else
		 
         Pos = mdlCell_placeCell(  &celorg, &scale, NULL, NULL, NULL, 0,
                                  TRUE, txset->lv, 0, txset->cell, NULL);
#endif
         if ( Pos == 0){
            mdlDialog_openInfoBox( L"セルがセルライブラリに存在しません");
            break;
         }

         /*--- 配置したセルを読む ---*/
         mdlElement_read( &ele, MASTERFILE, Pos);
		 mdlElmdscr_read( &dscr, Pos, MASTERFILE, NULL, NULL);
		 mdlElmdscr_setSymbology( &dscr, &txset->co, &txset->lc, &txset->wt, 0);
		 mdlElmdscr_duplicate( &Ndscr, dscr);
		 mdlElmdscr_rewrite( Ndscr, dscr, Pos);
		 mdlElmdscr_freeAll( &dscr);
		 mdlElmdscr_freeAll( &Ndscr);

         /*--- 点名ありの場合 ---*/
         if ( txset->pname == 0){
            /*--- ３次元の場合 ---*/
            if (mdlModelRef_is3D(MASTERFILE)){
               celsize = ( long)ele.cell_3d.rnghigh.x;
            }

            /*--- ２次元の場合 ---*/
            else{
               celsize = ( long)ele.cell_2d.rnghigh.x;
            }

            /*--- テキストの位置 -----*/
            if ( txset->txpos == 0){
               mx = mx + 0.001 * ( kan + celsize);
            }
            else{
               mx = mx + 0.001 * kan;
            }

            /*--- テキストの配置 -----*/
            txtSize.size.width	 = 2 * txset->scale;
            txtSize.size.height = 2 * txset->scale;
	        txorg.x = mx * 1000;   txorg.y = my * 1000 - kan;   txorg.z = mz * 1000L;
            txtSize.mode  = TXT_BY_TILE_SIZE;

            /*--- フォント -----*/
            txtParam.font	= txset->font;

            /*--- 入力基準 -----*/
            txtParam.just = TextElementJustification::LeftBaseline;

            if ( mdlText_create( &ele, NULL, tx, &txorg, NULL, &txtSize,
                                  &txtParam, NULL,MASTERFILE) == SUCCESS){
               mdlElement_setProperties( &ele, &txset->lv, NULL, NULL, NULL,
                                         NULL, NULL, NULL, NULL);
               mdlElement_setSymbology( &ele, &txset->co, &txset->wt, &txset->lc);
               mdlElement_display( &ele, DRAW_MODE_Normal);
               mdlElement_add( &ele);
            }
         }
      }
   }
   fclose( fp_txt);
}


/*-----------------------------------------------------------------------
**	SIMA
-----------------------------------------------------------------------*/
Private	void	sima( WChar *txtName)
{
int			ii;
double		mx = 0, my = 0, mz = 0;
WChar		tx[20], buf[256], string[128], num[10], *token;
MSElementDescr  *dscr, *Ndscr;


   if ( ( fp_txt = _wfopen( txtName, L"r")) == NULL){
      mdlDialog_openInfoBox( L"基準点ファイルが読めません");
   }

   /*--- 基準点ファイルの読み込み -----*/
   else{
      while( fgetws( buf, 128, fp_txt) != NULL){
         len = (int)wcslen( buf);
	     token = wcstok( buf, L",");
         if ( wmemcmp( token, L"A99", 3) == 0){
            break;
         }
         if ( wmemcmp( token, L"A01",3) != 0){
            continue;
         }

         /*--- ,をはずす -----*/
         len = len - (int)wcslen( token) - 1;
	     wcscpy( string, L"");
	     wcscat( string, token);
	     while( len > 1){
   	        token = wcstok( NULL, L",");
            len = len - (int)wcslen( token) - 1;
	        wcscat( string, L" ");
	        wcscat( string, token);
	     }

         /*--- スペース区切り -----*/
         token = wcstok( string, L" ");
         for( ii=0; ii<5; ii++){
   	        token = wcstok( NULL, L" ");
            if ( ii == 0){ wcscpy( num, token);}
            if ( ii == 1){ wcscpy( tx, token);}
            if ( ii == 2){ my = _wtof( token); }
            if ( ii == 3){ mx = _wtof( token); }
            if ( ii == 4){ mz = _wtof( token); }
         }

         celorg.x = mx * 1000;   celorg.y = my * 1000;   celorg.z = mz * 1000L;
         scale.x = 1;          scale.y = 1;          scale.z = 1;

         /*--- セルの配置 ---*/
#if VER == 8
         mbstowcs( wcelnam, txset->cell, sizeof( txset->cell)/sizeof( MSWideChar));
//         Pos = mdlCell_placeCell( &celorg, &scale, FALSE, NULL, NULL,
//                                  0, TRUE, txset->lv, 0, wcelnam, NULL);
#else
         Pos = mdlCell_placeCell(  &celorg, &scale, NULL, NULL, NULL, 0,
                                  TRUE, txset->lv, 0, txset->cell,NULL);
#endif
         
		 mdlElmdscr_read(&dscr, Pos, MASTERFILE, NULL, NULL);
		 mdlElmdscr_setSymbology(&dscr, &txset->co, &txset->lc, &txset->wt, 0);
		 mdlElmdscr_duplicate(&Ndscr, dscr);
		 mdlElmdscr_rewrite(Ndscr, dscr, Pos);
		 mdlElmdscr_freeAll(&dscr);
		 mdlElmdscr_freeAll(&Ndscr);

		 mdlElement_read(&ele, MASTERFILE, Pos);
         celsize = ( long)ele.cell_3d.rnghigh.x;

         /*--- テキストの位置 -----*/
         if ( txset->txpos == 0){
            mx = mx + 0.001 * ( kan + celsize);
         }
         else{
            mx = mx + 0.001 * kan;
         }

         /*--- テキストの配置 -----*/
         txtSize.size.width	 = 2 * txset->scale;
         txtSize.size.height = 2 * txset->scale;
         txorg.x = mx * 1000;   txorg.y = my * 1000 - kan;   txorg.z = mz * 1000L;
         txtSize.mode = TXT_BY_TILE_SIZE;

         /*--- フォント -----*/
         txtParam.font	= txset->font;

         /*--- 入力基準 -----*/
         txtParam.just = TextElementJustification::LeftBaseline;;

         if ( mdlText_create( &ele, NULL, tx, &txorg, NULL, &txtSize,
                               &txtParam, NULL, MASTERFILE) == SUCCESS){
            mdlElement_setProperties( &ele, &txset->lv, NULL, NULL, NULL,
                                      NULL, NULL, NULL, NULL);
            mdlElement_setSymbology( &ele, &txset->co, &txset->wt, &txset->lc);
            mdlElement_display( &ele, DRAW_MODE_Normal);
            mdlElement_add( &ele);
         }
      }
   }
   fclose( fp_txt);
}


/*-----------------------------------------------------------------------
**	座標（点名ありの場合）
-----------------------------------------------------------------------*/
Private	void	get_zahyou_ari( WChar *buf, int dim, WChar *kugiri, WChar *tx, double *mx, double *my, double *mz)
{
WChar	*token, tmp[20];

   /*--- 点名 -----*/
   token = wcstok( buf, kugiri);
   wcscpy( tx, token);

   /*--- X -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *mx = _wtof( tmp);}// 数学座標
   else{                   *my = _wtof( tmp);}// 測地座標

   /*--- Y -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *my = _wtof( tmp);}// 数学座標
   else{                   *mx = _wtof( tmp);}// 測地座標

   /*--- Z（３次元の場合） -----*/
   if ( dim == 3){
      token = wcstok( NULL, kugiri);
      wcscpy( tmp, token);
      *mz = _wtof( tmp);
   }
   else{
      *mz = 0;
   }
}


/*-----------------------------------------------------------------------
**	座標（点名なしの場合）
-----------------------------------------------------------------------*/
Private	void	get_zahyou_nasi( WChar *buf, int dim, WChar *kugiri, double *mx, double *my, double *mz)
{
WChar	*token, tmp[20];

   /*--- X -----*/
   token = wcstok( buf, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *mx = _wtof( tmp);}// 数学座標
   else{                   *my = _wtof( tmp);}// 測地座標

   /*--- Y -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *my = _wtof( tmp);}// 数学座標
   else{                   *mx = _wtof( tmp);}// 測地座標

   /*--- Z（３次元の場合） -----*/
   if ( dim == 3){
      token = wcstok( NULL, kugiri);
      wcscpy( tmp, token);
      *mz = _wtof( tmp);
   }
   else{
      *mz = 0;
   }
}
