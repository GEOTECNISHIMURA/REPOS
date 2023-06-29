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

/*-	�t�b�N�֐�		-*/

/*-	�v���g�^�C�v	-*/
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

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--------- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"�L��ϐ��擾�ł��܂���");
      return;
   }
   con = (CONTER*)value.val.valPointer;

   num = 0;   time1 = 0;
   setFunction( );

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
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
   mdlOutput_command( L"�_�u���N���b�N�ŏI��");
   mdlOutput_prompt( L"����/�߂�");
}


/*-----------------------------------------------------------------------
**	���I�\��
-----------------------------------------------------------------------*/
void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   inpoint[0].z = 0;
   memcpy( &catPoi[num], inpoint, sizeof( Dpoint3d));
   num++;

   /*--- �Q�_�ڈȍ~�̏ꍇ�́AB-�X�v���C����\������ ---*/
   if ( num >= 2){
      /*--- �\�� ---*/
      display(DRAW_MODE_TempDraw);
   }
   num--;
}


/*-----------------------------------------------------------------------
**	���W�̓���
-----------------------------------------------------------------------*/
void	input_point( Dpoint3d *inpoint)
{
	//mdlTransient_free(&msTransientElmP, true);	// ��������폜
	inpoint[0].z = 0;
   /*--- MicroStation �N����̌o�ߎ��� ---*/
   //time2 = mdlSystem_elapsedTime( );
   time2 = mdlSystem_getTicks();
   /*--- �V���O���N���b�N�̏ꍇ ---*/
   if ( time2 - time1 >= 12){
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
void	bspln_add( )
{
MSElement	ele;
Dpoint3d 	*lPoi;
int			ii, remainNv;

   /*--- 2�_�ڂ̑O�Ƀ_�u���N���b�N�����ꍇ ---*/
   num--;
   if ( num == 1){
      num++;
      /*--- �\�������� ---*/
      display( DRAW_MODE_Erase);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2�_�ڈȍ~�Ƀ_�u���N���b�N�����ꍇ ---*/
   else if ( num >= 2){
		/*--- ��������\�������� ---*/
		mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Erase);

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
			lPoi = (DPoint3d*)malloc( 10000 * sizeof( Dpoint3d));

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
void	cleanup( )
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
      mdlElmdscr_display( elmDP1, MASTERFILE, DRAW_MODE_Erase);

      /*--- B-�X�v���C���p�������̊J�� ---*/
      mdlBspline_freeCurve( &curve);

      /*--- �v�f�L�q�q�p�������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }

   /*--- B-�X�v���C���g�p���Ȃ��ꍇ ---*/
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &num, DRAW_MODE_Erase);
   }

   num = 0;
}


/*-----------------------------------------------------------------------
**	�\��
-----------------------------------------------------------------------*/
void	display( int drawMode)
{
MSElement	ele, u;
DPoint3d    *tmpPoi;
int			tmpnv, ii;
UInt32      lv, co, wt;
int         lc;

   /*--- B-�X�v���C���̏ꍇ ---*/
   if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, num);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- B-�X�v���C���̕\�� ---*/
	  mdlTransient_free(&msTransientElmP, true);
	  msTransientElmP = mdlTransient_addElemDescr(msTransientElmP, elmDP1, 0, 0xffff, drawMode, false, false, true);

      /*--- B-�X�v���C���p�������̊J�� ---*/
      mdlBspline_freeCurve( &curve);
   }

   /*--- B-�X�v���C���łȂ��ꍇ ---*/
   else{
		ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
		ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
		ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
		ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
		mdlLineString_create( &ele, NULL, catPoi, num);
		mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		mdlElement_setSymbology(&ele, &co, &wt, &lc);

      /*--- �\�� ---*/
	  mdlTransient_free(&msTransientElmP, true);
	  msTransientElmP = mdlTransient_addElement(msTransientElmP, &ele, 0, 0xffff, drawMode, false, false, true);

   }

   /*--- �v�f�L�q�q�p�������̊J�� ---*/
   mdlElmdscr_freeAll( &elmDP1);
}


/*-----------------------------------------------------------------------
**	�߂�
-----------------------------------------------------------------------*/
void	bspln_undo( )
{
   /*--- 2�_�ړ��͂̑O ---*/
   if ( num == 1){
      /*--- �\�������� ---*/
      display( DRAW_MODE_Erase);

      num--;
      mdlState_clear( );
      setFunction( );
   }

   /*--- 2�_�ڈȍ~ ---*/
   if ( num >= 2){
      /*--- �\�������� ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &num, DRAW_MODE_Erase);

      /*--- �P�_�߂� ---*/
      num--;
   }
}


/*----------------------------------------------------------------------+
**	���W�̌v�Z
+----------------------------------------------------------------------*/
void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance)
{
int			bufferSize;
double      maxDeviation;
Dpoint3d    *tmpP = NULL;

   *nv = 2;   bufferSize = 33;

   /*--- ���W�̃������̊��蓖�� ---*/
   if ( ( tmpP = (DPoint3d*)malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
      return;
   }
   do{
      *nv = ( *nv - 1) * 2 + 1;
      if ( *nv > bufferSize){
         free ( tmpP);

         /*--- ���W�̃��������R�{�ɂ��Ċ��蓖�Ă� ---*/
         bufferSize = *nv * 3;
         if ( ( tmpP = (DPoint3d*)malloc( bufferSize * sizeof( Dpoint3d))) == NULL){
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
void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP)
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
/*------------------------------------------------------------------------------------------------*/
/*--- �������C���X�g�����O�̔z�u -----------------------------------------------------------------*/
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
/*--- �������C���X�g�����O�̕\�� -----------------------------------------------------------------*/
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
**	mdlElement_display �̑���
-----------------------------------------------------------------------*/
void mdlElement_displayCE(MSElementP elm, DgnDrawMode drawMode)
{
	mdlTransient_free(&msTransientElmP, true);
	if (elm == NULL)
		return;

	msTransientElmP = mdlTransient_addElement(NULL, elm, 0, 0xffff, drawMode, false, false, true);
}
