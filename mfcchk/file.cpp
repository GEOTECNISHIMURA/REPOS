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

/*- プロトタイプ    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change( WChar  *fileName);
Private bool change_getfile( WChar *tempFileName);
Private bool change_openfile( WCharCP fileName);


/*-----------------------------------------------------------------------
**  メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;


	/*--------- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


    /*--------- 広域変数の取得 -------------*/
    if( mdlCExpression_getValue( &value, &result, "&mfcchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"広域変数が取得できません");
    }
    mfcchk = (MFCCHK*)value.val.valPointer;

    change( fileName);
}


/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  関数
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
**  ファイル名フルパスを取得
-----------------------------------------------------------------------*/
Private bool change_getfile( WChar *tempFileName)
{
	BeFileName	filePath;

    if( wcslen( tempFileName)){
        wcscpy( fileName, tempFileName);
    }

    else{
        /*--- テーブル ---*/
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
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイルを選択")){
                return( TRUE);
            }
        }
        /*--- テーブル ---*/
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
            if( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"バッチファイルを選択")){
                return( TRUE);
            }
        }
    }
    return( change_openfile( filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  ファイルを開く
-----------------------------------------------------------------------*/
Private bool change_openfile( WCharCP fileNm)
{
    /*--- テーブルファイル名 ---*/
    if( mfcchk->nn == 1){
        wcscpy( mfcchk->table, fileNm);
    }

    /*--- バッチファイル名 ---*/
    else if( mfcchk->nn == 2){
        wcscpy( mfcchk->bat, fileNm);
    }

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
