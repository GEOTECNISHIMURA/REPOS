/*-----------------------------------------------------------------------*/
/*-		���C���A�Z���A�e�L�X�g�̃��C���[�Ȃǂ�ς���					-*/
/*-			SINGCHG.MC													-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>

#include "../CommonCE/naka8.cpp"
#include "laychg.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE                    *fp, *fp2;
LAYSET                  *layset;
CExprResult             result;
CExprValue              value;
RscFileHandle           Rsc;
MSDialog                *db_p;

WChar                    full[80], buf[128], file[MAXFILELENGTH], filename[128], ins[256], logname[MAXDIRLENGTH];
ExtScanList             scanList;
int                     just, fileNum, Error, getSize, type, linkCount, chgnum;
UInt32					ggnum;
ULong                   filePos, filePos2, celfilePos, filendpos, cPos, complexPos;
ULong 				    cellPos;
MSElementDescr          *elmDP, *elmDP2, *NelmDP, *DelmDP;
MSElement               ele, ele2, elece, cele, elm;
DatabaseLink            *dblink;
int                     ii, jj, ce, len, lo, exitStatus, numelems;
Dpoint3d                scale, origin, center;
WChar                    string[256];
RotMatrix               rMatrix;
TextSize                tileSize;
WChar                   wcs[64];
WChar                    tp[2];
long                    quat[4];
long                    l, s, t;            /*- �e�[�u�����̂k�A�r�A�s�̐� -*/
int                     lc;     /*- �ǂݍ��񂾗v�f�̃��C���[ -*/
UInt32					lv, co, wt;
int                     clc; /*- �`�F�b�N�T�[�N���̃��C���[ -*/
UInt32					clv, cco, cwt;
int                     lcn; /*- �V�v�f�̃��C���[ -*/
UInt32					lvn, con, wtn;
WChar                   MSWcell[MAX_CELLNAME_LENGTH];
WChar                    cell[MAX_CELLNAME_BYTES];
int                     moji;
LevelId 					tmpolv, tmpnlv;
int 					status;
WChar 					bufstr[256];

/*-	�v���g�^�C�v	-*/
bool   unload(UnloadProgramReason unloadType);
void	kensaku( void);
void	kensaku_line( void);
void	kensaku_cell( void);
void	kensaku_text( void);
void	create_circle( void);
int cnv_level_name2IDb(LevelId *lvID, WChar *wlvName);
void appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt);
/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{

WChar   devP[64], dirP[256], nameP[256];
	
    /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error(L"LAY�̃��\�[�X�t�@�C�����J���܂���");
        mdlSystem_exit( exitStatus, 1);
    }

    /*--- �L��ϐ��̎擾 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&layset",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox(L"�L��ϐ����擾�ł��܂���");
    }
    layset = (LAYSET*)value.val.valPointer;

	/*--- �A�����[�h�֐��̃Z�b�g ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    lo = 0;


	/*	��w��->��wID*/

	status = cnv_level_name2IDb(&tmpolv, layset->olvstr);
	status = cnv_level_name2IDb(&tmpnlv, layset->nlvstr);
	
	layset->olv = tmpolv;
	layset->nlv = tmpnlv;
    /*--- �o�b�`�t�@�C�� ON �̏ꍇ ---*/
    if ( layset->tbat == -1){
        fp2 = _wfopen( layset->bfile, L"r");
        ii = 0;
        while( fwscanf( fp2, L"%ls\n", filename) != EOF){
            if ( memcmp( &filename[0], ";", 1) == 0){
                continue;
            }
            mdlSystem_newDesignFile( filename);

            /*--- ���O�t�@�C���� ---*/
            mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
            swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
            fp = _wfopen( logname, L"w");

            kensaku( );

            fclose( fp);
        }
        fclose( fp2);
    }
	
    /*--- �o�b�`�t�@�C�� OFF �̏ꍇ ---*/
    else{
        /*--- ���O�t�@�C���� ---*/
        mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
        swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
        fp = _wfopen( logname, L"w");
		
        kensaku( );

        fclose( fp);
    }

    mdlView_updateSingle( 0);

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
**	����
-----------------------------------------------------------------------*/
void	kensaku( )
{
	BeFileName	filePath, Cfile;
	
	WChar buf[256];
	
   /*--- EOF �� �t�@�C���|�W�V���� ---*/
    filendpos =mdlElement_getFilePos( FILEPOS_EOF, NULL);

    chgnum = 0;
    fwprintf( fp, L"%ls\n", tcb->dgnfilenm);

    /*--- �Z�����C�u�����̃A�^�b�` ---*/
	Cfile.SetName(layset->cfile);
    mdlCell_attachLibrary( filePath, &Cfile, NULL, TRUE);
	
    /*--- ���C���A�V�F�C�v�A�T�[�N���̏ꍇ ---*/
    if ( layset->type == 1){
        kensaku_line( );
    }
	mdlCell_getLibraryName(filePath);
    /*--- �Z���̏ꍇ ---*/
    if ( layset->type == 0){
		mdlCell_getLibraryName(filePath);
        if ( wcslen( filePath.GetWCharCP()) == 0){
            mdlDialog_openInfoBox( L"�Z�����C�u�����ɃA�^�b�`���ĉ�����");
            mdlSystem_exit( exitStatus, 1);
        }
        kensaku_cell( );
    }

    /*--- �����̏ꍇ ---*/
    if ( layset->type == 2 || layset->type == 3){
        kensaku_text( );
    }

    if ( chgnum == 0){
        fwprintf( fp, L"  �ύX�Ȃ�\n");
    }
}


