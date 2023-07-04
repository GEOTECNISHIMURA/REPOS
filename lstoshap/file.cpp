/*-----------------------------------------------------------------------*/
/*-	        FILE.MC							-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "lstoshap.h"

FILE            *fp;
GLOBAL          *glo;
int	 	        len, len1, len2, exitStatus;
WChar            fileName[MAXFILELENGTH];
WChar	        dir[128];

#ifndef Private
#define Private
#endif
/*- �v���g�^�C�v    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change(WChar  *fileNamea);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileNamea);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C"  void MdlMain
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
	if (mdlCExpression_getValue(&value, &result, "&glo",
		VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS) {
		mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
		return;
	}
    glo = (GLOBAL*)value.val.valPointer;

    change( fileName);
}


/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**  �֐�
-----------------------------------------------------------------------*/
Private int change(WChar  *fileNamea)
{
	if (!change_getfile(fileNamea)) {
		return(SUCCESS);
	}
	else {
		return(ERROR);
	}
}


/*-----------------------------------------------------------------------
**	�t�@�C�����t���p�X���擾
-----------------------------------------------------------------------*/
Private bool change_getfile(WChar *tempFileName)
{
	BeFileName	filePath;
	if (wcslen(tempFileName)) {
		wcscpy(fileName, tempFileName);
		filePath.SetName(tempFileName);
	}
	else {
        if ( glo->nn == 1){
            if( NULL != wmemchr( glo->bat, '\\', 4)){
                len1 = (int)wcslen( glo->bat);
                len2 = (int)wcslen( wcsrchr( glo->bat, '\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, glo->bat, len);
            }
            else{
                wcscpy( glo->bat, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
			filePath.SetName(fileName);
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"�o�b�`�t�@�C����I��")){
                return( TRUE);
            }
        }
        else{
            if( NULL != memchr( glo->table, '\\', 4)){
                len1 = (int)wcslen( glo->table);
                len2 = (int)wcslen( wcsrchr( glo->table, '\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, glo->table, len);
            }
            else{
                wcscpy( glo->table, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
			filePath.SetName(fileName);
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.tbl", dir, L"�e�[�u���t�@�C����I��")){
                return( TRUE);
            }
        }
    }
    return( change_openfile(filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  �t�@�C�����J��
-----------------------------------------------------------------------*/
Private bool change_openfile(WCharCP fileNamea)
{
    if ( glo->nn == 1){
		wcscpy(glo->bat, fileNamea);
    }
    if ( glo->nn == 2){
		wcscpy(glo->table, fileNamea);
    }
    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL);

    return( FALSE);
}
