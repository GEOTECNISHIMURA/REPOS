#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*-----------------------------------------------------------------------*/
/*-	セル、ライン、テキストを置き換える				-*/
/*-																		-*/
/*-	LAYCHG.MC															-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/*#include <check_kigen7.mc>*/
/*#include <lok.mc>*/
#include "laychg.h"
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
BeFileName		full;
int				ii, exitStatus, spro;
LAYSET			layset;


/*-	フック関数		-*/
void	laychg_dialog_hook( DialogMessage *dmP);
void	toggle_bat_hook( DialogItemMessage *dimP);
void	push_bat_hook( DialogItemMessage *dimP);
void	toggle_tbl_hook( DialogItemMessage *dimP);
void	option_type_hook( DialogItemMessage *dimP);
void	toggle_celltype_hook( DialogItemMessage *dimP);
void	toggle_scale_hook( DialogItemMessage *dimP);
void	toggle_angle_hook( DialogItemMessage *dimP);
void	toggle_th_hook( DialogItemMessage *dimP);
void	toggle_tw_hook( DialogItemMessage *dimP);
void	toggle_font_hook( DialogItemMessage *dimP);
void	push_tbl_hook( DialogItemMessage *dimP);
void	push_cel_hook( DialogItemMessage *dimP);
void	push_ok_hook( DialogItemMessage *dimP);
void	push_end_hook( DialogItemMessage *dimP);
 
DialogHookInfo	uHooks[]= {
    { HOOKID_LAYCHG_DIALOG,   (PFDialogHook)laychg_dialog_hook  },
    { HOOKID_TOGGLE_BAT,      (PFDialogHook)toggle_bat_hook     },
    { HOOKID_PUSH_BAT,        (PFDialogHook)push_bat_hook       },
    { HOOKID_TOGGLE_TBL,      (PFDialogHook)toggle_tbl_hook     },
    { HOOKID_OPTION_TYPE,     (PFDialogHook)option_type_hook    },
    { HOOKID_TOGGLE_CELLTYPE, (PFDialogHook)toggle_celltype_hook},
    { HOOKID_TOGGLE_SCALE,    (PFDialogHook)toggle_scale_hook   },
    { HOOKID_TOGGLE_ANGLE,    (PFDialogHook)toggle_angle_hook   },
    { HOOKID_TOGGLE_TH,       (PFDialogHook)toggle_th_hook      },
    { HOOKID_TOGGLE_TW,       (PFDialogHook)toggle_tw_hook      },
    { HOOKID_TOGGLE_FONT,     (PFDialogHook)toggle_font_hook    },
    { HOOKID_PUSH_TBL,        (PFDialogHook)push_tbl_hook       },
    { HOOKID_PUSH_CEL,        (PFDialogHook)push_cel_hook       },
    { HOOKID_PUSH_OK,         (PFDialogHook)push_ok_hook        },
    { HOOKID_PUSH_END,        (PFDialogHook)push_end_hook       },
};

/*-	プロトタイプ	-*/
void	new_design_file( void);
bool	unload(UnloadProgramReason unloadType);
void	task_unload( void);
int    add_combo_lvList_a(MSDialogP db_p, long comboID);

