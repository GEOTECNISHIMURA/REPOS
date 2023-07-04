/*-----------------------------------------------------------------------*/
/*-	�P�t�@�C���S�ă����N������		-*/
/*-									-*/
/*-	        ALL.MC					-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

FILE			*fp;
MSElementDescr	*elmDP, *elmDP2;
MSElement		ele;
MS_sqlda		sqlda;
DatabaseLink	*link;
ULong			cPos[100000], Pos, mslink;
WChar			buf[128], info[128], sql[128], ins[128], ccell[100][7];
WChar			cdbf[500][10], ldbf[500][10], tdbf[500][10];
WChar			cell[7];
WChar			tp[10], filename[128], file[128], full[128], zumen[128], ms[128], *aa, *token;
int				ii, jj, cc, ll, tt, cnum, lnum, tnum, elenum, renban, zumennum;
int				len, type, flag, linknum, exitStatus;
UInt32		    clv[500], cco[500], cwt[500];
UInt32			llv[500], lco[500], lwt[500];
UInt32			tlv[500], tco[500], twt[500];
int             clc[500], llc[500], tlc[500];
UInt32          lv, co, wt;
int				lc;
LINKSET			*linkset;

#ifndef Private
#define Private
#endif


/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
Private  void	kensaku_cell( void);
Private  void	kensaku_line( void);
Private  void	kensaku_complex( void);
Private  void	kensaku_text( void);
Private  void	link_judge_cell( void);
Private  void	link_judge_line( void);
Private  void	link_judge_text( void);
Private  void	link_cell( void);
Private  void	link_line( void);
Private  void	link_text( void);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

	BeFileName		cellib;
	CExprResult	result;
	CExprValue	value;
RscFileHandle   Rsc;

   /*--------- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"ALL�̃��\�[�X�t�@�C�����J���܂���");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- �A�����[�h�֐��Z�b�g ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--------- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- ���C���[�e�[�u���t�@�C���̓ǂݍ��� ---*/
   if ( ( fp = _wfopen( linkset->layer, L"r")) != NULL){
      cc = 0;  ll = 0;   tt = 0;
      while( fgetws( buf, 128, fp) != NULL){
         /*--- �Z���̏ꍇ ---*/
         if ( wmemcmp( &buf[0] ,L"C", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls %ls\n"
                      , tp, &clv[cc], &cco[cc], &cwt[cc], &clc[cc], ccell[cc], cdbf[cc]);
            cc++;
         }

         /*--- ���C���̏ꍇ ---*/
         if ( wmemcmp( &buf[0] ,L"L", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls\n"
                      , tp, &llv[ll], &lco[ll], &lwt[ll], &llc[ll], ldbf[ll]);
            ll++;
         }

         /*--- �e�L�X�g�̏ꍇ ---*/
         if ( wmemcmp( &buf[0] ,L"T", 1) == 0){
            swscanf( buf, L"%ls %d %d %d %d %ls\n"
                      , tp, &tlv[tt], &tco[tt], &twt[tt], &tlc[tt], tdbf[tt]);
            tt++;
         }
      }
      cnum = cc;   lnum = ll;   tnum = tt;
   }
   else{
      swprintf( info, L"%ls ���ǂ߂܂���", linkset->layer);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   fclose( fp);

   /*--- �o�b�`�̏ꍇ ---*/
   if ( linkset->kind == 0){
      /*--- �o�b�`�t�@�C���ǂݍ��� ---*/
      if ( ( fp = _wfopen( linkset->bat,L"r") ) != NULL){
         while( fwscanf( fp,L"%ls\n", filename) != EOF){
            if ( mdlSystem_newDesignFile( filename) == SUCCESS){
               wcscpy( full, filename);
               aa    = wcsrchr( full, '\\');
               token = wcstok( aa, L".");
               wcscpy( file, &aa[1]);
               len   = (int)wcslen( file);

               /*--- �t�@�C�����̉������� ---*/
               wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
               zumennum = _wtoi( zumen);

               renban = 0;
               kensaku_cell( );
               link_judge_cell( );

               kensaku_line( );
               link_judge_line( );

               kensaku_text( );
               link_judge_text( );
            }
            else{
               mdlSystem_exit( exitStatus, 1);
            }
         }
      }
      else{
         swprintf( info, L"%ls ���J���܂���", linkset->bat);
         mdlDialog_openInfoBox( info);
         mdlSystem_exit( exitStatus, 1);
      }
      fclose( fp);
   }

   /*--- �P�t�@�C���̏ꍇ ---*/
   if ( linkset->kind == 1){
      /*--- �t�@�C���� ---*/
      wcscpy( filename, tcb->dgnfilenm);
      wcscpy( full, filename);
      aa    = wcsrchr( full, '\\');
      token = wcstok( aa, L".");
      wcscpy( file, &aa[1]);
      len   = (int)wcslen( file);

      /*--- �t�@�C�����̉������� ---*/
      wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
      zumennum = _wtoi( zumen);

      renban = 0;
      kensaku_cell( );
      link_judge_cell( );

      kensaku_line( );
      kensaku_complex( );
      link_judge_line( );

      kensaku_text( );
      link_judge_text( );
   }

}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�Z���̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_cell( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--------- �X�L�������X�g�̏����� ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--------- �����͈͂̐ݒ� ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;

   /*--------- �X�L�������s ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS){ continue; }
		  type = mdlElement_getType(&ele);
		  if (type == 2) {
				cPos[elenum] = filePos;
				elenum++;
		  }
	  }
   }
}


/*-----------------------------------------------------------------------
**	���C���A���C���X�g�����O�A�V�F�C�v�̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_line( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- �X�L�������X�g�̏����� ---------------*/
   mdlScan_initScanlist( &scanList);

   /*---- �����͈͂̐ݒ� ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_SHAPE;

   /*--------- �X�L�������s ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);
		  if (type == 3) {
			  cPos[elenum] = filePos;
			  elenum++;
		  }
		  if (type == 4 || type == 6) {
			  cPos[elenum] = filePos;
			  elenum++;
		  }
	  }
   }
}


/*------------------------------------------------------------------------
**	�v�f����
------------------------------------------------------------------------*/
Private  void  kensaku_complex( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- �X�L�������X�g�̏����� ---*/
   mdlScan_initScanlist( &scanList);

   /*--- ���������̐ݒ� ---*/
   mdlScan_noRangeCheck( &scanList);

   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;
   scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;

   /*--- �X�L�������s ---*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlDialog_openInfoBox( L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while( TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);
		  mdlElement_getSymbology(&co, &wt, &lc, &ele);
		  mdlElement_getProperties(&lv, NULL, NULL, NULL
			  , NULL, NULL, NULL, NULL, &ele);

		  if (type != 12 && type != 14) { continue; }

		  for (ii = 0; ii < lnum; ii++) {
			  if (lv == llv[ii] && co == lco[ii] && wt == lwt[ii] && lc == llc[ii]) {
				  cPos[elenum] = filePos;
				  elenum++;
				  break;
			  }
		  }
	  }
   }
}


/*-----------------------------------------------------------------------
**	�e�L�X�g�̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_text( )
{
ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- �X�L�������X�g�̏����� ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ELEMTYPE;
   scanList.typmask[1] |= (unsigned short)TMSK1_TEXT;

   /*--- �X�L�������s ---------------*/
   filePos = 0;   elenum = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
      mdlSystem_exit( exitStatus, 1);
   }

   while(TRUE){
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
      if ( Error == 10){ break;}
      if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
      if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
      if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
      if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
      if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
	  for (i = 0; i < numAddr; i++)
	  {
		  if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		  type = mdlElement_getType(&ele);

		  cPos[elenum] = filePos;
		  elenum++;
	  }
   }
}


