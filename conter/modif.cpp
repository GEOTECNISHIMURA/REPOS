/*-----------------------------------------------------------------------*/
/*-	        MODIF.MC		-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include "../CommonCE/naka8.cpp"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "conter.h"


#define OPEN  			0
#define CLOSE			1
#define KITEN  			0
#define SYUTEN			1
#define DOUBLE_CLICK	0
#define SETUZOKU		1
#define SAME  			0
#define DIFFERENT		1

RscFileHandle   Rsc;
MSElement       ele, ele2, ele3;
MSBsplineCurve	curve;
MSElementDescr	*elmDP1, *elmDP2;
int				exitStatus;

int				sNum,  eNum;
int				siti,  eiti;
double			sleng, eleng;

Dpoint3d		newPoi[100000];
int	        	newnv;

ULong			linPos[100000];						/* �ŏ��ɉ����������C���Ɠ������C���[�̗v�f	*/
int				linnum;								/* �ŏ��ɉ����������C���Ɠ������C���[�̗v�f	*/

Dpoint3d		oPoi[2][100000];						/* ���̃��C���̍��W 					*/
Dpoint3d        tmp_oPoi[100000];
int             tmp_onv;
ULong			oPos[2][10000];						/* ���̃��C���̃t�@�C���ʒu 			*/
int	        	olc, onv[2], onum[2];/* ���̃��C���̃��C���[�A�_���A�v�f�� 	*/
UInt32          ggnum, olv, oco, owt;
int 			catnv;		 						/* �C�����̓_�� */
Dpoint3d		catPoi[100000], tmpPoi[100000];		/* �C�����̍��W�i�\���p�j */

double			time1, time2;						/* MicroStation�N����̎��� */

CONTER			*con;


/*-	�v���g�^�C�v	-*/
bool    unload(UnloadProgramReason unloadType);
void	startPrimitive( void);
void	get_conter_info( Dpoint3d *inpoint, int view);
//int		scan_conter( Dpoint3d *inpoint, int *nv, Dpoint3d *Poi, int *num, ULong *Pos, int *iti, int kisyu);
int		scan_conter_a(Dpoint3d *inpoint, int *iti, int kisyu);
void	scan_line( UInt32 olv, UInt32 oco, int olc, UInt32 owt, int *linnum, ULong *linPos);
void	setFunction( void);

void	place_conter( Dpoint3d *inpoint, int view);
void	conter_sakusei( int flag);

void	place_conter_open( int close);
void	place_conter_close( void);
void	place_conter_different( void);

void	bspln_add( int flag);
//int	convertCrvFunc( MSElement *header, void *params, int fileNum, MSElementDescr *edP, MSElementDescr **newEdPP);
void	computePointsByTolerance( Dpoint3d **lPoi, int *nv, MSBsplineCurve *curveP, double	tolerance);
void	computeMinDistToCurve( Dpoint3d	*poi, double *maxDeviationP, int nv, MSBsplineCurve *curveP);

void	exchange_syusei( void);
void	delete_orig( int num, ULong *Pos, int nv, Dpoint3d *Poi, int iti, Dpoint3d *start);
void	syusei_z( void);

