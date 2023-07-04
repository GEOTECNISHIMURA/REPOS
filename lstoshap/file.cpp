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
/*- プロトタイプ    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change(WChar  *fileNamea);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileNamea);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C"  void MdlMain
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
	if (mdlCExpression_getValue(&value, &result, "&glo",
		VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS) {
		mdlDialog_openInfoBox(L"広域変数が取得できません");
		return;
	}
    glo = (GLOBAL*)value.val.valPointer;

    change( fileName);
}


/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**  関数
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
**	ファイル名フルパスを取得
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
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"バッチファイルを選択")){
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
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイルを選択")){
                return( TRUE);
            }
        }
    }
    return( change_openfile(filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  ファイルを開く
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