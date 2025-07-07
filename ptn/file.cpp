/*-----------------------------------------------------------------------*/
/*-	        FILE.MC														-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>

#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include "ptn.h"

FILE        *fp;
int         len, len1, len2, exitStatus;
WChar        fileName[MAXFILELENGTH];
WChar        dir[128];
PTN         *ptn;

/*-	�v���g�^�C�v	-*/
bool    unload(UnloadProgramReason unloadType);
int	change(WChar  *fileNamea);
bool	change_getfile( WChar *tempFileName);
bool	change_openfile( WCharCP fileNamea);



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

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


    /*--------- �L��ϐ��̎擾 -------------*/
    if( mdlCExpression_getValue( &value, &result, "&ptn",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
    }
    ptn = (PTN*)value.val.valPointer;

	change(fileName);
    return;
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool    unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�֐�
-----------------------------------------------------------------------*/
int	change(WChar  *fileNamea)
{
    if( ! change_getfile( fileNamea)){
        return( SUCCESS);
    }
    else{
        return( ERROR);
    }
}


/*-----------------------------------------------------------------------
**	�t�@�C�����t���p�X���擾
-----------------------------------------------------------------------*/
bool  change_getfile( WChar *tempFileName)
{
	BeFileName		full;
    if( wcslen( tempFileName)){
        wcscpy( fileName, tempFileName);
		full.SetName(tempFileName);
    }

    else{
        /*--- �e�[�u�� ---*/
        if( ptn->nn == 1){

            if( NULL != memchr( ptn->table, L'\\', 4)){
                len1 = wcslen( ptn->table);
                len2 = wcslen( wcsrchr( ptn->table, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, ptn->table, len);
            }
            else{
                wcscpy( ptn->table, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.tbl", dir, L"�e�[�u���t�@�C����I��")){
                return( TRUE);
            }
        }
        /*--- �e�[�u�� ---*/
        else if( ptn->nn == 2){
            if( NULL != memchr( ptn->bat, L'\\', 4)){
                len1 = wcslen( ptn->bat);
                len2 = wcslen( wcsrchr( ptn->bat, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, ptn->bat, len);
            }
            else{
                wcscpy( ptn->bat, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.idx", dir, L"�o�b�`�t�@�C����I��")){
                return( TRUE);
            }
        }
        /*--- �e�[�u�� ---*/
        else if( ptn->nn == 3){
            if( NULL != memchr( ptn->cel, L'\\', 4)){
                len1 = wcslen( ptn->cel);
                len2 = wcslen( wcsrchr( ptn->cel, L'\\'));
                len = len1 - len2 + 1;
                wmemcpy( dir, ptn->cel, len);
            }
            else{
                wcscpy( ptn->cel, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.cel", dir, L"�Z�����C�u������I��")){
                return( TRUE);
            }
        }
    }
    return( change_openfile(full.GetName()));
}


/*-----------------------------------------------------------------------
**	�t�@�C�����J��
-----------------------------------------------------------------------*/
bool	change_openfile(WCharCP fileNamea)
{
    /*--- �e�[�u���t�@�C���� ---*/
    if( ptn->nn == 1){
        wcscpy( ptn->table, fileNamea);
    }

    /*--- �o�b�`�t�@�C���� ---*/
    else if( ptn->nn == 2){
        wcscpy( ptn->bat, fileNamea);
    }

    /*--- �Z�����C�u������ ---*/
    else if( ptn->nn == 3){
        wcscpy( ptn->cel, fileNamea);
    }

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
