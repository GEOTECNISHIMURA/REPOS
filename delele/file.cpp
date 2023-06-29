/*-----------------------------------------------------------------------*/
/*-	        FILE.MC							-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "delele.h"


FILE	*fp;
DELSET  *delset;
int	 len, len1, len2, exitStatus;
WChar	 fileName[MAXFILELENGTH];
WChar	 full[256], dir[128];
#ifndef Private
#define Private
#endif

/*-	プロトタイプ	-*/
bool unload(UnloadProgramReason unloadType);
void reload(int argc, WCharCP argv[]);
Private int	change(WChar  *fileName);

//Private boolean change_getfile(char *tempFileName);
//Private boolean change_openfile(char *fileName);
Private bool change_getfile(WChar *tempFileName);
Private bool change_openfile(WCharCP fileName);

/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;
    //#pragma		resourceID	FILE_ID

	/*--- アンロード関数セット ---------------*/
	//mdlSystem_setFunction(SYSTEM_UNLOAD_PROGRAM, unload);
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	SystemCallback::SetReloadProgramFunction(reload);


    /*--------- 広域変数の取得 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&delset",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR ) != SUCCESS ){
        mdlOutput_error(L"広域変数が取得できません");
    }
    delset = (DELSET*)value.val.valPointer;

    change(fileName);
}

/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{

	return(SUCCESS);
}


void reload(int argc, WCharCP argv[])
{
	mdlState_clear();
	mdlState_startDefaultCommand();
	change(fileName);
}


/*-----------------------------------------------------------------------
**	関数
-----------------------------------------------------------------------*/
Private	int	change(WChar *fileName)
{
    if ( !change_getfile( fileName) ){
        return( SUCCESS);
    }
    else{
        return( ERROR);
    }
}


/*-----------------------------------------------------------------------
**	ファイル名フルパスを取得
-----------------------------------------------------------------------*/
Private	bool change_getfile( WChar *tempFileName)
{
	int ret;
	BeFileName	filePath;
    if ( wcslen( tempFileName) ){
        wcscpy( fileName, tempFileName);
		filePath.SetName(tempFileName);
    }
    else{
        /*--- テーブル ---*/
        if ( delset->nn == 1){
            if( NULL != wmemchr( delset->table, L'\\', 4)){
                len1 = wcslen( delset->table);
                len2 = wcslen( wcsrchr( delset->table, L'\\'));
                len = len1 - len2 + 1;
				memcpy( dir, delset->table, len);
            }
            else{
                wcscpy( delset->table, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
			if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイル")){
                return(TRUE);
            }
        }
        /*--- テーブル ---*/
        else if ( delset->nn == 2){
            if( NULL != wmemchr( delset->bat, L'\\', 4)){
                len1 = (int)wcslen( delset->bat);
                len2 = (int)wcslen(wcsrchr( delset->bat, L'\\'));
                len = len1 - len2 + 1;
				memcpy( dir, delset->bat, len);
            }
            else{
                wcscpy( delset->bat, L"c:\\");
                wcscpy( dir, L"c:\\");
            }
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"バッチファイル")){
                return( TRUE);
            }
        }
    }
    return( change_openfile( filePath.GetName()) );
}


/*-----------------------------------------------------------------------
**	ファイルを開く
-----------------------------------------------------------------------*/
Private	bool change_openfile(WCharCP fileName)
{
	int ret;
    if ( delset->nn == 1){
        wcscpy( delset->table, fileName);
    }
    if ( delset->nn == 2){
        wcscpy( delset->bat, fileName);
    }

    mdlInput_sendKeyin( L"mdl unload FILEA", 1, INPUTQ_EOQ, TD_FILE);
	
    return( FALSE);
}
