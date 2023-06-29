/*-----------------------------------------------------------------------*/
/*-	        ONETANI.MC													-*/
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
MSElement		ele;
int				exitStatus;
int				resetFlag;
int				kounv;				// ���_�Ƃ̌�����
Dpoint3d		kouPoi[101];		// ���_�Ƃ̌�����
int				otco, otlc, otwt;	// �����E�J��
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
Private int	  unload( int);
Private void  startPrimitive_kousa( void);
Private void  display_kousa( Dpoint3d *inpoint, int view);	// ���_�ƌ���������̕\��
Private void  input_kousa( Dpoint3d *inpoint, int view);	// ���̒��_�̑I��
Private void  startPrimitive_onetani( void);
Private void  select_onetani( Dpoint3d *inpoint);			// �����E�J���̑I��
Private void  move_conter( void);							// �ړ�
Private int	  scan_onetani( Dpoint3d *inpoint, int *otnv, Dpoint3d *otPoi, ULong *otPos, int otlv, int *otco, int *otlc, int *otwt);// �����E�J���̌���
Private void  scan_all( int *num, ULong *Pos, int lv, int co, int lc, int wt);// �ݒ�ڲ԰�̑S�v�f������
Private void  fence_kousa( int otnv, DPoint3d *otPoi);	// ���_�ƌ������������t�F���X�쐬
Private int	  scan_conter( void);	// �R���^�[�̌���
Private void  reset( void);			// ���


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
main( )
{
#pragma			resourceID	ONETANI_ID
CExprResult     result;
CExprValue      value;

   /*--- �A�����[�h�֐��Z�b�g ---*/
   mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);

   /*--- �L��ϐ��̎擾 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox("�L��ϐ��擾�ł��܂���");
      return( SUCCESS);
   }
   con = value.val.valPointer;

   kounv = 0;
   startPrimitive_kousa( );

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
**	startPrimitive
-----------------------------------------------------------------------*/
Private void	startPrimitive_kousa( )
{
   mdlState_startPrimitive( input_kousa, startPrimitive_onetani, NULL, NULL);
   if ( kounv >= 1){
      mdlState_dynamicUpdate( display_kousa, FALSE);
   }

   mdlOutput_error( "���_�Ƃ̌��������́^���������͏I��");
}


/*-----------------------------------------------------------------------
**	���_�ƌ���������̕\��
-----------------------------------------------------------------------*/
Private void	display_kousa( Dpoint3d *inpoint, int view)
{
int		disnv, disco = 0, dislc = 0, diswt = 3;

   memcpy( &kouPoi[kounv], inpoint, sizeof( Dpoint3d));

   disnv = kounv + 1;
   mdlLineString_create( dgnBuf, NULL, kouPoi, disnv);

   set_layer( NULL, &disco, &dislc, &diswt, NULL, dgnBuf);
}


/*-----------------------------------------------------------------------
**	���_�ƌ�������������
-----------------------------------------------------------------------*/
Private void	input_kousa( Dpoint3d *inpoint, int view)
{
   display_kousa( inpoint, view);

   kounv++;
   startPrimitive_kousa( );
}


/*-----------------------------------------------------------------------
**	startPrimitive
-----------------------------------------------------------------------*/
Private void	startPrimitive_onetani( )
{
int		disco = 0, dislc = 0, diswt = 3;

   /*--- ���_�ƌ���������̕\�� ---*/
   lineString_display( &disco, &dislc, &diswt, kouPoi, &kounv, NORMALDRAW);

   resetFlag = 1;
   mdlState_startPrimitive( select_onetani, reset, NULL, NULL);
   mdlOutput_error( "�����E�J���̑I��/���");
}


