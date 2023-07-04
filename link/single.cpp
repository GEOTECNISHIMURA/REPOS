/*-----------------------------------------------------------------------*/
/*-	１要素のみリンクさせる			-*/
/*-									-*/
/*-	        SINGLE.MC				-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "link.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

MSElement		ele;
MSElementDescr  *elmDP1, *elmDP2;
int				exitStatus;
WChar			zumenname[128], *aa, *token;
WChar			zumen[128], file[128], info[128], ins[128];
LINKSET			*linkset;

#ifndef Private
#define Private
#endif


/*-	プロトタイプ	-*/
bool unload(UnloadProgramReason unloadType);
Private void	startModifyCommand( void);
Private int	attach( Dpoint3d *inpoint,  int view);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

CExprResult		result;
CExprValue		value;
RscFileHandle   Rsc;

   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error(L"SINGLEのリソースファイルが開きません");
	  mdlSystem_exit(exitStatus, 1);
   }

   /*--- 広域変数の取得 -------------*/
   if ( mdlCExpression_getValue( &value, &result, "&linkset",
      VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
      mdlOutput_error(L"広域変数が取得できません");
   }
   linkset = (LINKSET*)value.val.valPointer;

   /*--- アンロード関数セット ---*/
   //mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   startModifyCommand( );

}


/*-----------------------------------------------------------------------
**	アンロード関数
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	startModifyCommand
-----------------------------------------------------------------------*/
Private void	startModifyCommand( )
{
   /*--- リセットボタン、データボタンを押した時 ---*/
   mdlState_startModifyCommand( (StateFunc_Reset)startModifyCommand, (MdlFunc_Accept)attach,
                                NULL, NULL, NULL, NULL, NULL, TRUE, 0);
   mdlLocate_noElemNoLocked( );
   mdlLocate_normal( );
   mdlLocate_init( );
}


/*-----------------------------------------------------------------------
**	結合
-----------------------------------------------------------------------*/
Private int	attach( Dpoint3d *inpoint,  int view)
{
DatabaseLink	*dblink;
MS_sqlda		sqlda;
ULong			filePos;
UInt32          mslink;
int				len, linknum, zumennum;


   /*--- ファイル位置 ---*/
   filePos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);
   mdlElement_read( &ele, MASTERFILE, filePos);
   mdlDB_extractLinkages( &dblink, &linknum, &ele);

   /*--- リンク済み要素の場合 ---*/
   if ( linknum > 0){
      mdlDialog_openInfoBox( L"この要素は既にリンクされています");
	  mdlSystem_exit(exitStatus, 1);
	  return 0;
   }

   /*--- 図面名 ---*/
   wcscpy( zumenname, tcb->dgnfilenm);
   aa = wcsrchr( zumenname, '\\');
   token = wcstok( aa, L".");
   wcscpy( file, &aa[1]);
   len = (int)wcslen( file);

   /*--- 図面番号 ---*/
   if ( linkset->zumen == -1){
      wmemcpy( zumen, file + len - linkset->keta, linkset->keta);
   }

   /*--- 結合モード複製 ------*/
   mdlDB_activeLinkageMode( L"duplicate");

   /*--- データベース構造を読み込む ---*/
   if ( mdlDB_describeTable( &sqlda, linkset->table) != SUCCESS){
      mdlDialog_openInfoBox(L"データベーステーブルが読み込めません");
      mdlSystem_exit( exitStatus, 1);
	  return 0;
   }

   zumennum = _wtoi(zumen);
   /*--- 最大MSLINK＋１追加の場合 ---*/
   if ( linkset->kind == 2){
      mdlDB_largestMslink( &mslink, linkset->table);
      mslink = mslink + 1;

      /*--- SQL文（図面番号出力の場合） ---*/
      if ( linkset->zumen == -1){
         swprintf( ins, L"insert into %s (mslink, ZumenNum) values (%ld, '%ld')", linkset->table, mslink, zumennum);
      }

      /*--- SQL文（図面番号出力しない場合） ---*/
      else{
         swprintf( ins, L"insert into %s (mslink) values (%ld)", linkset->table, mslink);
      }

      /*--- データベースに追加する ---*/
      if ( mdlDB_processSQL( ins) != SUCCESS){
         mdlDialog_openInfoBox(L"データベースに書き込めません");
         mdlSystem_exit( exitStatus, 1);
		 return 0;
      }

      /*--- MSLINKのアタッチ ------*/
      attach_mslink_filePos( linkset->table, mslink, &filePos);
   }

   /*--- 設定して追加の場合 ---*/
   if ( linkset->kind == 3){
      /*--- SQL文（図面番号出力の場合） ---*/
      if ( linkset->zumen == -1){
         swprintf( ins, L"insert into %s (mslink, ZumenNum) values (%ld, '%ld')", linkset->table, linkset->mslink, zumennum);
      }

      /*--- SQL文（図面番号出力しない場合） ---*/
      else{
         swprintf( ins, L"insert into %s (mslink) values (%ld)", linkset->table, linkset->mslink);
      }

      /*--- データベースに追加する ---*/
      if ( mdlDB_processSQL( ins) != SUCCESS){
         mdlDialog_openInfoBox(L"データベースに書き込めません");
         mdlSystem_exit( exitStatus, 1);
		 return 0;
      }

      /*--- MSLINKのアタッチ ------*/
      attach_mslink_filePos( linkset->table, linkset->mslink, &filePos);
   }

/*--- DGNのみ追加の場合 ---*/
/*
if ( linkset->kind == 4){
   memset( &props, 0, sizeof( LinkProps));
   mdlDB_buildLink( &link, &linkLength, ODBC_LINKAGE, &props, linkset->table, linkset->mslink, 1);
   mdlElmdscr_read( &elmDP1, filePos, 0, 0, NULL);
   mdlElmdscr_duplicate( &elmDP2, elmDP1);
   mdlElmdscr_appendAttributes( &elmDP2, linkLength, &link);
   mdlElmdscr_rewrite( elmDP2, elmDP1, filePos);
   mdlElmdscr_freeAll( &elmDP1);
   mdlElmdscr_freeAll( &elmDP2);
}
*/

   /*--- 既存MSLINKの場合 ---*/
   if ( linkset->kind == 4){
      /*--- MSLINKのアタッチ ------*/
      swprintf( ins, L"select * from %s where mslink=%ld", linkset->table, linkset->mslink);
      attach_mslink_filePos( linkset->table, linkset->mslink, &filePos);
   }
}
