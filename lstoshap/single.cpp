/*-----------------------------------------------------------------------*/
/*-	        SINGLE.MC													-*/
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

RscFileHandle           Rsc;
MSElementDescr          *elmDP, *elmDP2, *oelmDP;
MSElement               oele, nele, ele, buf;
int                     exitStatus;
ULong                   linePos, cnvPos[10000];
int                     cnvnum;
GLOBAL                  *glo;

#ifndef Private
#define Private
#endif

/*-	�v���g�^�C�v	-*/

Private bool   unload(UnloadProgramReason unloadType);
Private void	syori( void);
Private void	set_searchtype_line( void);
Private void	set_searchtype_shape( void);
Private void	cnv_shape_to_ls( void);
Private void	cnv_ls_to_shape( void);
Private void	cnv_shape_to_shape( void);
Private void	cleanUp( void);


/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C"  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    CExprResult    result;
    CExprValue     value;

    /*--- ���\�[�X�t�@�C���̃I�[�v�� ---*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlDialog_openInfoBox(L"SINGLE �̃��\�[�X�t�@�C�����J���܂���");
		return;
    }

	/*--------- �A�����[�h�֐��Z�b�g ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--------- �L��ϐ��̎擾 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&glo",
        VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
		return;
    }
    glo = (GLOBAL*)value.val.valPointer;

    /*--- 1�v�f���������� ---*/
    syori( );
	
}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	����
-----------------------------------------------------------------------*/
Private void	syori( )
{
    linePos = 0;    cnvPos[0] = 0;
    /*--- ���C���X�g�����O���V�F�C�v ---*/
    if ( glo->option == 0){
        set_searchtype_line( );
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_ls_to_shape, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }

    /*--- �V�F�C�v�����C���X�g�����O ---*/
    if ( glo->option == 1){
        set_searchtype_shape( );
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_shape_to_ls, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }

    /*--- �V�F�C�v���V�F�C�v ---*/
    if ( glo->option == 2){
        set_searchtype_shape( );
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_shape_to_shape, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }
}


/*-----------------------------------------------------------------------
**	�v�f�ݒ� --- ���C���X�g�����O
-----------------------------------------------------------------------*/
Private void	set_searchtype_line( )
{
    static	int	searchType[] = { LINE_ELM, LINE_STRING_ELM};

    mdlLocate_init( );
    mdlLocate_noElemNoLocked( );
    mdlLocate_setElemSearchMask( sizeof( searchType )/sizeof( int ), searchType );
}


/*-----------------------------------------------------------------------
**	�v�f�ݒ� --- �V�F�C�v
-----------------------------------------------------------------------*/
Private void	set_searchtype_shape( )
{
    static	int	searchType[] = { SHAPE_ELM, CMPLX_SHAPE_ELM};

    mdlLocate_init( );
    mdlLocate_noElemNoLocked( );
    mdlLocate_setElemSearchMask( sizeof( searchType )/sizeof( int ), searchType );
}


/*-----------------------------------------------------------------------
**	���Z�b�g�{�^�� --- ���C���X�g�����O
-----------------------------------------------------------------------*/
Private	void	cleanUp( void )
{
    if ( glo->option == 0){
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_ls_to_shape, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }

    else if ( glo->option == 1){
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_shape_to_ls, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }

    else if ( glo->option == 2){
        mdlState_startModifyCommand( (StateFunc_Reset)cleanUp, (MdlFunc_Accept)cnv_shape_to_shape, NULL, NULL, NULL, 1, 3,FALSE, 1 );
    }

}