/*-----------------------------------------------------------------------
|  ���W���[�����Fkensaku_line   �X�L�����i���C���A�V�F�C�v�A�T�[�N���j  |
-----------------------------------------------------------------------*/
void	kensaku_line( )
{
    Dpoint3d	lPoi[5000];
    int			lnv;
	int 		scanwords;
	ULong		elemAddr[1];
	int			i;
    fileNum = 0;
    numelems = 0;


/*----------------------------------------------------------------------+
| ����������ݒ� 				                        |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| �X�L�������X�g�̏������� 			                        |
+----------------------------------------------------------------------*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

/*----------------------------------------------------------------------+
| scanList�̊e�����o�[�Ɉȉ����Z�b�g����B 			        |
+----------------------------------------------------------------------*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;*/
    /*scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* �\���\�v�f�w��	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;

/*----------------------------------------------------------------------+
| �����Ώۂ̌���B		 				        |
+----------------------------------------------------------------------*/
    if ( layset->type == 1){
    	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
        scanList.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING | TMSK0_CMPLX_STRING | TMSK0_CMPLX_SHAPE
    	                     | TMSK0_SHAPE | TMSK0_ELLIPSE;
    }
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
/*----------------------------------------------------------------------+
| ����ؽĂ̓ǂݍ��݁B 				                        |
+----------------------------------------------------------------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
    }
	filePos   = 0L;
    scanwords = sizeof(elemAddr)/sizeof( short);
/*----------------------------------------------------------------------+
| ���ݎ��s�B 					                        |
+----------------------------------------------------------------------*/
	
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        /*if ( Error == 10){ break;}*/
        if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read( &ele, MASTERFILE, elemAddr[0]);

/*----------------------------------------------------------------------+
| �v�f�ɼ���ۼނ�^����B 				                |
+----------------------------------------------------------------------*/

        type = mdlElement_getType( &ele);

        /*--- �����C���̃��C���[���擾 ---*/
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);

		
    	while (TRUE){
        /*--- ���v�f�̃��C���[���ݒ�l�ƈႤ�ꍇ ---*/
	        if ( lv != layset->olv){ break;}
	        if ( co != layset->oco){ break;}
	        if ( lc != layset->olc){ break;}
	        if ( wt != layset->owt){ break;}

	    	filePos2 = elemAddr[0];
	        memcpy( &ele2, &ele, sizeof( MSElement));

	        /*--- ���C���E���C���X�g�����O�E�V�F�C�v ---*/
	        if ( type == 3 || type == 4 || type == 6 ){
	            mdlLinear_extract( lPoi, &lnv, &ele, MASTERFILE);
	            center.x = ( lPoi[0].x + lPoi[1].x)/2;
	            center.y = ( lPoi[0].y + lPoi[1].y)/2;
	            center.z = ( lPoi[0].z + lPoi[1].z)/2;
	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
	        }

	        /*--- �T�[�N�� ---*/
	        if ( type == 15){
	            if (mdlModelRef_is3D(MASTERFILE)){
	                center.x = (double)ele2.ellipse_3d.origin.x;
	                center.y = (double)ele2.ellipse_3d.origin.y;
	                center.z = (double)ele2.ellipse_3d.origin.z;
	            }
	            else{

	                center.x = (double)ele2.ellipse_2d.origin.x;
	                center.y = (double)ele2.ellipse_2d.origin.y;
	                center.z = 0;
	            }
	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
	        }

	        /*--- �V�v�f�̃��C���[ ---*/
	        lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;
			
	        if( type == 12 || type == 14){
				
	            mdlElmdscr_read( &elmDP, filePos2, MASTERFILE, FALSE, NULL);
 				mdlElmdscr_duplicate(&NelmDP, elmDP);				
 //   			memcpy(&NelmDP, &elmDP, sizeof(elmDP));
				mdlElmdscr_setProperties( &NelmDP, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	            mdlElmdscr_setSymbology ( &NelmDP, &con,  &lcn, &wtn, NULL);
	            mdlElmdscr_rewrite( NelmDP, elmDP, filePos2);
 	            mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
	            mdlElmdscr_freeAll( &NelmDP);
	            mdlElmdscr_freeAll( &elmDP);

	            mdlElement_read( &elm, MASTERFILE, filePos2);
	            mdlLinear_extract( lPoi, &lnv, &elm, MASTERFILE);
	            center.x = ( lPoi[0].x + lPoi[1].x)/2;
	            center.y = ( lPoi[0].y + lPoi[1].y)/2;
	            center.z = ( lPoi[0].z + lPoi[1].z)/2;

	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);

	        }
	        else{
	        	
	            mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	            mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	            mdlElement_rewrite( &ele2, &ele, filePos2);
	            mdlElement_display( &ele2, DRAW_MODE_Normal);
	        }
    		break;
    	}
    if ( Error == 10){ break;}
    }

}


