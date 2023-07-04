/*-----------------------------------------------------------------------*/
/*-	        FILE.MC							-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <Mstn\MdlApi\MdlApi.h>
#include "txcell.h"

FILE	*fp;
TXSET 	*txset;
int	 	exitStatus;
WChar	fileName[MAXFILELENGTH];

#ifndef Private
#define Private
#endif

/*-	プロタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int		change( WChar  *fileName);
Private bool 	change_getfile( WChar *tempFileName);
Private bool 	change_openfile( WCharCP fileNamea);
Private	void	get_dir( WChar *dir, WChar *full);

/*-----------------------------------------------------------------------
**	背景地図テーブルファイル変更
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

   /*--- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&txset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlDialog_openInfoBox( L"広域変数が取得できません");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }
   txset = (TXSET*)value.val.valPointer;

   change( fileName);
}

/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
Private bool   unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	関数
-----------------------------------------------------------------------*/
Private	int	change( WChar  *fileNamea)
{
   if ( !change_getfile( fileNamea)){
      return( SUCCESS);
   }
   else{
      return( ERROR);
   }
}


/*-----------------------------------------------------------------------
**	ファイル名フルパスを取得
-----------------------------------------------------------------------*/
Private	bool	change_getfile( WChar *tempFileName)
{
WChar	dir[255];
BeFileName	filePath;


   if ( wcslen( tempFileName)){
      wcscpy( fileName, tempFileName);
	  filePath.SetName(tempFileName);
   }
   else{
      /*--- idxﾌｧｲﾙ選択の場合 ---*/
      if ( txset->nn == 1){
         get_dir( dir, txset->idx);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.idx",
                                  dir, L".idxﾌｧｲﾙ選択")){
            return( TRUE);
         }
      }

      /*--- ｼｰﾄﾞﾌｧｲﾙ選択の場合 ---*/
      else if ( txset->nn == 2){
         get_dir( dir, txset->seed);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.dgn",
                                  dir, L"ｼｰﾄﾞﾌｧｲﾙ選択")){
            return( TRUE);
         }
      }

      /*--- ｾﾙﾗｲﾌﾞﾗﾘ選択の場合 ---*/
      else if ( txset->nn == 3){
         get_dir( dir, txset->cellib);
         if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.cel",
                                  dir, L"ｾﾙﾗｲﾌﾞﾗﾘ選択")){
            return( TRUE);
         }
      }

      /*--- 基準点ﾌｧｲﾙ選択の場合 ---*/
      else{
         get_dir( dir, txset->file);

         /*--- .simﾌｧｲﾙ選択の場合 ---*/
         if ( txset->form == 2){
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.sim",
                                     dir, L".simﾌｧｲﾙ選択")){
               return( TRUE);
            }
         }

         /*--- .datﾌｧｲﾙ選択の場合 ---*/
         else{
            if ( mdlDialog_fileOpen( filePath, NULL, 0, NULL, L"*.dat",
                                     dir, L".datﾌｧｲﾙ選択")){
               return( TRUE);
            }
         }
      }
   }
   return( change_openfile( filePath.GetName()));
}


/*-----------------------------------------------------------------------
**	ファイルを開く
-----------------------------------------------------------------------*/
Private bool change_openfile(WCharCP fileNamea)
{
   /*--- idxﾌｧｲﾙ選択の場合 ---*/
   if ( txset->nn == 1){
      wcscpy( txset->idx, fileNamea);
   }

   /*--- ｼｰﾄﾞﾌｧｲﾙ選択の場合 ---*/
   else if ( txset->nn == 2){
      wcscpy( txset->seed, fileNamea);
   }

   /*--- ｾﾙﾗｲﾌﾞﾗﾘ選択の場合 ---*/
   else if ( txset->nn == 3){
      wcscpy( txset->cellib, fileNamea);
   }

   /*--- 基準点ﾌｧｲﾙ選択の場合 ---*/
   else{
      wcscpy( txset->file, fileNamea);
   }

   mdlInput_sendKeyin( L"mdl unload file", 1, INPUTQ_EOQ, NULL);
   return( FALSE);
}


/*-----------------------------------------------------------------------
**	最初に開くパス設定
-----------------------------------------------------------------------*/
Private	void  get_dir( WChar *dir, WChar *full)
{
int		len, len1, len2;

   /*--- 最初に開くパス設定 ---*/
   wcscpy( dir, L"");
   len1 = (int)wcslen( full);
   if ( wcsrchr( full, L'\\') != NULL){
      len2 = (int)wcslen( wcsrchr( full, L'\\'));
      len = len1 - len2 + 1;
      wmemcpy( dir, full, len);
   }
}
