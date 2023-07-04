/*-----------------------------------------------------------------------*/
/*-         �_���O���`�F�b�N                                            -*/
/*-              CHK.MC                                                 -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include "../CommonCE/naka8.cpp"
#include "dnglchk.h"

FILE            *fp;
RscFileHandle   Rsc;
MSElement       ele, oele;
Dpoint3d        lPoi[1000], grPoi[1000], errPoi[10000], linePoi[100000][2];

ULong           linePos[1000000], renPos[1000000];
int             exitStatus, linect, errct, dnglct, compct;
int             renv, mm;
UInt32          olv[100], oco[100],  owt[100], memo;
int             olc[100];
int             del[50000];
WChar            filename[128], info[256], buf[256];
double          percent, hankei;

MSDialog        *dbP;

DNGLCHK         *dnglchk;

#ifndef Private
#define Private
#endif
/*- �v���g�^�C�v    -*/
Private bool    unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private void    dngl_layer( void);
Private void    scan_line( void);
Private void    line_renzoku( void);
Private void    dongle_check( int *num, Dpoint3d *Poi, double limit);
Private void    return_check( int *num);


/*-----------------------------------------------------------------------
**  ���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    
    CExprResult     result;
    CExprValue      value;
	exitStatus = 1;
    /*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"CHK �̃��\�[�X�t�@�C�����J���܂���");
        mdlSystem_exit( exitStatus, 1);
    }

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- �L��ϐ��̎擾 ---*/
    if( mdlCExpression_getValue( &value, &result, "&dnglchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
        mdlSystem_exit( exitStatus, 1);
    }
    dnglchk = (DNGLCHK*)value.val.valPointer;

    syori( );

}


/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void    syori( void)
{
    

    /*--- �i�s�󋵕\���̊J�n ---*/
    dbP = mdlDialog_completionBarOpen( L"�i�s��");

    dngl_layer( );
    scan_line( );

    /*--- �i�s�󋵕\���̏I�� ---*/
	mdlDialog_completionBarClose( dbP);

    if( errct == 0 && dnglct == 0){
        mdlOutput_error( L"�G���[�͂���܂���B");
    }
    else if( errct == 0 && dnglct != 0){
        mdlOutput_printf( MSG_ERROR, L"�_���O���̐���%d�ł��B", dnglct);
    }
    else if( errct != 0 && dnglct == 0){
        mdlOutput_printf( MSG_ERROR, L"�܂�Ԃ��̐���%d�ł��B", errct);
    }
    else if( errct != 0 && dnglct != 0){
        mdlOutput_printf( MSG_ERROR, L"�܂�Ԃ��̐���%d�A�_���O���̐���%d�ł��B", errct, dnglct);
    }
}


