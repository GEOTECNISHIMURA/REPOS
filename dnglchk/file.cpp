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

/*- プロトタイプ    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change( WChar  *fileName);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileName);


/*-----------------------------------------------------------------------
**  メイン
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

    /*--------- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- 広域変数の取得 -------------*/
    if( mdlCExpression_getValue( &value, &result, "&dnglchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"広域変数が取得できません");
    }
    dnglchk = (DNGLCHK*)value.val.valPointer;

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
    if( ! change_getfile( fileName)){
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
	if (wcslen(tempFileName)) {
		wcscpy(fileName, tempFileName);
		filePath.SetName(tempFileName);
	}
    else{
        /*--- テーブル ---*/
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
        if( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイルを選択")){
            return( TRUE);
        }
    }
    return( change_openfile(filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  ファイルを開く
-----------------------------------------------------------------------*/
Private bool change_openfile( WCharCP fileName)
{
    /*--- テーブルファイル名 ---*/
    wcscpy( dnglchk->table, fileName);

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
