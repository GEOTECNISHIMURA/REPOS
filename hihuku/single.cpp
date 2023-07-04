/*-----------------------------------------------------------------------*/
/*-	�핢�L����`���v���O����	-*/
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
/*-	�v���g�^�C�v	-*/
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

   /*---���\�[�X�t�@�C���̃I�[�v�� ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"HIHUKU �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   /*--- �L��ϐ��̎擾 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&hihuku",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }
   hihuku = (HIHUKU*)value.val.valPointer;

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   num = 0;
   startPrimitive( );

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
**	startPrimitive
-----------------------------------------------------------------------*/
Private  void   startPrimitive( void)
{
   /*--- ����I�� ---*/
   if ( num == 0){
      mdlState_startPrimitive( (StateFunc_DataPoint)input_point, (StateFunc_Reset)startPrimitive, NULL, NULL);
      mdlOutput_error( L"�����I�����ĉ�����");
   }

   /*--- �����I�� ---*/
   if ( hihuku->tog!=-1 && num==1){
      mdlState_startPrimitive((StateFunc_DataPoint)input_point, (StateFunc_Reset)reset, NULL, NULL);
      mdlOutput_error( L"������I�����ĉ�����");
   }

   /*--- ���F/���� ---*/
   if ( ( hihuku->tog==-1 && num==1) || num==2){
      mdlState_startPrimitive((StateFunc_DataPoint)create_hihuku, (StateFunc_Reset)reset, NULL, NULL);
      mdlOutput_error( L"���F/����");
   }
}


