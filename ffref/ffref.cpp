#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*----------------------------------------------------------------------*/
/*-     FFREF.MC                                                        */
/*----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <Mstn\MdlApi\deprecated10_0.fdf>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "ffref.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE        *fp;
MSDialog    *db_p;
MSElement   ele;
int         ii, exitStatus;

FFREF       ffref;

#ifndef Private
#define Private
#endif

/*-    �t�b�N�֐�   -*/
Private void    dialog_hook( DialogMessage *dmP);
Private void    push_layer1_hook( DialogItemMessage *dimP);
Private void    push_layer2_hook( DialogItemMessage *dimP);
Private void    push_ok_hook( DialogItemMessage *dimP);
Private void    push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo  uHooks[] = {
   { HOOKID_FFREF_DIALOG, (PFDialogHook)dialog_hook     },
   { HOOKID_PUSH_LAYER1,   (PFDialogHook)push_layer1_hook },
   { HOOKID_PUSH_LAYER2,   (PFDialogHook)push_layer2_hook },
   { HOOKID_PUSH_OK,      (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_END,     (PFDialogHook)push_end_hook   },
};

Private  bool   unload(UnloadProgramReason unloadType);
Private  void   task_unload( void);
Private  void   startModify( void);
Private  void   set_layer_ffref( void);

/*-----------------------------------------------------------------------
**  MAIN
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

    RscFileHandle    Rsc;


/*
#if LOCK == 1
    main_lok( );
#endif
*/
	chk_kigen( );



    /*--- ���\�[�X�t�@�C�����J���� ---*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"FFREF �̃��\�[�X�t�@�C�����J���܂���");
        mdlSystem_exit( exitStatus, 1);
    }

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- �t�b�N�֐��̎g�p�錾 ---*/
    mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

    /*--- C����`�������ݒ� ---*/
	sprintf(structName, "FFREF_INFO");
	sprintf(variableName, "ffref");
    setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
    mdlDialog_publishComplexVariable( setP, structName, variableName, &ffref);

    /*--- �ݒ�l��ǂݍ��� -------------*/
    mdlFile_find( &full, L"ffref.tbl", L"MS_DEF", NULL);
    if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
        fwscanf( fp, L"%d\n", &ffref.lv);
        fwscanf( fp, L"%d\n", &ffref.co);
        fwscanf( fp, L"%d\n", &ffref.lc);
        fwscanf( fp, L"%d\n", &ffref.wt);
        fwscanf( fp, L"%d\n", &ffref.nolv);
        fwscanf( fp, L"%d\n", &ffref.noco);
        fwscanf( fp, L"%d\n", &ffref.nolc);
        fwscanf( fp, L"%d\n", &ffref.nowt);
        fwscanf( fp, L"%d\n", &ffref.rdl);
        fwscanf( fp, L"%d\n", &ffref.mode);
		fclose(fp);
    }
    else{
        ffref.lv       = 1;
        ffref.co       = 0;
        ffref.lc       = 0;
        ffref.wt       = 0;
        ffref.nolv     = 1;
        ffref.noco     = 0;
        ffref.nolc     = 0;
        ffref.nowt     = 0;
        ffref.rdl      = 0;
        ffref.mode     = 0;
    }
    
    /*--- �_�C�A���O�{�b�N�X�I�[�v�� ---*/
    db_p = mdlDialog_open( NULL, DIALOGID_FFREF);
    if( db_p == NULL){
        mdlOutput_error( L"FFREF �_�C�A���O�{�b�N�X���J���܂��� ");
    }

}


/*------------------------------------------------------------------------
**  �A�����[�h�֐�
------------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*------------------------------------------------------------------------
**  task_unload
------------------------------------------------------------------------*/
Private  void   task_unload( )
{
    if( mdlSystem_unloadMdlProgram( TD_FF) == 0){ }
}


