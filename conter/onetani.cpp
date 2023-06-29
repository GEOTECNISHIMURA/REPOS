/*-----------------------------------------------------------------------*/
/*-	        ONETANI.MC													-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include "../CommonCE/naka8.cpp"
#include "conter.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
MSElement		ele;
int				exitStatus;
int				resetFlag;
int				kounv;				// ���_�Ƃ̌�����
Dpoint3d		kouPoi[101];		// ���_�Ƃ̌�����
UInt32			otco,  otwt;	    // �����E�J��
int             otlc;               // �����E�J��
int				otnum, otnv;		// �����E�J��
Dpoint3d		otPoi[10000];		// �����E�J��
ULong			otPos[1000];		// �����E�J��
int				tyoconNum;			// ���_�Ƃ̌������ƌ�������R���^�[
ULong			tyoconPos[1000];	// ���_�Ƃ̌������ƌ�������R���^�[
int				connum, connv;		// �R���^�[�̘A��
Dpoint3d		tmpPoi[50000];		// �R���^�[�̘A��
Dpoint3d		conPoi[50000];		// �R���^�[�̘A��
ULong			conPos[5000];		// �R���^�[�̘A��
double			hanleng[1000][2];	// �͈͓_�̃R���^�[�n�_����̋���
Dpoint3d		hanPoi[1000][2];	// �͈͓_
int				haniti[1000][2];	// �͈͓_�̃R���^�[��̈ʒu
double			tyoleng[1000];		// �����_�̃R���^�[�n�_����̋���
Dpoint3d		tyoPoi[1000][2];	// �V���̒��_
int				tyoiti[1000];		// �����_�̃R���^�[��̈ʒu
CONTER			*con;

/*-	�v���g�^�C�v	-*/
bool  unload(UnloadProgramReason unloadType);
void  startPrimitive_kousa( void);
void  display_kousa( Dpoint3d *inpoint, int view);	// ���_�ƌ���������̕\��
void  input_kousa( Dpoint3d *inpoint, int view);	// ���̒��_�̑I��
void  startPrimitive_onetani( void);
void  select_onetani( Dpoint3d *inpoint);			// �����E�J���̑I��
void  move_conter( void);							// �ړ�
int	  scan_onetani( Dpoint3d *inpoint, int *otnv, Dpoint3d *otPoi, ULong *otPos, UInt32 otlv, UInt32 *otco, int *otlc, UInt32 *otwt);// �����E�J���̌���
void  scan_all( int *num, ULong *Pos, UInt32 lv, UInt32 co, int lc, UInt32 wt);// �ݒ�ڲ԰�̑S�v�f������
void  fence_kousa( int otnv, DPoint3d *otPoi);	// ���_�ƌ������������t�F���X�쐬
int	  scan_conter( void);	// �R���^�[�̌���
void  reset( void);			// ���


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

   /*--- �L��ϐ��̎擾 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"�L��ϐ��擾�ł��܂���");
      return;
   }
   con = (CONTER*)value.val.valPointer;

   kounv = 0;
   startPrimitive_kousa( );

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool    unload(UnloadProgramReason unloadType)
{
	mdlTransient_free(&msTransientElmP, true);
	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**	startPrimitive
-----------------------------------------------------------------------*/
void	startPrimitive_kousa( )
{
   mdlTransient_free(&msTransientElmP, true);
   mdlState_startPrimitive( (StateFunc_DataPoint)input_kousa, (StateFunc_Reset)startPrimitive_onetani, NULL, NULL);
   if ( kounv >= 1){
      mdlState_dynamicUpdate( (StateFunc_SimpleDynamics)display_kousa, FALSE);
   }

   mdlOutput_error( L"���_�Ƃ̌��������́^���������͏I��");
}


/*-----------------------------------------------------------------------
**	���_�ƌ���������̕\��
-----------------------------------------------------------------------*/
void	display_kousa( Dpoint3d *inpoint, int view)
{
int		disnv,  dislc = 0;
UInt32  disco = 0, diswt = 3;
   memcpy( &kouPoi[kounv], inpoint, sizeof( Dpoint3d));

   disnv = kounv + 1;
   mdlLineString_create( dgnBuf, NULL, kouPoi, disnv);

   set_layer( NULL, &disco, &dislc, &diswt, NULL, dgnBuf);
}