/*-----------------------------------------------------------------------
**	�I��������
-----------------------------------------------------------------------*/
Private  void   input_point( Dpoint3d *inpoint)
{
ULong		*Pos;

int			hinum,dislc;
UInt32      disco, diswt;
   /*--- �I��v�f�������i����j ---*/
   if ( num == 0){
      if ( scan_element_select( &tnv, tPoi, &tPos[0], inpoint, hihuku->tlv, hihuku->tco, hihuku->tlc, hihuku->twt) == ERROR){
         mdlDialog_openInfoBox( L"�����I�ђ����ĉ�����");
         startPrimitive( );
      }
      else{
         /*--- �I��v�f�Ɠ����C���[�̗v�f������ ---*/
         Pos = (ULong*)malloc( 10000 * sizeof( ULong));
         scan_element_all( &hinum, Pos, hihuku->tlv, hihuku->tco, hihuku->tlc, hihuku->twt);

         /*--- �I��v�f�̘A�������� ---*/
         tnum = 1;
         get_renzoku( hinum, Pos, &tnv, tPoi, &tnum, tPos);
         free( Pos);

         disco = 0;   dislc = 0;   diswt = 5;
         lineString_display( &disco, &dislc, &diswt, tPoi, &tnv, DRAW_MODE_Normal);
         num++;
         startPrimitive( );
      }
   }

   /*--- �I��v�f�������i�����j ---*/
   else{
      if ( scan_element_select( &bnv, bPoi, &bPos[0], inpoint, hihuku->blv, hihuku->bco, hihuku->blc, hihuku->bwt) == ERROR){
         mdlDialog_openInfoBox( L"������I�ђ����ĉ�����");
         startPrimitive( );
      }
      else{
         /*--- �I��v�f�Ɠ����C���[�̗v�f������ ---*/
         Pos = (ULong*)malloc( 10000 * sizeof( ULong));
         scan_element_all( &hinum, Pos, hihuku->blv, hihuku->bco, hihuku->blc, hihuku->bwt);

         /*--- �I��v�f�̘A�������� ---*/
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
**	�I��v�f�̌���
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

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;

   /*--- �X�L�������s ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
	  return( ERROR);
   }

   while( TRUE){
      Error = mdlScan_file( elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
	  if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return(ERROR); }


      /*--- �����A���w�b�_�͖��� ---*/
	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      type = mdlElement_getType( &ele);
      if ( type == 12){ continue;}

      get_layer( &lv, &co, &lc, &wt, NULL, &ele);

      /*--- �핢�̏ꍇ ---*/
      if ( lv==hilv && co==hico && lc==hilc && wt==hiwt){
         /*--- �X�L�����������C����ŁA���������_����ŒZ�����̓_ ---*/
         memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
         mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

         /*--- �ŒZ�����̓_��1mm�ȉ��̏ꍇ�́A�X�L�����������C���������������̂Ƃ��� ---*/
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
**	�I��v�f�Ɠ����C���[�̗v�f������
-----------------------------------------------------------------------*/
Private  void	scan_element_all( int *posnum, ULong *Pos, int hilv, int hico, int hilc, int hiwt)
{
ExtScanList	scanList;
ULong		filePos;
int			getSize, Error;
int			type, lc;
UInt32	    lv, co, wt;
ULong		elemAddr[1];

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);


   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;

   /*--- �X�L�������s ---*/
   *posnum = 0;  filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   while( TRUE){
      Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
	  if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return; }


      /*--- �����A���w�b�_�͖��� ---*/
	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      type = mdlElement_getType( &ele);
      if ( type == 12){ continue;}

      get_layer( &lv, &co, &lc, &wt, NULL, &ele);

      /*--- �I�������v�f�Ɠ����C���[�̏ꍇ ---*/
      if ( lv==hilv && co==hico && lc==hilc && wt==hiwt){
		  Pos[*posnum] = elemAddr[0];
         *posnum = *posnum + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	�핢�L������
-----------------------------------------------------------------------*/
Private  void   create_hihuku( Dpoint3d *inpoint)
{
double		xPitch, yPitch, hiLeng;
int			vernum, ii, olc;
UInt32      olv, oco, owt;

   /*--- ���̃A�N�e�B�u���C���[���擾 ---*/
   ActiveParams::GetValue(olv, ACTIVEPARAM_LEVEL);
   ActiveParams::GetValue(oco, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(owt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::GetValue(olc, ACTIVEPARAM_LINESTYLE);
   /*--- �핢�L���̃��C���[�ɂ��� ---*/
   ActiveParams::SetValue(hihuku->klv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(hihuku->kco, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(hihuku->kwt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::SetValue(hihuku->klc, ACTIVEPARAM_LINESTYLE);
   /*--- ����̉��� ---*/
   hiLeng = 0;
   for( ii=0; ii<tnv-1; ii++){
      hiLeng += get_leng( &tPoi[ii+1], &tPoi[ii]);
   }

   /*--- �����̊Ԋu ---*/
   if ( hihuku->scale ==   500){ xPitch = yPitch = 2120/2;}
   if ( hihuku->scale ==  1000){ xPitch = yPitch = 4240/2;}
   if ( hihuku->scale ==  2500){ xPitch =  5000/2;   yPitch = 3750;}
   if ( hihuku->scale == 10000){ xPitch = 10000/2;   yPitch = 5000;}

   /*--- �����̖{�� ---*/
   vernum = hiLeng / xPitch;

   if ( hihuku->tog != -1){ create_hihuku_multi( xPitch, yPitch, hiLeng, vernum);}
   else{                    create_hihuku_single( inpoint, xPitch, yPitch, hiLeng, vernum);}

   /*--- ���̃A�N�e�B�u���C���[�ɖ߂� ---*/
   ActiveParams::SetValue(olv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(oco, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(owt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::SetValue(olc, ACTIVEPARAM_LINESTYLE);

   reset( );
}


/*-----------------------------------------------------------------------
**	�핢�L���i������j����
-----------------------------------------------------------------------*/
Private  void   create_hihuku_multi( double	xPitch, double yPitch, double hiLeng, int vernum)
{
Dpoint3d	tmpPoi[2], sPoi[8], verPoi[2], kouPoi[50], ssPoi[8], scale, orig;
int			flag, ii, jj, kk, iti, knum, vernv, snv, kounum, minnum, celnum;
double		verLeng, rad, minleng, leng;

   vernv = 2;
   scale.x = scale.y = scale.z = 1;
   snv = 5;

   /*--- ���� ---*/
   celnum = 0;
   for( ii=1; ii<vernum; ii++){
      /*--- �����̏�����̓_ ---*/
      get_point_along_line( ii*xPitch, tnv, tPoi, &iti, &verPoi[0]);
      verPoi[0].z = 0;

      /*--- ����̕��� ---*/
      rad = atan2( tPoi[iti+1].y - tPoi[iti].y, tPoi[iti+1].x - tPoi[iti].x);

      /*--- ���̐��� ---*/
      tmpPoi[0].x = verPoi[0].x + 100000 * cos( rad + fc_piover2);
      tmpPoi[0].y = verPoi[0].y + 100000 * sin( rad + fc_piover2);
      tmpPoi[0].z = 0;
      tmpPoi[1].x = verPoi[0].x - 100000 * cos( rad + fc_piover2);
      tmpPoi[1].y = verPoi[0].y - 100000 * sin( rad + fc_piover2);
      tmpPoi[1].z = 0;

      /*--- ���̐����Ɖ����Ƃ̌�_ ---*/
      kounum = judge_kouten_all( &vernv, tmpPoi, &bnv, bPoi, kouPoi);
      if ( kounum == 0){ continue;}

      /*--- ���̐����Ɖ����Ƃ̌�_�̂�������Ɉ�ԋ߂����� ---*/
      minleng = 100000;
      for( jj=0; jj<kounum; jj++){
         leng = get_leng( &verPoi[0], &kouPoi[jj]);
         if ( leng < minleng){
            minleng = leng;
            minnum = jj;
         }
      }

      /*--- �����̉������̓_ ---*/
      memcpy( &verPoi[1], &kouPoi[minnum], sizeof( Dpoint3d));
//lineString_add( NULL, &ii, NULL, NULL, NULL, verPoi, &vernv);

      /*--- �����̉��� ---*/
      verLeng = get_leng( &verPoi[0], &verPoi[1]);

      /*--- �Z���ꍇ�͖��� ---*/
      if (  ( hihuku->scale==  500 && verLeng<=1210)
         || ( hihuku->scale== 1000 && verLeng<=2420)
         || ( hihuku->scale==10000 && verLeng<=5000)){
         continue;
      }

      /*--- �핢�L���̌� ---*/
      knum = verLeng / yPitch;

      /*--- �����̕��� ---*/
      rad = atan2( verPoi[1].y-verPoi[0].y, verPoi[1].x-verPoi[0].x);

      /*--- ������牺���ֈ��������̊Ԋu ---*/
      for( jj=0; jj<knum; jj++){
         if ( ii%2 == jj%2){ continue;}

         orig.x = verPoi[0].x + ( jj+1) * yPitch * cos( rad);
         orig.y = verPoi[0].y + ( jj+1) * yPitch * sin( rad);

         /*--- �Z���w�b�_�͈̔͂��擾 ---*/
         mdlCell_getElmDscr( &cellDP, NULL, &orig, &scale, NULL, NULL, NULL, 0, 0,NULL, hihuku->cell,NULL);
         mdlCell_extract( NULL, sPoi, NULL, NULL, NULL, NULL, &cellDP->el);
         mdlElmdscr_freeAll( &cellDP);

         /*--- �Z���Ə���Ƃ̌�_������ꍇ�͖��� ---*/
         if ( judge_kouten_all( &tnv, tPoi, &snv, sPoi, kouPoi) > 0){ continue;}

         /*--- �Z���Ɖ����Ƃ̌�_������ꍇ�͖��� ---*/
         if ( judge_kouten_all( &bnv, bPoi, &snv, sPoi, kouPoi) > 0){ continue;}

         /*--- �ȑO�ɔz�u�����Z���Ƃ̌�_������Ζ��� ---*/
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

         /*--- �����Əd�Ȃ�Ȃ��ꍇ�̓Z���z�u ---*/
         celPos[celnum] = mdlCell_placeCell( &orig, &scale, NULL, NULL, NULL, 0, TRUE, hihuku->klv, 0, hihuku->cell,NULL);
         celnum++;
      }
   }
}


/*-----------------------------------------------------------------------
**	�핢�L���i�P��̂݁j����
-----------------------------------------------------------------------*/
Private void create_hihuku_single(Dpoint3d *inpoint, double xPitch, double yPitch, double hiLeng, int vernum)
{
	Dpoint3d	scale, verPoi[2], sPoi[8], ssPoi[8], kouPoi[50];
	int			ii, jj, dir, iti, snv, vernv, celnum, flag;
	double		rad;

	scale.x = scale.y = scale.z = 1;
	snv = 5;   vernv = 2;

	/*--- ���F�{�^�����������ʒu������̍��E�ǂ��瑤�� ---*/
	if (judge_point_lr_lstring(inpoint, tnv, tPoi) == 1) { dir = 1; }
	else { dir = -1; }

	/*--- ���� ---*/
	celnum = 0;
	for (ii = 1; ii <= vernum; ii++) {
		if (ii % 2 == 0) { continue; }

		/*--- �����̏�����̓_ ---*/
		get_point_along_line(ii*xPitch, tnv, tPoi, &iti, &verPoi[0]);
		verPoi[0].z = 0;

		/*--- ����̕��� ---*/
		rad = atan2(tPoi[iti + 1].y - tPoi[iti].y, tPoi[iti + 1].x - tPoi[iti].x);

		/*--- �Z���̈ʒu ---*/
		verPoi[1].x = verPoi[0].x + yPitch * cos(rad + dir * fc_piover2);
		verPoi[1].y = verPoi[0].y + yPitch * sin(rad + dir * fc_piover2);
		verPoi[1].z = 0;
		//lineString_add( NULL, &ii, NULL, NULL, NULL, verPoi, &vernv);

			  /*--- �Z���w�b�_�͈̔͂��擾 ---*/
		mdlCell_getElmDscr(&cellDP, NULL, &verPoi[1], &scale, NULL, NULL, NULL, 0, 0, NULL, hihuku->cell, NULL);
		mdlCell_extract(NULL, sPoi, NULL, NULL, NULL, NULL, &cellDP->el);
		mdlElmdscr_freeAll(&cellDP);

		/*--- �Z���Ə���Ƃ̌�_������ꍇ�͖��� ---*/
		if (judge_kouten_all(&tnv, tPoi, &snv, sPoi, kouPoi) > 0) { continue; }

		/*--- �ȑO�ɔz�u�����Z���Ƃ̌�_������Ζ��� ---*/
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

		/*--- �Z���z�u ---*/
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

   /*--- ����I����̏ꍇ ---*/
   for( ii=0; ii<tnum; ii++){
      /*--- �������� ---*/
      mdlElement_read( &ele, MASTERFILE, tPos[ii]);
      co = 0;   lc = 0;   wt = 5;
      set_layer( NULL, &co, &lc, &wt, NULL, &ele);
      mdlElement_display( &ele, DRAW_MODE_Erase);

      /*--- �ĕ\�� ---*/
      mdlElement_read( &ele, MASTERFILE, tPos[ii]);
      mdlElement_display( &ele, DRAW_MODE_Normal);
   }

   /*--- �����I����̏ꍇ ---*/
   if ( num == 2){
      for( ii=0; ii<bnum; ii++){
         /*--- �������� ---*/
         mdlElement_read( &ele, MASTERFILE, bPos[ii]);
         co = 0;   lc = 0;   wt = 5;
         set_layer( NULL, &co, &lc, &wt, NULL, &ele);
         mdlElement_display( &ele, DRAW_MODE_Erase);

         /*--- �ĕ\�� ---*/
         mdlElement_read( &ele, MASTERFILE, bPos[ii]);
         mdlElement_display( &ele, DRAW_MODE_Normal);
      }
   }

   num = 0;
   startPrimitive( );
}


