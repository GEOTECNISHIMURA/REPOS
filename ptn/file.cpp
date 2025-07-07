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

/*-	プロトタイプ	-*/
bool    unload(UnloadProgramReason unloadType);
int	change(WChar  *fileNamea);
bool	change_getfile( WChar *tempFileName);
bool	change_openfile( WCharCP fileNamea);



/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);


    /*--------- 広域変数の取得 -------------*/
    if( mdlCExpression_getValue( &value, &result, "&ptn",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"広域変数が取得できません");
    }
    ptn = (PTN*)value.val.valPointer;

	change(fileName);
    return;
}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool    unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	関数
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
**	ファイル名フルパスを取得
-----------------------------------------------------------------------*/
bool  change_getfile( WChar *tempFileName)
{
	BeFileName		full;
    if( wcslen( tempFileName)){
        wcscpy( fileName, tempFileName);
		full.SetName(tempFileName);
    }

    else{
        /*--- テーブル ---*/
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
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.tbl", dir, L"テーブルファイルを選択")){
                return( TRUE);
            }
        }
        /*--- テーブル ---*/
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
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.idx", dir, L"バッチファイルを選択")){
                return( TRUE);
            }
        }
        /*--- テーブル ---*/
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
            if( mdlDialog_fileOpen( full, NULL, 0, NULL, L"*.cel", dir, L"セルライブラリを選択")){
                return( TRUE);
            }
        }
    }
    return( change_openfile(full.GetName()));
}


/*-----------------------------------------------------------------------
**	ファイルを開く
-----------------------------------------------------------------------*/
bool	change_openfile(WCharCP fileNamea)
{
    /*--- テーブルファイル名 ---*/
    if( ptn->nn == 1){
        wcscpy( ptn->table, fileNamea);
    }

    /*--- バッチファイル名 ---*/
    else if( ptn->nn == 2){
        wcscpy( ptn->bat, fileNamea);
    }

    /*--- セルライブラリ名 ---*/
    else if( ptn->nn == 3){
        wcscpy( ptn->cel, fileNamea);
    }

    mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL );
    return( FALSE);
}