/*-------------------------------------------------------------------------*/
//     dngl_layer
/*-------------------------------------------------------------------------*/
Private void  dngl_layer( void)
{
    /*--- ���C���[�e�[�u���̃I�[�v�� ---*/
    if( (fp = _wfopen( dnglchk->table, L"r")) == NULL){
        mdlOutput_error( L"�e�[�u�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    memo = 0;

    while( fgetws( buf, 256, fp) != NULL){
        /*--- ";"��ǂݔ�΂� ---*/
        if(wmemcmp( buf, L";", 1) == 0){
            continue;
        }

        swscanf( buf, L"%d %d %d %d\n", &olv[memo], &oco[memo], &olc[memo], &owt[memo]);
        memo++;
    }

    fclose( fp);
}


/*-----------------------------------------------------------------------
**  ���C���A���C���X�g�����O�̌���
-----------------------------------------------------------------------*/
Private void  scan_line( )
{
    ExtScanList     scanList;
    ULong           filePos;
    int             fileNum, getSize, Error;
    int             ii, type;
	UInt32		    lv, co, wt;
	int             lc;
	ULong		    elemAddr[1];

    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    //scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= ( unsigned short)TMSK0_LINE;
    scanList.typmask[0] |= ( unsigned short)TMSK0_LINE_STRING;
    scanList.typmask[0] |= ( unsigned short)TMSK0_CMPLX_STRING;

    /*--------- �X�L�������s ---------------*/
    fileNum  = 0;   filePos = 0;   linect = 0;
    dnglct   = 0;   errct   = 0;   compct = 0;

    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
        if( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
        if( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        type = mdlElement_getType( &ele);
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

        mm = 0;
        /*--- �ݒ背�C���[�ƈ�v����v�f�̃t�@�C���|�W�V�������擾 ---*/
        for( ii = 0; ii < memo; ii++){
            if( ( lv == olv[ii] || olv[ii] == -1) && ( co == oco[ii] || oco[ii] == -1) &&
                ( lc == olc[ii] || olc[ii] == -1) && ( wt == owt[ii] || owt[ii] == -1)){
                if( type == 3 || type == 4){
                    linePos[linect] = elemAddr[0];
                    linect++;
                }
            }
        }
    }
    line_renzoku( );
}

/*-----------------------------------------------------------------------
**  ���C���A���C���X�g�����O�̘A����
-----------------------------------------------------------------------*/
Private void  line_renzoku( )
{
    int         ii, jj, kk, lnv, rennum;

    for( ii = 0; ii < linect; ii++){
        del[ii] = 0;
    }

    for( ii = 0; ii < linect; ii++){
        if( del[ii] > 0){ continue;}

        /*--- 1�ڂ̃��C�� ---*/
        mdlElement_read( &oele, MASTERFILE, linePos[ii]);
        mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);

        /*--- �A�����郉�C�� ---*/
        rennum = 1;
        renPos[0] = linePos[ii];

        get_renzoku_doulayer( linect, linePos, &lnv, lPoi, &rennum, renPos);

        dongle_check( &lnv, lPoi, 1);
        return_check( &lnv);

        /*--- �g�p�ςݗv�f ---*/
        for( jj = 0; jj < rennum; jj++){
            for( kk = ii; kk < linect; kk++){
                if( renPos[jj] == linePos[kk]){
                    del[kk]++;
                    break;
                }
            }
        }

        /*--- �i�s�󋵕\�� ---*/
        percent = ( double)ii / ( double)( linect-1) * 100;
		mdlDialog_completionBarUpdate( dbP, L"�i�s��", percent);
    }
}


/*-----------------------------------------------------------------------
**  �_���O���̌���
-----------------------------------------------------------------------*/
Private void  dongle_check( int *num, Dpoint3d *Poi, double limit)
{
    int     ii, jj, numP;

    hankei = dnglchk->size * 1000;

    numP  = *num;
    for( ii = 0; ii < numP-1; ii++){
        for( jj = ii + 1; jj < numP; jj++){
            if ( sqrt( pow( Poi[jj].x - Poi[ii].x, 2)
                     + pow( Poi[jj].y - Poi[ii].y, 2)) < limit){
                if( ( ii == 0 && jj == numP-1)){
                    continue;
                }
                else{
                    create_errCircle( dnglchk->lv1, dnglchk->co1, dnglchk->lc1, dnglchk->wt1, hankei, &Poi[ii]);
                    dnglct++;
                }
            }
        }
    }
    *num = numP;
}


/*-----------------------------------------------------------------------
**  �ݒ肵���p�x���ł̐܂�Ԃ��̌���
-----------------------------------------------------------------------*/
Private void  return_check( int *num)
{
    double      rad, kakudo;
    int         ii, numP;

    numP = *num;

    /*--- �A�����������ɁA�������쐬 ---*/
    for( ii = 0; ii < numP-1; ii++){
        memcpy( &linePoi[ii][0], &lPoi[ii], sizeof( Dpoint3d));
        memcpy( &linePoi[ii][1], &lPoi[ii+1], sizeof( Dpoint3d));
    }

    for( ii = 0; ii < numP-2; ii++){
        rad = atan2( linePoi[ii][1].y   - linePoi[ii][0].y,   linePoi[ii][1].x   - linePoi[ii][0].x)
            - atan2( linePoi[ii+1][1].y - linePoi[ii+1][0].y, linePoi[ii+1][1].x - linePoi[ii+1][0].x);
        kakudo = rad / fc_piover180;
        if( ( 180 - dnglchk->angle <= kakudo && kakudo <= 180 + dnglchk->angle) ||
           ( -1 * ( 180 + dnglchk->angle) <= kakudo && kakudo <= -1 * (180 - dnglchk->angle))){
            create_errCircle( dnglchk->lv2, dnglchk->co2, dnglchk->lc2, dnglchk->wt2, hankei, &linePoi[ii][1]);
            errct++;
        }
    }
    *num = numP;
}


