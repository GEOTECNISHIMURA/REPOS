/*- 5:Ver.5,  95:Ver.95 -*/
/*-----------------------------------------------------------------------*/
/*-	        UNCUT.MC						-*/
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
MSElementDescr  *elmDP1, *elmDP2, *elmDP3;
ULong		firstPos, secondPos, Pos;
DPoint3d	Poi[2];                      /* �Z���̒��S */
DPoint3d	isP1, isP2, isP3[2];         /* �Z�����m�����Ԑ��ƃZ���̊O�`�Ƃ̌�_ */
DPoint3d	houPoi[2];                   /* ������ */
DPoint3d	rbP[2];                      /* ���o�[�o���h */
double		length, maxlength;
int		type, index, exitStatus;
DgnModelRef     *modelRef, *modelRef2;
UInt32		lv1, co1,  wt1;
UInt32		lv2, co2,  wt2;
int         lc1, lc2;
HOUSET      	*houset;

#ifndef Private
#define Private
#endif
/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
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
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult     result;
CExprValue      value;

   /*--------- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"UNCUT�̃��\�[�X�t�@�C�����J���܂���");
	  mdlSystem_exit(exitStatus, 1);
      return;
   }

   /*--------- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&houset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR ) != SUCCESS){
      mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
	  mdlSystem_exit(exitStatus, 1);
      return;
   }
   houset = (HOUSET*)value.val.valPointer;

   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   cell_first( );

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	��P�Z��
-----------------------------------------------------------------------*/
Private void	cell_first( )
{
   /*---------- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
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

   /*------ �Z�����̑ȉ~�v�f��ǂݍ��� ------*/
   firstPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
   type = mdlElement_getType( dgnBuf);

   /*------ �Z����I�񂾏ꍇ ------*/
   if ( type == 2){

      /*------ �Z���̊�_���W���擾 ------*/
      mdlCell_extract( &Poi[0], NULL, NULL, NULL, NULL, NULL, dgnBuf);
      Pos = firstPos;
      mdlElement_read( dgnBuf, modelRef, Pos);

      /*------ ���C���[���A�N�e�B�u�̏ꍇ ------*/
      if ( houset->layer == 0){
		  ActiveParams::GetValue(lv1, ACTIVEPARAM_LEVEL);
		  ActiveParams::GetValue(co1, ACTIVEPARAM_COLOR);
		  ActiveParams::GetValue(wt1, ACTIVEPARAM_LINEWEIGHT);
		  ActiveParams::GetValue(lc1, ACTIVEPARAM_LINESTYLE);
      }

      /*------ ���C���[���Z���Ɠ����ꍇ ------*/
      if ( houset->layer == 1){
         mdlElement_getProperties( &lv1, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, dgnBuf);
         mdlElement_getSymbology( &co1, &wt1, &lc1, dgnBuf);
      }

      /*------ ���C���[��ݒ肷��ꍇ ------*/
      if ( houset->layer == 2){
         lv1 = houset->lv;
         co1 = houset->co;
         lc1 = houset->lc;
         wt1 = houset->wt;
      }

      cell_second( );
   }

   /*------ �Z���ȊO��I��ł��܂����ꍇ ------*/
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
   /*---------- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
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
   memcpy( &rbP[0], &Poi[0], sizeof(Dpoint3d) );
   memcpy( &rbP[1], inpoint, sizeof(Dpoint3d) );

   mdlLineString_create( dgnBuf, NULL, rbP, 2);
}


/*-----------------------------------------------------------------------
**	��Q�Z���I��
-----------------------------------------------------------------------*/
Private void	cell_second_select( Dpoint3d *inpoint,  int view)
{

   /*------ �Z�����̑ȉ~�v�f��ǂݍ��� ------*/
   secondPos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef2);

   type = mdlElement_getType( dgnBuf);

   /*------ �Z����I�񂾏ꍇ ------*/
   if ( type == 2){

      /*------ �Z���̊�_���W���擾 ------*/
      mdlCell_extract( &Poi[1], NULL, NULL, NULL, NULL, NULL, dgnBuf);
      Pos = secondPos;
      mdlElement_read( dgnBuf, modelRef2, Pos);

      /*------ ���C���[���A�N�e�B�u�̏ꍇ ------*/
      if ( houset->layer == 0){
		  ActiveParams::GetValue(lv2, ACTIVEPARAM_LEVEL);
		  ActiveParams::GetValue(co2, ACTIVEPARAM_COLOR);
		  ActiveParams::GetValue(wt2, ACTIVEPARAM_LINEWEIGHT);
		  ActiveParams::GetValue(lc2, ACTIVEPARAM_LINESTYLE);
      }

      /*------ ���C���[���Z���Ɠ����ꍇ ------*/
      if ( houset->layer == 1){
         mdlElement_getProperties( &lv2, NULL, NULL, NULL, NULL,
                                   NULL, NULL, NULL, dgnBuf);
         mdlElement_getSymbology( &co2, &wt2, &lc2, dgnBuf);
      }

      /*------ ���C���[��ݒ肷��ꍇ ------*/
      if ( houset->layer == 2){
         lv2 = houset->lv;
         co2 = houset->co;
         lc2 = houset->lc;
         wt2 = houset->wt;
      }

      line( );
   }

   /*------ �Z���ȊO��I��ł��܂����ꍇ ------*/
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
   /*------ ��P�Z���Ƒ�Q�Z�������ԃ��C�����쐬 ------*/
   mdlLine_create( dgnBuf, NULL, Poi);
   mdlElmdscr_new( &elmDP3, NULL, dgnBuf);

   /*------ ��P�Z����ǂݍ��� ------*/
   mdlElmdscr_read( &elmDP1, firstPos, modelRef, 0, NULL);
   maxlength = 0;
   elmDP1 = elmDP1->h.firstElem;
   for( ; elmDP1; elmDP1 = elmDP1->h.next){

      /*------ ��P�Z�����̊e�v�f�ƃ��C���Ƃ̌�_ ------*/
      mdlIntersect_closestBetweenElms( &isP1, &isP3[0], &index, elmDP1,
                                       elmDP3, NULL, &Poi[0], 0.001);

      /*------ ��P�Z���̊�_����P�ԉ�����_�����߂� ------*/
      length = sqrt( pow( Poi[0].x - isP3[0].x, 2) 
                   + pow( Poi[0].y - isP3[0].y, 2) );
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[0], &isP3[0], sizeof( Dpoint3d) );
      }
   }

   /*------ ��Q�Z����ǂݍ��� ------*/
   mdlElmdscr_read( &elmDP2, secondPos, modelRef2, 0, NULL);
   maxlength = 0;
   elmDP2 = elmDP2->h.firstElem;
   for( ; elmDP2; elmDP2 = elmDP2->h.next){

      /*------ ��Q�Z�����̊e�v�f�ƃ��C���Ƃ̌�_ ------*/
      mdlIntersect_closestBetweenElms( &isP2, &isP3[1], &index, elmDP2,
                                       elmDP3, NULL, &Poi[1], 0.001);

      /*------ ��Q�Z���̊�_����P�ԉ�����_�����߂� ------*/
      length = sqrt( pow( Poi[1].x - isP3[1].x, 2) 
                   + pow( Poi[1].y - isP3[1].y, 2) );
      if ( length > maxlength){
         maxlength = length;
         memcpy( &houPoi[1], &isP3[1], sizeof( Dpoint3d) );
      }
   }

   /*------ ��������`�� ------*/
   mdlLineString_create( dgnBuf, NULL, houPoi, 2);
   mdlElement_setProperties( dgnBuf, &lv1, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL);
   mdlElement_setSymbology( dgnBuf, &co1, &wt1, &lc1);
   mdlElement_display( dgnBuf, DRAW_MODE_Normal);
   mdlElement_add( dgnBuf);

   /*------ ����̑�Q�Z��������̑�P�Z���ɂ��� ------*/
   memcpy( &Poi[0], &Poi[1], sizeof(Dpoint3d) );
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
   /*---------- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
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
    static int searchType[] = {CELL_HEADER_ELM};
    mdlLocate_setElemSearchMask (sizeof(searchType)/sizeof(int), searchType);
}
