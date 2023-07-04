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
**	���C��,
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

   /*--- �L��ϐ��̎擾 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&txset",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
   }
   txset = (TXSET*)value.val.valPointer;

   /*--- �e�L�X�g�ʒu���Z���̉E ---*/
   if ( txset->txpos == 0){
      kan = txset->scale;
   }

   /*--- �e�L�X�g�ʒu���Z����_ ---*/
   else{
      kan = 0;
   }

   /*--- �o�b�`�̏ꍇ ---*/
   if ( txset->tog_idx == -1){
      if ( ( fp_idx = _wfopen( txset->idx, L"r")) == NULL){
         swprintf( info, L"%ls ���ǂ߂܂���", txset->idx);
         mdlDialog_openInfoBox( info);
      }
      else{
         while( fwscanf( fp_idx, L"%ls\n", txtName) != EOF){
            /*--- ;�͂Ƃ΂� ---*/
            if ( wmemcmp( &txtName[0], L";", 1) != 0){
               mdlFile_parseName( txtName, dev, dir, name, NULL);
               swprintf( dgnName, L"%ls:%ls%ls.dgn", dev, dir, name);

               /*--- �V�[�h�t�@�C�����R�s�[���� ---*/
               if ( mdlFile_copy( dgnName, txset->seed) ==SUCCESS){
                  /*--- ���� ---*/
                  if ( mdlSystem_newDesignFile( dgnName) == SUCCESS){
                     syori( txtName);
                  }
               }
            }
         }
      }
      fclose( fp_idx);
   }

   /*--- �o�b�`�łȂ��ꍇ ---*/
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

   /*--- �Z�����C�u�����A�^�b�` ---*/
   cellib.SetName(txset->cellib);
   mdlCell_attachLibrary( filepath, &cellib, NULL, TRUE);

   /*--- ���V���{���W�[�ۑ� ---*/
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
   ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);

   /*--- ���x���̐ݒ� ---*/
   ActiveParams::SetValue(txset->lv, ACTIVEPARAM_LEVEL);

   /*--- �F�̐ݒ� ---*/
   ActiveParams::SetValue( txset->co, ACTIVEPARAM_COLOR);

   /*--- ����̐ݒ� ---*/
   ActiveParams::SetValue(txset->lc, ACTIVEPARAM_LINESTYLE);

   /*--- �����̐ݒ� ---*/
   ActiveParams::SetValue(txset->wt, ACTIVEPARAM_LINEWEIGHT);

   /*--- ���w���W�E���n���W ---*/
   if ( txset->form != 2){
      suu_soku( txtName);
   }

   /*--- SIMA ---*/
   if ( txset->form == 2){
      sima( txtName);
   }

   /*--- ���V���{���W�[�ݒ� ---*/
   ActiveParams::SetValue(lv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(lc, ACTIVEPARAM_LINESTYLE);
   ActiveParams::SetValue(wt, ACTIVEPARAM_LINEWEIGHT);

}


