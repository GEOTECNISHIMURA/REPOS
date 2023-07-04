/*-----------------------------------------------------------------------*/
/*-	�P�v�f�̂݃����N������			-*/
/*-									-*/
/*-	        SINGLE.MC				-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

MSElement		ele;
MSElementDescr  *elmDP1, *elmDP2;
int				exitStatus;
WChar			zumenname[128], *aa, *token;
WChar			zumen[128], file[128], info[128], ins[128];
LINKSET			*linkset;

#ifndef Private
#define Private
#endif


/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
Private void	startModifyCommand( void);
Private int	attach( Dpoint3d *inpoint,  int view);


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

   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"SINGLE�̃��\�[�X�t�@�C�����J���܂���");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- �A�����[�h�֐��Z�b�g ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   startModifyCommand( );

}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	startModifyCommand
-----------------------------------------------------------------------*/
Private void	startModifyCommand( )
{
   /*--- ���Z�b�g�{�^���A�f�[�^�{�^������������ ---*/
   mdlState_startModifyCommand( (StateFunc_Reset)startModifyCommand, (MdlFunc_Accept)attach,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_noElemNoLocked( );
   mdlLocate_normal( );
   mdlLocate_init( );
}


/*-----------------------------------------------------------------------
**	����
-----------------------------------------------------------------------*/
Private int	attach( Dpoint3d *inpoint,  int view)
{
DatabaseLink	*dblink;
MS_sqlda		sqlda;
ULong			filePos;
UInt32          mslink;
int				len, linknum, zumennum;


   /*--- �t�@�C���ʒu ---*/
   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);
   mdlElement_read( &ele, MASTERFILE, filePos);
   mdlDB_extractLinkages( &dblink, &linknum, &ele);

   /*--- �����N�ςݗv�f�̏ꍇ ---*/
   if ( linknum > 0){
      mdlDialog_openInfoBox( L"���̗v�f�͊��Ƀ����N����Ă��܂�");
	  mdlSystem_exit(exitStatus, 1);
	  return 0;
   }

   /*--- �}�ʖ� ---*/
   wcscpy( zumenname, tcb->dgnfilenm);
   aa = wcsrchr( zumenname, '\\');
   token = wcstok( aa, L".");
   wcscpy( file, &aa[1]);
   len = (int)wcslen( file);

   /*--- �}�ʔԍ� ---*/
   if ( linkset->zumen == -1){
      wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
   }

   /*--- �������[�h���� ------*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- �f�[�^�x�[�X�\����ǂݍ��� ---*/
   if ( mdlDB_describeTable( &sqlda, linkset->table) != SUCCESS){
      mdlDialog_openInfoBox(L"�f�[�^�x�[�X�e�[�u�����ǂݍ��߂܂���");
      mdlSystem_exit( exitStatus, 1);
	  return 0;
   }

   zumennum = _wtoi(zumen);
   /*--- �ő�MSLINK�{�P�ǉ��̏ꍇ ---*/
   if ( linkset->kind == 2){
      mdlDB_largestMslink( &mslink, linkset->table);
      mslink = mslink + 1;

      /*--- SQL���i�}�ʔԍ��o�͂̏ꍇ�j ---*/
      if ( linkset->zumen == -1){
         swprintf( ins, L"insert into %s (mslink, ZumenNum) values (%ld, '%ld')", linkset->table, mslink, zumennum);
      }

      /*--- SQL���i�}�ʔԍ��o�͂��Ȃ��ꍇ�j ---*/
      else{
         swprintf( ins, L"insert into %s (mslink) values (%ld)", linkset->table, mslink);
      }

      /*--- �f�[�^�x�[�X�ɒǉ����� ---*/
      if ( mdlDB_processSQL( ins) != SUCCESS){
         mdlDialog_openInfoBox(L"�f�[�^�x�[�X�ɏ������߂܂���");
         mdlSystem_exit( exitStatus, 1);
		 return 0;
      }

      /*--- MSLINK�̃A�^�b�` ------*/
      attach_mslink_filePos( linkset->table, mslink, &filePos);
   }

   /*--- �ݒ肵�Ēǉ��̏ꍇ ---*/
   if ( linkset->kind == 3){
      /*--- SQL���i�}�ʔԍ��o�͂̏ꍇ�j ---*/
      if ( linkset->zumen == -1){
         swprintf( ins, L"insert into %s (mslink, ZumenNum) values (%ld, '%ld')", linkset->table, linkset->mslink, zumennum);
      }

      /*--- SQL���i�}�ʔԍ��o�͂��Ȃ��ꍇ�j ---*/
      else{
         swprintf( ins, L"insert into %s (mslink) values (%ld)", linkset->table, linkset->mslink);
      }

      /*--- �f�[�^�x�[�X�ɒǉ����� ---*/
      if ( mdlDB_processSQL( ins) != SUCCESS){
         mdlDialog_openInfoBox(L"�f�[�^�x�[�X�ɏ������߂܂���");
         mdlSystem_exit( exitStatus, 1);
		 return 0;
      }

      /*--- MSLINK�̃A�^�b�` ------*/
      attach_mslink_filePos( linkset->table, linkset->mslink, &filePos);
   }

/*--- DGN�̂ݒǉ��̏ꍇ ---*/
/*
if ( linkset->kind == 4){
   memset( &props, 0, sizeof( LinkProps));
   mdlDB_buildLink( &link, &linkLength, ODBC_LINKAGE, &props, linkset->table, linkset->mslink, 1);
   mdlElmdscr_read( &elmDP1, filePos, 0, 0, NULL);
   mdlElmdscr_duplicate( &elmDP2, elmDP1);
   mdlElmdscr_appendAttributes( &elmDP2, linkLength, &link);
   mdlElmdscr_rewrite( elmDP2, elmDP1, filePos);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);
}
*/

   /*--- ����MSLINK�̏ꍇ ---*/
   if ( linkset->kind == 4){
      /*--- MSLINK�̃A�^�b�` ------*/
      swprintf( ins, L"select * from %s where mslink=%ld", linkset->table, linkset->mslink);
      attach_mslink_filePos( linkset->table, linkset->mslink, &filePos);
   }
}
