/*-----------------------------------------------------------------------*/
/*-         FILE.MC                                                     -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "dnglchk.h"

FILE        *fp;
int         len, len1, len2, exitStatus;
WChar        fileName[MAXFILELENGTH];
WChar        dir[128];
DNGLCHK     *dnglchk;
#ifndef Private
#define Private
#endif

/*- �v���g�^�C�v    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change( WChar  *fileName);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileName);


/*-----------------------------------------------------------------------
**  ���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
int             argc,
WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;
	
   // #pragma         resourceID    FILE_ID

    /*--------- �A�����[�h�֐��Z�b�g ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- �L��ϐ��̎擾 -------------*/
    if( mdlCExpression_getValue( &value, &result, "&dnglchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
    }
    dnglchk = (DNGLCHK*)value.val.valPointer;

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
    if( ! change_getfile( fileName)){
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
	if (wcslen(tempFileName)) {
		wcscpy(fileName, tempFileName);
		filePath.SetName(tempFileName);
	}
    else{
        /*--- �e�[�u�� ---*/
        if( NULL != wmemchr( dnglchk->table, L'\\', 4)){
            len1 = wcslen( dnglchk->table);
            len2 = wcslen( wcsrchr( dnglchk->table, '\\'));
            len = len1 - len2 + 1;
            memcpy( dir, dnglchk->table, len);
        }
        else{
            wcscpy( dnglchk->table, L"c:\\");
            wcscpy( dir, L"c:\\");
        }
        if( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"�e�[�u���t�@�C����I��")){
            return( TRUE);
        }
    }
    return( change_openfile(filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  �t�@�C�����J��
-----------------------------------------------------------------------*/
Private bool change_openfile( WCharCP fileName)
{
    /*--- �e�[�u���t�@�C���� ---*/
    wcscpy( dnglchk->table, fileName);

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