/*-----------------------------------------------------------------------
**	���w���W�E���n���W
-----------------------------------------------------------------------*/
Private	void	suu_soku( WChar *txtName)
{
WChar	kugiri[2], tx[20], buf[256];
double	mx, my, mz;
int		ii;
MSElementDescr  *dscr,*Ndscr;


   if ( ( fp_txt = _wfopen( txtName, L"r")) == NULL){
      mdlDialog_openInfoBox( L"��_�t�@�C�����ǂ߂܂���");
   }

   /*--- ��_�t�@�C���̓ǂݍ��� -----*/
   else{
      if ( txset->kugiri == 0){ wcscpy( kugiri, L" ");}// �X�y�[�X��؂�
      else{                     wcscpy( kugiri, L",");}// �J���}��؂�

      ii = 0;
      while( fgetws( buf, 128, fp_txt) != NULL){
mdlOutput_printf( MSG_ERROR, L"%d", ii);
         ii++;
         if ( wcslen( buf) <= 1){
            break;
         }

         /*--- ���W��ǂށi�_������̏ꍇ�j ---*/
         if ( txset->pname == 0){
            get_zahyou_ari( buf, tcb->ndices, kugiri, tx, &mx, &my, &mz);
         }

         /*--- ���W��ǂށi�_���Ȃ��̏ꍇ�j ---*/
         else{
            get_zahyou_nasi( buf, tcb->ndices, kugiri, &mx, &my, &mz);
         }

         celorg.x = mx * 1000;   celorg.y = my * 1000;   celorg.z = mz * 1000L;
         scale.x  = 1;           scale.y  = 1;           scale.z  = 1;

         /*--- �Z���̔z�u ---*/
#if VER == 8
         mbstowcs( wcelnam, txset->cell, sizeof( txset->cell)/sizeof( MSWideChar));
//         Pos = mdlCell_placeCell( &celorg, &scale, FALSE, NULL, NULL,
//                                  0, TRUE, txset->lv, 0, wcelnam, NULL);
#else
		 
         Pos = mdlCell_placeCell(  &celorg, &scale, NULL, NULL, NULL, 0,
                                  TRUE, txset->lv, 0, txset->cell, NULL);
#endif
         if ( Pos == 0){
            mdlDialog_openInfoBox( L"�Z�����Z�����C�u�����ɑ��݂��܂���");
            break;
         }

         /*--- �z�u�����Z����ǂ� ---*/
         mdlElement_read( &ele, MASTERFILE, Pos);
		 mdlElmdscr_read( &dscr, Pos, MASTERFILE, NULL, NULL);
		 mdlElmdscr_setSymbology( &dscr, &txset->co, &txset->lc, &txset->wt, 0);
		 mdlElmdscr_duplicate( &Ndscr, dscr);
		 mdlElmdscr_rewrite( Ndscr, dscr, Pos);
		 mdlElmdscr_freeAll( &dscr);
		 mdlElmdscr_freeAll( &Ndscr);

         /*--- �_������̏ꍇ ---*/
         if ( txset->pname == 0){
            /*--- �R�����̏ꍇ ---*/
            if (mdlModelRef_is3D(MASTERFILE)){
               celsize = ( long)ele.cell_3d.rnghigh.x;
            }

            /*--- �Q�����̏ꍇ ---*/
            else{
               celsize = ( long)ele.cell_2d.rnghigh.x;
            }

            /*--- �e�L�X�g�̈ʒu -----*/
            if ( txset->txpos == 0){
               mx = mx + 0.001 * ( kan + celsize);
            }
            else{
               mx = mx + 0.001 * kan;
            }

            /*--- �e�L�X�g�̔z�u -----*/
            txtSize.size.width	 = 2 * txset->scale;
            txtSize.size.height = 2 * txset->scale;
	        txorg.x = mx * 1000;   txorg.y = my * 1000 - kan;   txorg.z = mz * 1000L;
            txtSize.mode  = TXT_BY_TILE_SIZE;

            /*--- �t�H���g -----*/
            txtParam.font	= txset->font;

            /*--- ���͊ -----*/
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
      mdlDialog_openInfoBox( L"��_�t�@�C�����ǂ߂܂���");
   }

   /*--- ��_�t�@�C���̓ǂݍ��� -----*/
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

         /*--- ,���͂��� -----*/
         len = len - (int)wcslen( token) - 1;
	     wcscpy( string, L"");
	     wcscat( string, token);
	     while( len > 1){
   	        token = wcstok( NULL, L",");
            len = len - (int)wcslen( token) - 1;
	        wcscat( string, L" ");
	        wcscat( string, token);
	     }

         /*--- �X�y�[�X��؂� -----*/
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

         /*--- �Z���̔z�u ---*/
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

         /*--- �e�L�X�g�̈ʒu -----*/
         if ( txset->txpos == 0){
            mx = mx + 0.001 * ( kan + celsize);
         }
         else{
            mx = mx + 0.001 * kan;
         }

         /*--- �e�L�X�g�̔z�u -----*/
         txtSize.size.width	 = 2 * txset->scale;
         txtSize.size.height = 2 * txset->scale;
         txorg.x = mx * 1000;   txorg.y = my * 1000 - kan;   txorg.z = mz * 1000L;
         txtSize.mode = TXT_BY_TILE_SIZE;

         /*--- �t�H���g -----*/
         txtParam.font	= txset->font;

         /*--- ���͊ -----*/
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
**	���W�i�_������̏ꍇ�j
-----------------------------------------------------------------------*/
Private	void	get_zahyou_ari( WChar *buf, int dim, WChar *kugiri, WChar *tx, double *mx, double *my, double *mz)
{
WChar	*token, tmp[20];

   /*--- �_�� -----*/
   token = wcstok( buf, kugiri);
   wcscpy( tx, token);

   /*--- X -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *mx = _wtof( tmp);}// ���w���W
   else{                   *my = _wtof( tmp);}// ���n���W

   /*--- Y -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *my = _wtof( tmp);}// ���w���W
   else{                   *mx = _wtof( tmp);}// ���n���W

   /*--- Z�i�R�����̏ꍇ�j -----*/
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
**	���W�i�_���Ȃ��̏ꍇ�j
-----------------------------------------------------------------------*/
Private	void	get_zahyou_nasi( WChar *buf, int dim, WChar *kugiri, double *mx, double *my, double *mz)
{
WChar	*token, tmp[20];

   /*--- X -----*/
   token = wcstok( buf, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *mx = _wtof( tmp);}// ���w���W
   else{                   *my = _wtof( tmp);}// ���n���W

   /*--- Y -----*/
   token = wcstok( NULL, kugiri);
   wcscpy( tmp, token);
   if ( txset->form == 0){ *my = _wtof( tmp);}// ���w���W
   else{                   *mx = _wtof( tmp);}// ���n���W

   /*--- Z�i�R�����̏ꍇ�j -----*/
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