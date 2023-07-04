/*-----------------------------------------------------------------------*/
/*-     �Q�Ƃ��e�e�\�[�X                                                -*/
/*-         FF.MC                                                       -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../CommonCE/naka8.cpp"

#include "ffref.h"

FILE            *fp;
RscFileHandle   Rsc;
int             exitStatus, gridct, namect, elect, ii, polnv, masElemCt;
WChar           file[256], filename[256], info[256], orifile[256];
UInt32          CO, WT, LV;
int             LC, TYP;
ULong           gridPos[5000], shapePos;
Dpoint3d        polPoi[10];

FFREF           *ffref;
#ifndef Private
#define Private
#endif

/*- �v���g�^�C�v    -*/
Private  bool   unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private void    scanShape( void);
Private int     get_Number( void);
Private int     ff_copy( void);
Private int     ff_copy_text( void);
Private int     delete1( void);
Private void    no_del( void);
Private int fence_clip_outside(void);
Private int fence_clip_inside(void);
Private void    on_all_dgn(void);
Private void    on_all_ref(void);

/*-----------------------------------------------------------------------
**  ���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
UInt32           param[8][4];
int             slot;
CExprValue      value;
CExprResult     result;
DgnModelRefP		modelRefP;
ModelRefIteratorP	iterator;
bool				active;
UInt32          oldoptimized;


    for( ii=0; ii<8; ii++){
        param[ii][0] = 0xFFFF;
        param[ii][1] = 0xFFFF;
        param[ii][2] = 0xFFFF;
        param[ii][3] = 0x7FFF;
    }

    /*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"FF �̃��\�[�X�t�@�C�����J���܂���");
        mdlSystem_exit( exitStatus, 1);
    }

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- �L��ϐ��̎擾 ---*/
    if ( mdlCExpression_getValue( &value, &result, "&ffref",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"�L��ϐ����擾�ł��܂���");
        mdlSystem_exit( exitStatus, 1);
    }
    ffref = (FFREF *)value.val.valPointer;
    wcscpy( orifile,  tcb->dgnfilenm);

    /*--- �t�B�b�g�I�[���E�c�o�E�Q�ƃt�@�C����LV_ON�E�ݒ�l�̕ۑ� ---*/
    mdlView_fit( 0, 0);
    mdlView_setDisplayDepth( 0, -999999.0000, 3294968.2940);
    mdlView_updateSingle( 0);
	/*--- �œK�����ꂽ�g�N���b�v�𖳌��ɂ��� ---*/
	oldoptimized = userPrefsP->smartGeomFlags.optimizedFenceClip;
	userPrefsP->smartGeomFlags.optimizedFenceClip = 0;


	/*
DgnModelRefP		modelRefP;
ModelRefIteratorP	iterator;
bool				active;
WChar               valuestr[512];
LevelId             lvId;
int                 cnt = 0;
bool                ret;

swprintf(valuestr, L"");
mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
{
	mdlRefFile_getBooleanParameters(&active, REFERENCE_SLOTACTIVE, modelRefP);
	if (mdlModelRef_isReference(modelRefP)) {

		LevelIteratorR lviter = mdlLevelIterator_create(modelRefP);

		while (TRUE)
		{
			if (cnt == 0) {
				lvId = mdlLevelIterator_getFirst(lviter);
				cnt = cnt++;
			}
			else {
				lvId = mdlLevelIterator_getNext(lviter);
			}
			mdlLevel_setDisplay(modelRefP, lvId, FALSE);
			ret = mdlLevel_isValid(modelRefP, lvId);
			if (!ret) {
				break;
			}
		}
		mdlLevelIterator_free(lviter);
	}
}
mdlModelRefIterator_free(&iterator);







	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		mdlRefFile_getBooleanParameters(&active, REFERENCE_SLOTACTIVE, modelRefP);
        if (!mdlModelRef_isReference(modelRefP)) {
            //mdlRefFile_setParameters( param, REFERENCE_LEVELFLAGS, ii);REFERENCE_LEVELCONTROLSDISPLAY
			mdlRefFile_setIntegerParameters(param, REFERENCE_LEVEL, modelRefP);
            mdlRefFile_writeAttachment( modelRefP);
        }
    }
	*/
    mdlSystem_fileDesign( );

    scanShape( );

	userPrefsP->smartGeomFlags.optimizedFenceClip = oldoptimized;

}


