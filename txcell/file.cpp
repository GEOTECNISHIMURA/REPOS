/*-----------------------------------------------------------------------*/
/*-	        FILE.MC							-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "txcell.h"

FILE	*fp;
TXSET 	*txset;
int	 	exitStatus;
WChar	fileName[MAXFILELENGTH];

#ifndef Private
#define Private
#endif

/*-	�v���^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int		change( WChar  *fileName);
Private bool 	change_getfile( WChar *tempFileName);
Private bool 	change_openfile( WCharCP fileNamea);
Private	void	get_dir( WChar *dir, WChar *full);

/*-----------------------------------------------------------------------
**	�w�i�n�}�e�[�u���t�@�C���ύX
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
CExprResult     result;
CExprValue      value;

   /*--------- �A�����[�h�֐��Z�b�g ---------------*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �L��ϐ��̎擾 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&txset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }
   txset = (TXSET*)value.val.valPointer;

   change( fileName);
}

/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool   unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�֐�
-----------------------------------------------------------------------*/
Private	int	change( WChar  *fileNamea)
{
   if ( !change_getfile( fileNamea)){
      return( SUCCESS);
   }
   else{
      return( ERROR);
   }
}


/*-----------------------------------------------------------------------
**	�t�@�C�����t���p�X���擾
-----------------------------------------------------------------------*/
Private	bool	change_getfile( WChar *tempFileName)
{
WChar	dir[255];
BeFileName	filePath;


   if ( wcslen( tempFileName)){
      wcscpy( fileName, tempFileName);
	  filePath.SetName(tempFileName);
   }
   else{
      /*--- idx̧�ّI���̏ꍇ ---*/
      if ( txset->nn == 1){
         get_dir( dir, txset->idx);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.idx",
                                  dir, L".idx̧�ّI��")){
            return( TRUE);
         }
      }

      /*--- ����̧�ّI���̏ꍇ ---*/
      else if ( txset->nn == 2){
         get_dir( dir, txset->seed);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.dgn",
                                  dir, L"����̧�ّI��")){
            return( TRUE);
         }
      }

      /*--- ��ײ���ؑI���̏ꍇ ---*/
      else if ( txset->nn == 3){
         get_dir( dir, txset->cellib);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.cel",
                                  dir, L"��ײ���ؑI��")){
            return( TRUE);
         }
      }

      /*--- ��_̧�ّI���̏ꍇ ---*/
      else{
         get_dir( dir, txset->file);

         /*--- .sim̧�ّI���̏ꍇ ---*/
         if ( txset->form == 2){
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.sim",
                                     dir, L".sim̧�ّI��")){
               return( TRUE);
            }
         }

         /*--- .daţ�ّI���̏ꍇ ---*/
         else{
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.dat",
                                     dir, L".daţ�ّI��")){
               return( TRUE);
            }
         }
      }
   }
   return( change_openfile( filePath.GetName()));
}


/*-----------------------------------------------------------------------
**	�t�@�C�����J��
-----------------------------------------------------------------------*/
Private bool change_openfile(WCharCP fileNamea)
{
   /*--- idx̧�ّI���̏ꍇ ---*/
   if ( txset->nn == 1){
      wcscpy( txset->idx, fileNamea);
   }

   /*--- ����̧�ّI���̏ꍇ ---*/
   else if ( txset->nn == 2){
      wcscpy( txset->seed, fileNamea);
   }

   /*--- ��ײ���ؑI���̏ꍇ ---*/
   else if ( txset->nn == 3){
      wcscpy( txset->cellib, fileNamea);
   }

   /*--- ��_̧�ّI���̏ꍇ ---*/
   else{
      wcscpy( txset->file, fileNamea);
   }

   mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL);
   return( FALSE);
}


/*-----------------------------------------------------------------------
**	�ŏ��ɊJ���p�X�ݒ�
-----------------------------------------------------------------------*/
Private	void  get_dir( WChar *dir, WChar *full)
{
int		len, len1, len2;

   /*--- �ŏ��ɊJ���p�X�ݒ� ---*/
   wcscpy( dir, L"");
   len1 = (int)wcslen( full);
   if ( wcsrchr( full, L'\\') != NULL){
      len2 = (int)wcslen( wcsrchr( full, L'\\'));
      len = len1 - len2 + 1;
      wmemcpy( dir, full, len);
   }
}