/*-----------------------------------------------------------------------
|  ���W���[�����Fkensaku_cell  �X�L�����i�Z���j                         |
-----------------------------------------------------------------------*/
void	kensaku_cell( )
{
    MSElement           u;
	MSElementDescr      *TelmDP;
    ULong               celendPos;
    bool                solid;
    int                 length;
    UInt16              attrP[MAX_ATTRIBSIZE];
	int 				scanwords;
	ULong				elemAddr[1];
    double              transform2d[2][2];
    double              transform3d[3][3];
	int 				i;
	UInt32              cmpcnt;
	length = 0;
/*----------------------------------------------------------------------+
| ����������ݒ� 				                        |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| �X�L�������X�g�̏������� 			                        |
+----------------------------------------------------------------------*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

/*----------------------------------------------------------------------+
| scanList�̊e�����o�[�Ɉȉ����Z�b�g����B 			        |
+----------------------------------------------------------------------*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* �\���\�v�f�w��	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;
/*----------------------------------------------------------------------+
| �����Ώۂ̌���B		 				        |
+----------------------------------------------------------------------*/
    if ( layset->type == 0){
    	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
        scanList.typmask[0] = TMSK0_CELL_HEADER;
    }
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--- �X�L�������s ---*/
/*----------------------------------------------------------------------+
| ����ؽĂ̓ǂݍ��݁B 				                        |
+----------------------------------------------------------------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlDialog_openInfoBox(L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
    }
	filePos   = 0L;
    scanwords = sizeof(elemAddr)/sizeof( short);
    fileNum = 0;

/*----------------------------------------------------------------------+
| ���ݎ��s�B 					                        |
+----------------------------------------------------------------------*/
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
	
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
    	mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
		/*printf("celpos %ld err %ld filePos %ld filendpos %ld\n",elemAddr[0],Error,filePos,filendpos);*/
/*----------------------------------------------------------------------+
| �v�f�ɼ���ۼނ�^����B 				                |
+----------------------------------------------------------------------*/
        type = mdlElement_getType( &ele);

        switch( type){
	    case CELL_HEADER_ELM:
        	
                ce = 1;
        		celfilePos = elemAddr[0];
        	
                memcpy( &elece, &ele, sizeof( MSElement));

                /*--- ���Z���̊�_�ArMatrix�A�X�P�[���A�Z�������擾 ---*/
        		mdlCell_extract( &origin, NULL, &rMatrix, &scale, MSWcell, MAX_CELLNAME_LENGTH, &ele);
        	
                /*--- �����P�[�W�iUDLINK�AMSLINK�j�擾 ---*/
                mdlElement_extractAttributes( &length, attrP, &ele);
	    	    /*break;*/

                /*--- �Z���̃��C���[���擾 ---*/
				
				mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, FALSE, NULL);
				get_layer(&lv, &co, &lc, &wt, NULL, &elmDP->h.firstElem->el);
				mdlElmdscr_freeAll(&elmDP);
        		while (TRUE){
        			
	                /*--- ���Z���̃��C���[���ݒ�l�ƈ�v���Ȃ��ꍇ ---*/
        			
	                if ( lv != layset->olv){ break;}
	                if ( co != layset->oco){ break;}
	                if ( lc != layset->olc){ break;}
	                if ( wt != layset->owt){ break;}
	                if ( wcscmp( MSWcell, layset->ocell) != 0){ break;}
        			
	                /*--- �V�Z���̃Z�����C�u�������̃|�W�V�������擾 ---*/
					swprintf(MSWcell, L"%ls", layset->ncell);
	                cellPos = mdlCell_existsInLibrary( MSWcell);
	                /*--- �Z�����Z�����C�u�������ɑ��݂��Ȃ��ꍇ ---*/
	                if ( cellPos == FALSE){
	                	
	                    lo++;
	                    fwprintf( fp, L"%ls���Z�����C�u�������ɑ��݂��܂���B\n", layset->ncell);
	                    break;
	                }

	                /*--- �V�v�f�̃��C���[ ---*/
	                lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;

	                /*--- �X�P�[�� ---*/
	                if ( layset->tscale == -1){
	                    scale.x = scale.y = layset->scale;
	                    if (mdlModelRef_is3D(MASTERFILE)){ scale.z = layset->scale;}
	                    else{                       scale.z = 1;}
	                }

	                /*--- �p�x�̐ݒ� ---*/
	                if ( layset->tangle == -1){
	                    mdlRMatrix_fromAngle( &rMatrix, layset->angle * fc_pi/180);
	                }

	                /*--- �V�Z���̔z�u ---*/
					ActiveParams::GetValue( lv, ACTIVEPARAM_LEVEL);
					ActiveParams::GetValue( co, ACTIVEPARAM_COLOR);
					ActiveParams::GetValue( wt, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::GetValue( lc, ACTIVEPARAM_LINESTYLE);
					ActiveParams::SetValue( lvn, ACTIVEPARAM_LEVEL);
					ActiveParams::SetValue( con, ACTIVEPARAM_COLOR);
					ActiveParams::SetValue( wtn, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::SetValue( lcn, ACTIVEPARAM_LINESTYLE);

	                cPos = mdlCell_placeCell( &origin, &scale, FALSE, &rMatrix, NULL, ggnum, TRUE, lvn, 0, MSWcell, NULL);
					ActiveParams::SetValue( lv, ACTIVEPARAM_LEVEL);
					ActiveParams::SetValue( co, ACTIVEPARAM_COLOR);
					ActiveParams::SetValue( wt, ACTIVEPARAM_LINEWEIGHT);
					ActiveParams::SetValue( lc, ACTIVEPARAM_LINESTYLE);

	                center.x = origin.x;
	                center.y = origin.y;
	                center.z = origin.z;
	                create_circle( );
	                chgnum++;
	                fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);

	                /*--- �V�Z���w�b�_�쐬�i�}�`�Z���̏ꍇ�j ---*/
	                if ( layset->tcelltype == -1 && layset->ocelltype == 0){
	                    mdlCell_create( &u, MSWcell, &origin, FALSE);
	                }

	                /*--- �V�Z���w�b�_�쐬�i�_�Z���̏ꍇ�j ---*/
	                if ( layset->tcelltype == -1 && layset->ocelltype == 1){
	                    mdlCell_create( &u, MSWcell, &origin, TRUE);
	                }

	                /*--- �V�Z���w�b�_�쐬�i�Z���^�C�v�͌��̂܂܂̏ꍇ�j ---*/
	                if ( layset->tcelltype == 0){
	                    /*--- ���Z���w�b�_���_�Z���̏ꍇ ---*/
	                    if ( mdlCell_isPointCell( &elece) == TRUE){
	                        mdlCell_create( &u, MSWcell, &origin, TRUE);
	                    }
	                    /*--- ���Z���w�b�_���}�`�Z���̏ꍇ ---*/
	                    if ( mdlCell_isPointCell( &elece) == FALSE){
	                        mdlCell_create( &u, MSWcell, &origin, FALSE);
	                    }
	                }

	                /*--- �X�i�b�v�\�ɂ��� ---*/
	                if (mdlModelRef_is3D(MASTERFILE)){ u.cell_3d.dhdr.props.b.s = 0;}
	                else{                       u.cell_2d.dhdr.props.b.s = 0;}

	                /*--- �\���b�h�ɂ��� ---*/
	                solid = 0;
	                mdlElement_setProperties( &u, NULL, &ggnum, NULL, NULL, NULL, NULL, NULL, &solid);

	                /*--- �F�A����A�����̐ݒ� ---*/
	                mdlElement_setSymbology( &u, &layset->nco, &layset->nwt, &layset->nlc);

	                mdlElement_read( &ele, MASTERFILE, cPos);
	                if (mdlModelRef_is3D(MASTERFILE)){
	                    memcpy( u.cell_3d.transform, ele.cell_3d.transform, sizeof( transform3d));
	                }
	                else{
	                    memcpy( u.cell_2d.transform, ele.cell_2d.transform, sizeof( transform2d));
	                }
	                mdlElmdscr_new( &NelmDP, NULL, &u);

	                /*--- �ϊ��O�̃����P�[�W�iUDLINK�AMSLINK�j��^���� ---*/
	                mdlElmdscr_appendAttributes( &NelmDP, length, attrP);

					/*--- �V�Z���w�b�_�ɗv�f��ǉ� ---*/
					mdlElmdscr_read(&TelmDP, cPos, MASTERFILE, 0, NULL);
					if (mdlModelRef_is3D(MASTERFILE)) {
						cmpcnt = TelmDP->el.cell_3d.componentCount;
					}
					else {
						cmpcnt = TelmDP->el.cell_2d.componentCount;
					}
					appendElemtoDP(TelmDP, cmpcnt);

					/*--- ���̃Z���폜 ---*/
					mdlElmdscr_undoableDelete(TelmDP, cPos, TRUE);
					mdlElmdscr_freeAll(&TelmDP);
					
					mdlElmdscr_setSymbology(&NelmDP, &con, &lcn, &wtn, 0);

	                /*--- �V�Z���̔z�u ---*/
					mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, 0, NULL);
	                mdlElmdscr_validate( NelmDP, 0);
	                mdlCell_setRange( NelmDP, ACTIVEMODEL);
        			mdlLinkage_deleteStringLinkage( &NelmDP->el,1,1);
					mdlElmdscr_rewrite( NelmDP, elmDP, celfilePos);
	                mdlElmdscr_display( NelmDP, MASTERFILE, DRAW_MODE_Normal);
					mdlElmdscr_freeAll(&elmDP);
	                mdlElmdscr_freeAll( &NelmDP);

		       break;
        	}
    		break;
        }
    }
}