/*-----------------------------------------------------------------------
**	�����N���邩�ǂ����̔���i�Z���j
-----------------------------------------------------------------------*/
Private	void	link_judge_cell( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"�Z�� %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlCell_extract( NULL, NULL, NULL, NULL, cell,NULL, &ele);

      Pos = cPos[ii];
      mdlElement_read( &ele, MASTERFILE, Pos);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);
      for( jj=0; jj<cnum; jj++){
         if (  lv==clv[jj] && co==cco[jj] && wt==cwt[jj] && lc==clc[jj]
            && wcscmp( cell, ccell[jj]) == 0){
            link_cell( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	�����N���邩�ǂ����̔���i���C���j
-----------------------------------------------------------------------*/
Private	void	link_judge_line( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"���C�� %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);

      for( jj=0; jj<lnum; jj++){
         if (  lv==llv[jj] && co==lco[jj] && wt==lwt[jj] && lc==llc[jj]){
            link_line( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	�����N���邩�ǂ����̔���i�e�L�X�g�j
-----------------------------------------------------------------------*/
Private	void	link_judge_text( )
{
   for( ii=0; ii<elenum; ii++){
mdlOutput_printf( MSG_ERROR, L"�e�L�X�g %d/%d", ii+1, elenum);
      mdlElement_read( &ele, MASTERFILE, cPos[ii]);
      mdlDB_extractLinkages( &link, &linknum, &ele);
      if ( linknum > 0){
         continue;
      }
      mdlElement_getSymbology( &co, &wt, &lc, &ele);
      mdlElement_getProperties( &lv, NULL, NULL, NULL
                              , NULL, NULL, NULL, NULL, &ele);

      for( jj=0; jj<tnum; jj++){
         if (  lv==tlv[jj] && co==tco[jj] && wt==twt[jj] && lc==tlc[jj]){
            link_text( );
            break;
         }
      }
   }
}


/*-----------------------------------------------------------------------
**	�Z���������N����
-----------------------------------------------------------------------*/
Private	void	link_cell( )
{
   /*--- �A�N�e�B�u�G���e�B�e�B�̍쐬 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- �f�[�^�x�[�X�Ƃ̐ڑ��m�F ---*/
   swprintf( sql,L"select * from %ls", cdbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls ���J���܂���", cdbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- MSLINK�l ---*/
   renban++;
   if ( renban < 10){
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- �f�[�^�x�[�X�\����ǂݍ��� ---*/
   if ( mdlDB_describeTable( &sqlda, cdbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�e�[�u�����ǂݍ��߂܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- SQL���i�}�ʔԍ��o�͂̏ꍇ�j ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , cdbf[jj], mslink, zumennum);
   }

   /*--- SQL���i�}�ʔԍ��o�͂��Ȃ��ꍇ�j ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", cdbf[jj], mslink);
   }

   /*--- �f�[�^�x�[�X�ɒǉ����� ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�ɏ������߂܂���");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- �A�N�e�B�u�G���e�B�e�B���쐬 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", cdbf[jj], mslink);

   /*--- �A�N�e�B�u�G���e�B�e�B���A�^�b�` ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"�����N�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
   }
}


/*-----------------------------------------------------------------------
**	���C���������N����
-----------------------------------------------------------------------*/
Private	void	link_line( )
{
   /*--- �A�N�e�B�u�G���e�B�e�B�̍쐬 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- �f�[�^�x�[�X�Ƃ̐ڑ��m�F ---*/
   swprintf( sql,L"select * from %ls", ldbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls ���J���܂���", ldbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- �f�[�^�x�[�X�\����ǂݍ��� ---*/
   if ( mdlDB_describeTable( &sqlda, ldbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�e�[�u�����ǂݍ��߂܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- MSLINK�l ---*/
   renban++;
   if ( renban < 10){ 
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- SQL���i�}�ʔԍ��o�͂̏ꍇ�j ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , ldbf[jj], mslink, zumennum);
   }

   /*--- SQL���i�}�ʔԍ��o�͂��Ȃ��ꍇ�j ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", ldbf[jj], mslink);
   }

   /*--- �f�[�^�x�[�X�ɒǉ����� ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�ɏ������߂܂���");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- �A�N�e�B�u�G���e�B�e�B���쐬 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", ldbf[jj], mslink);

   /*--- �A�N�e�B�u�G���e�B�e�B���A�^�b�` ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"�����N�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
   }
}


/*-----------------------------------------------------------------------
**	�e�L�X�g�������N����
-----------------------------------------------------------------------*/
Private	void	link_text( )
{
   /*--- �A�N�e�B�u�G���e�B�e�B�̍쐬 ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- �f�[�^�x�[�X�Ƃ̐ڑ��m�F ---*/
   swprintf( sql,L"select * from %ls", tdbf[jj]);
   if ( mdlDB_openCursor( sql) != SUCCESS){
      swprintf( info, L"%ls ���J���܂���", tdbf[jj]);
      mdlDialog_openInfoBox( info);
      mdlSystem_exit( exitStatus, 1);
   }
   mdlDB_closeCursor( );

   /*--- MSLINK�l ---*/
   renban++;
   if ( renban < 10){
      swprintf( ms, L"%d000%d", zumennum, renban);
   }
   else if ( renban < 100){
      swprintf( ms, L"%d00%d", zumennum, renban);
   }
   else if ( renban < 1000){
      swprintf( ms, L"%d0%d", zumennum, renban);
   }
   else{
      swprintf( ms, L"%d%d", zumennum, renban);
   }
   mslink = _wtol( ms);

   /*--- �f�[�^�x�[�X�\����ǂݍ��� ---*/
   if ( mdlDB_describeTable( &sqlda, tdbf[jj]) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�e�[�u�����ǂݍ��߂܂���");
      mdlSystem_exit( exitStatus, 1);
	
   }

   /*--- SQL���i�}�ʔԍ��o�͂̏ꍇ�j ---*/
   if ( linkset->zumen == -1){
      swprintf( ins, L"insert into %ls (mslink, ZumenNum) values (%ld, '%ld')"
                  , tdbf[jj], mslink, zumennum);
   }
   
   /*--- SQL���i�}�ʔԍ��o�͂��Ȃ��ꍇ�j ---*/
   else{
      swprintf( ins, L"insert into %ls (mslink) values (%ld)", tdbf[jj], mslink);
   }

   /*--- �f�[�^�x�[�X�ɒǉ����� ---*/
   if ( mdlDB_processSQL( ins) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�ɏ������߂܂���");
      mdlSystem_exit( exitStatus, 1 );
   }

   /*--- �A�N�e�B�u�G���e�B�e�B���쐬 ---*/
   swprintf( ins, L"select * from %ls where mslink=%ld", tdbf[jj], mslink);

   /*--- �A�N�e�B�u�G���e�B�e�B���A�^�b�` ---*/
   if ( mdlDB_defineAEBySQLSelect( ins) == SUCCESS){
      mdlElmdscr_read( &elmDP, cPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &elmDP2, elmDP);
      mdlDB_detachAttributesDscr( &elmDP2);
      mdlDB_attachActiveEntityDscr( &elmDP2);
      mdlElmdscr_rewrite( elmDP2, elmDP, cPos[ii]);
      mdlElmdscr_freeAll( &elmDP);
      mdlElmdscr_freeAll( &elmDP2);
   }
   else{
      mdlDialog_openInfoBox( L"�����N�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
   }
}