/*-----------------------------------------------------------------------
**	�����E�J���̑I��
-----------------------------------------------------------------------*/
Private void	select_onetani( Dpoint3d *inpoint)
{
Dpoint3d	tmpkouPoi[10], orig;
ULong		*Pos;
int			ii, jj, disco = 0, dislc = 0, diswt = 3;
int			tmpkounum, num, iti, tmpnv, lv, co, lc, wt;
double		leng;

   /*--- �����E�J���̌��� ---*/
   if ( scan_onetani( inpoint, &otnv, otPoi, &otPos[0], con->otlv, &otco, &otlc, &otwt) == SUCCESS){
      Pos = malloc( 5000 * sizeof( ULong));

      /*--- �����E�J���̘A�������� ---*/
      scan_all( &num, Pos, con->otlv, otco, otlc, otwt);
      otnum = 1;
      get_renzoku( num, Pos, &otnv, otPoi, &otnum, otPos);

      /*--- ���_�Ƃ̌����������ɘg�𔭐������� ---*/
      tyoconNum = 0;
      fence_kousa( kounv, kouPoi);

      /*--- ���_�ƌ���������̕\�� ---*/
      lineString_display( &disco, &dislc, &diswt, kouPoi, &kounv, NORMALDRAW);

      /*--- �R���^�[�̃t�@�C���|�W�V���� ---*/
      for( ii=0; ii<tyoconNum; ii++){
         mdlElement_read( &ele, 0, tyoconPos[ii]);
         get_layer( &lv, &co, &lc, &wt, NULL, &ele);
         mdlLinear_extract( conPoi, &connv, &ele, 0);
         conPos[0] = tyoconPos[ii];

         /*--- �R���^�[�̘A�������� ---*/
         scan_all( &num, Pos, lv, co, lc, wt);
         connum = 1;
         get_renzoku( num, Pos, &connv, conPoi, &connum, conPos);

         /*--- �����_ ---*/
         tmpkounum = judge_kouten_all( &kounv, kouPoi, &connv, conPoi, tmpkouPoi);
         del_doubled3( &tmpkounum, tmpkouPoi, 1);
         if ( tmpkounum >= 2){
            mdlDialog_openInfoBox( "�P�̃R���^�[�ɒ��_���Q����܂�");
            disco = 3;
            lineString_display( &disco, &dislc, &diswt, conPoi, &connv, NORMALDRAW);
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
         lineString_display( &disco, &dislc, &diswt, tmpPoi, &tmpnv, NORMALDRAW);
      }

      /*--- �����E�J���̕\�� ---*/
      lineString_display( &disco, &dislc, &diswt, otPoi, &otnv, NORMALDRAW);

      free( Pos);

      resetFlag = 2;
      mdlState_startPrimitive( move_conter, reset, NULL, NULL);
      mdlOutput_error( "���s/���");
   }
   else{
      mdlDialog_openInfoBox( "�����E�J����I�����ĉ�����");
      startPrimitive_onetani( );
   }
}


/*-----------------------------------------------------------------------
**	�����E�J���̈ړ�
-----------------------------------------------------------------------*/
Private void  move_conter( void)
{
Dpoint3d	idou, orig, nearPoi;
ULong		*Pos;
int			num, tmpnv, ii, jj, lv, co, lc, wt;
int			disco = 0, dislc = 0, diswt = 3;
double		leng;

   Pos = malloc( 5000 * sizeof( ULong));

   /*--- �R���^�[�̃t�@�C���|�W�V���� ---*/
   for( ii=0; ii<tyoconNum; ii++){
      mdlElement_read( &ele, 0, tyoconPos[ii]);
      get_layer( &lv, &co, &lc, &wt, NULL, &ele);
      mdlLinear_extract( conPoi, &connv, &ele, 0);
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
         mdlElement_read( &ele, 0, conPos[jj]);
         set_layer( NULL, &disco, &dislc, &diswt, NULL, &ele);
         mdlElement_display( &ele, ERASE);
         mdlElement_undoableDelete( &ele, conPos[jj], ERASE);
      }
      lineString_display( &co, &lc, &wt, tmpPoi, &tmpnv, NORMALDRAW);
   }
   free( Pos);

   reset( );
}