/*-----------------------------------------------------------------------
**  �A�����[�h�֐�
-----------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*----------------------------------------------------------------------+
|  ���W���[�����FscanShape                                              |
+----------------------------------------------------------------------*/
Private void scanShape( )
{
int    		getSize;
int    		Error;
int    		word;
ULong  		filePos, EndPos, percent;

MSElement   ele;
ExtScanList    scanList;
ULong		elemAddr[1];
//int			i, numAddr;

    /*--- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

   // scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= TMSK0_SHAPE;       /* �Ώۂͼ���� */

    /*--- �X�L�������s ---------------*/
    filePos = 0;   gridct = 0;

    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlOutput_message( L"�A�����[�h���܂����B");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
        //Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);

        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        get_layer( &LV, &CO, &LC, &WT, NULL, &ele);

        if ( ( ffref->lv == LV) && ( ffref->co == CO) && ( ffref->wt == WT) && ( ffref->lc == LC)){
            //gridPos[gridct] = filePos;
			gridPos[gridct] = elemAddr[0];
            gridct++;
        }
    }

    /*--- �V�F�C�v�Ńt�F���X���쐬 ---*/
    for( ii=0; ii<gridct; ii++){
        elect    = 0;   namect = 0;
        shapePos = gridPos[ii];
        mdlElement_read( &ele, MASTERFILE, shapePos);
        mdlFence_fromShape( &ele);

        /*--- �t�F���X�̐ݒ�i�����d�Ȃ�j ---*/
        set_fence_overlap( );
        tcb->fenvw = 0;

        /*--- �t�F���X���̃e�L�X�g��ǂݍ��� ---*/
        mdlState_startFenceCommand((StateFunc_FenceContentOp)get_Number, NULL, NULL, NULL, NULL, NULL, NULL);
        mdlFence_process( NULL);

        /*--- �A�N�e�B�u�̃e�L�X�g���P�ŁA�Q�Ɨv�f��1�ȏ゠��ꍇ ---*/
        if ( elect != 0 && namect == 1){
            mdlLinear_extract( polPoi, &polnv, &ele, MASTERFILE);
            mdlFile_copy( file, tcb->dgnfilenm);
            if ( mdlSystem_newDesignFile( filename) == SUCCESS){
				masElemCt = 0;
                syori( );
				if (masElemCt == 0) mdlFile_delete(filename);
            }
        }
        mdlFence_clear( TRUE);
    } // FOR II END
}


