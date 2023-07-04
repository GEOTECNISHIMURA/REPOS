/*-----------------------------------------------------------------------*/
/*-		��������			-*/
/*-							-*/
/*-	        DEL.MC			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;
MSElement		ele;
MSElementDescr  *elmDP1, *elmDP2;
int				exitStatus;
LINKSET			*linkset;

#ifndef Private
#define Private
#endif

/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
Private void	startModifyCommand( void);
Private void	attach( Dpoint3d *inpoint);


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
Private void	attach( Dpoint3d *inpoint)
{
ULong	filePos;

   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);

   /*--- �f�[�^���폜�̏ꍇ ---*/
   if ( linkset->tdel==-1){ mdlDB_activeDeleteMode( TRUE);}
   else{                    mdlDB_activeDeleteMode( FALSE);}

   mdlElmdscr_read( &elmDP1, filePos, MASTERFILE, 0, NULL);
   mdlElmdscr_duplicate( &elmDP2, elmDP1);

   mdlDB_detachAttributesDscr( &elmDP2);
   mdlElmdscr_rewrite( elmDP2, elmDP1, filePos);
   mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Normal);

   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);
}