/*-----------------------------------------------------------------------
**	メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];
	/*#if LOCK == 1
		main_lok( );
	#endif
	*/
		chk_kigen( );


	/*--- リソースファイルのオープン -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"LAYCHGのリソースファイルが開きません");
		mdlSystem_exit(exitStatus, 1);
	}

	/*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	/*--- フック関数の使用宣言 -----------*/
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	/*--- C式定義初期設定 -------------*/
	sprintf(structName, "LAYSET_INFO");
	sprintf(variableName, "layset");
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName, &layset);



	/*--- テーブルファイルのフルパス取得 -------------*/
	mdlFile_find(&full, L"laychg.tbl", L"MS_DEF", NULL);

	if ((fp = _wfopen(full.GetWCharCP(), L"r")) != NULL) {
		fwscanf(fp, L"%d", &layset.ttbl);
		fwscanf(fp, L"%d", &layset.type);
		fwscanf(fp, L"%ls", layset.ocell);
		fwscanf(fp, L"%d", &layset.olv);
		fwscanf(fp, L"%d", &layset.oco);
		fwscanf(fp, L"%d", &layset.olc);
		fwscanf(fp, L"%d\n", &layset.owt);
		fwscanf(fp, L"%ls\n", layset.ncell);
		fwscanf(fp, L"%d\n", &layset.nlv);
		fwscanf(fp, L"%d\n", &layset.nco);
		fwscanf(fp, L"%d\n", &layset.nlc);
		fwscanf(fp, L"%d\n", &layset.nwt);
		fwscanf(fp, L"%d\n", &layset.tscale);
		fwscanf(fp, L"%d\n", &layset.tangle);
		fwscanf(fp, L"%d\n", &layset.tth);
		fwscanf(fp, L"%d\n", &layset.ttw);
		fwscanf(fp, L"%d\n", &layset.tfont);
		fwscanf(fp, L"%lf\n", &layset.scale);
		fwscanf(fp, L"%lf\n", &layset.angle);
		fwscanf(fp, L"%lf\n", &layset.th);
		fwscanf(fp, L"%lf\n", &layset.tw);
		fwscanf(fp, L"%d\n", &layset.font);
		fwscanf(fp, L"%ls\n", layset.file);
		fwscanf(fp, L"%d\n", &layset.check);
		fwscanf(fp, L"%d\n", &layset.tbat);
		fwscanf(fp, L"%ls\n", layset.bfile);
		fwscanf(fp, L"%ls\n", layset.cfile);
		fwscanf(fp, L"%d\n", &layset.tcelltype);
		fwscanf(fp, L"%d\n", &layset.ocelltype);
		fwscanf(fp, L"%ls\n", layset.nstr);
		fwscanf(fp, L"%ls\n", layset.ostr);
		fwscanf(fp, L"%ls\n", layset.olvstr);
		fwscanf(fp, L"%ls\n", layset.nlvstr);
		fclose(fp);
	}
	else {
		layset.ttbl = 0;
		layset.type = 0;
		swprintf(layset.ocell, L"0\0");
		layset.olv = 1;
		layset.oco = 0;
		layset.olc = 0;
		layset.owt = 0;
		swprintf(layset.ncell, L"0\0");
		layset.nlv = 2;
		layset.nco = 1;
		layset.nlc = 1;
		layset.nwt = 1;
		layset.tscale = 0;
		layset.tangle = 0;
		layset.tth = 0;
		layset.ttw = 0;
		layset.tfont = 0;
		layset.scale = 1;
		layset.angle = 0;
		layset.th = 5;
		layset.tw = 5;
		layset.font = 0;
		swprintf(layset.file, L"c:\\\0");
		layset.check = -1;
		layset.tbat = 0;
		swprintf(layset.bfile, L"c:\\\0");
		swprintf(layset.cfile, L"c:\\\0");
		layset.tcelltype = 0;
		layset.ocelltype = 0;
		swprintf(layset.nstr, L"0\0");
		swprintf(layset.ostr, L"0\0");
		swprintf(layset.olvstr, L"画層1");
		swprintf(layset.nlvstr, L"画層1");
	}


	/*--- ダイアログボックスオープン --------*/
	db_p = mdlDialog_open(NULL, DIALOGID_LAYCHG);
	if (db_p == NULL) {
		mdlOutput_error(L"LAYCHGダイアログボックスが開けません");
	}

	/*--- 新しいデザインファイルを開いた時 -----*/
	new_design_file();
	SystemCallback::SetNewDesignFileFunction((SystemFunc_NewDesignFile)new_design_file);
	/*--- バッチファイル使用しない場合 -----*/
	if (layset.tbat != -1) {
		mdlDialog_itemSetEnabledState(db_p, 28, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 29, FALSE, FALSE);
	}

	/*--- テーブル使用する場合 -----*/
	if (layset.ttbl == -1) {
		for (ii = 3; ii < 26; ii++) {
			mdlDialog_itemSetEnabledState(db_p, ii, FALSE, FALSE);
		}
		mdlDialog_itemSetEnabledState(db_p, 30, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 31, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 36, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 37, FALSE, FALSE);
	}

	/*--- テーブル使用しない場合 -----*/
	else {
		for (ii = 1; ii < 3; ii++) {
			mdlDialog_itemSetEnabledState(db_p, ii, FALSE, FALSE);
		}

		/*--- セルの場合 -----*/
		if (layset.type == 0) {
			for (ii = 18; ii < 21; ii++) {
				mdlDialog_itemSetEnabledState(db_p, ii, FALSE, FALSE);
			}
			for (ii = 23; ii < 26; ii++) {
				mdlDialog_itemSetEnabledState(db_p, ii, FALSE, FALSE);
			}
			if (layset.tscale == 0) {
				mdlDialog_itemSetEnabledState(db_p, 21, FALSE, FALSE);
			}
			if (layset.tangle == 0) {
				mdlDialog_itemSetEnabledState(db_p, 22, FALSE, FALSE);
			}
			if (layset.tcelltype == 0) {
				mdlDialog_itemSetEnabledState(db_p, 31, FALSE, FALSE);
			}
			mdlDialog_itemSetEnabledState(db_p, 36, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 37, FALSE, FALSE);
		}

		/*--- ライン、シェイプ、サークルの場合 -----*/
		if (layset.type == 1) {
			mdlDialog_itemSetEnabledState(db_p, 5, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 11, FALSE, FALSE);
			for (ii = 16; ii < 26; ii++) {
				mdlDialog_itemSetEnabledState(db_p, ii, FALSE, FALSE);
			}
			mdlDialog_itemSetEnabledState(db_p, 30, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 31, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 36, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 37, FALSE, FALSE);
		}

		/*--- テキストの場合 -----*/
		if (layset.type == 2 || layset.type == 3) {
			mdlDialog_itemSetEnabledState(db_p, 5, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 11, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 16, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 21, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 30, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 31, FALSE, FALSE);
			if (layset.tangle == 0) {
				mdlDialog_itemSetEnabledState(db_p, 22, FALSE, FALSE);
			}
			if (layset.tth == 0) {
				mdlDialog_itemSetEnabledState(db_p, 23, FALSE, FALSE);
			}
			if (layset.tw == 0) {
				mdlDialog_itemSetEnabledState(db_p, 24, FALSE, FALSE);
			}
			if (layset.tfont == 0) {
				mdlDialog_itemSetEnabledState(db_p, 25, FALSE, FALSE);
			}
			if (layset.type == 2) {
				mdlDialog_itemSetEnabledState(db_p, 36, FALSE, FALSE);
				mdlDialog_itemSetEnabledState(db_p, 37, FALSE, FALSE);
			}
			else {
				mdlDialog_itemSetEnabledState(db_p, 36, TRUE, FALSE);
				mdlDialog_itemSetEnabledState(db_p, 37, TRUE, FALSE);
			}
		}
	}
}