/*-----------------------------------------------------------------------
**	���_�ƌ�������������
-----------------------------------------------------------------------*/
void	input_kousa( Dpoint3d *inpoint, int view)
{
   display_kousa( inpoint, view);

   kounv++;
   startPrimitive_kousa( );
}


/*-----------------------------------------------------------------------
**	startPrimitive
-----------------------------------------------------------------------*/
void	startPrimitive_onetani( )
{
int		dislc = 0;
UInt32  disco = 0, diswt = 3;

   /*--- ���_�ƌ���������̕\�� ---*/

   mdlLineString_create(&ele, NULL, kouPoi, kounv);
   mdlElement_setSymbology(&ele, &disco, &diswt, &dislc);
  // mdlElmdscr_new(&elmDP1, NULL, &ele);

   resetFlag = 1;
   mdlState_startPrimitive( (StateFunc_DataPoint)select_onetani, (StateFunc_Reset)reset, NULL, NULL);
   mdlTransient_free(&msTransientElmP, true);
   msTransientElmP = mdlTransient_addElement(msTransientElmP, &ele, 0, 0xffff, DRAW_MODE_TempDraw, false, false, true);

   mdlOutput_error( L"�����E�J���̑I��/���");
}


/*-----------------------------------------------------------------------
**	�����E�J���̑I��
-----------------------------------------------------------------------*/
void	select_onetani( Dpoint3d *inpoint)
{
Dpoint3d	tmpkouPoi[10], orig;
ULong		*Pos;
int			ii, jj, dislc = 0;
UInt32      disco = 0, diswt = 3;
int			tmpkounum, num, iti, tmpnv, lc;
UInt32      lv, co, wt;
double		leng;
MSElementDescr	*elmDP1;
MSElement       u;

   /*--- �����E�J���̌��� ---*/
   if ( scan_onetani( inpoint, &otnv, otPoi, &otPos[0], con->otlv, &otco, &otlc, &otwt) == SUCCESS){
      Pos = (ULong*)malloc( 5000 * sizeof( ULong));

      /*--- �����E�J���̘A�������� ---*/
      scan_all( &num, Pos, con->otlv, otco, otlc, otwt);
      otnum = 1;
      get_renzoku( num, Pos, &otnv, otPoi, &otnum, otPos);

      /*--- ���_�Ƃ̌����������ɘg�𔭐������� ---*/
      tyoconNum = 0;
      fence_kousa( kounv, kouPoi);

      /*--- ���_�ƌ���������̕\�� ---*/
	  mdlLineString_create(&ele, NULL, kouPoi, kounv);
	  mdlElement_setSymbology(&ele, &disco, &diswt, &dislc);
	  mdlCell_create( &u,NULL,NULL,TRUE);
	  mdlElmdscr_new(&elmDP1, NULL, &u);
	  mdlElmdscr_appendElement(elmDP1, &ele);
	 

      /*--- �R���^�[�̃t�@�C���|�W�V���� ---*/
      for( ii=0; ii<tyoconNum; ii++){
         mdlElement_read( &ele, MASTERFILE, tyoconPos[ii]);
         get_layer( &lv, &co, &lc, &wt, NULL, &ele);
         mdlLinear_extract( conPoi, &connv, &ele, MASTERFILE);
         conPos[0] = tyoconPos[ii];

         /*--- �R���^�[�̘A�������� ---*/
         scan_all( &num, Pos, lv, co, lc, wt);
         connum = 1;
         get_renzoku( num, Pos, &connv, conPoi, &connum, conPos);

         /*--- �����_ ---*/
         tmpkounum = judge_kouten_all( &kounv, kouPoi, &connv, conPoi, tmpkouPoi);
         del_doubled3( &tmpkounum, tmpkouPoi, 1);
         if ( tmpkounum >= 2){
            mdlDialog_openInfoBox( L"�P�̃R���^�[�ɒ��_���Q����܂�");
            disco = 3;

			//lineString_display( &disco, &dislc, &diswt, conPoi, &connv, DRAW_MODE_TempDraw);
			mdlLineString_create(&ele, NULL, conPoi, connv);
			mdlElement_setSymbology(&ele, &disco, &diswt, &dislc);
			mdlElmdscr_appendElement(elmDP1, &ele);

            break;
         }
         memcpy( &tyoPoi[ii][0], &tmpkouPoi[0], sizeof( Dpoint3d));

         /*--- �����E�J����ŋ����_����ŒZ�����̓_ ---*/
         get_closest_point( &otnv, otPoi, &tyoPoi[ii][0], &tyoPoi[ii][1], &iti, &leng);

         /*--- �����_�̃R���^�[�n�_����̋��� ---*/
         memcpy( &orig, &conPoi[0], sizeof( Dpoint3d));
         get_leng_along_line( &tyoleng[ii], connv, conPoi, &orig, &tyoPoi[ii][0]);// ���_

         /*--- �͈͓_�̃R���^�[�n�_����̋��� ---*/
         hanleng[ii][0] = tyoleng[ii] - 1000 * con->hani;// �n�_���͈͓_
         hanleng[ii][1] = tyoleng[ii] + 1000 * con->hani;// �I�_���͈͓_

         /*--- �͈͓_�̍��W�ƃR���^�[��̈ʒu ---*/
         get_point_along_line( hanleng[ii][0], connv, conPoi, &haniti[ii][0], &hanPoi[ii][0]);
         get_point_along_line( hanleng[ii][1], connv, conPoi, &haniti[ii][1], &hanPoi[ii][1]);

         /*--- �ړ��͈͂�\�� ---*/
         memcpy( &tmpPoi[0], &hanPoi[ii][0], sizeof( Dpoint3d));
         for( jj=haniti[ii][0]+1; jj<=haniti[ii][1]; jj++){
            memcpy( &tmpPoi[jj-haniti[ii][0]], &conPoi[jj], sizeof( Dpoint3d));
         }
         memcpy( &tmpPoi[haniti[ii][1]-haniti[ii][0]+1], &hanPoi[ii][1], sizeof( Dpoint3d));
         tmpnv = haniti[ii][1] - haniti[ii][0] + 2;
         //lineString_display( &disco, &dislc, &diswt, tmpPoi, &tmpnv, DRAW_MODE_TempDraw);
		 mdlLineString_create(&ele, NULL, tmpPoi, tmpnv);
		 mdlElement_setSymbology(&ele, &disco, &diswt, &dislc);
		 mdlElmdscr_appendElement(elmDP1, &ele);
      }

      /*--- �����E�J���̕\�� ---*/
      //lineString_display( &disco, &dislc, &diswt, otPoi, &otnv, DRAW_MODE_TempDraw);
	  mdlLineString_create(&ele, NULL, otPoi, otnv);
	  mdlElement_setSymbology(&ele, &disco, &diswt, &dislc);
	  mdlElmdscr_appendElement(elmDP1, &ele);

      free( Pos);

      resetFlag = 2;
      mdlState_startPrimitive( (StateFunc_DataPoint)move_conter, (StateFunc_Reset)reset, NULL, NULL);
	  mdlTransient_free(&msTransientElmP, true);
	  msTransientElmP = mdlTransient_addElemDescr(msTransientElmP, elmDP1, 0, 0xffff, DRAW_MODE_TempDraw, false, false, true);
	  mdlElmdscr_freeAll(&elmDP1);
      mdlOutput_error( L"���s/���");
   }
   else{
      mdlDialog_openInfoBox( L"�����E�J����I�����ĉ�����");
      startPrimitive_onetani( );
   }
}