void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode);
void	display( int drawMode);
void	bspln_undo( void);
void	cleanup( void);
void	get_hirei_z( Dpoint3d *Poi, Dpoint3d *Poi1, Dpoint3d *Poi2);
int		judge_conter( int lnv, Dpoint3d *lPoi);

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

   /*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox(L"CONTER �̃��\�[�X�t�@�C�����J���܂���");
      return;
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �L��ϐ��̎擾 ---*/
   if ( mdlCExpression_getValue( &value, &result, "&con",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
      return;
   }
   con = (CONTER*)value.val.valPointer;
   catnv = 0;   time1 = 0;

   startPrimitive( );

   return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool    unload(UnloadProgramReason unloadType)
{
	mdlTransient_free(&msTransientElmP, true);
	mdlState_clear();
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	startPrimitive
-----------------------------------------------------------------------*/
void	startPrimitive( void)
{
   mdlTransient_free(&msTransientElmP, true);
   mdlState_startPrimitive( (StateFunc_DataPoint)get_conter_info, (StateFunc_Reset)startPrimitive, NULL, NULL);
   mdlOutput_error(L"�C�����J�n����ʒu���������ĉ�����");
}


/*-----------------------------------------------------------------------
**	�R���^�[�̌���
-----------------------------------------------------------------------*/
void	get_conter_info( Dpoint3d *inpoint, int view)
{
   //scan_conter( inpoint, &onv[0], oPoi[0], &onum[0], oPos[0], &siti, KITEN);
   scan_conter_a(inpoint, &siti, KITEN);
}


/*-----------------------------------------------------------------------
**	�R���^�[�̌���
-----------------------------------------------------------------------*/
/*int		scan_conter( Dpoint3d *inpoint,int *nv, Dpoint3d *Poi, int *num, ULong *Pos, int *iti, int kisyu)
{
ExtScanList	scanList;
Dpoint3d	nearPoi;
DSegment3d	seg;
ULong		filePos, elemAddr[1];
int			flag, word, getSize, Error;
int			ii, lc;
UInt32      lv, co, wt;

   if ( kisyu == KITEN){
      memcpy( &catPoi[0], inpoint, sizeof( Dpoint3d));
   }

   ///*--- �X�L�������X�g�̏����� ---
   mdlScan_initScanlist( &scanList);

   ///*--- �����͈͂̐ݒ� ---
   mdlScan_noRangeCheck( &scanList);


   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   ///*--- �X�L�������s ---
   filePos = 0;   flag = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
	  return(ERROR);
   }

   while( TRUE){
      Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
	  if ( Error >= 65) { mdlSystem_exit(exitStatus, 1);return(ERROR); }

	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);

      /*--- �I�_���̏ꍇ�̓��C���[�`�F�b�N ---
      if ( kisyu == SYUTEN){
         get_layer( &lv, &co, &lc, &wt, NULL, &ele);
         if ( olv!=lv || oco!=co || olc!=lc || owt!=wt){
            continue;
         }
      }

      /*--- �X�L�����������C����ŁA���������_����ŒZ�����̓_ ---
      memcpy( &nearPoi, inpoint, sizeof( Dpoint3d));
      mdlLinear_getClosestSegment( &seg, &nearPoi, &ele);

      /*--- �ŒZ�����̓_��1mm�ȉ��̏ꍇ�́A�X�L�����������C���������������̂Ƃ��� ---
      if ( get_leng( &nearPoi, inpoint) < 1){
         if ( kisyu == KITEN){
            get_layer( &olv, &oco, &olc, &owt, &ggnum, &ele);
         }
         mdlLinear_extract(tmp_oPoi, &tmp_onv, &ele, MASTERFILE);
         memcpy(Poi, tmp_oPoi, tmp_onv * sizeof(Dpoint3d));
         Pos[0] = elemAddr[0];
         flag++;
         break;
      }
   }

   /*--- �v�f��������Ȃ������ꍇ ---
   if ( flag == 0){
      if ( kisyu == KITEN){
         mdlDialog_openInfoBox(L"�I�ђ����ĉ�����");
         startPrimitive( );
      }
      return( ERROR);
   }

   /*--- �v�f�����������ꍇ ---
   else{
      if ( kisyu == KITEN){ catnv = 1;}
      linnum = 0;
      scan_line( olv, oco, olc, owt, &linnum, linPos);

      /*--- �����������C���̘A�������� ---
      *num = 1;
      get_renzoku( linnum, linPos, &tmp_onv, tmp_oPoi, num, Pos);

      /*--- ���������ʒu ---
      get_closest_point(&tmp_onv, tmp_oPoi, inpoint, &nearPoi, iti, &sleng);

      memcpy(Poi, tmp_oPoi, tmp_onv * sizeof(Dpoint3d));
	  if (kisyu == KITEN) { onv[0] = tmp_onv; }
	  else{ onv[1] = tmp_onv;}
      setFunction( );

      return( SUCCESS);
   }
}*/


/*-----------------------------------------------------------------------
**	�����������C���Ɠ����C���[�̃��C��������
------------------------------------------------------------------------*/
 void  scan_line( UInt32 olv, UInt32 oco, int olc, UInt32 owt, int *linnum, ULong *linPos)
{
ExtScanList	scanList;
ULong		filePos, elemAddr[1];
int			word, fileNum, getSize, Error;
UInt32		lv, co, wt;
int         lc;

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   /*--- �������x���̐ݒ� ---*/
   if ( olv != -1){
      scanList.scantype |= (unsigned short)LEVELS;

      scanList.levmask[0] = scanList.levmask[1] = scanList.levmask[2] = scanList.levmask[3] = 0;
      set_lvmask( &olv, (Short*)scanList.levmask);
   }

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

   /*--- �X�L�������s ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   while( TRUE){
      Error = mdlScan_file( elemAddr, &getSize, sizeof(elemAddr), &filePos);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
	  if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return; }

	  mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
      get_layer( &lv, &co, &lc, &wt, NULL, &ele);
      if ( ( olv==-1 || lv==olv) && ( oco==-1 || co==oco) && ( olc==-1 || lc==olc) && ( owt==-1 || wt==owt)){
         linPos[*linnum] = elemAddr[0];
         *linnum = *linnum + 1;
      }
   }
}


/*-----------------------------------------------------------------------
**	setFunction
-----------------------------------------------------------------------*/
void	setFunction( void)
{
   mdlTransient_free(&msTransientElmP, true);
   StateCallback::SetComplexDynamicsFunction((StateFunc_ComplexDynamics)complexDynamics);
   StateCallback::SetDataPointFunction((StateFunc_DataPoint)place_conter);
   StateCallback::SetResetFunction((StateFunc_Reset)bspln_undo);
   StateCallback::SetCommandCleanupFunction((StateFunc_CommandCleanup)cleanup);

   mdlOutput_command( L"�_�u���N���b�N�ŏI��");
   mdlOutput_error(L"����/���");
}


/*-----------------------------------------------------------------------
**	�C�����郉�C���̂Q�_�ڈȍ~�����͂��ꂽ��
-----------------------------------------------------------------------*/
void	place_conter( Dpoint3d *inpoint, int view)
{
Dpoint3d	nearPoi;
double		leng;

   memcpy( &catPoi[catnv], inpoint, sizeof( Dpoint3d));
   catnv++;

   /*--- ���͂����_�����̃��C����ɂ���ꍇ ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[catnv-1], &nearPoi, &eiti, &eleng);
   leng = get_leng( &catPoi[catnv-1], &nearPoi);


   if ( leng < 1){
      onv[1] = onv[0];
      memcpy( oPoi[1], oPoi[0], onv[0] * sizeof( Dpoint3d));
      onum[1] = onum[0];
      memcpy( oPos[1], oPos[0], onum[0] * sizeof( ULong));
      conter_sakusei( SAME);

      catnv = 0;
      startPrimitive( );
   }

   /*--- ���͂����_�����̃��C����ɂȂ��ꍇ ---*/
   else{
      /*--- ���̃��C���Ɠ����C���[�̂��̂����� ---*/
      linnum = 0;
      //if ( scan_conter( inpoint, &onv[1], oPoi[1], &onum[1], oPos[1], &eiti, SYUTEN) == SUCCESS){
	  if (scan_conter_a(inpoint, &eiti, SYUTEN) == SUCCESS) {
         conter_sakusei( DIFFERENT);

         catnv = 0;

         startPrimitive( );
      }

      /*--- ���͂����_�����̃��C���Ɠ����C���[�̃��C����ɂȂ��ꍇ ---*/
      else{
         /*--- MicroStation �N����̌o�ߎ��� ---*/

		  time2 = mdlSystem_getTicks();
         /*--- �_�u���N���b�N�̏ꍇ ---*/
         if ( time2 - time1 < 12){
            catnv--;
            /*--- �C�������̍��� ---*/
            syusei_z( );

            /*--- ���̃��C���̂����̒Z�������폜 ---*/
            delete_orig( onum[0], oPos[0], onv[0], oPoi[0], siti, &catPoi[0]);

            /*--- B-�X�v���C���̔z�u ---*/
            bspln_add( DOUBLE_CLICK);

            catnv = 0;
            startPrimitive( );
         }
         time1 = time2;
      }
   }
}


/*-----------------------------------------------------------------------
**	�R���^�[�̍쐬
-----------------------------------------------------------------------*/
void	conter_sakusei( int flag)
{
   /*--- siti ���N�_���ɂ��� ---*/
   if ( flag == SAME){   exchange_syusei( );}



   /*--- �ʃ��C���̏ꍇ ---*/
   if ( flag == DIFFERENT){
      place_conter_different( );
   }
   else{
      /*--- �N�I�_��v�̏ꍇ ---*/
      if ( memcmp( &oPoi[0][0], &oPoi[0][onv[0]-1], sizeof( Dpoint3d)) == 0){
         place_conter_close( );
      }

      /*--- �N�I�_��v���Ȃ��ꍇ ---*/
      else{
         place_conter_open( OPEN);
      }
   }
}


/*-----------------------------------------------------------------------
**	�ʃ��C���̏ꍇ
-----------------------------------------------------------------------*/
void	place_conter_different( )
{
Dpoint3d	nearPoi;
int			iti;
double		leng;

   /*--- �C�����N�_�̍��� ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[0], &nearPoi, &iti, &leng);
   catPoi[0].z = oPoi[0][iti].z 
               + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[0])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- �N�_�����C���폜 ---*/
   delete_orig( onum[0], oPos[0], onv[0], oPoi[0], siti, &catPoi[0]);

   /*--- �C�����I�_�̍��� ---*/
   get_closest_point( &onv[1], oPoi[1], &catPoi[catnv-1], &nearPoi, &iti, &leng);
   catPoi[catnv-1].z = oPoi[1][iti].z 
                     + ( oPoi[1][iti+1].z - oPoi[1][iti].z)*get_leng( &oPoi[1][iti], &catPoi[catnv-1])/get_leng( &oPoi[1][iti], &oPoi[1][iti+1]);

   /*--- �I�_�����C���폜 ---*/
   delete_orig( onum[1], oPos[1], onv[1], oPoi[1], eiti, &catPoi[catnv-1]);

   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	�J���Ă���ꍇ
-----------------------------------------------------------------------*/
void	place_conter_open( int close)
{
Dpoint3d	nearPoi;
int			ii, iti;
double		leng;

   /*--- �폜 ---*/
   for( ii=0; ii<onum[0]; ii++){
      mdlElement_read( &ele, MASTERFILE, oPos[0][ii]);
      mdlElement_undoableDelete( &ele, oPos[0][ii], TRUE);
   }

   /*--- �C�����N�_�̍��� ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[0], &nearPoi, &iti, &leng);
   catPoi[0].z = oPoi[0][iti].z 
               + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[0])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- �C����������O ---*/
   for( ii=0; ii<=siti; ii++){
      memcpy( &newPoi[ii], &oPoi[0][ii], sizeof( Dpoint3d));
   }
   memcpy( &newPoi[siti+1], &catPoi[0], sizeof( Dpoint3d));
   newnv = siti + 2;
   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- �C�����I�_�̍��� ---*/
   get_closest_point( &onv[0], oPoi[0], &catPoi[catnv-1], &nearPoi, &iti, &leng);
   catPoi[catnv-1].z = oPoi[0][iti].z 
                     + ( oPoi[0][iti+1].z - oPoi[0][iti].z)*get_leng( &oPoi[0][iti], &catPoi[catnv-1])/get_leng( &oPoi[0][iti], &oPoi[0][iti+1]);

   /*--- �C���������� ---*/
   memcpy( &newPoi[0], &catPoi[catnv-1], sizeof( Dpoint3d));
   for( ii=eiti+1; ii<onv[0]; ii++){
      memcpy( &newPoi[ii-eiti], &oPoi[0][ii], sizeof( Dpoint3d));
   }
   newnv = onv[0] - eiti;
   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- �C������ ---*/
   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	���Ă���ꍇ
-----------------------------------------------------------------------*/
void	place_conter_close( void)
{
//Dpoint3d	Poi[101], nearPoi;
//ULong		tmpPos[1000];
int			ii, nv, iti, slv, sco, slc, swt, elv, eco, elc, ewt;
double		leng1, leng2;

   /*--- �폜 ---*/
   for( ii=0; ii<onum[0]; ii++){
      mdlElement_read( &ele, MASTERFILE, oPos[0][ii]);
      mdlElement_undoableDelete( &ele, oPos[0][ii], TRUE);
   }

   leng1 = 0;   leng2 = 0;
   /*--- �N�I�_�o�R���Ȃ����� ---*/
   leng1 = get_leng( &oPoi[0][siti], &oPoi[0][siti+1]) - sleng;
   for( ii=siti+1; ii<eiti; ii++){
      leng1 = leng1 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   leng1 = leng1 + eleng;

   /*--- �N�I�_�o�R�̋��� ---*/
   leng2 = get_leng( &oPoi[0][eiti], &oPoi[0][eiti+1]) - eleng;
   for( ii=eiti+1; ii<onv[0]-1; ii++){
      leng2 = leng2 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   for( ii=0; ii<siti; ii++){
      leng2 = leng2 + get_leng( &oPoi[0][ii], &oPoi[0][ii+1]);
   }
   leng2 = leng2 + sleng;

   /*--- ���̕��� ---*/
   /*--- �C���������N�I�_�o�R����ꍇ ---*/
   if ( leng2 < leng1){
      memcpy( &newPoi[0], &catPoi[0], sizeof( Dpoint3d));
      for( ii=siti+1; ii<=eiti; ii++){
         memcpy( &newPoi[ii - siti], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      memcpy( &newPoi[eiti - siti + 1], &catPoi[catnv-1], sizeof( Dpoint3d));
      newnv = eiti - siti + 2;
   }

   /*--- �C���������N�I�_�o�R���Ȃ��ꍇ ---*/
   else{
      memcpy( &newPoi[0], &catPoi[catnv-1], sizeof( Dpoint3d));
      for( ii=eiti+1; ii<onv[0]; ii++){
         memcpy( &newPoi[ii - eiti], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      newnv = onv[0] - eiti;
      for( ii=0; ii<=siti; ii++){
         memcpy( &newPoi[newnv + ii], &oPoi[0][ii], sizeof( Dpoint3d));
      }
      memcpy( &newPoi[newnv + siti + 1], &catPoi[0], sizeof( Dpoint3d));
      newnv = newnv + siti + 2;
   }

   del_doubled3( &newnv, newPoi, 1);
   if ( newnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, newPoi, &newnv);}

   /*--- �C������ ---*/
   bspln_add( SETUZOKU);
}


/*-----------------------------------------------------------------------
**	B-�X�v���C���̔z�u
-----------------------------------------------------------------------*/
void	bspln_add( int flag)
{
MSElement	ele;
Dpoint3d	*lPoi, nearPoi;
int			ii, nearnum, remainNv;
double		leng;

   /*--- �_�u���N���b�N�����ꍇ ---*/
   if ( flag == DOUBLE_CLICK){ catnv--;}

   /*--- 2�_�ڈȑO�Ƀ_�u���N���b�N�����ꍇ ---*/
   if ( catnv == 1){
      catnv++;
      /*--- �\�������� ---*/
      display( DRAW_MODE_Erase);
      mdlElmdscr_freeAll( &elmDP2);
   }

   /*--- 2�_�ڈȍ~�Ƀ_�u���N���b�N�A�܂��́A�ڑ��ŏI�������ꍇ ---*/
   else if ( catnv >= 2){
      /*--- ��������\�������� ---*/
      display( TEMPERASE);
      mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Erase);

      /*--- �����̐ݒ� ---*/
      for( ii=0; ii<catnv; ii++){
         catPoi[ii].z = catPoi[0].z;
      }

/*--- B-�X�v���C���̏ꍇ ---*/
if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- �v�f�L�q�q�p�������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP2);
      mdlElmdscr_freeAll( &elmDP1);

      /*--- �������̊��蓖�� ---*/
      lPoi = (DPoint3d*)malloc( 10000 * sizeof( Dpoint3d));

      /*--- �ϊ���̊e�_�̍��W���v�Z ---*/
      computePointsByTolerance( &lPoi, &remainNv, &curve, 1000 * con->genkou);

      /*--- �������łȂ��ꍇ ---*/
      if ( judge_conter( onv[0], oPoi[0]) == FALSE){
         /*--- �C�������̍��� ---*/
         for( ii=0; ii<remainNv; ii++){
            /*--- �C�������̓_����̍ŒZ���������߂� ---*/
            get_closest_point( &onv[0], oPoi[0], &lPoi[ii], &nearPoi, &nearnum, &leng);

            /*--- ���z���ɂ�荂�������߂� ---*/
            if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
               && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
               lPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
            }
            else{
               get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
               lPoi[ii].z = nearPoi.z;
            }
         }
      }

      /*--- �C�������z�u ---*/
      del_doubled3( &remainNv, lPoi, 1);
      if ( remainNv >= 2){lineString_add( &olv, &oco, &olc, &owt, &ggnum, lPoi, &remainNv);}

      /*--- �������̊J�� ---*/
      mdlBspline_freeCurve( &curve);
      free( lPoi);
}
/*--- ���C���̏ꍇ ---*/
else{
      del_doubled3( &catnv, catPoi, 1);

      /*--- �C�������̍��� ---*/
      for( ii=0; ii<catnv; ii++){
         /*--- �C�������̓_����̍ŒZ���������߂� ---*/
         get_closest_point( &onv[0], oPoi[0], &catPoi[ii], &nearPoi, &nearnum, &leng);

         /*--- ���z���ɂ�荂�������߂� ---*/
         if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
            && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
            catPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
         }
         else{
            get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
            catPoi[ii].z = nearPoi.z;
         }
      }
      if ( catnv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, catPoi, &catnv);}
}
   }

   /*--- �������̊J�� ---*/
   mdlBspline_freeCurve( &curve);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);
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


/*-----------------------------------------------------------------------
**	�C�������̓���ւ�
-----------------------------------------------------------------------*/
void	exchange_syusei( )
{
int		ii;

   if ( siti>eiti || ( siti==eiti && sleng > eleng)){
      exchangei( &siti, &eiti);
      exchanged( &sleng, &eleng);

      for( ii=0; ii<catnv; ii++){
         memcpy( &tmpPoi[ii], &catPoi[ii], sizeof( Dpoint3d));
      }
      for( ii=0; ii<catnv; ii++){
         memcpy( &catPoi[ii], &tmpPoi[catnv-1-ii], sizeof( Dpoint3d));
      }
   }
}


/*----------------------------------------------------------------------+
**	���̃��C�����폜����
+----------------------------------------------------------------------*/
void	delete_orig( int num, ULong *Pos, int nv, Dpoint3d *Poi, int iti, Dpoint3d *start)
{
double		leng1, leng2;
int			ii;

   /*--- ���̃��C�����폜 ---*/
   for( ii=0; ii<num; ii++){
      mdlElement_read( &ele, MASTERFILE, Pos[ii]);
      mdlElement_undoableDelete( &ele, Pos[ii], TRUE);
   }

   leng1 = leng2 = 0;
   /*--- ���������_���N�_�� ---*/
   for( ii=0; ii<iti; ii++){
      leng1 += get_leng( &Poi[ii], &Poi[ii+1]);
   }
   leng1 += get_leng( &Poi[iti], start);

   /*--- ���������_���I�_�� ---*/
   leng2 += get_leng( &Poi[iti+1], start);
   for( ii=iti+1; ii<nv-1; ii++){
      leng2 += get_leng( &Poi[ii], &Poi[ii+1]);
   }
   /*--- �N�_���������ꍇ ---*/
   if ( leng1 > leng2){
      memcpy( &Poi[iti+1], start, sizeof( Dpoint3d));
      nv = iti + 2;
      del_doubled3( &nv, Poi, 1);

      if ( nv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, Poi, &nv);}
   }

   /*--- �I�_���������ꍇ ---*/
   else{
      for( ii=0; ii<nv; ii++){
         memcpy( &tmpPoi[ii], &Poi[ii], sizeof( Dpoint3d));
      }
      for( ii=0; ii<nv-iti-1; ii++){
         memcpy( &Poi[ii], &tmpPoi[nv-1-ii], sizeof( Dpoint3d));
      }
      memcpy( &Poi[nv - iti - 1], start, sizeof( Dpoint3d));
      nv = nv - iti;
      del_doubled3( &nv, Poi, 1);

      if ( nv >= 2){ lineString_add( &olv, &oco, &olc, &owt, &ggnum, Poi, &nv);}
   }
}


/*----------------------------------------------------------------------+
**	�C�������ɍ�����^����
+----------------------------------------------------------------------*/
void	syusei_z( void)
{
Dpoint3d	nearPoi;
int			ii, nearnum;
double		leng;

   /*--- �C�������̍��� ---*/
   for( ii=0; ii<catnv; ii++){
      /*--- �C�������̓_����̍ŒZ���������߂� ---*/
      get_closest_point( &onv[0], oPoi[0], &catPoi[ii], &nearPoi, &nearnum, &leng);

      /*--- ���z���ɂ�荂�������߂� ---*/
      if (  oPoi[0][nearnum].x == oPoi[0][nearnum+1].x
         && oPoi[0][nearnum].y == oPoi[0][nearnum+1].y){
         catPoi[ii].z = ( oPoi[0][nearnum].z + oPoi[0][nearnum+1].z)/2;
      }
      else{
         get_hirei_z( &nearPoi, &oPoi[0][nearnum], &oPoi[0][nearnum+1]);
         catPoi[ii].z = nearPoi.z;
      }
   }
}


/*-----------------------------------------------------------------------
**	�߂�
-----------------------------------------------------------------------*/
void	bspln_undo( )
{
   /*--- 2�_�ړ��͂̑O ---*/
   if ( catnv == 1){
      /*--- �\�������� ---*/
      display(DRAW_MODE_Erase);
      mdlState_clear( );
      catnv = 0;
      startPrimitive( );
   }

   /*--- 2�_�ڈȍ~ ---*/
   if ( catnv >= 2){
      /*--- �\�������� ---*/
      lineString_display( NULL, NULL, NULL, catPoi, &catnv, DRAW_MODE_Erase);

      /*--- �P�_�߂� ---*/
      catnv--;
   }
}


/*-----------------------------------------------------------------------
**	�N���[���A�b�v
-----------------------------------------------------------------------*/
void	cleanup( )
{
   /*--- �C�������̕\���̏��� ---*/
   catnv++;

   /*--- B-�X�v���C���g�p�̏ꍇ ---*/
   if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

      /*--- �\���̏�񂩂�v�f�L�q�q�쐬 ---*/
      mdlBspline_createCurve( &elmDP1, NULL, &curve);

      /*--- ���O��B-�X�v���C���̕\�������� ---*/
      mdlElmdscr_display( elmDP1, MASTERFILE, DRAW_MODE_Erase);
	  
      /*--- B-�X�v���C���p�������̊J�� ---*/
      mdlBspline_freeCurve( &curve);

      /*--- �v�f�L�q�q�p�������̊J�� ---*/
      mdlElmdscr_freeAll( &elmDP1);
   }
   else{
      lineString_display( NULL, NULL, NULL, catPoi, &catnv, DRAW_MODE_Erase);
   }

   catnv = 0;
   startPrimitive( );
}


/*-----------------------------------------------------------------------
**	���o�[�o���h�\��
-----------------------------------------------------------------------*/
void	complexDynamics( Dpoint3d *inpoint, int view, int drawMode)
{
   memcpy( &catPoi[catnv], inpoint, sizeof( Dpoint3d));
   catnv++;

   /*--- �Q�_�ڈȍ~�̏ꍇ�́AB-�X�v���C����\������ ---*/
   if ( catnv >= 2){
      /*--- �\�� ---*/
      display(DRAW_MODE_TempDraw);

   }
   catnv--;
}


/*-----------------------------------------------------------------------
**	�\��
-----------------------------------------------------------------------*/
void	display( int drawMode)
{
MSElement	u;
Dpoint3d	*tmpPoi;
int			ii, tmpnv;
UInt32      lv, co, wt;
int         lc;

   ActiveParams::SetValue( oco, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue( olc, ACTIVEPARAM_LINESTYLE);
   ActiveParams::SetValue( owt, ACTIVEPARAM_LINEWEIGHT);

   /*--- B-�X�v���C���̏ꍇ ---*/
   if ( con->bspln == -1){
      /*--- B-�X�v���C���i�J�^�}�����j�̍\���̏��쐬 ---*/
      mdlBspline_catmullRomCurve( &curve, catPoi, NULL, catnv);

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
	   mdlLineString_create(&ele, NULL, catPoi, catnv);
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
**	�Q�_�Ԃ̔��z���ō����擾
-----------------------------------------------------------------------*/
void	get_hirei_z( Dpoint3d *Poi, Dpoint3d *Poi1, Dpoint3d *Poi2)
{
Dpoint3d	tmpPoi[2], nearPoi;
int			iti, nv;
double		leng;

   memcpy( &tmpPoi[0], Poi1, sizeof( Dpoint3d));
   memcpy( &tmpPoi[1], Poi2, sizeof( Dpoint3d));
   nv = 2;

   /*--- ������^����_�����ƂȂ�2�_�����Ԑ����Ɉ����������̑� ---*/
   get_closest_point( &nv, tmpPoi, Poi, &nearPoi, &iti, &leng);

   /*--- ��ƂȂ�2�_�́iX, Y�j�������ꍇ ---*/
   if ( get_leng( Poi1, Poi2) < 1){
      Poi->z = ( Poi1->z + Poi2->z)/2;
   }

   /*--- ��ƂȂ�2�_�́iX, Y�j���Ⴄ�ꍇ ---*/
   else{
      Poi->z = Poi1->z + ( Poi2->z - Poi1->z) * get_leng( &nearPoi, Poi1) / get_leng( Poi2, Poi1);
   }
}


/*-----------------------------------------------------------------------
**	���������ǂ����̔���
-----------------------------------------------------------------------*/
int	judge_conter( int lnv, Dpoint3d *lPoi)
{
int			ii;

   /*--- �C������ ---*/
   for( ii=1; ii<lnv; ii++){
      if ( lPoi[ii].z != lPoi[0].z){
         return( FALSE);
      }
   }

   return( TRUE);
}


/*-----------------------------------------------------------------------
**	�R���^�[�̌���
-----------------------------------------------------------------------*/
int		scan_conter_a(Dpoint3d *inpoint, int *iti, int kisyu)
{
	ExtScanList	scanList;
	Dpoint3d	nearPoi;
	DSegment3d	seg;
	ULong		filePos, elemAddr[1], Pos[1000];
	int			flag, word, getSize, Error;
	int			ii, lc;
	UInt32      lv, co, wt;
	int         num;

	if (kisyu == KITEN) {
		memcpy(&catPoi[0], inpoint, sizeof(Dpoint3d));
	}

	/*--- �X�L�������X�g�̏����� ---*/
	mdlScan_initScanlist(&scanList);

	/*--- �����͈͂̐ݒ� ---*/
	mdlScan_noRangeCheck(&scanList);


	scanList.scantype |= (unsigned short)ONEELEM;
	scanList.scantype |= (unsigned short)ELEMTYPE;

	scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
	scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

	/*--- �X�L�������s ---*/
	filePos = 0;   flag = 0;
	if (mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS) {
		mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
		mdlSystem_exit(exitStatus, 1);
		return(ERROR);
	}

	while (TRUE) {
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
		if (Error == 10) { break; }
		if (Error == 65) { mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�"); }
		if (Error == 66) { mdlDialog_openInfoBox(L"EOF�G���["); }
		if (Error == 67) { mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��"); }
		if (Error == 68) { mdlDialog_openInfoBox(L"�����ȗv�f"); }
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return(ERROR); }

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);

		/*--- �I�_���̏ꍇ�̓��C���[�`�F�b�N ---*/
		if (kisyu == SYUTEN) {
			get_layer(&lv, &co, &lc, &wt, NULL, &ele);
			if (olv != lv || oco != co || olc != lc || owt != wt) {
				continue;
			}
		}

		/*--- �X�L�����������C����ŁA���������_����ŒZ�����̓_ ---*/
		memcpy(&nearPoi, inpoint, sizeof(Dpoint3d));
		mdlLinear_getClosestSegment(&seg, &nearPoi, &ele);

		/*--- �ŒZ�����̓_��1mm�ȉ��̏ꍇ�́A�X�L�����������C���������������̂Ƃ��� ---*/
		if (get_leng(&nearPoi, inpoint) < 1) {
			if (kisyu == KITEN) {
				get_layer(&olv, &oco, &olc, &owt, &ggnum, &ele);
			}
			mdlLinear_extract(tmp_oPoi, &tmp_onv, &ele, MASTERFILE);
			Pos[0] = elemAddr[0];
			flag++;
			break;
		}
	}

	/*--- �v�f��������Ȃ������ꍇ ---*/
	if (flag == 0) {
		if (kisyu == KITEN) {
			mdlDialog_openInfoBox(L"�I�ђ����ĉ�����");
			startPrimitive();
		}
		return(ERROR);
	}

	/*--- �v�f�����������ꍇ ---*/
	else {
		if (kisyu == KITEN) { catnv = 1; }
		linnum = 0;
		scan_line(olv, oco, olc, owt, &linnum, linPos);

		/*--- �����������C���̘A�������� ---*/
		num = 1;
		get_renzoku(linnum, linPos, &tmp_onv, tmp_oPoi, &num, Pos);

		/*--- ���������ʒu ---*/
		get_closest_point(&tmp_onv, tmp_oPoi, inpoint, &nearPoi, iti, &sleng);

		onv[kisyu] = tmp_onv; 
		memcpy(oPoi[kisyu], tmp_oPoi, tmp_onv * sizeof(Dpoint3d));
		onum[kisyu] = num;
		memcpy(oPos[kisyu], Pos, num * sizeof(ULong));
		if (kisyu == KITEN) { setFunction(); }
		

		return(SUCCESS);
	}
}

