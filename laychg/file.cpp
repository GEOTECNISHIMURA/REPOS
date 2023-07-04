/*- 5:Ver.5,  95:Ver.95 -*/
/*-----------------------------------------------------------------------*/
/*-	        FILE.MC							-*/
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "laychg.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE	 *fp;
int	 	 len, len1, len2, exitStatus;
WChar	 fileName[MAXFILELENGTH];
WChar	 full[128], dir[128];
LAYSET 	 *layset;

/*-	�v���^�C�v	-*/
bool   unload(UnloadProgramReason unloadType);
int	change( WChar  *fileNm);
bool	change_getfile( WChar *tempFileName);
bool	change_openfile( WCharCP fileNm);


/*-----------------------------------------------------------------------
**	���C��
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

    /*--------- �L��ϐ��̎擾 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&layset",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
    }
    layset = (LAYSET*)value.val.valPointer;

    change( fileName);
}



/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}



/*-----------------------------------------------------------------------
**	�֐�
-----------------------------------------------------------------------*/
int	change( WChar  *fileNm)
{
    if ( !change_getfile( fileNm)){
        return( SUCCESS);
    }
    else{
        return( ERROR);
    }
}



/*-----------------------------------------------------------------------
**	�t�@�C�����t���p�X���擾
-----------------------------------------------------------------------*/
bool change_getfile(WChar *tempFileName)
{

	BeFileName	filePath;
	if (wcslen(tempFileName)) {
		wcscpy(fileName, tempFileName);
		filePath.SetName(tempFileName);
	}
    else{
        if ( layset->nn == 1){
        	
            /*--- �e�[�u���t�@�C���I�[�v�� ---*/
            if( NULL != wmemchr( layset->file, L'\\', 4)){
                len1 = wcslen( layset->file);
                len2 = wcslen( wcsrchr( layset->file, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, layset->file, len);
            }
            else{
                wcscpy( layset->file, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"�e�[�u���t�@�C����I��")){
            	return( TRUE);
            }
        }
        else if ( layset->nn == 2){
            /*--- �e�[�u���t�@�C���I�[�v�� ---*/
            if( NULL != wmemchr( layset->bfile, L'\\', 4)){
                len1 = wcslen( layset->bfile);
                len2 = wcslen( wcsrchr( layset->bfile, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, layset->bfile, len);
            }
            else{
                wcscpy( layset->bfile, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"�o�b�`�t�@�C����I��")){
                return( TRUE);
            }
        }
        else if ( layset->nn == 3){
            /*--- �e�[�u���t�@�C���I�[�v�� ---*/
            if( NULL != wmemchr( layset->cfile, L'\\', 4)){
                len1 = wcslen( layset->cfile);
                len2 = wcslen( wcsrchr( layset->cfile, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, layset->cfile, len);
            }
            else{
                wcscpy( layset->cfile, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.cel", dir, L"�Z�����C�u������I��")){
                return( TRUE);
            }
        }
    }
    return( change_openfile( filePath.GetName()));
}



/*-----------------------------------------------------------------------
**	�t�@�C�����J��
-----------------------------------------------------------------------*/
bool	change_openfile(WCharCP fileNm)
{
    if ( layset->nn == 1){
        wcscpy( layset->file, fileNm);
    }
    else if ( layset->nn == 2){
        wcscpy( layset->bfile, fileNm);
    }
    else if ( layset->nn == 3){
        wcscpy( layset->cfile, fileNm);
    }
    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL);
    return( FALSE);
}