/*-----------------------------------------------------------------------
**	�����E�J���̈ړ�
-----------------------------------------------------------------------*/
void  move_conter( void)
{
Dpoint3d	idou, orig, nearPoi;
ULong		*Pos;
int			num, tmpnv, ii, jj, lc;
UInt32      lv, co, wt;
UInt32		disco = 0, diswt = 3;
int         dislc = 0;
double		leng;

   Pos = (ULong*)malloc( 5000 * sizeof( ULong));

   /*--- �R���^�[�̃t�@�C���|�W�V���� ---*/
   for( ii=0; ii<tyoconNum; ii++){
      mdlElement_read( &ele, MASTERFILE, tyoconPos[ii]);
      get_layer( &lv, &co, &lc, &wt, NULL, &ele);
      mdlLinear_extract( conPoi, &connv, &ele, MASTERFILE);
      conPos[0] = tyoconPos[ii];

      /*--- �R���^�[�̘A�������� ---*/
      scan_all( &num, Pos, lv, co, lc, wt);
      connum = 1;
      get_renzoku( num, Pos, &connv, conPoi, &connum, conPos);

      for( jj=0; jj<connv; jj++){
         memcpy( &tmpPoi[jj], &conPoi[jj], sizeof( Dpoint3d));
      }
      tmpnv = connv;

      /*--- ���_�̈ړ��� ---*/
      idou.x = tyoPoi[ii][1].x - tyoPoi[ii][0].x;
      idou.y = tyoPoi[ii][1].y - tyoPoi[ii][0].y;

      /*--- �����_���R���^�[��̂ǂ̈ʒu�� ---*/
      get_closest_point( &connv, conPoi, &tyoPoi[ii][0], &nearPoi, &tyoiti[ii], &leng);

      /*--- �R���^�[�̎n�_ ---*/
      memcpy( &orig, &conPoi[0], sizeof( Dpoint3d));

      /*--- �͈́i�n�_���j�`���_�̊e�_���ړ� ---*/
      for( jj=haniti[ii][0]+1; jj<=tyoiti[ii]; jj++){
         get_leng_along_line( &leng, tmpnv, tmpPoi, &orig, &conPoi[jj]);
         conPoi[jj].x = conPoi[jj].x + idou.x * ( leng - hanleng[ii][0])/( tyoleng[ii] - hanleng[ii][0]);
         conPoi[jj].y = conPoi[jj].y + idou.y * ( leng - hanleng[ii][0])/( tyoleng[ii] - hanleng[ii][0]);
      }

      /*--- ���_�`�͈́i�I�_���j�̊e�_���ړ� ---*/
      for( jj=tyoiti[ii]+1; jj<=haniti[ii][1]; jj++){
         get_leng_along_line( &leng, tmpnv, tmpPoi, &orig, &conPoi[jj]);
         conPoi[jj].x = conPoi[jj].x + idou.x * ( hanleng[ii][1] - leng)/( hanleng[ii][1] - tyoleng[ii]);
         conPoi[jj].y = conPoi[jj].y + idou.y * ( hanleng[ii][1] - leng)/( hanleng[ii][1] - tyoleng[ii]);
      }

      /*--- ���������� ---*/
      for( jj=0; jj<connv; jj++){
         memcpy( &tmpPoi[jj], &conPoi[jj], sizeof( Dpoint3d));
      }
      lineString_add( &lv, &co, &lc, &wt, NULL, conPoi, &connv);
      for( jj=0; jj<connum; jj++){
         mdlElement_read( &ele, MASTERFILE, conPos[jj]);
         set_layer( NULL, &disco, &dislc, &diswt, NULL, &ele);
         mdlElement_display( &ele, DRAW_MODE_Erase);
         mdlElement_undoableDelete( &ele, conPos[jj], DRAW_MODE_Erase);
      }
      lineString_display( &co, &lc, &wt, tmpPoi, &tmpnv, DRAW_MODE_TempDraw);
   }
   free( Pos);

   reset( );
}