/*-----------------------------------------------------------------------
**  get_Number : �V�F�C�v���̐}�ʔԍ����L������
-----------------------------------------------------------------------*/
Private int get_Number( void)
{
ULong  		   Position;
WChar   		str[128];
WChar   		devP[64], dirP[64], nameP[64];
int    		   fileNum, typ;
MSElement      ele;
DgnModelRefP   ModelRefP;
WString        wstr;

    Position = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

	if (!mdlModelRef_isValid(ModelRefP))
		return ERROR;


    /*--- �Q�ƃt�@�C���̏ꍇ ---*/
    if (mdlModelRef_isReference(ModelRefP)){
       elect++;
    }

    /*--- �A�N�e�B�u�t�@�C���̏ꍇ ---*/
    if (!mdlModelRef_isReference(ModelRefP)){
        mdlElement_read( &ele, MASTERFILE, Position);
        typ = mdlElement_getType( &ele);

        /*--- �e�L�X�g�̏ꍇ ---*/
        if ( typ == 17){
            mdlElement_read( &ele, MASTERFILE, Position);
            mdlText_extract( &wstr, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
			swprintf(str, L"%ls", wstr.GetWCharCP());
            get_layer( &LV, &CO, &LC, &WT, NULL, &ele);
            wcscpy( file, L"");
            if ( ffref->nolv == LV && ffref->noco == CO && ffref->nowt == WT && ffref->nolc == LC){
                mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
                if ( ffref->rdl != -1){
                    swprintf( file, L"%s:%s%s.dgn", devP, dirP, str);
                }
                else{
                    swprintf( file, L"%s:%s%s.rdl", devP, dirP, str);
                }
                wcscpy( filename, file);
                namect++;
            }
        }
    }
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void  syori(void)
{
	MSElement       buf, pele, oele;
	MSElementDescr    *elmDP;
	static short    viewList[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
	static short    levelMask[4] = { 0xFFFF, 0xFFFF, 0xFFFF, 0x7FFF };
	int             snv, polNo;
	Dpoint3d        sPoi[5];  // �}�s��4���̍��W
	Dpoint3d        fPoi[5];  // �}�s����1mm������4���̍��W
	Dpoint3d        oPoi[5];  // �}�s����1mm�O����4���̍��W
	double          AB_len, BC_len;

	tcb->fenvw = 0;
	/*--- �}�s�̏��𒲂ׂ� ---*/
	mdlElement_read(&buf, MASTERFILE, shapePos);
	mdlLinear_extract(sPoi, &snv, &buf, MASTERFILE);


	/*--- �}�s����1mm�����ɕ��s�ړ������g�̍��W�����߂� ---*/
	AB_len = get_leng(&sPoi[0], &sPoi[1]);
	BC_len = get_leng(&sPoi[1], &sPoi[2]);

	/*--- ��AB����BC��蒷���ꍇ ---*/
	if (AB_len >= BC_len) {
		/*--- ���_A��X���W�����_B��X���W�����������ꍇ ---*/
		if (sPoi[0].x < sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;   oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x - 1;               oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x - 1;               oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x + 1;               oPoi[3].x = sPoi[3].x - 1;

			/*--- ���_B��Y���W�����_C��Y���W�����傫���ꍇ ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 1;
			}

			/*--- ���_B��Y���W�����_C��Y���W�����������ꍇ ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 2;
			}
		}

		/*--- ���_A��X���W�����_B��X���W�����傫���ꍇ ---*/
		else if (sPoi[0].x > sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- ���_B��Y���W�����_C��Y���W�����傫���ꍇ ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 3;
			}

			/*--- ���_B��Y���W�����_C��Y���W�����������ꍇ ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 4;
			}
		}

		/*--- ���_B��X���W�����_C��X���W�����������ꍇ ---*/
		else if (sPoi[1].x < sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;   oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x + 1;               oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x - 1;               oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x - 1;               oPoi[3].x = sPoi[3].x + 1;

			/*--- ���_A��Y���W�����_B��Y���W�����傫���ꍇ ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 9;
			}

			/*--- ���_A��Y���W�����_B��Y���W�����������ꍇ ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 10;
			}
		}

		/*--- ���_B��X���W�����_C��X���W�����傫���ꍇ ---*/
		else if (sPoi[1].x > sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- ���_A��Y���W�����_B��Y���W�����傫���ꍇ ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 11;
			}

			/*--- ���_A��Y���W�����_B��Y���W�����������ꍇ ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 12;
			}
		}
	} // IF(AB > BC) END

	/*--- ��AB����BC���Z���ꍇ ---*/
	else if (AB_len < BC_len) {
		/*--- ���_B��X���W�����_C��X���W�����������ꍇ ---*/
		if (sPoi[1].x < sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x - 1;                oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- ���_A��Y���W�����_B��Y���W�����傫���ꍇ ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 5;
			}

			/*--- ���_A��Y���W�����_B��Y���W�����������ꍇ ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 6;
			}
		}

		/*--- ���_B��X���W�����_C��X���W�����傫���ꍇ ---*/
		else if (sPoi[1].x > sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- ���_A��Y���W�����_B��Y���W�����傫���ꍇ ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 7;
			}

			/*--- ���_A��Y���W�����_B��Y���W�����������ꍇ ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 8;
			}
		}

		/*--- ���_A��X���W�����_B��X���W�����������ꍇ ---*/
		if (sPoi[0].x < sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x - 1;                oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- ���_B��Y���W�����_C��Y���W�����傫���ꍇ ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 13;
			}

			/*--- ���_B��Y���W�����_C��Y���W�����������ꍇ ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 14;
			}
		}

		/*--- ���_A��X���W�����_B��X���W�����傫���ꍇ ---*/
		else if (sPoi[0].x > sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- ���_B��Y���W�����_C��Y���W�����傫���ꍇ ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 15;
			}

			/*--- ���_B��Y���W�����_C��Y���W�����������ꍇ ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 16;
			}
		}
	} // ELSE IF(AB < BC) END

		/*--- dgn �S��w�\�� ---*/
	on_all_dgn();

	/*--- ref �S��w�\�� ---*/
	on_all_ref();

	/*--- �t�F���X�̐ݒ�i�O���d�Ȃ�j ---*/
	set_fence_void_overlap();

	/*--- �}�s����1mm�O���ɃV�F�C�v���쐬 ---*/
	mdlShape_create(&oele, &buf, oPoi, snv, -1);
	mdlFence_fromShape(&oele);

	/*--- �g�O�i�אڐ}�s�Ɨאڐ}�ԁj���폜���� ---*/
	mdlState_startFenceCommand((StateFunc_FenceContentOp)delete1, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
	mdlFence_process(NULL);

	tcb->fenvw = 0;
	/*--- �������S�v�f�̏ꍇ ---*/
	if (ffref->mode == 0) {
		/*--- �t�F���X�̐ݒ�i�����؎�j ---*/
		set_fence_clip();

		/*--- �}�s�Řg�쐬 ---*/
		mdlFence_fromShape(&buf);

		//--- �t�F���X�����d�Ȃ� ---
		mdlParams_setLock(0, ACTIVELOCK_FENCECLIP);
		mdlParams_setLock(1, ACTIVELOCK_FENCEOVERLAP);
		mdlParams_setLock(0, ACTIVELOCK_FENCEVOID);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_NO_CLIP);
		mdlFence_process(NULL);

		//--- �t�F���X�O���؂��� ---
		mdlParams_setLock(1, ACTIVELOCK_FENCECLIP);
		mdlParams_setLock(0, ACTIVELOCK_FENCEOVERLAP);
		mdlParams_setLock(1, ACTIVELOCK_FENCEVOID);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_outside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
		mdlFence_process(NULL);

		//--- �t�F���X�̐ݒ�i�O���d�Ȃ�j ---
		set_fence_void_overlap();
		mdlFence_fromShape(&oele);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_outside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
		mdlFence_process(NULL);

	}

	/*--- �������Z���݂̂̏ꍇ ---*/
	if (ffref->mode == 1) {
		/*--- �t�F���X�̐ݒ�i�����d�Ȃ�j ---*/
		set_fence_overlap();

		/*--- �}�s�Řg�쐬 ---*/
		mdlFence_fromShape(&buf);

		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);

	}

	/*--- �|���S���̏ꍇ ---*/
	if (ffref->mode == 2) {
		/*--- �t�F���X�̐ݒ�i�����d�Ȃ�j ---*/
		set_fence_overlap();

		/*--- �}�s���������1mm�]�ڂŘg�쐬 ---*/
		mdlShape_create(&pele, &buf, fPoi, snv, -1);
		mdlFence_fromShape(&pele);

		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);
	}
	/*

	//--- �Q�ƃt�@�C������g�����R�s�[ ---
	mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
	mdlFence_process( NULL);
	*/
	//--- �������S�v�f�E�|���S���̏ꍇ ---
	if (ffref->mode != 1) {
		//--- �t�F���X�̐ݒ�i�����d�Ȃ�j ---
		set_fence_overlap();

		//--- �Q�ƃt�@�C������g�����R�s�[�i�e�L�X�g�v�f�j ---
		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy_text, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);
	}

	mdlRefFile_detach(REFERENCE_All_References);                                   // �Q�ƃt�@�C����S�f�^�b�`
	mdlView_setLevels(TRUE, levelMask, viewList, TRUE);      // Lv�����ׂ�ON

	/*--- �}�ʔԍ����폜 ---*/
	no_del();
	if (ffref->mode != 0) {
		mdlElmdscr_read(&elmDP, shapePos, MASTERFILE, 0, NULL);
		mdlElmdscr_undoableDelete(elmDP, shapePos, TRUE);
		mdlElmdscr_freeAll(&elmDP);
	}
	/*--- �v�f�̑��݌��� ---*/
	if (ffref->mode == 0) {
		set_fence_clip();
	}
	else {
		set_fence_overlap();
	}
	mdlFence_fromShape(&buf);
	mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_inside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
	mdlFence_process(NULL);
	
	mdlElement_add(&buf);
	mdlElement_display(&buf, DRAW_MODE_Normal);
	
    mdlView_fit( 0, 0);                                      // �t�B�b�g�I�[��
    mdlView_setDisplayDepth( 0, -999999.0000, 3294968.2940); // DP�̍Đݒ�
    mdlView_updateSingle( 0);                                // �r���[�̍ĕ\��
    mdlSystem_fileDesign( );                                 // �ݒ�l��ۑ�

    mdlSystem_newDesignFile( orifile);
}


/*-----------------------------------------------------------------------
**  ff_copy
-----------------------------------------------------------------------*/
Private int  ff_copy( void)
{
MSElement         ele;
MSElementDescr    *elmDP;
ULong             elePos;
int               fileNum, celct;
Dpoint3d          celPoi;
DgnModelRefP      ModelRefP;
WChar  fileName[512];
int  fileNameBufSize;

    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

	//fileNameBufSize = 512;
	//mdlModelRef_getFileName( ModelRefP,	fileName, fileNameBufSize);
	
	
    /*--- �Q�ƃt�@�C������v�f���R�s�[ ---*/
	if (mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, ModelRefP, elePos);
        TYP = mdlElement_getType( &ele);

        /*--- �Z�� ---*/
        if ( TYP == 2){
            celct = 0;
            /*--- �Z���݂̂̏ꍇ ---*/
            if ( ffref->mode == 1){
                mdlCell_extract( &celPoi, NULL, NULL, NULL, NULL, NULL, &ele);

                /*--- �Z���̊���|���S���O�̏ꍇ ---*/
                if ( 0 == judge_inout( &polnv, polPoi, &celPoi)){
                    celct++;
                }
            }

            if ( celct == 0){
                mdlElmdscr_read( &elmDP, elePos, ModelRefP, TRUE, NULL);
                mdlElmdscr_add( elmDP);
                mdlElmdscr_display( elmDP, ModelRefP, DRAW_MODE_Normal);
                mdlElmdscr_freeAll( &elmDP);
            }
        }

        /*--- �����v�f ---*/
        else if ( TYP == 12 || TYP == 14){
            /*--- �Z���݂̂łȂ��ꍇ ---*/
            if ( ffref->mode != 1){
               mdlElmdscr_read( &elmDP, elePos, ModelRefP, TRUE, NULL);
               mdlElmdscr_add( elmDP);
               mdlElmdscr_display( elmDP, ModelRefP, DRAW_MODE_Normal);
               mdlElmdscr_freeAll( &elmDP);
           }
        }

        /*--- �e�L�X�g ---*/
        else if ( TYP == 17){
            /*--- �Z���݂̂łȂ��ꍇ ---*/
            //if ( ffref->mode != 1){
                //mdlElement_add( &ele);
                //mdlElement_display( &ele, NORMALDRAW);
            //}
        }
        /*--- ���̑� ---*/
        else{
            /*--- �Z���݂̂łȂ��ꍇ ---*/
            if ( ffref->mode != 1){
                mdlElement_add( &ele);
                mdlElement_display( &ele, DRAW_MODE_Normal);
            }
        }
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  ff_copy_text
-----------------------------------------------------------------------*/
Private int  ff_copy_text( void)
{
MSElement         ele;
ULong             elePos;
int               fileNum;
Dpoint3d          orig;
DgnModelRefP      ModelRefP;

    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

    /*--- �Q�ƃt�@�C������v�f���R�s�[ ---*/
	if (mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, ModelRefP, elePos);
        TYP = mdlElement_getType( &ele);

        /*--- �e�L�X�g�̏ꍇ ---*/
        if ( TYP == 17){
            mdlText_extract( NULL, &orig, NULL, NULL, NULL, NULL,   NULL, &ele);

            /*--- �e�L�X�g�̊�_���}�s���̏ꍇ ---*/
            if ( 1 == judge_inout( &polnv, polPoi, &orig)){
               mdlElement_add( &ele);
               mdlElement_display( &ele, DRAW_MODE_Normal);
            }
        }
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  delete1
-----------------------------------------------------------------------*/
Private int  delete1( void)
{
MSElement    ele;
ULong        elePos;
int          fileNum;
int          lv, co, lc, wt;
MSElementDescr  *elmDP;
DgnModelRefP      ModelRefP;


    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

    /*--- �A�N�e�B�u�ȃf�U�C���t�@�C���̗v�f�̂ݍ폜 ---*/
    if ( !mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, MASTERFILE, elePos);
        mdlElmdscr_read( &elmDP, elePos, MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, elePos, TRUE);
        mdlElmdscr_freeAll( &elmDP);
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  �}�ʔԍ��폜
-----------------------------------------------------------------------*/
Private void  no_del( void)
{
int    getSize;
int    Error;
int    word;
UInt32 tlv, tco, twt;
int    tlc;
ULong  filePos, delPos;

MSElementDescr  *elmDP;
MSElement       ele;
ExtScanList        scanList;
ULong		elemAddr[1];
//int			i, numAddr;

    /*--- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    scanList.scantype |= ( unsigned short)FILEPOS;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= TMSK0_LINE;        /* �Ώۂ�ײ� */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* �Ώۂ�ײݽ��ݸ� */
    scanList.typmask[1] |= TMSK1_TEXT;        /* �Ώۂ�÷�� */

    /*--- �X�L�������s ---------------*/
    filePos = 0;

    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlOutput_message( L"�A�����[�h���܂����B");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);

        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox( L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox( L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox( L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox( L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        get_layer( &tlv, &tco, &tlc, &twt, NULL, &ele);

        /*--- �w�肵���}�ʔԍ��Ɠ������C���[�̃e�L�X�g���폜���� ---*/
        if ( ( ffref->nolv == tlv) && ( ffref->noco == tco) && ( ffref->nowt == twt) && ( ffref->nolc == tlc)){
			delPos = elemAddr[0];
            mdlElmdscr_read( &elmDP, delPos, MASTERFILE, 0, NULL);
            mdlElmdscr_undoableDelete( elmDP, delPos, TRUE);
            mdlElmdscr_freeAll( &elmDP);
        }

        /*--- �w�肵���}�g�Ɠ������C���[�̃��C�����폜���� ---*/
        else if ( ( ffref->lv == tlv) && ( ffref->co == tco) && ( ffref->wt == twt) && ( ffref->lc == tlc)){
            delPos = elemAddr[0];
            mdlElmdscr_read( &elmDP, delPos, MASTERFILE, 0, NULL);
            mdlElmdscr_undoableDelete( elmDP, delPos, TRUE);
            mdlElmdscr_freeAll( &elmDP);
        }
    } // WHILE END
}

/*------------------------------------------------------------------------
**  �t�F���X�i�O���؂���j
------------------------------------------------------------------------*/
int fence_clip_outside(void)
{
	DgnModelRefP    model;
	ULong           filePos;
	MSElementDescr  *dscr;
	int             type;

	filePos = mdlElement_getFilePos(FILEPOS_CURRENT, &model);
	if (ffref->mode == 0) {
		if (model == MASTERFILE) {
			mdlElmdscr_read(&dscr, filePos, model, FALSE, NULL);

			mdlElmdscr_undoableDelete(dscr, filePos, TRUE);
			mdlElmdscr_freeAll(&dscr);
			return(SUCCESS);

		}
	}
	return(SUCCESS);
}

/*------------------------------------------------------------------------
**  �t�F���X�i�����؂���j
------------------------------------------------------------------------*/
int fence_clip_inside(void)
{
	DgnModelRefP    model;
	ULong           filePos;
	MSElementDescr  *dscr;
	int             type;

	filePos = mdlElement_getFilePos(FILEPOS_CURRENT, &model);
	
	if (model == MASTERFILE) {

		masElemCt++;
		return(SUCCESS);

	}

	return(SUCCESS);
}

/*------------------------------------------------------------------------
**  dgn �S��w�\��
------------------------------------------------------------------------*/
Private void on_all_dgn(void)
{
	int        ii;
	short      viewList[8];
	short      levMask[4];

	for (ii = 0; ii < 8; ii++) { viewList[ii] = 1; }
	for (ii = 0; ii < 4; ii++) { levMask[ii] = 0xFF; }
	mdlView_setLevels(TRUE, levMask, viewList, FALSE);

}

/*------------------------------------------------------------------------
**  ref �S��w�\��
------------------------------------------------------------------------*/
Private void on_all_ref(void)
{
	ModelRefIteratorP   iterator = NULL;
	DgnModelRefP        model;
	UInt32              ii;
	LevelId             levelID;
	bool                disp = TRUE;

	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_Root | MRITERATE_PrimaryChildRefs, 0);

	/*--- �Q�ƃt�@�C�� ---*/
	while ((model = mdlModelRefIterator_getNext(iterator)) != NULL) {
		/*--- �Q�ƃt�@�C����\�� ---*/
		mdlRefFile_setBooleanParameters(disp, REFERENCE_DISPLAY, model);

		/*--- �ő��wID ---*/
		mdlLevelTable_getHighestLevelId(&levelID, model);

		/*--- ��w�\�� ---*/
		for (ii = 1; ii <= levelID; ii++) {
			mdlView_setLevelDisplay(model, tcb->lstvw, ii, TRUE);
		}

		/*--- �Q�Ə���ۑ� ---*/
		mdlRefFile_writeAttachmentConditionally(model);
	}

	mdlModelRefIterator_free(&iterator);
}


