/*-----------------------------------------------------------------------*/
/*-         CHK.MC                                                      -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "../CommonCE/naka8.cpp"
#include "mfcchk.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE                    *fp_save, *fp_mfc, *fp_err, *fp_log, *fp_list, *fp_sort;
RscFileHandle           Rsc;
int                     exitStatus, linect, txtct, cellct, memo, bat_ct, bat_now;
WChar                   filename[128], info[256], logfile[256], file[64], file_err[64], file_sort[64];
int                     LC, TYP;
UInt32                  CO, WT, LV;
int                     cell[10000][6], txt[10000][7], line[10000][6], TC, just, kijyun[10000], FT, numelems;
WChar                   cname[10000][7], tjust[3], ejust[3], str[256], battxt[64];

double                  txth[10000][2], hankei;
TextSizeParam           textSizeParam;
TextStyleInfo           tsi;
ULong                   linePos[10000], cellPos[10000], txtPos[10000];
WChar                   s[128], pp[1000][128], keep[128];
int                     max_str = 100;          /* ������̍ő咷 */
MSDialog                *dbP;
MFCCHK                  *mfcchk;

#ifndef Private
#define Private
#endif

/*- �v���g�^�C�v    -*/
Private  bool	unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private int     checkstart( void);
Private void    sort( void);
Private void    scanStart( void);
Private void    scanStart_Cel( void);
Private void    print_err( void);
Private void    err_mem( void);

/*-----------------------------------------------------------------------
**  ���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult     result;
    CExprValue      value;
    WChar   devP[64], dirP[64], nameP[64];

    /*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
    if( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"DEL �̃��\�[�X�t�@�C�����J���܂���");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

	/*--- �A�����[�h�֐��Z�b�g ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- �L��ϐ��̎擾 ---*/
    if( mdlCExpression_getValue( &value, &result, "&mfcchk",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
        mdlSystem_exit( exitStatus, 1);
		return;
    }
    mfcchk = (MFCCHK*)value.val.valPointer;

    /*--- �o�b�`�����̏ꍇ ---*/
    if( mfcchk->mode == 1){
        bat_ct = 0;
        bat_now = 0;

        /*--- �o�b�`�������̎擾 ---*/
        if( ( fp_save = _wfopen(mfcchk->bat,L"r")) != NULL){
            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                bat_ct++;
            }
			fclose(fp_save);
        }
        

        /*--- �o�b�`�t�@�C���̋L�� ---*/
        if( ( fp_save = _wfopen( mfcchk->bat,L"r")) != NULL){
            mdlFile_parseName( mfcchk->bat, devP, dirP, nameP, NULL);
            swprintf( logfile, L"%ls:%ls%ls.log", devP, dirP, nameP);
            if( ( fp_log = _wfopen( logfile, L"w")) == NULL){
                mdlOutput_error( L"�o�̓t�@�C�����J���܂���");
				mdlSystem_exit(exitStatus, 1);
				return;
            }
            fwprintf( fp_log, L"�f�U�C���t�@�C�����i�t���p�X�j                         �G���[��");
            fwprintf( fp_log, L"\n");

            while( fwscanf( fp_save, L"%ls\n", filename) != EOF){
                if( wmemcmp( &filename[0], L";", 1) == 0){
                    continue;
                }
                /*--- �L�������t�@�C�����J�� ---*/
                if( mdlSystem_newDesignFile( filename) == SUCCESS){
                    bat_now++;
                    syori( );
                    fwprintf( fp_log, L"%-55s %4d\n", tcb->dgnfilenm, mfcchk->ct);
                }
            } // WHILE END
			fclose(fp_save);
			fclose(fp_log);
        }
        else{
            swprintf( info, L"%ls ��������܂���", mfcchk->bat);
            mdlDialog_openInfoBox( info);
        }

    }

    /*--- 1�t�@�C���̏ꍇ ---*/
    else{
        syori( );

    }

    mdlSystem_loadMdlProgram( TD_MFCCHK, TD_MFCCHK, L"");
    mdlSystem_exit( exitStatus, 1);
	return;
    
}

/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
Private  bool	unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}