/*-----------------------------------------------------------------------
**	�����E�J���̌���
-----------------------------------------------------------------------*/
int  scan_onetani( Dpoint3d *inpoint, int *otnv, Dpoint3d *otPoi, ULong *otPos, UInt32 otlv, UInt32 *otco, int *otlc, UInt32 *otwt)
{
ExtScanList	scanList;
ULong		filePos, elemAddr[1];
DSegment3d	seg;
Dpoint3d	nearPoi;
int			word, fileNum, getSize, Error;

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.scantype |= (unsigned short)LEVELS;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
   set_lvmask( &con->otlv, (short*)scanList.levmask);

   /*--- �X�L�������s ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
	  return(ERROR);
   }

   while( TRUE){
      Error = mdlScan_file( elemAddr, &getSize, sizeof( elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
	  if ( Error >= 65) { mdlSystem_exit(exitStatus, 1);return(ERROR); }

	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      get_layer( NULL, otco, otlc, otwt, NULL, &ele);

      /*--- �X�L�����������C����ŁA���������_����ŒZ�����̓_ ---*/
      memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

      /*--- �ŒZ�����̓_��1mm�ȉ��̏ꍇ�́A�X�L�����������C���������������̂Ƃ��� ---*/
      if ( get_leng( &nearPoi, inpoint) < 1){
         mdlLinear_extract( otPoi, otnv, &ele, MASTERFILE);
         *otPos = elemAddr[0];
         return( SUCCESS);
      }
   }
   return( ERROR);
}