/*-----------------------------------------------------------------------
**	���C���X�g�����O���V�F�C�v
-----------------------------------------------------------------------*/
Private void	cnv_ls_to_shape( )
{
    Dpoint3d    *lPoi;
    ULong       renPos[1000];
    int         ii,jj, lnv, rennum, dir, flag;
    int         lc,lc2, Lc, Lc2;
    ExtScanList    scanList;
    ULong       filePos;
    int         getSize, Error;
	UInt32      lv, co, wt, lv2, co2, wt2, ggnum,Lv, Co, Wt, Lv2, Co2, Wt2;
	ULong		elemAddr[50];
	int			i, numAddr;
	MSElement	ele1;
	DgnModelRefP  modelRefP;

    linePos = 0;

    lPoi = (DPoint3d*)malloc( 50000 * sizeof( DPoint3d));

    /*----- ���C���A���C���X�g�����O -----*/
    /*--- 1�ڂ̃��C�� ---*/
	modelRefP = mdlModelRef_getActive(); //MASTERFILE
    linePos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRefP);
    mdlElement_read( &oele, MASTERFILE, linePos);
    mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);
    get_layer( &Lv, &Co, &Lc, &Wt, NULL, &oele);

    rennum = 1;
    renPos[0] = linePos;


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
			if (mdlElement_read(&ele1, MASTERFILE, elemAddr[i]) != SUCCESS) continue;
			
			get_layer(&Lv2, &Co2, &Lc2, &Wt2, NULL, &ele1);
			/*--- �ݒ背�C���[�Ɉ�v������� ---*/
			if ((Lv == Lv2) && (Co == Co2) && (Lc == Lc2) && (Wt == Wt2)) {

				cnvPos[cnvnum] = elemAddr[i];
				cnvnum++;
			}
		}
    } // WHILE END

    /*--- �A�����郉�C�� ---*/
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
    } // FOR JJ END

    if ( flag > 0){
        syori( );
    }

    else{
        /*--- �N�I�_��v���Ȃ��ꍇ�͂��̂܂� ---*/
        if ( get_leng( &lPoi[0], &lPoi[lnv-1]) > 1){
            syori( );
        }
        else{
            /*--- �|���S���̌��� ---*/
            dir = get_round_dir( lnv, lPoi);
           
            mdlShape_create( &nele, &oele, lPoi, lnv, 0);
            mdlElmdscr_new( &elmDP, NULL, &nele);

            /*--- �t�����ɂ��Ĕz�u����ꍇ ---*/
            if ( glo->dir != dir){
                mdlElmdscr_reverse( &elmDP2, elmDP, MASTERFILE);
                mdlElmdscr_add( elmDP2);
                mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Normal);

                mdlElmdscr_freeAll( &elmDP);
                mdlElmdscr_freeAll( &elmDP2);
            }

            /*--- ���̂܂܂̌����Ŕz�u����ꍇ ---*/
            else{
                mdlElmdscr_add( elmDP);
                mdlElmdscr_display( elmDP, MASTERFILE, DRAW_MODE_Normal);
                mdlElmdscr_freeAll( &elmDP);
            }
           
            /*--- �폜 ---*/
            for( jj = 0; jj < rennum; jj++){
                mdlElement_read( &oele, MASTERFILE, renPos[jj]);
                mdlElement_undoableDelete( &oele, renPos[jj], DRAW_MODE_Normal);
            }
            syori( );
        }
    } // WHILE END
    free( lPoi);
}


