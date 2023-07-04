/*-----------------------------------------------------------------------*/
/*-	        CNV.MC														-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "lstoshap.h"

FILE                    *fp;
RscFileHandle           Rsc;
MSDialog                *db_p;
MSElementDescr          *elmDP, *elmDP2, *oelmDP;
MSElement               nele, ele;
int                     exitStatus, count;
int                     del[50000];
UInt32                  cnvlv[1000], cnvco[1000], cnvwt[1000];
int                     laynum, cnvlc[1000];
ULong                   cnvPos[50000], compPos[50000], renPos[10000];
Dpoint3d                lPoi[50000];
int                     cnvnum, compnum;
WChar                   info[256], filename[256], copy[256], *copy2;
GLOBAL                  *glo;

#ifndef Private
#define Private
#endif

/*-	�v���g�^�C�v	-*/
bool unload(UnloadProgramReason unloadType);
Private void	syori( void);
Private void	read_layer( void);
Private void	scan_line( void);
Private void	scan_shape( void);
Private void	cnv_shape_to_ls( void);
Private void	cnv_ls_to_shape( void);
Private void	cnv_shape_to_shape( void);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult    result;
    CExprValue     value;


	/*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"SINGLE�̃��\�[�X�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
	}

	/*--- �A�����[�h�֐��Z�b�g ---*/
	//mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	/*--- �L��ϐ��̎擾 -------------*/
	if (mdlCExpression_getValue(&value, &result, "&glo",
		VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS) {
		mdlOutput_error(L"�L��ϐ����擾�ł��܂���");
	}
    glo = (GLOBAL*)value.val.valPointer;


    /*--- ���C���[�e�[�u����ǂ� ---*/
    read_layer( );

    /*--- �o�b�`�̏ꍇ ---*/
    if ( glo->tog == -1){
        if ( ( fp = _wfopen( glo->bat,L"r")) != NULL){
            while( fwscanf( fp, L"%s\n", filename) != EOF){
                if ( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                /*--- �o�b�N�A�b�v ---*/
                wcscpy( copy, filename);
                copy2 = wcstok( copy, L".");
                wcscat( copy2, L".bak");
                mdlFile_copy( copy, filename);

                /*--- �t�@�C�����J�� ---*/
                if ( mdlSystem_newDesignFile( filename) == SUCCESS){
                    syori( );
                }
            }
        }
        else{
            swprintf( info, L"%s ��������܂���", glo->bat);
            mdlDialog_openInfoBox( info);
            mdlSystem_exit( exitStatus, 1);
			return;
        }
        fclose( fp);
    }
    else{
        /*--- �o�b�N�A�b�v ---*/
        wcscpy( copy, tcb->dgnfilenm);
        copy2 = wcstok( copy, L".");
        wcscat( copy2, L".bak");
        mdlFile_copy( copy, tcb->dgnfilenm);

        /*--- ���� ---*/
        syori( );
    }

    mdlSystem_exit( exitStatus, 1);
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	���C���[�e�[�u����ǂ�
-----------------------------------------------------------------------*/
Private void	read_layer( )
{
    WChar    buf[128];

    if ( ( fp = _wfopen( glo->table,L"r")) != NULL){
        while( NULL != fgetws( buf, 128, fp)){
            /*--- �P�����ڂ��G�łȂ��ꍇ ---*/
            if ( wmemcmp( &buf[0], L";", 1) != 0){
                swscanf( buf, L"%d %d %d %d\n", &cnvlv[laynum], &cnvco[laynum], &cnvlc[laynum], &cnvwt[laynum]);
                laynum++;
            }
        }
    }
    else{
        swprintf( info, L"%s ���ǂ߂܂���", glo->table);
        mdlDialog_openInfoBox( info);
        mdlSystem_exit( exitStatus, 1);
    }
    fclose( fp);
}


/*-----------------------------------------------------------------------
**	����
-----------------------------------------------------------------------*/
Private void	syori( )
{
    /*--- ���C���X�g�����O���V�F�C�v ---*/
    if ( glo->option == 0){
        scan_line( );
        cnv_ls_to_shape( );
    }

    /*--- �V�F�C�v�����C���X�g�����O ---*/
    if ( glo->option == 1){
        scan_shape( );
        cnv_shape_to_ls( );
    }

    /*--- �V�F�C�v���V�F�C�v ---*/
    if ( glo->option == 2){
        scan_shape( );
        cnv_shape_to_shape( );
    }
}


/*-----------------------------------------------------------------------
**	���C���A���C���X�g�����O�̌���
-----------------------------------------------------------------------*/
Private void	scan_line( )
{
    ExtScanList    scanList;
    ULong       filePos;
    int         getSize, Error;
	UInt32      lv, co,  wt;
    int         ii,  lc;
	ULong		elemAddr[50];
	int			i, numAddr;
	MSElement	ele1;

    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;

    scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
    scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;

    /*--------- �X�L�������s ---------------*/
    filePos = 0;   cnvnum = 0;
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		numAddr = getSize / sizeof(short);
		for (i = 0; i < numAddr; i++)
		{
			if (mdlElement_read(&ele1, MASTERFILE, elemAddr[i]) != SUCCESS)continue;
			get_layer(&lv, &co, &lc, &wt, NULL, &ele1);

			/*--- �ݒ背�C���[�Ɉ�v������� ---*/
			for (ii = 0; ii < laynum; ii++) {
				if ((lv == cnvlv[ii] || cnvlv[ii] == -1) && (co == cnvco[ii] || cnvco[ii] == -1)
					&& (lc == cnvlc[ii] || cnvlc[ii] == -1) && (wt == cnvwt[ii] || cnvwt[ii] == -1)) {
					cnvPos[cnvnum] = elemAddr[i];
					cnvnum++;
				}
			}
		}
    }
}


/*-----------------------------------------------------------------------
**	�V�F�C�v�̌���
-----------------------------------------------------------------------*/
Private void	scan_shape( )
{
    ExtScanList    scanList;
    ULong       filePos;
    int         getSize, Error;
	UInt32      lv, co, wt;
	int         ii, lc;;
	ULong		elemAddr[50];
	int			i, numAddr;
	MSElement	ele1;
	int type;

    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;

    scanList.typmask[0] |= (unsigned short)TMSK0_SHAPE;
    scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;
    scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;

    /*--------- �X�L�������s ---------------*/
    filePos = 0;   cnvnum = compnum = 0;
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		numAddr = getSize / sizeof(short);
		for (i = 0; i < numAddr; i++)
		{
			if (mdlElement_read(&ele1, MASTERFILE, elemAddr[i]) != SUCCESS)continue;
			get_layer(&lv, &co, &lc, &wt, NULL, &ele1);
			type = mdlElement_getType(&ele1);

			/*--- �ݒ背�C���[�Ɉ�v������� ---*/
			for (ii = 0; ii < laynum; ii++) {
				if ((lv == cnvlv[ii] || cnvlv[ii] == -1) && (co == cnvco[ii] || cnvco[ii] == -1)
					&& (lc == cnvlc[ii] || cnvlc[ii] == -1) && (wt == cnvwt[ii] || cnvwt[ii] == -1)) {

					/*--- �V�F�C�v ---*/
					if (type == 6) {
						cnvPos[cnvnum] = elemAddr[i];
						cnvnum++;
					}

					/*--- �����}�` ---*/
					if (type == 14) {
						compPos[compnum] = elemAddr[i];
						compnum++;
					}
				}
			}
		}
    }
}


/*-----------------------------------------------------------------------
**	���C���X�g�����O���V�F�C�v
-----------------------------------------------------------------------*/
Private void	cnv_ls_to_shape( )
{
    int         ii, jj, kk, lnv, rennum, dir, flag;
    int         i1, i2, lc, lc2;
	UInt32      lv, co, wt, lv2, co2, wt2, ggnum;
	MSElement	ele1;


    /*----- �i�s�󋵕\���J�n -----*/
    db_p = mdlDialog_completionBarOpen(L"������");
    i1 = i2 = 0;

    count = 0;

    for( ii = 0; ii < cnvnum; ii++){
        del[ii] = 0;
    }

    /*----- ���C���A���C���X�g�����O -----*/
    for( ii = 0; ii < cnvnum; ii++){
        if ( del[ii] > 0){ continue;}

        /*----- �i�s�󋵕\�� -----*/
        i2 = 100 * (double)ii/(double)cnvnum;
        if ( i2 > i1){
//            swprintf( text, "������ %d:Element Accessed", count);
            mdlDialog_completionBarUpdate( db_p, L"������", i2);
            i1 = i2;
        }

        /*--- 1�ڂ̃��C�� ---*/
        mdlElement_read( &ele1, MASTERFILE, cnvPos[ii]);
        mdlLinear_extract( lPoi, &lnv, &ele1, MASTERFILE);


        /*--- �A�����郉�C�� ---*/
        rennum = 1;
        renPos[0] = cnvPos[ii];

        get_renzoku( cnvnum, cnvPos, &lnv, lPoi, &rennum, renPos);

        /*--- �قȂ郌�C���[�̂��̂�����΂��̂܂� ---*/
        flag = 0;
        mdlElement_read( &ele, MASTERFILE, renPos[0]);
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);
        for( jj = 1; jj < rennum; jj++){
            mdlElement_read( &ele, MASTERFILE, renPos[jj]);
            get_layer( &lv2, &co2, &lc2, &wt2, NULL, &ele);
            if ( lv!=lv2 || co!=co2 || lc!=lc2 || wt!=wt2){
                flag++;
                break;
            }
        }
        if ( flag > 0){
            continue;
        }

        /*--- �N�I�_��v���Ȃ��ꍇ�͂��̂܂� ---*/
        if ( get_leng( &lPoi[0], &lPoi[lnv-1]) > 1){
            continue;
        }

        /*--- �|���S���̌��� ---*/
        dir = get_round_dir( lnv, lPoi);

        mdlShape_create( &nele, &ele1, lPoi, lnv, 0);
        mdlElmdscr_new( &elmDP, NULL, &nele);

        /*--- �t�����ɂ��Ĕz�u����ꍇ ---*/
        if ( glo->dir != dir){
            mdlElmdscr_reverse( &elmDP2, elmDP, MASTERFILE);
            mdlElmdscr_add( elmDP2);

            mdlElmdscr_freeAll( &elmDP);
            mdlElmdscr_freeAll( &elmDP2);
            count++;
        }

        /*--- ���̂܂܂̌����Ŕz�u����ꍇ ---*/
        else{
            mdlElmdscr_add( elmDP);
            mdlElmdscr_freeAll( &elmDP);
            count++;
        }


        /*--- �폜 ---*/
        for( jj = 0; jj < rennum; jj++){
            mdlElement_read( &ele1, MASTERFILE, renPos[jj]);
            mdlElement_undoableDelete( &ele1, renPos[jj], DRAW_MODE_Normal);
        }

        /*--- �g�p�ςݗv�f ---*/
        for( jj = 0; jj < rennum; jj++){
            for( kk = ii; kk < cnvnum; kk++){
                if ( renPos[jj] == cnvPos[kk]){
                    del[kk]++;
                    break;
                }
            }
        }
    }

    /*----- �i�s�󋵕\���I�� -----*/
    mdlDialog_completionBarClose( db_p);
}


/*-----------------------------------------------------------------------
**	�V�F�C�v�����C���X�g�����O
-----------------------------------------------------------------------*/
Private void	cnv_shape_to_ls( )
{
//    Dpoint3d    lPoi[1000];
	Dpoint3d    *lPoia;
    ULong       tmpPos[100];
    int         ii, jj, lnv, numele, word, rennum, elenum;
	int         i1, i2, lc;
	UInt32      lv, co, wt, ggnum;
	MSElement	ele1;



    /*----- �i�s�󋵕\���J�n -----*/
    db_p = mdlDialog_completionBarOpen( L"������");
    i1 = i2 = 0;
    elenum = cnvnum + compnum;

    /*--- �V�F�C�v�̏ꍇ ---*/
    for( ii = 0; ii < cnvnum; ii++){
        /*----- �i�s�󋵕\�� -----*/
        i2 = 100 * (double)ii/(double)elenum;
        if ( i2 > i1){
            mdlDialog_completionBarUpdate( db_p, L"������", i2);
            i1 = i2;
        }

        mdlElement_read( &ele1, MASTERFILE, cnvPos[ii]);
        mdlLinear_extract( lPoi, &lnv, &ele1, MASTERFILE);
        mdlLineString_create( &nele, &ele1, lPoi, lnv);
        mdlElement_rewrite( &nele, &ele1, cnvPos[ii]);
    }

    /*--- �����}�`�̏ꍇ ---*/
    for( ii = 0; ii < compnum; ii++){

		lPoia = (DPoint3d*)malloc(50000 * sizeof(DPoint3d));
        /*----- �i�s�󋵕\�� -----*/
        i2 = 100 * ( (double)ii + (double)cnvnum)/(double)elenum;
        if ( i2 > i1){
            mdlDialog_completionBarUpdate( db_p, L"������", i2);
            i1 = i2;
        }

        mdlElement_read( &ele1, MASTERFILE, compPos[ii]);
		mdlElmdscr_read(&oelmDP, compPos[ii], MASTERFILE, 0, NULL);
		mdlElmdscr_stroke(&lPoia, &lnv, oelmDP, NULL);
		get_layer(&lv, &co, &lc, &wt, &ggnum, &ele1);

        /*--- ���Ă���ꍇ ---*/
        if ( memcmp( &lPoia[0], &lPoia[lnv-1], sizeof( Dpoint3d)) == 0){
            lineString_add( &lv, &co, &lc, &wt, &ggnum, lPoia, &lnv);

            /*--- �폜 ---*/
            //mdlElmdscr_read( &oelmDP, compPos[ii], MASTERFILE, 0, NULL);
            mdlElmdscr_undoableDelete( oelmDP, compPos[ii], DRAW_MODE_Normal);
        }
		dlmSystem_mdlFree(lPoia);
		mdlElmdscr_freeAll(&oelmDP);

    }

    /*----- �i�s�󋵕\���I�� -----*/
    mdlDialog_completionBarClose( db_p);
}


/*-----------------------------------------------------------------------
**	�V�F�C�v���V�F�C�v
-----------------------------------------------------------------------*/
Private void	cnv_shape_to_shape( )
{
//    Dpoint3d    lPoi[1000];
	Dpoint3d    *lPoia;
    ULong       tmpPos[100];
    int         ii, jj, lnv, numele, rennum, elenum, dir, flag;
    int         i1, i2, lc;
	UInt32      lv, co, wt, ggnum;
	MSElement	ele1;

    /*----- �i�s�󋵕\���J�n -----*/
    db_p = mdlDialog_completionBarOpen(L"������");
    i1 = i2 = 0;
    elenum = cnvnum + compnum;

    /*--- �V�F�C�v�̏ꍇ ---*/
    for( ii = 0; ii < cnvnum; ii++){
        /*----- �i�s�󋵕\�� -----*/
        i2 = 100 * (double)ii/(double)elenum;
        if ( i2 > i1){
            mdlDialog_completionBarUpdate( db_p, L"������", i2);
            i1 = i2;
        }

        mdlElement_read( &ele1, MASTERFILE, cnvPos[ii]);
        mdlLinear_extract( lPoi, &lnv, &ele1, MASTERFILE);

        /*--- �|���S���̌��� ---*/
        dir = get_round_dir( lnv, lPoi);

        /*--- �t�����ɂ��Ĕz�u����ꍇ ---*/
        if ( glo->dir != dir){
            mdlElmdscr_new( &elmDP, NULL, &ele1);
            mdlElmdscr_reverse( &elmDP2, elmDP, MASTERFILE);
            mdlElmdscr_rewrite( elmDP2, elmDP, cnvPos[ii]);

            mdlElmdscr_freeAll( &elmDP);
            mdlElmdscr_freeAll( &elmDP2);
        }
    }

    /*--- �����}�`�̏ꍇ ---*/
    for( ii = 0; ii < compnum; ii++){

		lPoia = (DPoint3d*)malloc(50000 * sizeof(DPoint3d));
        /*--- �i�s�󋵕\�� ---*/
        i2 = 100 * ( (double)ii + (double)cnvnum)/(double)elenum;
        if ( i2 > i1){
            mdlDialog_completionBarUpdate( db_p, L"������", i2);
            i1 = i2;
        }

        /*--- �w�b�_��ǂ� ---*/
        mdlElmdscr_read( &oelmDP, compPos[ii], MASTERFILE, 0, NULL);

        /*--- �|���S���̌��� ---*/
		mdlElmdscr_stroke(&lPoia, &lnv, oelmDP, NULL);
        dir = get_round_dir( lnv, lPoia);
		dlmSystem_mdlFree(lPoia);
        /*--- �t�����ɂ��Ĕz�u����ꍇ ---*/
        if ( glo->dir != dir){
            /*--- �����}�`�쐬 ---*/

            mdlElmdscr_reverse( &elmDP2, oelmDP, MASTERFILE);
            mdlElmdscr_rewrite( elmDP2, oelmDP, compPos[ii]);
            mdlElmdscr_freeAll( &elmDP2);
        }
        mdlElmdscr_freeAll( &oelmDP);
    }

    /*----- �i�s�󋵕\���I�� -----*/
    mdlDialog_completionBarClose( db_p);
}
