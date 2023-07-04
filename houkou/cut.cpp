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
DPoint3d		Poi[2];                      /* �Z���̒��S */
DPoint3d		isP1, isP2, isP3[2];         /* �Z�����m�����Ԑ��ƃZ���̊O�`�Ƃ̌�_ */
DPoint3d		houPoi[2];                   /* ������ */
DPoint3d		rbP[2];                      /* ���o�[�o���h */
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

/*-	�v���g�^�C�v	-*/
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
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult     result;
CExprValue      value;

   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"CUT�̃��\�[�X�t�@�C�����J���܂���");
      return;

   }

   /*--- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&houset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
      return;
   }
   houset = (HOUSET*)value.val.valPointer;

   /*--- �A�����[�h�֐��Z�b�g ---------------*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   cell_first( );

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private	bool	unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	��P�Z��
-----------------------------------------------------------------------*/
Private void	cell_first( )
{
   /*--- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)cell_first, (MdlFunc_Accept)cell_first_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"�Z����I��ŉ�����");
   mdlOutput_error(L"");
}


/*-----------------------------------------------------------------------
**	��P�Z���I��
-----------------------------------------------------------------------*/
Private void	cell_first_select( Dpoint3d *inpoint,  int view)
{
MSElementDescrP  dscrP;

   /*--- �Z�����̑ȉ~�v�f��ǂݍ��� ------*/
   firstPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
   type = mdlElement_getType( dgnBuf);

   /*--- �Z����I�񂾏ꍇ ------*/
   if ( type == 2){
      /*--- �Z���̊�_���W���擾 ------*/
      mdlCell_extract( &Poi[0], NULL, NULL, NULL, NULL, NULL, dgnBuf);
      Pos = firstPos;
      mdlElement_read( dgnBuf, modelRef, Pos);
	  mdlElmdscr_read( &dscrP, Pos, MASTERFILE, false, NULL);

      /*--- ���C���[���A�N�e�B�u�̏ꍇ ------*/
      if ( houset->layer == 0){
		  ActiveParams::GetValue(lv1, ACTIVEPARAM_LEVEL);
		  ActiveParams::GetValue(co1, ACTIVEPARAM_COLOR);
		  ActiveParams::GetValue(wt1, ACTIVEPARAM_LINEWEIGHT);
		  ActiveParams::GetValue(lc1, ACTIVEPARAM_LINESTYLE);

      }

      /*--- ���C���[���Z���Ɠ����ꍇ ------*/
      if ( houset->layer == 1){
		 mdlElement_getProperties( &lv1, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, &dscrP->h.firstElem->el);
         mdlElement_getSymbology( &co1, &wt1, &lc1, dgnBuf);
		 
      }

      /*--- ���C���[��ݒ肷��ꍇ ------*/
      if ( houset->layer == 2){
         lv1 = houset->lv;
         co1 = houset->co;
         lc1 = houset->lc;
         wt1 = houset->wt;
      }
	  mdlElmdscr_freeAll(&dscrP);
      cell_second( );
   }

   /*--- �Z���ȊO��I��ł��܂����ꍇ ------*/
   else{
      mdlOutput_prompt(L"�q�{�^���������ĉ�����");
      mdlOutput_error(L"���̗v�f�͑I�ׂ܂���");
   }
}


/*-----------------------------------------------------------------------
**	��Q�Z��
-----------------------------------------------------------------------*/
Private void	cell_second( )
{
   /*--- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)reset, (MdlFunc_Accept)cell_second_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"�Z����I��ŉ�����");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)dynamic, FALSE);
}


/*-----------------------------------------------------------------------
**	�������̃��o�[�o���h
-----------------------------------------------------------------------*/
Private void	dynamic( Dpoint3d *inpoint,  int view)
{
   memcpy( &rbP[0], &Poi[0], sizeof( Dpoint3d));
   memcpy( &rbP[1], inpoint, sizeof( Dpoint3d));

   mdlLineString_create( dgnBuf, NULL, rbP, 2);
}


/*-----------------------------------------------------------------------
**	��Q�Z���I��
-----------------------------------------------------------------------*/
Private void	cell_second_select( Dpoint3d *inpoint,  int view)
{
MSElementDescrP  dscrP;

   /*--- �Z�����̑ȉ~�v�f��ǂݍ��� ------*/
   secondPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);

   type = mdlElement_getType( dgnBuf);

   /*--- �Z����I�񂾏ꍇ ------*/
   if ( type == 2){
      /*--- �Z���̊�_���W���擾 ------*/
      mdlCell_extract( &Poi[1], NULL, NULL, NULL, NULL, NULL, dgnBuf);

      Pos = secondPos;
      mdlElement_read( dgnBuf, modelRef, Pos);
	  mdlElmdscr_read(&dscrP, Pos, MASTERFILE, false, NULL);

      /*--- ���C���[���A�N�e�B�u�̏ꍇ ------*/
      if ( houset->layer == 0){
		 ActiveParams::GetValue(lv2, ACTIVEPARAM_LEVEL);
		 ActiveParams::GetValue(co2, ACTIVEPARAM_COLOR);
		 ActiveParams::GetValue(wt2, ACTIVEPARAM_LINEWEIGHT);
		 ActiveParams::GetValue(lc2, ACTIVEPARAM_LINESTYLE);

      }

      /*--- ���C���[���Z���Ɠ����ꍇ ------*/
      if ( houset->layer == 1){
         mdlElement_getProperties( &lv2, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, &dscrP->h.firstElem->el);
         mdlElement_getSymbology( &co2, &wt2, &lc2, dgnBuf);
		 
      }

      /*--- ���C���[��ݒ肷��ꍇ ------*/
      if ( houset->layer == 2){
         lv2 = houset->lv;
         co2 = houset->co;
         lc2 = houset->lc;
         wt2 = houset->wt;
      }
	  mdlElmdscr_freeAll(&dscrP);
      line( );
   }

   /*--- �Z���ȊO��I��ł��܂����ꍇ ------*/
   else{
      mdlOutput_prompt(L"�q�{�^���������ĉ�����");
      mdlOutput_error(L"���̗v�f�͑I�ׂ܂���");
   }
}


/*-----------------------------------------------------------------------
**	��������z�u
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


   /*--- ��P�Z���Ƒ�Q�Z�������ԃ��C�����쐬 ------*/
   mdlLine_create( dgnBuf, NULL, Poi);
   mdlElmdscr_new( &elmDP3, NULL, dgnBuf);

   /*--- ��P�Z����ǂݍ��� ------*/
   mdlElmdscr_read( &elmDP1, firstPos, modelRef, 0, NULL);
   maxlength = 0;
   elmDP1 = elmDP1->h.firstElem;
   for( ; elmDP1; elmDP1 = elmDP1->h.next){
      /*--- ��P�Z�����̊e�v�f�ƃ��C���Ƃ̌�_ ------*/
      mdlIntersect_closestBetweenElms( &isP1, &isP3[0], &index, elmDP1,
                                       elmDP3, NULL, &Poi[0], 0.001);

      /*--- ��P�Z���̊�_����P�ԉ�����_�����߂� ------*/
      length = sqrt( pow( Poi[0].x - isP3[0].x, 2) 
                   + pow( Poi[0].y - isP3[0].y, 2));
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[0], &isP3[0], sizeof( Dpoint3d));
      }
   }

   /*--- �����������C���̏ꍇ ------*/
   if ( houset->cut == 1){
      /*--- ��P�Z������}��1mm�̓_�̍��W ------*/
      houPoi[1].x = houPoi[0].x + houset->scale * ( houPoi[0].x - Poi[0].x)
                  / sqrt( pow( houPoi[0].x-Poi[0].x, 2) + pow( houPoi[0].y-Poi[0].y, 2));
      houPoi[1].y = houPoi[0].y + houset->scale * ( houPoi[0].y - Poi[0].y)
                  / sqrt( pow( houPoi[0].x-Poi[0].x, 2) + pow( houPoi[0].y-Poi[0].y, 2));

      /*--- ��P�Z������}��1mm�̕��������쐬 ------*/
      lineString_add( &lv1, &co1, &lc1, &wt1, NULL, houPoi, &nv);
   }

   /*--- ���������Z���̏ꍇ ------*/
   else{
      scale.x = scale.y = scale.z = 1;
      rad = atan2( Poi[1].y - Poi[0].y, Poi[1].x - Poi[0].x);
      mdlRMatrix_fromAngle( &rMatrix, rad);
      mdlCell_getElmDscr( &cellDP, &txtNodeDP, &Poi[0], &scale, NULL, &rMatrix, NULL, 0, 0, NULL, houset->cell, NULL);

      /*--- �Z���w�b�_ ------*/
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

      /*--- �w�b�_�� ------*/
      cellDP = cellDP->h.firstElem;
      for( ; cellDP; cellDP = cellDP->h.next){
         mdlElement_setProperties( &cellDP->el, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &cellDP->el, &co1, &wt1, &lc1);

         /*--- �_�Z���̏ꍇ�̓X�i�b�v�s�\�ɂ��� ------*/
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

   /*--- ��Q�Z����ǂݍ��� ------*/
   mdlElmdscr_read( &elmDP2, secondPos, modelRef2, 0, NULL);
   maxlength = 0;
   elmDP2    = elmDP2->h.firstElem;
   for( ; elmDP2; elmDP2 = elmDP2->h.next){
      /*--- ��Q�Z�����̊e�v�f�ƃ��C���Ƃ̌�_ ------*/
      mdlIntersect_closestBetweenElms( &isP2, &isP3[1], &index, elmDP2,
                                       elmDP3, NULL, &Poi[1], 0.001);

      /*--- ��Q�Z���̊�_����P�ԉ�����_�����߂� ------*/
      length = sqrt( pow( Poi[1].x - isP3[1].x, 2) 
                   + pow( Poi[1].y - isP3[1].y, 2));
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[0], &isP3[1], sizeof( Dpoint3d));
      }
   }

   /*--- �����������C���̏ꍇ ------*/
   if ( houset->cut == 1){
      /*--- ��Q�Z������}��1mm�̓_�̍��W ------*/
      houPoi[1].x = houPoi[0].x + houset->scale * ( houPoi[0].x - Poi[1].x)
                  / sqrt( pow( houPoi[0].x-Poi[1].x, 2) + pow( houPoi[0].y-Poi[1].y, 2));
      houPoi[1].y = houPoi[0].y + houset->scale * ( houPoi[0].y - Poi[1].y)
                  / sqrt( pow( houPoi[0].x-Poi[1].x, 2) + pow( houPoi[0].y-Poi[1].y, 2));

      /*--- ��Q�Z������}��1mm�̕��������쐬 ------*/
      lineString_add( &lv2, &co2, &lc2, &wt2, NULL, houPoi, &nv);
   }

   /*--- ���������Z���̏ꍇ ------*/
   else{
      scale.x = scale.y = scale.z = 1;
      rad = atan2( Poi[0].y - Poi[1].y, Poi[0].x - Poi[1].x);
      mdlRMatrix_fromAngle( &rMatrix, rad);
      mdlCell_getElmDscr( &cellDP, &txtNodeDP, &Poi[1], &scale,NULL, &rMatrix, NULL, 0, 0, NULL, houset->cell, NULL);

      /*--- �Z���w�b�_ ------*/
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

      /*--- �w�b�_�� ------*/
      cellDP = cellDP->h.firstElem;
      for( ; cellDP; cellDP = cellDP->h.next){
         mdlElement_setProperties( &cellDP->el, &lv2, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &cellDP->el, &co2, &wt2, &lc2);

         /*--- �_�Z���̏ꍇ�̓X�i�b�v�s�\�ɂ��� ------*/
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

   /*--- ����̑�Q�Z��������̑�P�Z���ɂ��� ------*/
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
**	���Z�b�g���������ꍇ
-----------------------------------------------------------------------*/
Private void	reset( )
{
   /*--- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)end, (MdlFunc_Accept)cell_second_select,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"�Z����I��ŉ�����");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)dynamic, FALSE);
}


/*-----------------------------------------------------------------------
**	���Z�b�g�Q��ŏI��
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