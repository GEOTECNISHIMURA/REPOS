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

/*-	プロタイプ	-*/
bool   unload(UnloadProgramReason unloadType);
int	change( WChar  *fileNm);
bool	change_getfile( WChar *tempFileName);
bool	change_openfile( WCharCP fileNm);


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

	/*--------- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- 広域変数の取得 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&layset",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlOutput_error(L"広域変数が取得できません");
    }
    layset = (LAYSET*)value.val.valPointer;

    change( fileName);
}



/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}



/*-----------------------------------------------------------------------
**	関数
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
**	ファイル名フルパスを取得
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
        	
            /*--- テーブルファイルオープン ---*/
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
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイルを選択")){
            	return( TRUE);
            }
        }
        else if ( layset->nn == 2){
            /*--- テーブルファイルオープン ---*/
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
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.idx", dir, L"バッチファイルを選択")){
                return( TRUE);
            }
        }
        else if ( layset->nn == 3){
            /*--- テーブルファイルオープン ---*/
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
            if ( mdlDialog_fileOpen(filePath, NULL, 0, NULL, L"*.cel", dir, L"セルライブラリを選択")){
                return( TRUE);
            }
        }
    }
    return( change_openfile( filePath.GetName()));
}



/*-----------------------------------------------------------------------
**	ファイルを開く
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