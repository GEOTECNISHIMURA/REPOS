/*-----------------------------------------------------------------------*/
/*-	�t�@�C������MSLINK�ŗ^����		-*/
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
#include "../CommonCE/naka8.cpp"
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

FILE			*fp;
MSElement		ele;
int				exitStatus;
LINKSET			*linkset;

#ifndef Private
#define Private
#endif

/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
Private  void	syori( void);
Private  void	kensaku_cell( int *cnum, ULong *cPos);
Private  void	kensaku_line( int *lnum, ULong *lPos);
Private  void	kensaku_text( int *tnum, ULong *tPos);
Private  void	link_element( int num, ULong *Pos, WChar *nameP);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult		result;
CExprValue		value;
RscFileHandle   Rsc;
WChar			info[256], filename[256];

   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"ALL�̃��\�[�X�t�@�C�����J���܂���");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- �A�����[�h�֐��Z�b�g ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error( L"�L��ϐ����擾�ł��܂���");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- �o�b�`�t�@�C���ǂݍ��� ---*/
   if ( ( fp = _wfopen( linkset->bat,L"r")) != NULL){
      while( fscanf( fp, "%ls\n", filename) != EOF){
         if ( mdlSystem_newDesignFile( filename) == SUCCESS){
            syori( );
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

   mdlSystem_exit( exitStatus, 1);
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	syori
-----------------------------------------------------------------------*/
Private	void	syori( )
{
WChar		nameP[256];
int			cnum, lnum, tnum;
ULong		*cPos, *lPos, *tPos;

   /*--- �������[�h ---*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- �f�[�^�x�[�X�Ɛڑ� ---*/
   mdlDB_activeDatabase( linkset->dbsource);

   cPos = (ULong*)calloc( 100000, sizeof( ULong));
   lPos = (ULong*)calloc( 100000, sizeof( ULong));
   tPos = (ULong*)calloc( 100000, sizeof( ULong));

   /*--- �t�@�C���� ---*/
   mdlFile_parseName( tcb->dgnfilenm, NULL, NULL, nameP, NULL);

   /*--- �Z�� ---*/
   cnum = 0;
   kensaku_cell( &cnum, cPos);
   link_element( cnum, cPos, nameP);

   /*--- ���C���E���C���X�g�����O�E�V�F�C�v ---*/
   lnum = 0;
   kensaku_line( &lnum, lPos);
   link_element( lnum, lPos, nameP);

   /*--- �e�L�X�g ---*/
   tnum = 0;
   kensaku_text( &tnum, tPos);
   link_element( tnum, tPos, nameP);

   free( tPos);   free( lPos);   free( cPos);
}


/*-----------------------------------------------------------------------
**	�Z���̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_cell( int *cnum, ULong *cPos)
{
ExtScanList	scanList;
ULong		filePos;
int			type, i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- �X�L�������X�g�̏����� ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;

   /*--- �X�L�������s ---------------*/
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
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
		  if (type == 2) {
			  cPos[*cnum] = filePos;
			  *cnum = *cnum + 1;
		  }
      }
   }
}


/*-----------------------------------------------------------------------
**	���C���A���C���X�g�����O�A�V�F�C�v�̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_line( int *lnum, ULong *lPos)
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
   filePos = 0;
   if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
      mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
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
		  lPos[*lnum] = filePos;
		  *lnum = *lnum + 1;
	  }
   }
}


/*-----------------------------------------------------------------------
**	�e�L�X�g�̌���
-----------------------------------------------------------------------*/
Private	void	kensaku_text( int *tnum, ULong *tPos)
{
	ExtScanList	scanList;
ULong		filePos;
int			i, getSize, Error, numAddr;
ULong		elemAddr[50];

   /*--- �X�L�������X�g�̏����� ---------------*/
   mdlScan_initScanlist( &scanList);

   /*--- �����͈͂̐ݒ� ---------------*/
   mdlScan_noRangeCheck( &scanList);
   scanList.scantype |= (unsigned short)ONEELEM;
   scanList.scantype |= (unsigned short)ELEMTYPE;

   scanList.typmask[1] |= (unsigned short)TMSK1_TEXT;

   /*--------- �X�L�������s ---------------*/
   filePos = 0;
   if (mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS) {
	   mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
	   mdlSystem_exit(exitStatus, 1);
   }

   while (TRUE) {
	   Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
	   numAddr = getSize / sizeof(short);
	   if (Error == 10) { break; }
	   if (Error == 65) { mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�"); }
	   if (Error == 66) { mdlDialog_openInfoBox(L"EOF�G���["); }
	   if (Error == 67) { mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��"); }
	   if (Error == 68) { mdlDialog_openInfoBox(L"�����ȗv�f"); }
	   if (Error >= 65) { mdlSystem_exit(exitStatus, 1); }
	   for (i = 0; i < numAddr; i++)
	   {
		   if (mdlElement_read(&ele, MASTERFILE, elemAddr[i]) != SUCCESS) { continue; }
		   tPos[*tnum] = filePos;
		   *tnum = *tnum + 1;
	   }
   }
}


/*-----------------------------------------------------------------------
**	�����N����
-----------------------------------------------------------------------*/
Private	void	link_element( int num, ULong *Pos, WChar *nameP)
{
WChar			ins[256];
int				ii;

   /*--- �f�[�^�x�[�X�ɒǉ� ---*/
   swprintf( ins, L"insert into %ls (MSLINK) values (%ld)"
               , linkset->table, _wtol( nameP));
   if ( process_SQL( ins) == ERROR){ mdlSystem_exit( exitStatus, 1);}

   /*--- �v�f��MSLINK��t�� ---*/
   for( ii=0; ii<num; ii++){
      attach_mslink_filePos( linkset->table, _wtol( nameP), &Pos[ii]);
   }
}