/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void  syori( void)
{
    WChar   buf[256], devP[64], dirP[64], nameP[64];

    memo = 0;   linect = 0;   txtct  = 0;   cellct = 0;

    /*--- �i�s�󋵕\���E�B���h�E�̃I�[�v�� ---*/
    dbP = mdlDialog_completionBarOpen( L"�i�s��");

    /*--- �l�e�b�e�[�u���̃I�[�v�� ---*/
    if( ( fp_mfc = _wfopen( mfcchk->table, L"r")) == NULL){
        mdlOutput_error( L"�l�e�b�e�[�u�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- �\�[�g�O�G���[�t�@�C�����̎擾 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, NULL, NULL);
    swprintf( file_err, L"%ls:%lserr.txt", devP, dirP);

    /*--- �\�[�g�O�G���[�t�@�C���̃I�[�v�� ---*/
    if( ( fp_err = _wfopen( file_err, L"w")) == NULL){
        mdlOutput_error( L"�G���[�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    while( fgetws( buf, 256, fp_mfc) != NULL){
        /*--- ";"��ǂݔ�΂� ---*/
        if( wmemcmp( buf, L";", 1) == 0)
            continue;

        /*--- �^�C�v�ʂɕ�����ǂݍ��ށB ---*/
        /*--- �Z���̏ꍇ ---*/
        if( wmemcmp( buf, L" 2", 2) == 0){
            swscanf( buf, L"%d %d %d %d %d %ls\n", &cell[memo][0], &cell[memo][1], &cell[memo][2],
                    &cell[memo][3], &cell[memo][4], cname[memo]);
            if( wcscmp( cname[memo], L";") == 0){
                wcscpy( cname[memo], L"");
            }
            memo++;
        }

        /*--- �e�L�X�g�̏ꍇ ---*/
        else if( wmemcmp( buf, L"17", 2) == 0){
            swscanf( buf, L"%d %d %d %d %d %lf %lf %ls %d %d\n", &txt[memo][0], &txt[memo][1],
                    &txt[memo][2], &txt[memo][3], &txt[memo][4], &txth[memo][0], &txth[memo][1],
                     tjust, &txt[memo][5], &txt[memo][6]);
            TC = txt[memo][5];

            if(      wcscmp( tjust, L"LT") == 0){ kijyun[memo] =  0;} /*--- ��_������̏ꍇ ---*/
            else if( wcscmp( tjust, L"LC") == 0){ kijyun[memo] =  1;} /*--- ��_���������̏ꍇ ---*/
            else if( wcscmp( tjust, L"LB") == 0){ kijyun[memo] =  2;} /*--- ��_�������̏ꍇ ---*/
            else if( wcscmp( tjust, L"CT") == 0){ kijyun[memo] =  6;} /*--- ��_��������̏ꍇ ---*/
            else if( wcscmp( tjust, L"CC") == 0){ kijyun[memo] =  7;} /*--- ��_�����������̏ꍇ ---*/
            else if( wcscmp( tjust, L"CB") == 0){ kijyun[memo] =  8;} /*--- ��_���������̏ꍇ ---*/
            else if( wcscmp( tjust, L"RT") == 0){ kijyun[memo] = 12;} /*--- ��_���E��̏ꍇ ---*/
            else if( wcscmp( tjust, L"RC") == 0){ kijyun[memo] = 13;} /*--- ��_���E�����̏ꍇ ---*/
            else if( wcscmp( tjust, L"RB") == 0){ kijyun[memo] = 14;} /*--- ��_���E���̏ꍇ ---*/
            else if( wcscmp( tjust, L"-1") == 0){ kijyun[memo] = -1;} /*--- ��_���S�Ă̏ꍇ ---*/

            memo++;
        }

        /*--- ���̑��̏ꍇ ---*/
        else{
            swscanf( buf, L"%d %d %d %d %d\n", &line[memo][0], &line[memo][1],
                    &line[memo][2], &line[memo][3], &line[memo][4]);
            memo++;
        }
    } // WHILE END

    /*--- �����J�n ---*/
    checkstart( );
    fclose( fp_err);
    mfcchk->ct = linect + txtct + cellct;

    if( mfcchk->ct == 0){
        /*--- �o�̓t�@�C�����̎擾 ---*/
        mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
        swprintf( file, L"%ls:%ls%ls.err", devP, dirP, nameP);
        /*--- �o�̓t�@�C���̃I�[�v�� ---*/
        if( ( fp_list = _wfopen( file, L"w")) == NULL){
            mdlOutput_error( L"�o�̓t�@�C�����J���܂���");
			mdlSystem_exit(exitStatus, 1);
			return;
        }
        fwprintf( fp_list, L"%ls\n", tcb->dgnfilenm);
        fwprintf( fp_list, L"\nTYP   LV   CO   WT   LC  �Z����   ����   ����   �   ����   ����  �G���[\n");
        fwprintf( fp_list, L"�G���[�͂���܂���B\n");
        fwprintf( fp_list, L"\n                                                          �G���[������%d�ł��B\n", mfcchk->ct);

        fclose( fp_list);
    }
    else{
        sort( );
        print_err( );
    }

    fclose( fp_mfc);

    _wremove( file_err);
    _wremove( file_sort);

    /*--- �i�s�󋵕\���E�B���h�E�̃N���[�Y ---*/
    mdlDialog_completionBarClose( dbP);
}

/*-----------------------------------------------------------------------
**  checkstart
-----------------------------------------------------------------------*/
Private int  checkstart( )
{
    RscFileHandle   rfHandle;

    hankei = mfcchk->size * 1000;

    scanStart( );
    scanStart_Cel( );
    mdlResource_openFile( &rfHandle, NULL, FALSE);

    return( SUCCESS);
}

/*----------------------------------------------------------------------+
|  ���W���[�����FscanStart                                              |
+----------------------------------------------------------------------*/
Private void scanStart( )
{
    int             getSize;
    int             Error;
    int             ii, flag1, flag2;
    ULong           filePos, complexPos,  EndPos, percent, elemAddr[1];
    double          integral, decimal;
    MSElement       buf;
    MSElement       err;
    ExtScanList     scanList;
    Dpoint3d        userOrigin;
    MSElement       ele;
    MSElementDescr  *elmDP,*NelmDP;
	TextParamWide   textParamW;
	TextParam       textParams;
	WString        wstr;
	MSTextSize      textSize;
	char           mbstr[1024];

    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanList�̊e�����o�[�Ɉȉ����Z�b�g --------------*/
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;
	scanList.extendedType = FILEPOS;

    /*--------- �����Ώۂ̌��� ---------------*/
    scanList.typmask[0] |= TMSK0_LINE;        /* �Ώۂ�ײ� */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* �Ώۂ�ײݽ��ݸ� */
    scanList.typmask[0] |= TMSK0_SHAPE;       /* �Ώۂͼ���� */
    scanList.typmask[0] |= TMSK0_TEXT_NODE;   /* �Ώۂ�÷��ɰ�� */
    scanList.typmask[0] |= TMSK0_CMPLX_STRING;/* �Ώۂͺ���گ��ײ� */
    scanList.typmask[0] |= TMSK0_CMPLX_SHAPE; /* �Ώۂͺ���گ������� */
    scanList.typmask[0] |= TMSK0_ELLIPSE;     /* �Ώۂͻ��� */
    scanList.typmask[0] |= TMSK0_ARC;         /* �Ώۂͱ�� */
    scanList.typmask[1] |= TMSK1_TEXT;        /* �Ώۂ�÷�� */

    /*--------- �X�L�������s ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlOutput_message( L"�A�����[�h���܂����B");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- �e��ϐ��̏����� ---*/
    filePos = 0;   numelems = 0;

    /*--- ��Ԍ�̃t�@�C���|�W�V�������L�� ---*/
    EndPos = mdlElement_getFilePos( FILEPOS_EOF, NULL);

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
        if( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
        if( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read(&buf, MASTERFILE, elemAddr[0]);
        /*--- �i�s�󋵕\���̃p�[�Z���e�[�W���v�Z ---*/
        percent = elemAddr[0] / EndPos * 100;

        /*--- �P�t�@�C���̏ꍇ ---*/
        if( mfcchk->mode == 0){
            wcscpy( battxt, L"�i�s��");
        }

        /*--- �o�b�`�����̏ꍇ ---*/
        else if( mfcchk->mode == 1){
            swprintf( battxt, L"�i�s��                (%d/%d)", bat_now, bat_ct);
        }
        mdlDialog_completionBarUpdate( dbP, battxt, percent);

        TYP = mdlElement_getType( &buf);
        get_layer( &LV, &CO, &LC, &WT, NULL, &buf);

        /*--- �e�L�X�g�̏ꍇ ---*/
        if( TYP == 17){
            mdlText_extract( &wstr, NULL, &userOrigin, NULL, &textSizeParam, &textParamW,  NULL, &buf);
			swprintf(str, L"%ls", wstr.GetWCharCP());
			memcpy(&textSize, &textSizeParam.size, sizeof(MSTextSize));
			just = (int)textParamW.just;
            /*--- �t�H���gON�̏ꍇ ---*/
            if( mfcchk->font == -1){
                if( textParamW.font == mfcchk->han){       FT = 0;}  // �t�H���g�����p�ݒ�Ɠ����̏ꍇ
                else if( textParamW.font == mfcchk->zen){  FT = 1;}  // �t�H���g���S�p�ݒ�Ɠ����̏ꍇ
                else{                               FT = 2;}  // �t�H���g���ݒ�ƈ�v���Ȃ��ꍇ
            }

            /*--- ���������l�̌ܓ����� ---*/
            integral = floor( textSize.height);        /* �������� */
            decimal  = textSize.height - integral;     /* �����_�ȉ� */

            if( decimal >= 0.5){ textSize.height = integral + 1;}
            else{                textSize.height = integral;    }

            /*--- ���������l�̌ܓ����� ---*/
            integral = floor( textSize.width);        /* �������� */
            decimal  = textSize.width - integral;     /* �����_�ȉ� */

            if( decimal >= 0.5){ textSize.width = integral + 1;}
            else{                textSize.width = integral;    }
        }

        /*--- �����m�[�h�̏ꍇ ---*/
        if( TYP == 7){
            mdlTextNode_extract ( &userOrigin, NULL, &textSize, NULL, &textParams, NULL, &buf,NULL);

            /*--- �����m�[�h�Ɋ܂܂��A�����̐����擾 ---*/
            /*--- �Q�����̏ꍇ ---*/
            if( mdlModelRef_is3D(MASTERFILE) != TRUE){
                numelems = buf.text_node_2d.componentCount;
            }

            /*--- �R�����̏ꍇ ---*/
            else{
                numelems = buf.text_node_3d.componentCount;
            }

            /*--- �t�H���gON�̏ꍇ ---*/
            if( mfcchk->font == -1){
                if( textParams.font == mfcchk->han){       FT = 0;}  // �t�H���g�����p�ݒ�Ɠ����̏ꍇ
                else if( textParams.font == mfcchk->zen){  FT = 1;}  // �t�H���g���S�p�ݒ�Ɠ����̏ꍇ
                else{                                      FT = 2;}  // �t�H���g���ݒ�ƈ�v���Ȃ��ꍇ
            }
        }

        flag1 = 0, flag2 = 0;

        /*--- MFC�e�[�u������ǂݍ��񂾃��C���[�Ɣ�r���� ---*/
        for( ii = 0; ii < memo; ii++){

            /*--- �e�L�X�g�ƃZ���ȊO�̗v�f�ƈ�v����ꍇ ---*/
            if( line[ii][0] == TYP && line[ii][1] == LV && line[ii][2] == CO &&
                line[ii][3] == WT && line[ii][4] == LC){ 
                flag1++;  //��v������t���O�𗧂Ă�
            }

            /*--- �e�L�X�g�ƈ�v����ꍇ ---*/
            else if( txt[ii][0] == TYP && txt[ii][1] == LV && txt[ii][2] == CO &&
                     txt[ii][3] == WT && txt[ii][4] == LC && txth[ii][0] == textSize.height/1000 &&
                     txth[ii][1] == textSize.width/1000 && 
                   ( kijyun[ii] == just ||  kijyun[ii] == -1) &&
                   ( ( mfcchk->font == -1 && txt[ii][6] == FT) ||  mfcchk->font != -1)){
				wcstombs(mbstr, str, 1024);
                /*--- �Q�����̏ꍇ ---*/
                if( mdlModelRef_is3D(MASTERFILE) != TRUE){
					
                    if( strlen( mbstr) == buf.text_2d.numchars){     flag2++;} // �������S�Ĕ��p�̏ꍇ�t���O�𗧂Ă�
                    if( strlen( mbstr)/2 == buf.text_2d.numchars/2 - 1){ flag2++;} // �������S�đS�p�̏ꍇ�t���O�𗧂Ă�
                }

                /*--- �R�����̏ꍇ ---*/
                else{
                    if( strlen(mbstr) == buf.text_3d.numchars){     flag2++;} // �������S�Ĕ��p�̏ꍇ�t���O�𗧂Ă�
                    if( strlen(mbstr) / 2 == buf.text_3d.numchars/2 - 1){ flag2++;} // �������S�đS�p�̏ꍇ�t���O�𗧂Ă�
                }

                /*--- �t�H���gON�̏ꍇ ---*/
                if( mfcchk->font == -1){

                    /*--- �G���[�̂Ȃ�������̏ꍇ ---*/
                    if( flag2 > 0){
                        flag2 = 0;
                        /*--- ���p�Őݒ肵���t�H���g�ƈ�v���邩CK  ---*/
                        if( tsi.font == mfcchk->han){
                            /*--- �Q�����̏ꍇ ---*/
                            if( mdlModelRef_is3D(MASTERFILE) != TRUE ){
                                if( strlen(mbstr) == buf.text_2d.numchars){ flag2++;} // �������S�Ĕ��p�̏ꍇ�t���O�𗧂Ă�
                            }

                            /*--- �R�����̏ꍇ ---*/
                            else{
                                if( strlen(mbstr) == buf.text_3d.numchars){ flag2++;} // �������S�Ĕ��p�̏ꍇ�t���O�𗧂Ă�
                            }
                        }

                        else if( tsi.font == mfcchk->zen){
                            /*--- �Q�����̏ꍇ ---*/
                            if( mdlModelRef_is3D(MASTERFILE) != TRUE ){
                                if( strlen(mbstr) / 2 == buf.text_2d.numchars/2 - 1){ flag2++;} // �������S�đS�p�̏ꍇ�t���O�𗧂Ă�
                            }

                            /*--- �R�����̏ꍇ ---*/
                            else{
                                if( strlen(mbstr) / 2 == buf.text_3d.numchars/2 - 1){ flag2++;} // �������S�đS�p�̏ꍇ�t���O�𗧂Ă�
                            }
                        }
                    }
                }

                /*--- �X�y�[�XON�̏ꍇ ---*/
                if( mfcchk->space == -1){
                    if( wcschr( str, L' ') != NULL){  flag2 = 0;} // ���p�X�y�[�X�����݂�����A�t���O�����Z�b�g����
                    if( wcschr( str, L'�@') != NULL){ flag2 = 0;} // �S�p�X�y�[�X�����݂�����A�t���O�����Z�b�g����
                }
            }
        } // FOR ii END

        /*--- �^�C�v��3,4,6,12,14,15,16�̂ǂꂩ�ŁA�t���O�������Ȃ������ꍇ ---*/
        if( ( TYP == 3 || TYP == 4 || TYP == 6 || TYP == 12 || TYP == 14 || TYP == 15 || TYP == 16) && flag1 == 0){

            /*--- �G���[���O�ɃG���[�v�f�̃��C���[���������� ---*/
            fwprintf ( fp_err, L"%3d %4d %4d %4d %4d\n", TYP, LV, CO, WT, LC);

            /*--- �G���[�t���OON�̏ꍇ�A�t�@�C���|�W�V�������L�� ---*/
            if( mfcchk->err == -1){
                linePos[linect] = elemAddr[0];
            }
            linect++;
        }

        /*--- �e�L�X�g�Ńt���O�������Ȃ������ꍇ ---*/
        else if( TYP == 17 && flag2 == 0){
            if( just == 0){       wcscpy( ejust, L"LT");} // ������_���u  ����  �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just ==  1){ wcscpy( ejust, L"LC");} // ������_���u ������ �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just ==  2){ wcscpy( ejust, L"LB");} // ������_���u  ����  �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just ==  6){ wcscpy( ejust, L"CT");} // ������_���u ������ �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just ==  7){ wcscpy( ejust, L"CC");} // ������_���u���������v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just ==  8){ wcscpy( ejust, L"CB");} // ������_���u ������ �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just == 12){ wcscpy( ejust, L"RT");} // ������_���u  �E��  �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just == 13){ wcscpy( ejust, L"RC");} // ������_���u �E���� �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����
            else if( just == 14){ wcscpy( ejust, L"RB");} // ������_���u  �E��  �v�̏ꍇ�A�G���[���O�p�ɃR�s�[����

            /*--- �G���[���O�ɃG���[�v�f�̃��C���[������������ ---*/
            fwprintf( fp_err, L"%3d %4d %4d %4d %4d %14.2f %6.2f %5s %6d %6d \n", TYP, LV, CO, WT, LC,
                              textSize.height/1000, textSize.width/1000, ejust, TC, tsi.font);

            /*--- �G���[�t���OON�̏ꍇ�A�t�@�C���|�W�V�������L�� ---*/
            if( mfcchk->err == -1){
                txtPos[txtct] = elemAddr[0];
            }
            txtct++;
        }

        /*--- �e�L�X�g�m�[�h�̏ꍇ ---*/
        else if( TYP == 7 && flag1 == 0){
            if( textParams.just == TextElementJustification::LeftTop)            { wcscpy( ejust, L"LT");} // ������_���u  ����  �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::LeftMiddle)    { wcscpy( ejust, L"LC");} // ������_���u ������ �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::LeftBaseline)  { wcscpy( ejust, L"LB");} // ������_���u  ����  �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::CenterTop)     { wcscpy( ejust, L"CT");} // ������_���u ������ �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::CenterMiddle)  { wcscpy( ejust, L"CC");} // ������_���u���������v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::CenterBaseline){ wcscpy( ejust, L"CB");} // ������_���u ������ �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::RightTop)      { wcscpy( ejust, L"RT");} // ������_���u  �E��  �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::RightMiddle)   { wcscpy( ejust, L"RC");} // ������_���u �E���� �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����
            else if( textParams.just == TextElementJustification::RightBaseline) { wcscpy( ejust, L"RB");} // ������_���u  �E��  �v�̏ꍇ�A�װ۸ޗp�ɺ�߰����

            fwprintf( fp_err, L"%3d %4d %4d %4d %4d %14.2f %6.2f %5s %6d %6d \n", TYP, LV, CO, WT, LC,
                              textSize.height/1000, textSize.width/1000, ejust, TC, textParams.font);

            if( mfcchk->err == -1){
                txtPos[txtct] = elemAddr[0];
            }
            txtct++;
        }
    } // WHILE END

    /*--- �G���[�t���OON�̏ꍇ ---*/
    if( mfcchk->err == -1){

        /*--- �Z���A�e�L�X�g�ȊO�̗v�f�̏ꍇ ---*/
        for( ii = 0; ii < linect; ii++){
            mdlElement_read( &buf, MASTERFILE, linePos[ii]);
            TYP = mdlElement_getType( &buf);

            /*--- �����A���A�����}�`�̏ꍇ ---*/
            if( TYP == 12 || TYP == 14){
				mdlElmdscr_read(&elmDP, linePos[ii], MASTERFILE, FALSE, NULL);
				mdlElmdscr_duplicate(&NelmDP, elmDP);
                //( &mfcchk->lv, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL, &ele);
				mdlElmdscr_setProperties(&NelmDP, &mfcchk->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				mdlElmdscr_setSymbology(&NelmDP, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL);
				mdlElmdscr_add(NelmDP);
				mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
                mdlElmdscr_freeAll( &NelmDP);
				mdlElmdscr_freeAll(&elmDP);
            }

            /*--- ���̑��̏ꍇ ---*/
            else{
                memcpy( &err, &buf, sizeof( MSElement));
                set_layer( &mfcchk->lv, &mfcchk->co, &mfcchk->lc, &mfcchk->wt, NULL, &err);
                mdlElement_add( &err);
                mdlElement_display( &err, DRAW_MODE_Normal);
            }
        } // FOR ii END

        /*--- �e�L�X�g�̏ꍇ ---*/
        for( ii = 0; ii < txtct; ii++){
            mdlElement_read( &buf, MASTERFILE, txtPos[ii]);
            TYP = mdlElement_getType( &buf);
			mdlText_extract(NULL, NULL, &userOrigin, NULL, NULL, NULL, NULL, &buf);
            /*--- �e�L�X�g�m�[�h�̏ꍇ ---*/
            if( TYP == 7){
                txtPos[ii] = txtPos[ii];
                mdlElement_read( &buf, MASTERFILE, txtPos[ii]);
				if (mdlModelRef_is3D(MASTERFILE)) {
					memcpy(&userOrigin, &buf.text_node_3d.origin, sizeof(Dpoint3d));
				}
				else
				{
					memcpy(&userOrigin, &buf.text_node_2d.origin, sizeof(Dpoint3d));
				}
				//mdlText_extract(NULL, &userOrigin, NULL, NULL, NULL, NULL, NULL, &buf);
            }
            
            create_errCircle( mfcchk->lv, mfcchk->co, mfcchk->lc, mfcchk->wt, hankei, &userOrigin);
            mdlElement_display( &buf, DRAW_MODE_Normal);
        } // FOR ii END
    }
}

/*----------------------------------------------------------------------+
|  ���W���[�����FscanStart_Cel                                          |
+----------------------------------------------------------------------*/
Private void scanStart_Cel( )
{
    int    getSize;
    int    Error;
    int    jj, flag;
    ULong  filePos;
    WChar   cellName[7];
    MSElement       buf;
	MSElementDescr	*descr;
    ExtScanList     scanList;
    Dpoint3d        origin;
	ULong				elemAddr[1];
	int 				scanwords;

    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*--------- scanList�̊e�����o�[�Ɉȉ����Z�b�g --------------*/
	scanList.scantype =  ELEMTYPE | ONEELEM;
	scanList.extendedType = FILEPOS;
	scanList.clasmsk = 0xFFF1;
    /*--------- �����Ώۂ̌��� ---------------*/
    scanList.typmask[0] |= TMSK0_CELL_HEADER; /* �Ώۂ;� */

    /*--------- �X�L�������s ---------------*/
    if( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlOutput_message( L"�A�����[�h���܂����B");
        mdlSystem_exit( exitStatus, 1);
		return;
    }

    /*--- �e��ϐ��̏����� ---*/
    filePos = 0;
	//scanwords = sizeof(elemAddr) / sizeof(short);
    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
        if( Error == 10){ break;}
        if( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
        if( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
        if( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
        if( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
        if( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read(&buf, MASTERFILE, elemAddr[0]);
        TYP = mdlElement_getType ( &buf);

        /*--- �Z���̏ꍇ ---*/
        if( TYP == 2){
            mdlCell_extract( &origin, NULL, NULL, NULL, cellName,NULL,  &buf);
			mdlCell_extractName(cellName, 256, &buf);

			mdlElmdscr_read(&descr, elemAddr[0], MASTERFILE, 0, NULL);
			get_layer( &LV, &CO, &LC, &WT, NULL, &descr->h.firstElem->el);
			mdlElmdscr_freeAll(&descr);
			flag = 0;
			for( jj = 0; jj <= memo; jj++){
				if( ( cell[jj][1] == LV) && ( cell[jj][2] == CO) &&
					( cell[jj][3] == WT) && ( cell[jj][4] == LC) &&
					( wcscmp( cname[jj], cellName) == 0)){
					flag++;
				}
			}
			if( flag == 0){
				fwprintf( fp_err, L"  2 %4d %4d %4d %4d %7s\n", LV, CO, WT, LC, cellName);
				if( mfcchk->err == -1){

					cellPos[cellct] = elemAddr[0];
					mdlElement_read( &buf, MASTERFILE, cellPos[cellct]);
					create_errCircle( mfcchk->lv, mfcchk->co, mfcchk->lc, mfcchk->wt, hankei, &origin);
					mdlElement_display( &buf, DRAW_MODE_Normal);
				}
				cellct++;
			}
		}

    } // WHILE END
}

/*----------------------------------------------------------------------+
|  ���W���[�����Fsort                                                   |
+----------------------------------------------------------------------*/
Private void sort ( )
{
    int   total_str = 10000;       /* ������̑��� */
    int   ii, jj,  mm, nn;
    WChar  devP[64], dirP[64];

    if( ( fp_err = _wfopen( file_err, L"r")) == NULL){
        mdlOutput_error( L"���̓t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- �\�[�g�t�@�C�����̎擾 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, NULL, NULL);
    swprintf( file_sort, L"%ls:%lssort.txt", devP, dirP);

    if( ( fp_sort = _wfopen( file_sort, L"w")) == NULL){
        mdlOutput_error( L"�o�̓t�@�C�����J���܂���");
		mdlSystem_exit( exitStatus, 1);
		return;
    }

    ii = 0;

    /* �P����t�@�C�����烁�����ɓǂݍ��� */
    while( fgetws( s, max_str, fp_err) != NULL){
        /* ���s�R�[�h���폜 */
        jj = wcslen( s);
        if( ( *( s + jj - 1)) == '\n')
            ( *( s + jj - 1)) = '\0';
        wcscpy( pp[ii], L"");
        wcscpy( pp[ii], s);
        ii++;
    }

    /* �o�^�����P��̌����\�z�������Ȃ��ꍇ */
    if( ii < total_str){
        total_str = ii;
    }

    /* �f�[�^���\�[�g */
    for( mm = 0; mm < total_str - 1; mm++){
        for( nn = mm + 1; nn < total_str; nn++){
            /* �f�[�^�̔�r, ���� */
            if( wcscmp( pp[mm], pp[nn]) > 0) {
                wcscpy( keep, pp[nn]);
                wcscpy( pp[nn], pp[mm]);
                wcscpy( pp[mm], keep);
            }
        }
    }


    /* �\�[�g�̌��ʂ�\�� */
    for( ii = 0; ii < total_str; ii++){
        fwprintf( fp_sort, L"%ls\n", pp[ii]);
    }

    fclose( fp_err);
    fclose( fp_sort);
}

/*----------------------------------------------------------------------+
|  ���W���[�����Ferr_mem                                                |
+----------------------------------------------------------------------*/
Private void err_mem( )
{
    mdlOutput_error( L"�������̊m�ۂɎ��s���܂����B");
	mdlSystem_exit(exitStatus, 1);
	return;
}

/*----------------------------------------------------------------------+
|  ���W���[�����Fprint_err                                              |
+----------------------------------------------------------------------*/
Private void print_err( )
{
    int   count = 0;
    int   ii;
    WChar  before[256], now[256], sort_now[256], sort_before[256];
    WChar  devP[64], dirP[64], nameP[64];

    if( ( fp_sort = _wfopen( file_sort, L"r")) == NULL){
        mdlOutput_error( L"�\�[�g�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
    }

    /*--- �o�̓t�@�C�����̎擾 ---*/
    mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
    swprintf( file, L"%ls:%ls%ls.err", devP, dirP, nameP);

    /*--- �o�̓t�@�C���̃I�[�v�� ---*/
    if( ( fp_list = _wfopen( file, L"w")) == NULL){
        mdlOutput_error( L"�o�̓t�@�C�����J���܂���");
		mdlSystem_exit( exitStatus, 1);
		return;
        
    }

    fwprintf( fp_list, L"%ls\n", tcb->dgnfilenm);
    fwprintf( fp_list, L"\nTYP   LV   CO   WT   LC  �Z����   ����   ����   �   ����   ����  �G���[\n");

    fgetws( before, max_str, fp_sort);
    ii = wcslen( before);

    if( ( *( before + ii - 1)) == '\n')
        ( *( before + ii - 1)) = '\0';
    wcscpy( sort_before, before);

    fclose( fp_sort);

    if( ( fp_sort = _wfopen( file_sort, L"r")) == NULL){
        mdlOutput_error( L"�\�[�g�t�@�C�����J���܂���");
		mdlSystem_exit( exitStatus, 1);
		return;
    }

    while( fgetws( now, max_str, fp_sort) != NULL){
        ii = wcslen( now);

        if( ( *( now + ii - 1)) == '\n')
            ( *( now + ii - 1)) = '\0';
        wcscpy( sort_now, now);

        if( wcscmp( sort_before, sort_now) == 0){
            count++;
        }
        else{
            fwprintf( fp_list, L"%-67s %3d\n", sort_before, count);
            wcscpy( sort_before, sort_now);
            count = 1;
        }
    }
    fwprintf( fp_list, L"%-67s %3d\n", sort_now, count);
    fwprintf( fp_list, L"\n                                                         �G���[������%d�ł��B\n", mfcchk->ct);
    fwprintf( fp_list, L"���� 2:��                        LT�F����");
    fwprintf( fp_list, L"\n���� 3:ײ�                       LC�F������");
    fwprintf( fp_list, L"\n���� 4:ײݽ��ݸ�                 LB�F����");
    fwprintf( fp_list, L"\n���� 6:�����                     CT�F������");
    fwprintf( fp_list, L"\n���� 7:÷��ɰ��                  CC�F��������");
    fwprintf( fp_list, L"\n����12:����گ������              CB�F������");
    fwprintf( fp_list, L"\n����14:����گ�������             RT�F�E��");
    fwprintf( fp_list, L"\n����15:����                      RC�F�E����");
    fwprintf( fp_list, L"\n����16:���                       RB�F�E��");
    fwprintf( fp_list, L"\n����17:÷��");
    fwprintf( fp_list, L"\n");

    fclose( fp_sort);
    fclose( fp_list);
}