/*-----------------------------------------------------------------------
**	�ݒ�ڲ԰�̑S�v�f�̌���
-----------------------------------------------------------------------*/
void  scan_all( int *num, ULong *Pos, UInt32 lv, UInt32 co, int lc, UInt32 wt)
{
ExtScanList	scanList;
ULong		filePos, elemAddr[1];
int			word, fileNum, getSize, Error, linlc;
UInt32		linco,  linwt;

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.scantype |= (unsigned short)LEVELS;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
   set_lvmask( &lv, (short*)scanList.levmask);

   /*--- �X�L�������s ---*/
   *num = 0;   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
      Error = mdlScan_file( elemAddr, &getSize, sizeof( elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
	  if ( Error >= 65) { mdlSystem_exit(exitStatus, 1);return; }

	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      get_layer( NULL, &linco, &linlc, &linwt, NULL, &ele);

      if ( linco==co && linlc==lc && linwt==wt){
         Pos[*num] = elemAddr[0];
         *num = *num + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	���_�ƌ������������g�𔭐�������
-----------------------------------------------------------------------*/
void  fence_kousa( int kounv, DPoint3d *kouPoi)
{
Dpoint3d	fPoi[101], *tmpPoi, center, delta1, delta2;
ULong		fileMask[8];
short		viewList[8], oriLvMask[4];
int			ii, tmpnv;
UInt32       actlv;
double		zoomFactor;
DgnModelRefListP    modelList;

   /*--- �t�F���X�����d�Ȃ�ݒ� ---*/
   set_fence_overlap( );

   /*--- ��w�\���� ---*/

   ActiveParams::GetValue(actlv, ACTIVEPARAM_LEVEL);
   mdlView_getLevels( oriLvMask, tcb->lstvw);

   /*--- �t�B�b�g ---*/
   mdlView_getParameters( NULL, &center, &delta1, NULL, NULL, 0);
   mdlModelRefList_create(&modelList);
   mdlModelRefList_add(modelList, MASTERFILE);
   mdlView_fit( tcb->lstvw, modelList);
   mdlModelRefList_free(&modelList);

   /*--- ���W�ۑ� ---*/
   tmpPoi = (DPoint3d*)malloc( 2000 * sizeof( Dpoint3d));
   for( ii=0; ii<kounv; ii++){
      memcpy( &tmpPoi[ii], &kouPoi[ii], sizeof( Dpoint3d));
   }
   tmpnv = kounv;

   /*--- ���S��50�_���g���ăt�F���X�쐬 ---*/
   while( 1){
      if ( tmpnv > 50){
         /*--- �t�F���X�쐬 ---*/
         for( ii=0; ii<50; ii++){
            memcpy( &fPoi[ii], &tmpPoi[ii], sizeof( Dpoint3d));
         }
         for( ii=50; ii<100; ii++){
            memcpy( &fPoi[ii], &tmpPoi[99-ii], sizeof( Dpoint3d));
         }
         mdlShape_create( &ele, NULL, fPoi, 100, 0);
         mdlFence_fromShape( &ele);

         /*--- �t�F���X������ ---*/
         mdlState_startFenceCommand( (StateFunc_FenceContentOp)scan_conter, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
         mdlFence_process( NULL);

         for( ii=0; ii<tmpnv-49; ii++){
            memcpy( &tmpPoi[ii], &tmpPoi[ii+49], sizeof( Dpoint3d));
         }
         tmpnv = tmpnv - 49;
      }
      else{
         /*--- �t�F���X�쐬 ---*/
         for( ii=0; ii<tmpnv; ii++){
            memcpy( &fPoi[ii], &tmpPoi[ii], sizeof( Dpoint3d));
         }
         for( ii=tmpnv; ii<2*tmpnv; ii++){
            memcpy( &fPoi[ii], &tmpPoi[2*tmpnv-1-ii], sizeof( Dpoint3d));
			fPoi[ii].x = fPoi[ii].x + 1;
			fPoi[ii].y = fPoi[ii].y + 1;
         }
		 fPoi[ii].x = fPoi[0].x;
		 fPoi[ii].y = fPoi[0].y;
		 fPoi[ii].z = 0;
         mdlShape_create( &ele, NULL, fPoi, 2*tmpnv +1, 0);
         mdlFence_fromShape( &ele);

         /*--- �t�F���X������ ---*/
         mdlState_startFenceCommand((StateFunc_FenceContentOp)scan_conter, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
         mdlFence_process( NULL);
         break;
      }
   }
   free( tmpPoi);

   /*--- ���̃r���[�T�C�Y�ɖ߂� ---*/
   mdlView_getParameters( NULL, NULL, &delta2, NULL, NULL, 0);
   zoomFactor = delta1.x/delta2.x;
   mdlView_zoom( tcb->lstvw, tcb->lstvw, &center, zoomFactor);

   /*--- ���̉�w�ɖ߂� ---*/
   for( ii=0; ii<8; ii++){ viewList[ii] = 0;}
   viewList[tcb->lstvw] = 1;
   ActiveParams::SetValue(actlv, ACTIVEPARAM_LEVEL);
   set_view_lvmask( viewList, oriLvMask);
   mdlView_updateSingle( tcb->lstvw);

   mdlFence_clear( TRUE);
}


/*-----------------------------------------------------------------------
**	���_�Ƃ̌������ƌ�������R���^�[
-----------------------------------------------------------------------*/
int  scan_conter( )
{
ULong	filePos;
int		fileNum, type;
UInt32  lv;
DgnModelRefP modelRef;

   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);

   /*--- �A�N�e�B�u�t�@�C���̂� ---*/
   if ( mdlModelRef_isActiveModel(modelRef) == TRUE){
      mdlElement_read( &ele, MASTERFILE, filePos);
      type = mdlElement_getType( &ele);

      /*--- ���C���E���C���X�g�����O�̂� ---*/
      if ( type==3 || type==4){
         get_layer( &lv, NULL, NULL, NULL, NULL, &ele);
         if ( lv==con->klv || lv==con->slv){
            tyoconPos[tyoconNum] = filePos;
            tyoconNum++;
         }
      }
   }

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	���
-----------------------------------------------------------------------*/
void	reset( )
{
int		disco = 0, dislc = 0, diswt = 3;

mdlView_updateSingle( tcb->lstvw);
//   lineString_display( &disco, &dislc, &diswt, kouPoi, &kounv, DRAW_MODE_Erase);

//   if ( resetFlag = 2){
//      lineString_display( &disco, &dislc, &diswt, otPoi, &otnv, DRAW_MODE_Erase);
//      lineString_display( &otco, &otlc, &otwt, otPoi, &otnv, DRAW_MODE_Normal);
//   }
   kounv = 0;
   startPrimitive_kousa( );
}
