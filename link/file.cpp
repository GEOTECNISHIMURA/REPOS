/*-----------------------------------------------------------------------*/
/*-         FILE.MC                                                     -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "link.h"

FILE        *fp;
int         exitStatus;
WChar        dir[128];
WChar        fileName[MAXFILELENGTH];
LINKSET  *linkset;
#ifndef Private
#define Private
#endif

/*- プロトタイプ    -*/
Private bool   unload(UnloadProgramReason unloadType);
Private int change(WChar  *fileNamea);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileNamea);


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
	if (mdlCExpression_getValue(&value, &result, "&linkset",
		VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS) {
		mdlDialog_openInfoBox(L"広域変数が取得できません");
	}
	linkset = (LINKSET*)value.val.valPointer;

	change(fileName);
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
**  ファイル名フルパスを取得
-----------------------------------------------------------------------*/
Private bool change_getfile(WChar *tempFileName)
{
	BeFileName	filePath;
	if (wcslen(tempFileName)) {
		wcscpy(fileName, tempFileName);
		filePath.SetName(tempFileName);
	}
	else {
		if (linkset->nn == 1) {
			if (mdlDialog_fileOpen(filePath, NULL, 0, NULL,
				L"*.tbl", NULL, L"ファイルリスト")) {
				return(TRUE);
			}
		}
		if (linkset->nn == 2) {
			if (mdlDialog_fileOpen(filePath, NULL, 0, NULL,
				L"*.idx", NULL, L"ファイルリスト")) {
				return(TRUE);
			}
		}
	}
	return(change_openfile(filePath.GetName()));
}


/*-----------------------------------------------------------------------
**  ファイルを開く
-----------------------------------------------------------------------*/
Private bool change_openfile(WCharCP fileNamea)
{
	/*--- テーブルファイル名 ---*/
	if (linkset->nn == 1) {
		wcscpy(linkset->layer, fileNamea);
	}
	if (linkset->nn == 2) {
		wcscpy(linkset->bat, fileNamea);
	}

	mdlInput_sendKeyin(L"mdl unload file", 1, INPUTQ_EOQ, NULL);
	return(FALSE);
}