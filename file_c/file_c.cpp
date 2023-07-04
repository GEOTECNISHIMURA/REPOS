#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-		ファイル設定		    -*/
/*-					    		-*/
/*-		file_c			        -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "file_c.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
int				exitStatus;
FILE_C     	    file_c;

#ifndef Private
#define Private
#endif
/*-	フック関数		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_cell_hook( DialogItemMessage *dimP);
Private void	push_cell_hook( DialogItemMessage *dimP);
Private void	push_bat_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_TOGGLE_CELL, (PFDialogHook)toggle_cell_hook},
   { HOOKID_PUSH_CELL,   (PFDialogHook)push_cell_hook  },
   { HOOKID_PUSH_BAT,    (PFDialogHook)push_bat_hook   },
   { HOOKID_PUSH_OK,     (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_END,    (PFDialogHook)push_end_hook   },
};

/*-	プロトタイプ	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private	int 	syori( void);


/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	char            structName[128];
	char            variableName[128];
	SymbolSet		*setP;
	BeFileName		full;

    chk_kigen( );

   /*--- リソースファイルのオープン ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"file_c のリソースファイルが開きません");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- アンロード関数のセット ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- フック関数の使用宣言 ---*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C式定義初期設定 ---*/
   sprintf(structName, "FILE_C_INFO");
   sprintf(variableName, "file_c");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &file_c);

   /*--- テーブルファイルオープン ---*/
   mdlFile_find( &full, L"file_c.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%ld\n", &file_c.reffile);
      fwscanf( fp, L"%ld\n", &file_c.viewtop);
      fwscanf( fp, L"%ld\n", &file_c.textnodes);
      fwscanf( fp, L"%ld\n", &file_c.levels);
      fwscanf( fp, L"%ld\n", &file_c.tog_cell);
      fwscanf( fp, L"%ls\n", file_c.cellib);
      fwscanf( fp, L"%ld\n", &file_c.filedesign);
      fwscanf( fp, L"%ls\n", file_c.idxname);
	  fclose(fp);
   }
   else{
      file_c.reffile    = 0;
      file_c.viewtop    = 0;
      file_c.textnodes  = 0;
      file_c.levels     = 0;
      file_c.tog_cell   = 0;
	  swprintf(file_c.cellib, L"c:\\\0");
      file_c.filedesign = 0;
	  swprintf(file_c.idxname, L"c:\\\0");
   }
   

   /*--- ダイアログボックスオープン ---*/
   db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"MAIN ダイアログボックスが開けません");
   }

   /*--- １ファイルの場合 ---*/
   if ( file_c.tog_cell != -1){
      mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
   }
}


