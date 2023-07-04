/*-----------------------------------------------------------------------*/
/*-         FILE.MC                                                     -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "mfcchk.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE        *fp;
int         len, len1, len2, exitStatus;
WChar        fileName[MAXFILELENGTH];
WChar        dir[128];
MFCCHK      *mfcchk;

#ifndef Private
#define Private
#endif

/*- �v���g�^�C�v    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change( WChar  *fileName);
Private bool change_getfile( WChar *tempFileName);
Private bool change_openfile( WCharCP fileName);


/*-----------------------------------------------------------------------
**  ���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
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
    if( mdlCExpression_getValue( &value, &result, "&mfcchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
    }
    mfcchk = (MFCCHK*)value.val.valPointer;

    change( fileName);
}


/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  �֐�
-----------------------------------------------------------------------*/
Private int change( WChar  *fileName)
{
    if( !change_getfile( fileName)){
        return( SUCCESS);
    }
    else{
        return( ERROR);
    }
}


/*-----------------------------------------------------------------------
**  �t�@�C�����t���p�X���擾
-----------------------------------------------------------------------*/
Private bool change_getfile( WChar *tempFileName)
{
	BeFileName	filePath;

    if( wcslen( tempFileName)){
        wcscpy( fileName, tempFileName);
    }

    else{
        /*--- �e�[�u�� ---*/
        if( mfcchk->nn == 1){
            if( NULL != memchr( mfcchk->table, '\\', 4)){
                len1 = wcslen( mfcchk->table);
                len2 = wcslen( wcsrchr( mfcchk->table, '\\'));
                len = len1 - len2 + 1;
                memcpy( dir, mfcchk->table, len);
            }
            else{
                wcscpy( mfcchk->table, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"�e�[�u���t�@�C����I��")){
                return( TRUE);
            }
        }
        /*--- �e�[�u�� ---*/
        else if ( mfcchk->nn == 2){
            if( NULL != memchr( mfcchk->bat, L'\\', 4)){
                len1 = wcslen( mfcchk->bat);
                len2 = wcslen( wcsrchr( mfcchk->bat, L'\\'));
                len = len1 - len2 + 1;
                memcpy( dir, mfcchk->bat, len);
            }
            else{
                wcscpy( mfcchk->bat, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"�o�b�`�t�@�C����I��")){
                return( TRUE);
            }
        }
    }
    return( change_openfile( filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  �t�@�C�����J��
-----------------------------------------------------------------------*/
Private bool change_openfile( WCharCP fileNm)
{
    /*--- �e�[�u���t�@�C���� ---*/
    if( mfcchk->nn == 1){
        wcscpy( mfcchk->table, fileNm);
    }

    /*--- �o�b�`�t�@�C���� ---*/
    else if( mfcchk->nn == 2){
        wcscpy( mfcchk->bat, fileNm);
    }

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
