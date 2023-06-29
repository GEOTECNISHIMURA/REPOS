/*-----------------------------------------------------------------------*/
/*-		�폜					                -*/
/*-									-*/
/*-	        DEL.MC							-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MdlApi\MdlApi.h>

#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>

#include "delele.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE		*fp;
MSElement	ele;
MSElementDescr	*elmDP;
ULong		Pos, celPos[100000], comPos[10000];
int		exitStatus, type, ii, flag, flag2, laynum, word;
UInt32  lv, co, wt;
//int  lv, co, wt;
int		lc,  tlv[10000], tco[10000], tlc[10000], twt[10000];
WChar		info[128], filename[128], buf[128], copy[128], *copy2 ,del[128];

DELSET		*delset;

#ifndef Private
#define Private
#endif

bool unload(UnloadProgramReason unloadType);
Private void	read_tbl( void);
Private void	kensaku( void);
Private void	kensaku_cell( void);
Private void	kensaku_cmplx( void);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    //#pragma         resourceID    DEL_ID
    CExprResult     result;
    CExprValue      value;

	/*--- �A�����[�h�֐��Z�b�g ---------------*/
    //mdlSystem_setFunction(SYSTEM_UNLOAD_PROGRAM, unload);
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- �L��ϐ��̎擾 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&delset",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR ) != SUCCESS){
        mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
		mdlSystem_exit(exitStatus, 1);
    }
    delset = (DELSET*)value.val.valPointer;

    read_tbl( );

    /*--- �o�b�`�����̏ꍇ ---*/
    if ( delset->mode == 1){
        if ( ( fp = _wfopen( delset->bat,L"r")) != NULL){
            while( fwscanf( fp, L"%s\n", filename) != EOF){
                if ( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                /*--- �o�b�N�A�b�v ---*/
				wcscpy( copy, filename);
                copy2 = wcstok( copy, L".");
				wcscat( copy2, L".bak");
                mdlFile_copy(copy, filename);

                /*--- ���� ---*/
                if ( mdlSystem_newDesignFile(filename) == SUCCESS){
                    kensaku( );
                    kensaku_cell( );
                    kensaku_cmplx( );
                }
            }
        }
        else{
			swprintf( info, L"%s ��������܂���", delset->bat);
            mdlDialog_openInfoBox(info);
        }
        fclose( fp);
    }
    else{
        /*--- �o�b�N�A�b�v ---*/
		wcscpy( copy, tcb->dgnfilenm);
        copy2 = wcstok( copy, L".");
        wcscat( copy2, L".bak");
        mdlFile_copy(copy, tcb->dgnfilenm);
        /*--- ���� ---*/
        kensaku( );
        kensaku_cell( );
        kensaku_cmplx( );
    }

    mdlSystem_exit( exitStatus, 1);
    ///return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
	//mdlState_clear();
	//mdlState_startDefaultCommand();
	//mdlSystem_exit(exitStatus, 1);

	return(SUCCESS);
}


/*-----------------------------------------------------------------------
**	�폜���C���[���w�肵���e�[�u���t�@�C����ǂݍ���
-----------------------------------------------------------------------*/
Private	void	read_tbl( )
{
	
    if ( ( fp = _wfopen( delset->table,L"r")) != NULL){
        ii = 0;
        while( NULL != fgetws( buf, 128, fp) ){
            if ( wmemcmp( &buf[0], L";", 1) == 0){
                continue;
            }
            if ( wmemcmp( buf, L"#DEL", 4) == 0){
                swscanf( buf, L"%s %d %d %d %d\n", del, &tlv[ii], &tco[ii], &twt[ii], &tlc[ii]);
                ii++;
            }
            else{
                swscanf( buf, L"%d %d %d %d\n", &tlv[ii], &tco[ii], &twt[ii], &tlc[ii]);
                ii++;
            }
			
        }
		fclose(fp);
    }
    else{
		swprintf(info, L"%s ��������܂���", delset->table);
        mdlDialog_openInfoBox(info);
		mdlSystem_exit(exitStatus, 1);
		return;
    }
    
    laynum = ii;
}