/*-----------------------------------------------------------------------
**	�V�F�C�v�����C���X�g�����O
-----------------------------------------------------------------------*/
Private void	cnv_shape_to_ls( )
{
    Dpoint3d    *lPoi;
    ULong       tmpPos[100], renPos[100], shapePos;
    int         ii, jj, lnv, numele, word, rennum, elenum;
    int         i1, i2, lc, typ;
	UInt32      lv, co, wt, ggnum;
    DgnModelRefP  modelRefP;


    lPoi = (DPoint3d*)malloc( 50000 * sizeof( DPoint3d));

    shapePos = 0;
	modelRefP = mdlModelRef_getActive(); //MASTERFILE
	shapePos = mdlElement_getFilePos(FILEPOS_CURRENT, &modelRefP);

    mdlElement_read( &oele, MASTERFILE, shapePos);
    typ = mdlElement_getType( &oele);

    /*--- �V�F�C�v�̏ꍇ ---*/
    if ( typ == 6){
        mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);
        mdlLineString_create( &nele, &oele, lPoi, lnv);
        mdlElement_rewrite( &nele, &oele, shapePos);
        mdlElement_display( &nele, DRAW_MODE_Normal);
		free(lPoi);
    }

    /*--- �����}�`�̏ꍇ ---*/
    else if ( typ == 14){
		mdlElmdscr_read(&oelmDP, shapePos, MASTERFILE, 0, NULL);
        mdlElmdscr_stroke(&lPoi, &lnv, oelmDP, NULL);
		get_layer(&lv, &co, &lc, &wt, &ggnum, &oele);
		/*--- ���Ă���ꍇ ---*/
		if (memcmp(&lPoi[0], &lPoi[lnv - 1], sizeof(Dpoint3d)) == 0) {
			lineString_add(&lv, &co, &lc, &wt, &ggnum, lPoi, &lnv);

			/*--- �폜 ---*/
			mdlElmdscr_read(&oelmDP, shapePos, MASTERFILE, 0, NULL);
			mdlElmdscr_undoableDelete(oelmDP, shapePos, DRAW_MODE_Normal);
		}
		dlmSystem_mdlFree(lPoi);
		mdlElmdscr_freeAll(&oelmDP);

    }
	else {
		free(lPoi);
	}
    syori( );


}


/*-----------------------------------------------------------------------
**	�V�F�C�v���V�F�C�v
-----------------------------------------------------------------------*/
Private void	cnv_shape_to_shape( )
{
    Dpoint3d    *lPoi;
    ULong       tmpPos[100], renPos[100], shapePos;
    int         ii, jj, lnv, rennum, elenum, dir, flag;
    int         lc, typ;
	UInt32      lv, co, wt, ggnum;
	DgnModelRefP  modelRefP;

    shapePos = 0;

    lPoi = (DPoint3d*)malloc( 50000 * sizeof( DPoint3d));

	modelRefP = mdlModelRef_getActive(); //MASTERFILE
	shapePos = mdlElement_getFilePos(FILEPOS_CURRENT, &modelRefP);

    mdlElement_read( &oele, MASTERFILE, shapePos);
    typ = mdlElement_getType( &oele);

    /*--- �V�F�C�v�̏ꍇ ---*/
    if ( typ == 6){
        mdlElement_read( &oele, MASTERFILE, shapePos);
        mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);

        /*--- �|���S���̌��� ---*/
        dir = get_round_dir( lnv, lPoi);

        /*--- �t�����ɂ��Ĕz�u ---*/
        if ( glo->dir != dir){
            mdlElmdscr_new( &elmDP, NULL, &oele);
            mdlElmdscr_reverse( &elmDP2, elmDP, MASTERFILE);
            mdlElmdscr_rewrite( elmDP2, elmDP, shapePos);
            mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Normal);

            mdlElmdscr_freeAll( &elmDP);
            mdlElmdscr_freeAll( &elmDP2);
        }
		free(lPoi);
    }

    /*--- �����}�`�̏ꍇ ---*/
    else if ( typ == 14){
        /*--- �w�b�_��ǂ� ---*/
        mdlElmdscr_read( &oelmDP, shapePos, MASTERFILE, 0, NULL);

        /*--- �|���S���̌��� ---*/
		mdlElmdscr_stroke(&lPoi, &lnv, oelmDP, NULL);
        dir = get_round_dir( lnv, lPoi);
		dlmSystem_mdlFree(lPoi);

        /*--- �t�����ɂ��Ĕz�u ---*/
        if ( glo->dir != dir){
            mdlElmdscr_reverse( &elmDP2, oelmDP, MASTERFILE);
            mdlElmdscr_rewrite( elmDP2, oelmDP, shapePos);
            mdlElmdscr_display( elmDP2, MASTERFILE, DRAW_MODE_Normal);

            mdlElmdscr_freeAll( &elmDP);
            mdlElmdscr_freeAll( &elmDP2);
        }
        mdlElmdscr_freeAll( &oelmDP);
    }
	else {
		free(lPoi);

	}
    syori( );

}