/*-----------------------------------------------------------------------
**	アンロード関数 
-----------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	トグルボタン（セルライブラリ）フック関数
-----------------------------------------------------------------------*/
Private void	toggle_cell_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- バッチの場合 ---*/
         if ( file_c.tog_cell == -1){
            mdlDialog_itemSetEnabledState( db_p, 5, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, TRUE, FALSE);
         }
         /*--- １ファイルの場合 ---*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 6, FALSE, FALSE);
         }
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
| name		プッシュボタン（セルライブラリ）                            |
+----------------------------------------------------------------------*/
Private void    push_cell_hook( DialogItemMessage *dmP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.cel");
         swprintf( title,  L"セルライブラリを選択");

         mdlFile_parseName( file_c.cellib, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(file_c.cellib, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dmP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
| name		プッシュボタン（バッチファイル）                            |
+----------------------------------------------------------------------*/
Private void    push_bat_hook( DialogItemMessage *dmP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title, L"idxファイルを選択");

         mdlFile_parseName( file_c.idxname, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(file_c.idxname, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dmP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*----------------------------------------------------------------------+
| name		プッシュボタン (実行)                                       |
+----------------------------------------------------------------------*/
Private void    push_ok_hook( DialogItemMessage *dmP)
{
	BeFileName		full;
	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType){
		case DITEM_MESSAGE_BUTTON:
            /*--- 設定値保存 ---*/
            mdlFile_find( &full, L"file_c.tbl", L"MS_DEF", NULL);
            fp = _wfopen(full.GetWCharCP(), L"w");
                fwprintf( fp, L"%ld\n", file_c.reffile);
                fwprintf( fp, L"%ld\n", file_c.viewtop);
                fwprintf( fp, L"%ld\n", file_c.textnodes);
                fwprintf( fp, L"%ld\n", file_c.levels);
                fwprintf( fp, L"%ld\n", file_c.tog_cell);
                fwprintf( fp, L"%ls\n", file_c.cellib);
                fwprintf( fp, L"%ld\n", file_c.filedesign);
                fwprintf( fp, L"%ls\n", file_c.idxname);
            fclose( fp);

			syori( );
		    break;

		default:
			dmP -> msgUnderstood = TRUE;
			break;
	}
}


/*-----------------------------------------------------------------------
**	プッシュボタン(終了）フック関数
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	処理
-----------------------------------------------------------------------*/
Private	int     syori( void)
{
FILE	    *fin;
WChar	    info[512], string[256];
RotMatrix   outMatrix;
int	        ii;
short	    levels[4], viewlist[8];
ULong       fileMask[8];
WChar	    cellfile[1000];
BeFileName  filename, inputname;
DgnModelRefListP    modelList;

	levels[0] = levels[1] = levels[2] = 0xFFFF;
	levels[3] = 0x7FFF;

	for( ii=0; ii<8; ii++){
		viewlist[ii] = 0;
	}
	viewlist[0] = 1;

	for( ii=0; ii<8; ii++){
       fileMask[ii] = 0;
    }
    fileMask[0] = 1;

	if ( ( fin = _wfopen(file_c.idxname, L"r")) == NULL){
        swprintf( info, L"idxファイル %ls が読めません ", file_c.idxname);
        mdlDialog_openInfoBox( info);
        return( ERROR);
    }
	while( EOF != fwscanf( fin, L"%ls", string)){
		mdlSystem_newDesignFile( string);

        /*--- 参照ファイルデタッチ ---*/
		if ( file_c.reffile == -1){
			mdlRefFile_detach(REFERENCE_All_References);
		}

        /*--- ビュートップ ---*/
		if ( file_c.viewtop == -1){
			for( ii=0; ii<8; ii++){
				mdlView_turnOff( ii);
			}
			mdlView_turnOn( 0);
			mdlRMatrix_rotate( &outMatrix, NULL, 0, 0, 0);
			mdlView_rotateToRMatrix( &outMatrix, 0);
			//mdlView_fit( 0, fileMask);
			mdlModelRefList_create(&modelList);
			mdlModelRefList_add(modelList, MASTERFILE);
			//mdlView_fit(tcb->lstvw, modelList);
			mdlView_fit(0, modelList);
			mdlModelRefList_free(&modelList);

		}

        /*--- 文字ノードON/OFF ---*/
		if ( file_c.textnodes == -1){
			for( ii=0; ii<8; ii++){
				mdlView_setDisplayControl( VIEWCONTROL_TEXT_NODES, ii, TRUE);
			}
		}
		else{
			for( ii=0; ii<8; ii++){
				mdlView_setDisplayControl( VIEWCONTROL_TEXT_NODES, ii, FALSE);
			}
		}

        /*--- 全画層をON ---*/
		if ( file_c.levels == -1){
			mdlView_setLevels( TRUE, levels, viewlist, TRUE);
		}

        /*--- セルライブラリをアタッチ ---*/
		if ( file_c.tog_cell == -1){
			filename.SetName(cellfile);
			inputname.SetName(file_c.cellib);
			mdlCell_attachLibrary(filename, &inputname, NULL, TRUE);
		}

        /*--- 設定値保存 ---*/
		if ( file_c.filedesign == -1){
			mdlSystem_fileDesign( );
		}
	}
	fclose( fin);
    return( SUCCESS);
}
/*-----------------------------------------------------------------------
**	ダイアログフック関数
-----------------------------------------------------------------------*/
Private	void	dialog_hook(DialogMessage *dmP)
{
	DialogBoxRsc	*dbr_P;

	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType) {
	case DIALOG_MESSAGE_CREATE:
		dbr_P = dmP->u.create.dialogBoxRP;
		dmP->u.create.interests.updates = TRUE;
		dmP->u.create.interests.mouses = TRUE;
		dmP->u.create.interests.dialogFocuses = TRUE;
		dmP->u.create.interests.itemFocuses = TRUE;
		break;

	case DIALOG_MESSAGE_DESTROY:
		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
		break;

	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}