/*-----------------------------------------------------------------------
**	�����E�J���̌���
-----------------------------------------------------------------------*/
Private int  scan_onetani( Dpoint3d *inpoint, int *otnv, Dpoint3d *otPoi, ULong *otPos, int otlv, int *otco, int *otlc, int *otwt)
{
Scanlist	scanList;
ULong		filePos;
Dvector3d	seg;
Dpoint3d	nearPoi;
int			word, fileNum, getSize, Error;

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ELEMDATA;
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.scantype |= (unsigned short)LEVELS;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
   set_lvmask( &con->otlv, scanList.levmask);

   /*--- �X�L�������s ---*/
   fileNum = 0;   filePos = 0;
   if ( mdlScan_initialize( fileNum, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( "�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
      Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( "�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( "EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( "�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( "�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

      word = mdlElement_igdsSize( &ele, 0);
      get_layer( NULL, otco, otlc, otwt, NULL, &ele);

      /*--- �X�L�����������C����ŁA���������_����ŒZ�����̓_ ---*/
      memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

      /*--- �ŒZ�����̓_��1mm�ȉ��̏ꍇ�́A�X�L�����������C���������������̂Ƃ��� ---*/
      if ( get_leng( &nearPoi, inpoint) < 1){
         mdlLinear_extract( otPoi, otnv, &ele, 0);
         *otPos = filePos - 2 * ( ULong)word;
         return( SUCCESS);
      }
   }
   return( ERROR);
}


/*-----------------------------------------------------------------------
**	�ݒ�ڲ԰�̑S�v�f�̌���
-----------------------------------------------------------------------*/
Private void  scan_all( int *num, ULong *Pos, int lv, int co, int lc, int wt)
{
Scanlist	scanList;
ULong		filePos;
int			word, fileNum, getSize, Error;
int			linco, linlc, linwt;

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ELEMDATA;
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.scantype |= (unsigned short)LEVELS;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
   set_lvmask( &lv, scanList.levmask);

   /*--- �X�L�������s ---*/
   *num = 0;   fileNum = 0;   filePos = 0;
   if ( mdlScan_initialize( fileNum, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( "�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
      Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( "�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( "EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( "�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( "�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

      word = mdlElement_igdsSize( &ele, 0);
      get_layer( NULL, &linco, &linlc, &linwt, NULL, &ele);

      if ( linco==co && linlc==lc && linwt==wt){
         Pos[*num] = filePos - 2 * (ULong)word;
         *num = *num + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	���_�ƌ������������g�𔭐�������
-----------------------------------------------------------------------*/
Private void  fence_kousa( int kounv, DPoint3d *kouPoi)
{
Dpoint3d	fPoi[101], *tmpPoi, center, delta1, delta2;
ULong		fileMask[8];
short		viewList[8], oriLvMask[4];
int			ii, actlv, tmpnv;
double		zoomFactor;

   /*--- �t�F���X�����d�Ȃ�ݒ� ---*/
   set_fence_overlap( );

   /*--- ��w�\���� ---*/
   mdlParams_getActive( &actlv, ACTIVEPARAM_LEVEL);
   mdlView_getLevels( oriLvMask, tcb->lstvw);

   /*--- �t�B�b�g ---*/
   mdlView_getParameters( NULL, &center, &delta1, NULL, NULL, 0);
   fileMask[0] = fileMask[1] = fileMask[2] = fileMask[3] = fileMask[4] = fileMask[5] = fileMask[6] = fileMask[7] = 0xFFFFFFFF;
   mdlView_fit( tcb->lstvw, fileMask);

   /*--- ���W�ۑ� ---*/
   tmpPoi = malloc( 2000 * sizeof( Dpoint3d));
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
         mdlState_startFenceCommand( scan_conter, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
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
         }
         mdlShape_create( &ele, NULL, fPoi, 2*tmpnv, 0);
         mdlFence_fromShape( &ele);

         /*--- �t�F���X������ ---*/
         mdlState_startFenceCommand( scan_conter, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
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
   mdlParams_setActive( ( int *)actlv, ACTIVEPARAM_LEVEL);
   set_view_lvmask( viewList, oriLvMask);
   mdlView_updateSingle( tcb->lstvw);

   mdlFence_clear( TRUE);
}


/*-----------------------------------------------------------------------
**	���_�Ƃ̌������ƌ�������R���^�[
-----------------------------------------------------------------------*/
Private int  scan_conter( )
{
ULong	filePos;
int		fileNum, type, lv;

   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &fileNum);

   /*--- �A�N�e�B�u�t�@�C���̂� ---*/
   if ( fileNum == 0){
      mdlElement_read( &ele, 0, filePos);
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
Private void	reset( )
{
int		disco = 0, dislc = 0, diswt = 3;

mdlView_updateSingle( tcb->lstvw);
//   lineString_display( &disco, &dislc, &diswt, kouPoi, &kounv, ERASE);

//   if ( resetFlag = 2){
//      lineString_display( &disco, &dislc, &diswt, otPoi, &otnv, ERASE);
//      lineString_display( &otco, &otlc, &otwt, otPoi, &otnv, NORMALDRAW);
//   }
   kounv = 0;
   startPrimitive_kousa( );
}
