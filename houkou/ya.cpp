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
DPoint3d	isP1, isP2;         /* �Z�����m�����Ԑ��ƃZ���̊O�`�Ƃ̌�_ */
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
/*-	�v���g�^�C�v	-*/
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
      mdlOutput_error(L"CUT�̃��\�[�X�t�@�C�����J���܂���");
      return;
   }

   /*--------- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&houset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR ) != SUCCESS){
      mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
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
      mdlCell_extract( &orig, NULL, NULL, NULL, NULL, NULL, dgnBuf);
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

      ya( );
   }

   /*------ �Z���ȊO��I��ł��܂����ꍇ ------*/
   else{
      mdlOutput_prompt(L"�q�{�^���������ĉ�����");
      mdlOutput_error(L"���̗v�f�͑I�ׂ܂���");
   }
}



/*-----------------------------------------------------------------------
**	���
-----------------------------------------------------------------------*/
Private void	ya( )
{
   /*---------- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
   StateCallback::SetDataPointFunction((StateFunc_DataPoint)ya_place);
   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)ya_dynamic, FALSE);
}



/*-----------------------------------------------------------------------
**	�������̃��o�[�o���h
-----------------------------------------------------------------------*/
Private void	ya_dynamic( Dpoint3d *inpoint,  int view)
{
   memcpy( &rbP[0], &orig, sizeof(Dpoint3d) );
   memcpy( &rbP[1], inpoint, sizeof(Dpoint3d) );

   mdlLineString_create( dgnBuf, NULL, rbP, 2);
}



/*-----------------------------------------------------------------------
**	�����쐬
-----------------------------------------------------------------------*/
Private void	ya_place( Dpoint3d *inpoint,  int view)
{
   memcpy( &linPoi[1], &orig, sizeof(Dpoint3d) );
   memcpy( &linPoi[0], inpoint, sizeof(Dpoint3d) );
   memcpy( &yarPoi[0], inpoint, sizeof(Dpoint3d) );
   memcpy( &yalPoi[0], inpoint, sizeof(Dpoint3d) );

   mdlLine_create( &ele, NULL, linPoi);
   mdlElmdscr_new( &elmDP2, NULL, &ele);

   /*------ ��P�Z����ǂݍ��� ------*/
   mdlElmdscr_read( &elmDP1, firstPos, modelRef, 0, NULL);
   maxlength = 0;
   elmDP1 = elmDP1->h.firstElem;
   for( ; elmDP1; elmDP1 = elmDP1->h.next){

      /*------ ��P�Z�����̊e�v�f�ƃ��C���Ƃ̌�_ ------*/
      mdlIntersect_closestBetweenElms( &isP1, &isP2, &index, elmDP1,
		  elmDP2, NULL, &linPoi[0], 0.001);

      /*------ ��P�Z���̊�_����P�ԉ�����_�����߂� ------*/
      length = sqrt( pow( isP2.x - orig.x, 2) 
                   + pow( isP2.y - orig.y, 2) );
      if ( length > maxlength){
         maxlength = length;
         memcpy( &linPoi[1], &isP2, sizeof( Dpoint3d) );
      }
   }

   /*------ �������쐬 ------*/
   mdlLine_create( &ele, NULL, linPoi);
   mdlElement_setProperties( &ele, &lv1, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   mdlElement_setSymbology( &ele, &co1, &wt1, &lc1);
   mdlElement_add( &ele);
   mdlElement_display( &ele, DRAW_MODE_Normal);

   /*------ �������̊p�x ------*/
   angle = atan2( linPoi[1].y-linPoi[0].y, linPoi[1].x-linPoi[0].x);

   /*------ ���쐬 ------*/
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
**	���Z�b�g���������ꍇ
-----------------------------------------------------------------------*/
Private void	reset( )
{
   /*---------- ���Z�b�g�{�^���A�f�[�^�{�^������������ ----------*/
   mdlState_startModifyCommand( (StateFunc_Reset)end, (MdlFunc_Accept)ya_place,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_init( );
   mdlLocate_noElemAllowLocked( );
   elemType( );
   mdlOutput_prompt(L"�Z����I��ŉ�����");
   mdlOutput_error(L"");

   mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)ya_dynamic, FALSE);
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
    mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);
}