/*-----------------------------------------------------------------------
**	新しいデザインファイルを開いた時
-----------------------------------------------------------------------*/
void	new_design_file( void)
{
   /*--- コンボボックスに画層リストを追加する -----*/
   add_combo_lvList_a( db_p, COMBOID_OLV);
   add_combo_lvList_a( db_p, COMBOID_NLV);
}
/*-----------------------------------------------------------------------
**	アンロード関数 
-----------------------------------------------------------------------*/
bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	task_unload
-----------------------------------------------------------------------*/
void	task_unload( )
{
   if ( mdlSystem_unloadMdlProgram( TD_FILE) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_LAY) == 0){ }
   if ( mdlSystem_unloadMdlProgram( TD_SINGCHG) == 0){ }
}


/*-----------------------------------------------------------------------
**	ダイアログフック関数
-----------------------------------------------------------------------*/
void	laychg_dialog_hook( DialogMessage *dmP)
{
DialogBoxRsc	*dbr_P;

    dmP->msgUnderstood = TRUE;
    switch( dmP->messageType){
        case DIALOG_MESSAGE_CREATE:
		    dbr_P = dmP->u.create.dialogBoxRP;
		    dmP->u.create.interests.updates       = TRUE;
	    	dmP->u.create.interests.mouses        = TRUE;
	    	dmP->u.create.interests.dialogFocuses = TRUE;
	    	dmP->u.create.interests.itemFocuses   = TRUE;
            break;

        case DIALOG_MESSAGE_DESTROY:
            task_unload( );

 	        mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
            break;

        default:
            dmP->msgUnderstood = FALSE;
            break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（テーブルファイル選択）　フック関数
-----------------------------------------------------------------------*/
void	toggle_tbl_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            /*--- テーブル使用する場合 -----*/
            if ( layset.ttbl == -1){
                for( ii=1; ii<3; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, TRUE, FALSE);
                }
                for( ii=3; ii<26; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                }
                mdlDialog_itemSetEnabledState( db_p, 30, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 31, FALSE, FALSE);
				mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
				mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
            }

            /*--- テーブル使用しない場合 -----*/
            else{
                for( ii = 1; ii < 3; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                }
                for( ii = 3; ii < 26; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, TRUE, FALSE);
                }

                /*--- セルの場合 -----*/
                if ( layset.type == 0){
                    for( ii = 18; ii < 21; ii++){
                        mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                    }
                    for( ii = 23; ii < 26; ii++){
                        mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                    }
                    if ( layset.tscale == 0){
                        mdlDialog_itemSetEnabledState( db_p, 21, FALSE, FALSE);
                    }
                    if ( layset.tangle == 0){
                        mdlDialog_itemSetEnabledState( db_p, 22, FALSE, FALSE);
                    }
                    mdlDialog_itemSetEnabledState( db_p, 30, TRUE, FALSE);
                    if ( layset.ocelltype == 0){
                        mdlDialog_itemSetEnabledState( db_p, 31, TRUE, FALSE);
                    }
					mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
					mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
                }

                /*--- ライン、シェイプ、サークルの場合 -----*/
                if ( layset.type == 1){
                    mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
                    mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
                    for( ii = 16; ii < 26; ii++){
                        mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                    }
					mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
					mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
                }

                /*--- テキストの場合 -----*/
                if ( layset.type == 2 || layset.type == 3){
                    mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
                    mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
                    mdlDialog_itemSetEnabledState( db_p, 16, FALSE, FALSE);
                    mdlDialog_itemSetEnabledState( db_p, 21, FALSE, FALSE);
                    if ( layset.tangle == 0){
                       mdlDialog_itemSetEnabledState( db_p, 22, FALSE, FALSE);
                    }
                    if ( layset.tth == 0){
                       mdlDialog_itemSetEnabledState( db_p, 23, FALSE, FALSE);
                    }
                    if ( layset.tw == 0){
                       mdlDialog_itemSetEnabledState( db_p, 24, FALSE, FALSE);
                    }
                    if ( layset.tfont == 0){
                       mdlDialog_itemSetEnabledState( db_p, 25, FALSE, FALSE);
                    }
					if ( layset.type == 2){
					   mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
					   mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
					}
					else{
					   mdlDialog_itemSetEnabledState( db_p, 36, TRUE, FALSE);
					   mdlDialog_itemSetEnabledState( db_p, 37, TRUE, FALSE);
					}
                }
            }
            break;

        default:
	    	dimP->msgUnderstood = FALSE;
	    	break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（バッチファイル選択）　フック関数