/*-----------------------------------------------------------------------
**	�X�L�����i�e�L�X�g�j
-----------------------------------------------------------------------*/
void	kensaku_text( )
{
TextSizeParam   txtSize;
TextParamWide   txtParamWide;
//WChar			stro[256], strn[256];
int             scanwords;
ULong           elemAddr[1];
int				i;
WString         wstr;
    fileNum = 0;
    /*--------- �X�L�������X�g�̏����� ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--------- �����͈͂̐ݒ� ---------------*/
    mdlScan_noRangeCheck( &scanList);

    /*scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* �\���\�v�f�w��	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;
	
	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
    scanList.typmask[1] = TMSK1_TEXT;
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--------- �X�L�������s ---------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
        mdlSystem_exit( exitStatus, 1);
    }
    filePos   = 0L;
    scanwords = sizeof( elemAddr)/sizeof( short);
    while( TRUE){
        Error = mdlScan_file( elemAddr, &getSize, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOF�G���[");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"�����ȗv�f");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
		mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
    	
        type = mdlElement_getType( &ele);

        /*--- ���e�L�X�g�̃��C���[���擾 ---*/
        get_layer( &lv, &co, &lc, &wt, &ggnum, &ele);

        /*--- �͈͓��̗v�f������ ---*/
        switch( type){
	    	case TEXT_ELM:
        		while( TRUE){	
	                /*--- ���v�f�̃��C���[���ݒ�l�ƈႤ�ꍇ ---*/
	                if ( lv != layset->olv){ break;}
	                if ( co != layset->oco){ break;}
	                if ( lc != layset->olc){ break;}
	                if ( wt != layset->owt){ break;}

	                /*--- ��_�A������A�p�x�A�t�H���g�A��_�̈ʒu�A�����T�C�Y���擾�A�����Ԋu���擾 ---*/
	                txtSize.mode = TXT_BY_TILE_SIZE;
	                mdlText_extract( &wstr, NULL, &origin, &rMatrix, &txtSize, &txtParamWide, NULL, &ele);
					swprintf(wcs, L"%ls", wstr.GetWCharCP());

	                /*--- ���v�f�̕����񂪐ݒ�l�ƈႤ�ꍇ ---*/
	                if ( layset->type == 3){
	                   if ( wcscmp( layset->ostr, wcs) != 0){
	                      break;
	                   }
	                }

	                /*word = mdlElement_igdsSize( &ele, 0);*/
	                /*filePos2 = filePos - 2 * (ULong)word;*/
	        		filePos2 = elemAddr[0];

	                /*--- �V�v�f�̃��C���[ ---*/
	                lvn = layset->nlv;   con = layset->nco;   lcn = layset->nlc;   wtn = layset->nwt;

	                /*--- �V�v�f�̕����� ---*/
	                if ( layset->type == 3){
	                   wcscpy( wcs, layset->nstr);
	                }

	                /*------ �����T�C�Y�̐ݒ� ------*/
	                if ( layset->tth == -1){ txtSize.size.height = layset->th * 1000;}
	                if ( layset->ttw == -1){ txtSize.size.width  = layset->tw * 1000;}

	                /*------ �p�x�̐ݒ� ------*/
	                if ( layset->tangle == -1){
	                    mdlRMatrix_fromAngle( &rMatrix, layset->angle * fc_piover180);
	                }

	                /*------ �t�H���g�̐ݒ� ------*/
	                if ( layset->tfont == -1){
	                    txtParamWide.font = layset->font;
	                }

	                mdlText_create( &ele2, &ele, wcs, &origin, &rMatrix, &txtSize, &txtParamWide, NULL,MASTERFILE);
	                mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	                mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	                mdlElement_rewrite( &ele2, &ele, filePos2);
	                center.x = origin.x;
	                center.y = origin.y;
	                center.z = origin.z;
	                create_circle( );
	                chgnum++;
	                fwprintf( fp, L"%4d %12.3lf %12.3lf %12.3lf\n", chgnum, center.x/1000, center.y/1000, center.z/1000);
		        	break;
        		}
	        	break;
        }
    }
}

    
/*-----------------------------------------------------------------------
**	�`�F�b�N�p�̃T�[�N���쐬
-----------------------------------------------------------------------*/
void	create_circle( )
{
    if ( layset->check == -1){
        clv = 63;   cco = 0;   clc = 0;   cwt = 2;
        mdlEllipse_create( &cele, NULL, &center, 2000, 2000, NULL, 0);
        element_add( &clv, &cco, &clc, &cwt, NULL, &cele);
    }
}

int cnv_level_name2IDb(LevelId *lvID, WChar *wlvName)
{

	StatusInt	status;

	/*----- �i6-3562�j -----*/
	status = mdlLevel_getIdFromName(lvID, MASTERFILE, LEVEL_NULL_ID, wlvName);

	return(status);
}
/*-----------------------------------------------------------------------
**	�v�f�L�q�q�̍\���v�f��ǉ�����
-----------------------------------------------------------------------*/
void  appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt)
{
	while (cmpcnt != 0)
	{
		if (TelmDP->h.isHeader) {
			appendElemtoDP( TelmDP->h.firstElem, cmpcnt);
			return;
		}
		else
		{
			mdlElmdscr_appendElement(NelmDP, &TelmDP->el);
			cmpcnt--;
			appendElemtoDP(TelmDP->h.next, cmpcnt);
			return;
		}
	}
	return;
}