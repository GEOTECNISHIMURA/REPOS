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

/*-	�t�b�N�֐�		-*/

/*-	�v���g�^�C�v	-*/
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
**	���C��
-----------------------------------------------------------------------*/
main( )
{
#pragma			resourceID	NEW_ID
CExprResult     result;
CExprValue      value;

   /*--------- �A�����[�h�֐��Z�b�g ---------------*/
   mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);

   /*--------- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox("�L��ϐ��擾�ł��܂���");
      return( SUCCESS);
   }
   con = value.val.valPointer;

   num = 0;   time1 = 0;
   setFunction( );

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
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
   mdlOutput_command( "�_�u���N���b�N�ŏI��");
   mdlOutput_prompt( "����/�߂�");
}


/*-----------------------------------------------------------------------
**	���I�\��
-----------------------------------------------------------------------*/
Private void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
   num++;

   /*--- �Q�_�ڈȍ~�̏ꍇ�́AB-�X�v���C����\������ ---*/
   if ( num >= 2){
      /*--- �\�� ---*/
      display( TEMPDRAW);
   }
   num--;
}


/*-----------------------------------------------------------------------
**	���W�̓���
-----------------------------------------------------------------------*/
Private void	input_point( Dpoint3d *inpoint)
{
   /*--- MicroStation �N����̌o�ߎ��� ---*/
   time2 = mdlSystem_elapsedTime( );

   /*--- �V���O���N���b�N�̏ꍇ ---*/
   if ( time2 - time1 >= 0.2){
      /*--- ���W�̕ۑ� ---*/
      memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
      num++;

      /*--- ���I�\�����\�ɂ��� ---*/
      if ( num == 1){
         setFunction( );
      }

      /*--- �����̕ۑ� ---*/
      time1 = time2;
   }

   /*--- �_�u���N���b�N�̏ꍇ ---*/
   else{
      /*--- B-�X�v���C���̔z�u ---*/
      bspln_add( );
   }
}


/*-----------------------------------------------------------------------
**	B-�X�v���C���̔z�u
-----------------------------------------------------------------------*/
Private void	bspln_add( )
{
MSElement	ele;
Dpoint3d 	*lPoi;
int			ii, remainNv;

   /*--- 2�_�ڂ̑O�Ƀ_�u���N���b�N�����ꍇ ---*/
   num--;
   if ( num == 1){
      num++;
      /*--- �\�������� ---*/
      display( ERASE);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2�_�ڈȍ~�Ƀ_�u���N���b�N�����ꍇ ---*/
   else if ( num >= 2){
      /*--- ��������\�������� ---*/
      mdlElmdscr_display( elmDP2, 0, ERASE);

      /*--- �����̐ݒ� ---*/
      for( ii=0; ii<num; ii++){
         catPoi[ii].z = 1000 * con->zz;
      }

/*--- B-�X�v���C���̏ꍇ ---*/
if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- �v�f�L�q�q�p�������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP2);
      mdlElmdscr_freeAll( &elmDP1);

      /*--- �������̊��蓖�� ---*/
      lPoi = malloc( 10000 * sizeof( Dpoint3d));

      /*--- �ϊ���̊e�_�̍��W���v�Z ---*/
      computePointsByTolerance( &lPoi, &remainNv, &curve, 1000 * con->genkou);

      lineString_add( NULL, NULL, NULL, NULL, NULL, lPoi, &remainNv);

      /*--- �������̊J�� ---*/
      mdlBspline_freeCurve( &curve);
      free( lPoi);
}
/*--- ���C���̏ꍇ ---*/
else{
      lineString_add( NULL, NULL, NULL, NULL, NULL, catPoi, &num);
}
   }

   /*--- �������̊J�� ---*/
   mdlBspline_freeCurve( &curve);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);

   num = 0;
   mdlState_clear( );
   setFunction( );
}


/*-----------------------------------------------------------------------
**	�N���[���A�b�v
-----------------------------------------------------------------------*/
Private void	cleanup( )
{
   /*--- �C�������̕\���̏��� ---*/
   num++;

   /*--- B-�X�v���C���g�p�̏ꍇ ---*/
   if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- ���O�̕\�������� ---*/
      mdlElmdscr_display( elmDP1, 0, ERASE);

      /*--- B-�X�v���C���p�������̊J�� ---*/
      mdlBspline_freeCurve( &curve);

      /*--- �v�f�L�q�q�p�������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }

   /*--- B-�X�v���C���g�p���Ȃ��ꍇ ---*/
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &num, ERASE);
   }

   num = 0;
}