/*-----------------------------------------------------------------------
**	���� ( kensaku )
-----------------------------------------------------------------------*/
Private	void	kensaku( )
{
	ExtScanList     scanList;
    ULong			filePos;
    int             fileNum, getSize, Error;
	ULong		    elemAddr[1];
	LevelId         lv_t;

    /*---------- �X�L�������X�g�̏����� --------------------*/
    mdlScan_initScanlist(&scanList);

    /*---------- �����͈͂̐ݒ� ----------------------------*/
    mdlScan_noRangeCheck(&scanList);

    /*---------- scanList�̊e�����o�[�Ɉȉ����Z�b�g����B---*/
    //scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;

    /*---------- �����Ώۂ̌��� ----------------------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* �Ώۂ�ײ� */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* �Ώۂ�ײݽ��ݸ� */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* �Ώۂͼ���� */
    scanList.typmask[0] |= TMSK0_TEXT_NODE;   /* �Ώۂ�÷��ɰ�� */
    scanList.typmask[0] |= TMSK0_ELLIPSE;     /* �Ώۂͻ��� */
    scanList.typmask[1] |= TMSK1_TEXT;        /* �Ώۂ�÷�� */

    /*---------- �X�L�������s ------------------------------*/
    fileNum = 0;
    filePos = 0;
    if ( mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
	mdlOutput_message( L"�A�����[�h���܂����B" );
        mdlSystem_exit(exitStatus, 1);
    }
    /*---------- �v�f�ɼ���ۼނ�^���� ---------------------*/
    while( TRUE){
		//Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
		if ( getSize <= 0){
			break;
		}
		if ( Error >= 65){
			break;
		}
		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
		//get_layer(&lv, &co, &lc, &wt, NULL, &ele);
        mdlElement_getSymbology( &co, &wt, &lc, &ele);
        mdlElement_getProperties( &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
        flag = 0;
        for( ii = 0; ii < laynum; ii++){
			mdlLevel_getIdFromCode(&lv_t, MASTERFILE, tlv[ii]);
            if ( ( lv==lv_t || lv_t==-1) && ( co==tco[ii] || tco[ii]==-1) &&
                 ( lc==tlc[ii] || tlc[ii]==-1) && ( wt==twt[ii] || twt[ii]==-1)){
                flag++;
                break;
            }
        }
        if ( flag == 0){
            continue;
        }

		/*---- �v�f�̈ʒu�����߂� ----*/
    	//word = mdlElement_igdsSize( &ele, 0);             
	    //Pos = filePos - 2 * ( ULong)word;
	    Pos = elemAddr[0];

        /*---- �v�f���폜���� ----*/
        mdlElmdscr_read( &elmDP, Pos, MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, Pos, TRUE);
        mdlElmdscr_freeAll( &elmDP);
    }
}


/*-----------------------------------------------------------------------
**	���� ( kensaku_cel )
-----------------------------------------------------------------------*/
Private	void	kensaku_cell( )
{
	ExtScanList    scanList;
    ULong          filePos;
    int            getSize, Error, jj = 0;
	ULong		   elemAddr[1];
	LevelId         lv_t;

    /*---------- �X�L�������X�g�̏����� --------------------*/
    mdlScan_initScanlist( &scanList);

    /*---------- �����͈͂̐ݒ� ----------------------------*/
    mdlScan_noRangeCheck( &scanList);

    /*---------- scanList�̊e�����o�[�Ɉȉ����Z�b�g����B---*/
    //scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= TMSK0_CELL_HEADER; /* �Ώۂ;� */

    /*---------- �X�L�������s ------------------------------*/
    filePos = 0;
    if ( mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
    }

    /*---------- �v�f�ɼ���ۼނ�^���� ---------------------*/
    while( TRUE){
		//Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
		if (getSize <= 0) {
			break;
		}
		if (Error >= 65) {
			break;
		}
		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
		mdlElmdscr_read(&elmDP, elemAddr[0], MASTERFILE, 0, NULL);


        type = mdlElement_getType( &ele);
        if ( type == 2){
			celPos[jj] = elemAddr[0];
			mdlElement_getSymbology(&co, &wt, &lc, &ele);
			mdlElement_getProperties(&lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &elmDP->h.firstElem->el);
			
			for (ii = 0; ii < laynum; ii++) {
				mdlLevel_getIdFromCode(&lv_t, MASTERFILE, tlv[ii]);
				if ((lv == lv_t || lv_t == -1) && (co == tco[ii] || tco[ii] == -1)
					&& (lc == tlc[ii] || tlc[ii] == -1) && (wt == twt[ii] || twt[ii] == -1)) {
					jj++;
					break;
				}
			}
        }
    }
    for ( ii = 0; ii < jj; ii++){
        mdlElmdscr_read( &elmDP, celPos[ii], MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, celPos[ii], TRUE);
        mdlElmdscr_freeAll( &elmDP);
    }
}


/*-----------------------------------------------------------------------
**	���� ( kensaku_cmplx )
-----------------------------------------------------------------------*/
Private	void	kensaku_cmplx( )
{
	ExtScanList    scanList;
    ULong          filePos;
    int            fileNum, getSize, Error, jj = 0;
	ULong		   elemAddr[1];
	LevelId         lv_t;

    /*---------- �X�L�������X�g�̏����� --------------------*/
    mdlScan_initScanlist(&scanList);

    /*---------- �����͈͂̐ݒ� ----------------------------*/
    mdlScan_noRangeCheck(&scanList);

    /*---------- scanList�̊e�����o�[�Ɉȉ����Z�b�g����B---*/
    //scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;

    /*---------- �����Ώۂ̌��� ----------------------------*/
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* �Ώۂͺ���گ��ײ� */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* �Ώۂͺ���گ������� */

    /*---------- �X�L�������s ------------------------------*/
    fileNum = 0;
    filePos = 0;
    if ( mdlScan_initialize(MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
	mdlOutput_message( L"�A�����[�h���܂����B" );
        mdlSystem_exit(exitStatus, 1);
    }
    /*---------- �v�f�ɼ���ۼނ�^���� ---------------------*/
    while( TRUE){
		//Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
		if (getSize <= 0) {
			break;
		}
		if (Error >= 65) {
			break;
		}
		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);



		/*---- �v�f�̈ʒu�����߂� ----*/
        //word = mdlElement_igdsSize( &ele, 0);             /*---- �v�f�̈ʒu�����߂� ----*/
        //comPos[jj] = filePos - 2 * ( ULong)word;
		comPos[jj] = elemAddr[0];
		//get_layer(&lv, &co, &lc, &wt, NULL, &ele);
        mdlElement_getSymbology( &co, &wt, &lc, &ele);
        mdlElement_getProperties( &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ele);

        flag = 0;
        //numelems = ele.complex_string.numelems;
		//numelems = ele.complex_string.componentCount;
        for( ii = 0; ii < laynum; ii++){
			mdlLevel_getIdFromCode(&lv_t, MASTERFILE, tlv[ii]);
            if ( ( lv==lv_t || lv_t==-1) && ( co==tco[ii] || tco[ii]==-1) &&
                 ( lc==tlc[ii] || tlc[ii]==-1) && ( wt==twt[ii] || twt[ii]==-1)){
                jj++;
                flag++;
                break;
            }
        }
        if ( flag == 0){
            continue;
        }
    }
    for ( ii = 0; ii < jj; ii++){
        mdlElmdscr_read( &elmDP, comPos[ii], MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, comPos[ii], TRUE);
        mdlElmdscr_freeAll( &elmDP);
    }
}