/*------------------------------------------------------------------------
**  �_�C�A���O�t�b�N�֐�
------------------------------------------------------------------------*/
Private void  dialog_hook( DialogMessage *dmP)
{
    DialogBoxRsc    *dbr_P;

    dmP->msgUnderstood = TRUE;
    switch( dmP->messageType){
        case DIALOG_MESSAGE_CREATE:
            dbr_P = dmP->u.create.dialogBoxRP;
            dmP->u.create.interests.updates         = TRUE;
            dmP->u.create.interests.mouses          = TRUE;
            dmP->u.create.interests.dialogFocuses   = TRUE;
            dmP->u.create.interests.itemFocuses     = TRUE;
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


/*------------------------------------------------------------------------
**  �v�b�V���{�^���i�}�g���C���[�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private void  push_layer1_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            ffref.nn = 1;
            startModify( );
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�}�ԃ��C���[�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_layer2_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            ffref.nn = 2;
            startModify( );
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**  startModify
------------------------------------------------------------------------*/
Private  void   startModify( )
{
    static int searchType[] = { SHAPE_ELM, TEXT_ELM};

    mdlState_startModifyCommand((StateFunc_Reset)startModify, (MdlFunc_Accept)set_layer_ffref, NULL, NULL, NULL, NULL, NULL, TRUE, 0);

    mdlLocate_init( );

    /*--- �v�f�^�C�v�ݒ�̏ꍇ ---*/
    mdlLocate_noElemAllowLocked( );
    mdlLocate_setElemSearchMask( sizeof( searchType) / sizeof( int), searchType);

    /*--- �v�f�^�C�v�ݒ肵�Ȃ��ꍇ ---*/
    mdlLocate_allowLocked( );
}


/*------------------------------------------------------------------------
**  ���C���[�ݒ�
------------------------------------------------------------------------*/
Private  void   set_layer_ffref( void)
{
ULong    filePos;











    filePos = mdlElement_getFilePos( FILEPOS_CURRENT, NULL);
    mdlElement_read( &ele, MASTERFILE, filePos);

    /*--- ���C���[�ݒ� ---*/
    if ( ffref.nn == 1){
        get_layer( &ffref.lv, &ffref.co, &ffref.lc, &ffref.wt, NULL, &ele);
    }
    else if( ffref.nn == 2){
        get_layer( &ffref.nolv, &ffref.noco, &ffref.nolc, &ffref.nowt, NULL, &ele);
    }

    mdlDialog_itemsSynch( db_p);
    startModify( );
}


/*------------------------------------------------------------------------
**  �v�b�V���{�^���i���s�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_ok_hook( DialogItemMessage *dimP)
{
	BeFileName		full;
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );

            /*--- �ݒ�l�̕ۑ� ---*/
            mdlFile_find( &full, L"ffref.tbl", L"MS_DEF", NULL);
            fp = _wfopen( full, L"w");
                fwprintf( fp, L"%d\n", ffref.lv);
                fwprintf( fp, L"%d\n", ffref.co);
                fwprintf( fp, L"%d\n", ffref.lc);
                fwprintf( fp, L"%d\n", ffref.wt);
                fwprintf( fp, L"%d\n", ffref.nolv);
                fwprintf( fp, L"%d\n", ffref.noco);
                fwprintf( fp, L"%d\n", ffref.nolc);
                fwprintf( fp, L"%d\n", ffref.nowt);
                fwprintf( fp, L"%d\n", ffref.rdl);
                fwprintf( fp, L"%d\n", ffref.mode);
            fclose( fp);

            if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel,
                 L"�t�@�C���̈��k�͂��܂������H\n�{���Ɏ��s���܂����H", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
                mdlSystem_loadMdlProgram( TD_FFREF, TD_FF,L"");

            }
            break;

        default:
            dimP->msgUnderstood = FALSE;
            break;
    }
}


/*------------------------------------------------------------------------
**  �v�b�V���{�^���i�I���j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void  push_end_hook( DialogItemMessage *dimP)
{
    dimP->msgUnderstood = TRUE;
    switch( dimP->messageType){
        case DITEM_MESSAGE_BUTTON:
            task_unload( );
            mdlSystem_exit( exitStatus, 1);
            break;

        default:
            dimP->msgUnderstood = FALSE;
         	break;
    }
}