-----------------------------------------------------------------------*/
void	toggle_bat_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            /*--- バッチファイル使用しない場合 -----*/
            if ( layset.tbat != -1){
                mdlDialog_itemSetEnabledState( db_p, 28, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 29, FALSE, FALSE);
            }

            /*--- バッチファイル使用する場合 -----*/
            else{
                mdlDialog_itemSetEnabledState( db_p, 28, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 29, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	オプションボタン（タイプ）　フック関数
-----------------------------------------------------------------------*/
void	option_type_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
		    /*--- セルの場合 -----*/
		    if ( layset.type == 0){
                mdlDialog_itemSetEnabledState( db_p, 5, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 16, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 17, TRUE, FALSE);
                if ( layset.tscale == -1){
                    mdlDialog_itemSetEnabledState( db_p, 21, TRUE, FALSE);
                }
                if ( layset.tangle == -1){
                    mdlDialog_itemSetEnabledState( db_p, 22, TRUE, FALSE);
                }
	    	    for( ii = 18; ii < 21; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
      	        }
                for( ii = 23; ii < 26; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                }
                mdlDialog_itemSetEnabledState( db_p, 30, TRUE, FALSE);
                if ( layset.tcelltype == -1){
                    mdlDialog_itemSetEnabledState( db_p, 31, TRUE, FALSE);
                }
				mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
				mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
            }

            /*--- ライン、シェイプの場合 -----*/
            if ( layset.type == 1){
                mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
                for( ii = 16; ii < 26; ii++){
                    mdlDialog_itemSetEnabledState( db_p, ii, FALSE, FALSE);
                }
                mdlDialog_itemSetEnabledState( db_p, 30, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 31, FALSE, FALSE);
				mdlDialog_itemSetEnabledState( db_p, 36, FALSE, FALSE);
				mdlDialog_itemSetEnabledState( db_p, 37, FALSE, FALSE);
            }

            /*--- テキストの場合 -----*/
            if ( layset.type == 2 || layset.type == 3){
                mdlDialog_itemSetEnabledState( db_p, 5, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 11, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 16, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 21, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 17, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 18, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 19, TRUE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 20, TRUE, FALSE);
                if ( layset.tangle == -1){
                    mdlDialog_itemSetEnabledState( db_p, 22, TRUE, FALSE);
                }
                if ( layset.tth == -1){
                    mdlDialog_itemSetEnabledState( db_p, 23, TRUE, FALSE);
                }
                if ( layset.ttw == -1){
                    mdlDialog_itemSetEnabledState( db_p, 24, TRUE, FALSE);
                }
                if ( layset.tfont == -1){
                    mdlDialog_itemSetEnabledState( db_p, 25, TRUE, FALSE);
                }
                mdlDialog_itemSetEnabledState( db_p, 30, FALSE, FALSE);
                mdlDialog_itemSetEnabledState( db_p, 31, FALSE, FALSE);
				if ( layset.type == 2){
				   mdlDialog_itemSetEnabledState( db_p, 36, FALSE,  FALSE);
				   mdlDialog_itemSetEnabledState( db_p, 37, FALSE,  FALSE);
				}
				else{
				   mdlDialog_itemSetEnabledState( db_p, 36, TRUE,  FALSE);
				   mdlDialog_itemSetEnabledState( db_p, 37, TRUE,  FALSE);
				}
            }
            break;

        default:
	    	dimP->msgUnderstood = FALSE;
	    	break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（セルタイプ）　フック関数
-----------------------------------------------------------------------*/
void	toggle_celltype_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.tcelltype == 0){
                mdlDialog_itemSetEnabledState( db_p, 31, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 31, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（ｽｹｰﾙ）　フック関数
-----------------------------------------------------------------------*/
void	toggle_scale_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.tscale == 0){
                mdlDialog_itemSetEnabledState( db_p, 21, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 21, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（角度）　フック関数
-----------------------------------------------------------------------*/
void	toggle_angle_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.tangle == 0){
                mdlDialog_itemSetEnabledState( db_p, 22, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 22, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（文字高さ）　フック関数
-----------------------------------------------------------------------*/
void	toggle_th_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.tth == 0){
                mdlDialog_itemSetEnabledState( db_p, 23, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 23, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（文字幅）　フック関数
-----------------------------------------------------------------------*/
void	toggle_tw_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.ttw == 0){
                mdlDialog_itemSetEnabledState( db_p, 24, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 24, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	トグルボタン（ﾌｫﾝﾄ）　フック関数
-----------------------------------------------------------------------*/
void	toggle_font_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_STATECHANGED:
            if ( layset.tfont == 0){
                mdlDialog_itemSetEnabledState( db_p, 25, FALSE, FALSE);
            }
            else{
                mdlDialog_itemSetEnabledState( db_p, 25, TRUE, FALSE);
            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（テーブルファイル選択）　フック関数
-----------------------------------------------------------------------*/
void	push_tbl_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            layset.nn = 1;

            mdlSystem_loadMdlProgram( TD_LAYCHG, TD_FILE, L"");
            mdlDialog_itemsSynch( db_p);
			mdlSystem_unloadMdlProgram(TD_FILE);
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（バッチファイル選択）　フック関数
-----------------------------------------------------------------------*/
void	push_bat_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            layset.nn = 2;
            mdlSystem_loadMdlProgram( TD_LAYCHG, TD_FILE, L"");
            mdlDialog_itemsSynch( db_p);
			mdlSystem_unloadMdlProgram(TD_FILE);
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（セルライブラリ選択）　フック関数
-----------------------------------------------------------------------*/
void	push_cel_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            layset.nn = 3;
            mdlSystem_loadMdlProgram( TD_LAYCHG, TD_FILE, L"");
            mdlDialog_itemsSynch( db_p);
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}


/*-----------------------------------------------------------------------
**	プッシュボタン（実行）　フック関数
-----------------------------------------------------------------------*/
void	push_ok_hook( DialogItemMessage *dimP)
{
	int length;
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--------- テーブルファイルのフルパス取得 -------------*/
            mdlFile_find( &full, L"laychg.tbl", L"MS_DEF", NULL);

            fp = _wfopen( full.GetWCharCP(),L"w");
                fwprintf( fp, L"%d\n", layset.ttbl);
                fwprintf( fp, L"%d\n", layset.type);
                fwprintf( fp, L"%ls\n", layset.ocell);
                fwprintf( fp, L"%d\n", layset.olv);
                fwprintf( fp, L"%d\n", layset.oco);
                fwprintf( fp, L"%d\n", layset.olc);
                fwprintf( fp, L"%d\n", layset.owt);
                fwprintf( fp, L"%ls\n", layset.ncell);
                fwprintf( fp, L"%d\n", layset.nlv);
                fwprintf( fp, L"%d\n", layset.nco);
                fwprintf( fp, L"%d\n", layset.nlc);
                fwprintf( fp, L"%d\n", layset.nwt);
                fwprintf( fp, L"%d\n", layset.tscale);
                fwprintf( fp, L"%d\n", layset.tangle);
                fwprintf( fp, L"%d\n", layset.tth);
                fwprintf( fp, L"%d\n", layset.ttw);
                fwprintf( fp, L"%d\n", layset.tfont);
                fwprintf( fp, L"%lf\n", layset.scale);
                fwprintf( fp, L"%lf\n", layset.angle);
                fwprintf( fp, L"%lf\n", layset.th);
                fwprintf( fp, L"%lf\n", layset.tw);
                fwprintf( fp, L"%d\n", layset.font);
                fwprintf( fp, L"%ls\n", layset.file);
                fwprintf( fp, L"%d\n", layset.check);
                fwprintf( fp, L"%d\n", layset.tbat);
                fwprintf( fp, L"%ls\n", layset.bfile);
                fwprintf( fp, L"%ls\n", layset.cfile);
                fwprintf( fp, L"%d\n", layset.tcelltype);
                fwprintf( fp, L"%d\n", layset.ocelltype);
                fwprintf( fp, L"%ls\n", layset.nstr);
                fwprintf( fp, L"%ls\n", layset.ostr);
    			fwprintf( fp, L"%ls\n", layset.olvstr);
    			fwprintf( fp, L"%ls\n", layset.nlvstr);
            fclose( fp);

            /*--------- テーブルを使用する場合 -------------*/
            if ( layset.ttbl == -1){
				length = wcslen(layset.file);
                if ( length == 0){
                    mdlDialog_openInfoBox(L"テーブルファイルを選んで下さい");
                }
                else{
                    mdlSystem_loadMdlProgram( TD_LAYCHG, TD_LAY, L"");
                }
            }

            /*--------- 単独の場合 -------------*/
            else{
            	
                mdlSystem_loadMdlProgram( TD_LAYCHG, TD_SINGCHG, L"");

            }
            break;

        default:
		    dimP->msgUnderstood = FALSE;
		    break;
    }
}
/*-----------------------------------------------------------------------
**	コンボボックスに画層名リストを追加
-----------------------------------------------------------------------*/
int add_combo_lvList_a(MSDialogP db_p, long comboID)
{
	StringList	*stringListP;
	UInt32		ii;
	LevelId		hlvID;
	long		index = 0;
	DialogItem 	*diP;
	RawItemHdr	*comboP;
	WChar		*lvName;
	lvName = (WChar*)malloc(MAX_LEVEL_NAME_LENGTH * sizeof(WChar));
	stringListP = NULL;
	/*--- コンボボックスへのポインタを取得（20-211） ------*/
	diP = mdlDialog_itemGetByTypeAndId(db_p, RTYPE_ComboBox, comboID, 0);
	comboP = diP->rawItemP;

	/*--- 最大画層ID（204-3638） ---*/
	mdlLevelTable_getHighestLevelId(&hlvID, MASTERFILE);

	/*--- ストリングリストの初期化（128-2204） ------*/
	stringListP = mdlStringList_create(0, 2);
	for (ii = 1; ii <= hlvID; ii++) {

		if (cnv_level_ID2name(lvName, ii) == SUCCESS) {
			//--- ストリングリストの最後に追加（128-2208） ------
			mdlStringList_insertMember(&index, stringListP, -1, 1);
			//--- ストリングリストに追加する文字列を設定（128-2218） ------
			mdlStringList_setMember(stringListP, index, lvName, NULL);
			mdlStringList_setInfoFieldInt32(stringListP, index, 1, ii);
		}
	}
	/*--- コンボボックスへストリングリストをセット（13-100） ------*/
	mdlDialog_comboBoxSetStrListP(comboP, stringListP, index);

	free(lvName);
	return(index);
}

/*-----------------------------------------------------------------------
**	プッシュボタン(終了）　フック関数
-----------------------------------------------------------------------*/
void	push_end_hook(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType) {
	case DITEM_MESSAGE_BUTTON:
		task_unload();

		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
		break;
	case DITEM_MESSAGE_DESTROY:

		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