/*-----------------------------------------------------------------------
**	�\��
-----------------------------------------------------------------------*/
Private void	display( int drawMode)
{
MSElement	ele, u;
Dpoint3d	*tmpPoi;
int			tmpnv, ii;

   /*--- B-�X�v���C���̏ꍇ ---*/
   if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- B-�X�v���C���̕\�� ---*/
      mdlElmdscr_display( elmDP1, 0, drawMode);

      /*--- B-�X�v���C���p�������̊J�� ---*/
      mdlBspline_freeCurve( &curve);
   }

   /*--- B-�X�v���C���łȂ��ꍇ ---*/
   else{
      /*--- �w�b�_�쐬 ---*/
      mdlComplexChain_createHeader( &u, FALSE, 0);
      mdlElmdscr_new( &elmDP1, NULL, &u);

      /*--- �ۑ� ---*/
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

      /*--- �\�� ---*/
      mdlElmdscr_display( elmDP1, 0, drawMode);

      /*--- �������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP1);
      free( tmpPoi);
   }

   /*--- �v�f�L�q�q�p�������̊J�� ---*/
   mdlElmdscr_freeAll( &elmDP1);
}


/*-----------------------------------------------------------------------
**	�߂�
-----------------------------------------------------------------------*/
Private void	bspln_undo( )
{
   /*--- 2�_�ړ��͂̑O ---*/
   if ( num == 1){
      /*--- �\�������� ---*/
      display( TEMPERASE);

      num--;
      mdlState_clear( );
      setFunction( );
   }

   /*--- 2�_�ڈȍ~ ---*/
   if ( num >= 2){
      /*--- �\�������� ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &num, ERASE);

      /*--- �P�_�߂� ---*/
      num--;
   }
}


/*----------------------------------------------------------------------+
**	���W�̌v�Z
+----------------------------------------------------------------------*/
Private void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance)
{
int			bufferSize;
double      maxDeviation;
Dpoint3d    *tmpP = NULL;

   *nv = 2;   bufferSize = 33;

   /*--- ���W�̃������̊��蓖�� ---*/
   if ( ( tmpP = malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
      return;
   }
   do{
      *nv = ( *nv - 1) * 2 + 1;
      if ( *nv > bufferSize){
         free ( tmpP);

         /*--- ���W�̃��������R�{�ɂ��Ċ��蓖�Ă� ---*/
         bufferSize = *nv * 3;
         if ( ( tmpP = malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
			return;
         }
	  }

      /*---	B-�X�v���C���Ȑ�����ŒZ�����̍��W���v�Z���� ---*/
      computeMinDistToCurve( tmpP, &maxDeviation, *nv, curveP);
   }
   while( maxDeviation > tolerance);

   *lPoi = tmpP;
}


/*----------------------------------------------------------------------+
**	B-�X�v���C���Ȑ�����ŒZ�����̍��W���v�Z����
+----------------------------------------------------------------------*/
Private void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP)
{
double      deviation, step, param;
Dpoint3d    mid, *pP, *endP;

   *maxDeviationP = param = 0.0;
   step = fc_1 /( nv - 1);
   for( pP = endP = poi, endP += nv; pP < endP; pP++){
      /*--- B-�X�v���C���Ȑ���̓_ ---*/
      mdlBspline_evaluateCurvePoint( pP, NULL, curveP, param);
	  if ( pP != poi){
         /*--- �O�̓_�Ƃ̒��_�����߂� ---*/
	   	 mdlVec_addPoint( &mid, pP, pP-1);
	     mdlVec_scale( &mid, &mid, 0.5);

         /*--- �_����ŒZ�����ɂ���B-�X�v���C���Ȑ���̓_ ---*/
	     mdlBspline_minimumDistanceToCurve( &deviation, NULL, NULL, &mid, curveP);
	     if ( deviation > *maxDeviationP){
			*maxDeviationP = deviation;
         }
      }
	  param += step;
   }
}
