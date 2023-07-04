#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-	        ���C��														-*/
/*-	        KISYUTEN.MC													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "kisyuten.h"

#define MAXBUF			512
#define MAX_LAYERCNT	500

FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;
KISYUTEN 		kisyuten;

MSElementDescr          *elmDP, *elmDP2, *NelmDP, *DelmDP;

typedef struct {
	UInt32  lv, co, wt;
	int		st;
} ELMSYMA;

/*-    �e�[�u���t�@�C�����   -*/
typedef struct{
	ELMSYMA		line[MAX_LAYERCNT];
} TABLEINFO;
TABLEINFO	tblI;
int		tblICnt;

#ifndef Private
#define Private
#endif

/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_cellib_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_MAIN_DIALOG, (PFDialogHook)dialog_hook     },
	{ HOOKID_TOGGLE_IDX,  (PFDialogHook)toggle_idx_hook },
	{ HOOKID_PUSH_IDX,    (PFDialogHook)push_idx_hook   },
	{ HOOKID_PUSH_TABLE,  (PFDialogHook)push_table_hook },
	{ HOOKID_PUSH_CELLIB, (PFDialogHook)push_cellib_hook},
	{ HOOKID_PUSH_OK,     (PFDialogHook)push_ok_hook    },
	{ HOOKID_PUSH_END,    (PFDialogHook)push_end_hook   },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int		read_table( WChar *datName);
Private	int		syori( void);
Private	int		scan_line( ULong *Pos);
Private void	create_cell( int nv, Dpoint3d *Poi);
Private int		elmsymAbChka( MSElement *el, ELMSYMA *tmpElm, ULong offset, MSElementDescr *eld);
Private void	delete_flag( int *del, int chknum, ULong *chkPos, int rennum, ULong *renPos);
void  elmdscrModify(MSElementDescr *TelmDP, UInt32 cmpcnt);

/*-----------------------------------------------------------------------
**	���C��
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
	BeFileName		full, cellib;
/*
#if LOCK == 1
   main_lok( );
#endif
*/
	chk_kigen( );


   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"KISYUTEN �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "KISYUTEN_INFO");
   sprintf(variableName, "kisyuten");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &kisyuten);

   /*--- �e�[�u���t�@�C���I�[�v�� -----*/
   mdlFile_find( &full, L"kisyuten.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%ld\n", &kisyuten.tog_idx);
      fwscanf( fp, L"%ls\n", kisyuten.idx);
      fwscanf( fp, L"%ls\n", kisyuten.table);
      fwscanf( fp, L"%ls\n", kisyuten.cellib);
      fwscanf( fp, L"%ls\n", kisyuten.kiten);
      fwscanf( fp, L"%ls\n", kisyuten.syuten);
      fwscanf( fp, L"%ld\n", &kisyuten.lv);
      fwscanf( fp, L"%ld\n", &kisyuten.co);
      fwscanf( fp, L"%ld\n", &kisyuten.wt);
      fwscanf( fp, L"%ld\n", &kisyuten.st);
      fwscanf( fp, L"%ld\n", &kisyuten.renzoku);
	  fclose(fp);
   }
   else{
      kisyuten.tog_idx   = 0;
	  swprintf(kisyuten.idx, L"c:\\\0");
	  swprintf(kisyuten.table, L"c:\\\0");
	  swprintf(kisyuten.cellib, L"c:\\\0");
	  swprintf(kisyuten.kiten, L"KITEN\0");
	  swprintf(kisyuten.syuten, L"SYUTEN\0");
      kisyuten.lv        = 1;
      kisyuten.co        = 0;
      kisyuten.wt        = 0;
      kisyuten.st        = 0;
      kisyuten.renzoku   = 1;
   }


   /*--- �_�C�A���O�{�b�N�X ------------*/
   db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"MAIN �_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �P�t�@�C���̏ꍇ ---*/
   if ( kisyuten.tog_idx != -1){
      mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
   }

}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
Private bool    unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
-----------------------------------------------------------------------*/
Private	void	dialog_hook( DialogMessage *dmP)
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
	     mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	�g�O���{�^���iIDX�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   toggle_idx_hook( DialogItemMessage *dimP)
{


   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
          /*--- �P�t�@�C���̏ꍇ ---*/
          if ( kisyuten.tog_idx != -1){
             mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
             mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
          }

          /*--- �o�b�`�̏ꍇ ---*/
          else{
             mdlDialog_itemSetEnabledState( db_p, 1, TRUE, FALSE);
             mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
          }
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���iIDX�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          swprintf( filter, L"*.idx");
          swprintf( title,  L"idx�t�@�C����I��");

          mdlFile_parseName( kisyuten.idx, devP, dirP, NULL, NULL);
          swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	      mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		  swprintf(kisyuten.idx, L"%ls", filePath.GetWCharCP());
          mdlDialog_itemsSynch( db_p);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�e�[�u���j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_table_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          swprintf( filter, L"*.tbl");
          swprintf( title,  L"�e�[�u���t�@�C����I��");

          mdlFile_parseName( kisyuten.table, devP, dirP, NULL, NULL);
          swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	      mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		  swprintf(kisyuten.table, L"%ls", filePath.GetWCharCP());
		  mdlDialog_itemsSynch( db_p);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�Z�����C�u�����j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_cellib_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          swprintf( filter, L"*.cel");
          swprintf( title,  L"�e�[�u���t�@�C����I��");

          mdlFile_parseName( kisyuten.cellib, devP, dirP, NULL, NULL);
          swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	      mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		  swprintf(kisyuten.cellib, L"%ls", filePath.GetWCharCP());
          mdlDialog_itemsSynch( db_p);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	ڲ԰ð��ق�ǂ�
------------------------------------------------------------------------*/
Private  int  read_table( WChar *datName)
{

WChar	s_buf[MAXBUF], info[MAXBUF];

    /*----- ð���̧�ق��J�� -----*/
	if ( NULL == ( fp = _wfopen( datName, L"rt"))){
		swprintf( info, L"ð���̧�� %ls ���ǂ߂܂���", datName);
        mdlDialog_openInfoBox( info);
		return( ERROR);
	}

    tblICnt = 0;
	while( 1){
		fgetws( s_buf, 512, fp);
		if ( feof( fp) || ferror( fp)){ break;}

        if ( tblICnt >= MAX_LAYERCNT){
            mdlDialog_openInfoBox( L"���C���[ ��ނ��ő�l�𒴂��܂���\n");
            fclose( fp);
            return( ERROR);
        }

		if ( 0 == wcsncmp( s_buf, L";", 1)){	 continue;}
		if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

	    swscanf( s_buf, L"%d %d %d %d"
                     , &tblI.line[tblICnt].lv, &tblI.line[tblICnt].co, &tblI.line[tblICnt].wt, &tblI.line[tblICnt].st);
        tblICnt++;
	}
	fclose( fp);

    return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_ok_hook( DialogItemMessage *dimP)
{
WChar	info[256], s_buf[256], dgnName[256];
BeFileName		full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �ݒ�ۑ� ---*/
		 mdlFile_find(&full, L"kisyuten.tbl", L"MS_DEF", NULL);
		 fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp, L"%d\n", kisyuten.tog_idx);
            fwprintf( fp, L"%ls\n", kisyuten.idx);
            fwprintf( fp, L"%ls\n", kisyuten.table);
            fwprintf( fp, L"%ls\n", kisyuten.cellib);
            fwprintf( fp, L"%ls\n", kisyuten.kiten);
            fwprintf( fp, L"%ls\n", kisyuten.syuten);
            fwprintf( fp, L"%d\n", kisyuten.lv);
            fwprintf( fp, L"%d\n", kisyuten.co);
            fwprintf( fp, L"%d\n", kisyuten.wt);
            fwprintf( fp, L"%d\n", kisyuten.st);
            fwprintf( fp, L"%d\n", kisyuten.renzoku);
         fclose( fp);

         /*--- �e�[�u���t�@�C����ǂ� ---*/
         if ( read_table( kisyuten.table) == ERROR){
            break;
         }

         /*--- �P�t�@�C���̏ꍇ ---*/
         if ( kisyuten.tog_idx != -1){
            syori( );
         }

         /*--- �o�b�`�̏ꍇ ---*/
         else{
            /*----- ���̓t�@�C�����X�g���J�� -----*/
	        if ( NULL == ( fp = _wfopen( kisyuten.idx, L"r"))){
		       swprintf( info, L"idx�t�@�C�� %ls ���J���܂���", kisyuten.idx);
               mdlDialog_openInfoBox( info);
		       break;
            }

            /*----- ���̓t�@�C�����X�g��ǂ� -----*/
            while( 1){
		       fgetws( s_buf, 512, fp);
               if ( feof( fp) || ferror( fp)){ break;}

               if ( 0 == wcsncmp( s_buf, L";", 1)){ continue;}

               swscanf( s_buf, L"%ls", dgnName);

               /*----- dgn���J�� -----*/
               if ( mdlSystem_newDesignFile( dgnName) == SUCCESS){
                  if ( syori( ) == ERROR){
                     break;
                  }
               }
            }
            fclose( fp);
         }
         break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  int   syori( )
{
MSElement	ele;
int			i, num, nv, *del, renNum;
ULong		*Pos, renPos[1000];
Dpoint3d	*Poi;
WChar		info[256];
BeFileName		cellib, kisyutencellib;
   /*--- �Z�����C�u�����A�^�b�` ---*/

   kisyutencellib.SetName(kisyuten.cellib);
   if ( mdlCell_attachLibrary( cellib, &kisyutencellib, NULL, NULL) != SUCCESS){
      swprintf( info, L"��ײ���� %ls ���A�^�b�`�ł��܂���", cellib.GetWCharCP());
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   /*--- �N�_�Z�����Z�����C�u�����ɑ��݂��邩���� ---*/
   if ( mdlCell_existsInLibrary( kisyuten.kiten) == FALSE){
      swprintf( info, L" %ls ����ײ���؂ɑ��݂��܂���", kisyuten.kiten);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   /*--- �I�_�Z�����Z�����C�u�����ɑ��݂��邩���� ---*/
   if ( mdlCell_existsInLibrary( kisyuten.syuten) == FALSE){
      swprintf( info, L" %ls ����ײ���؂ɑ��݂��܂���", kisyuten.syuten);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }


   ActiveParams::SetValue(kisyuten.lv, ACTIVEPARAM_LEVEL);
   ActiveParams::SetValue(kisyuten.co, ACTIVEPARAM_COLOR);
   ActiveParams::SetValue(kisyuten.wt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::SetValue(kisyuten.st, ACTIVEPARAM_LINESTYLE);
   Pos = (ULong*)malloc( 50000 * sizeof( ULong));
   Poi = (Dpoint3d*)malloc( 10000 * sizeof( Dpoint3d));

   /*--- ���C������ ---*/
   num = scan_line( Pos);

   /*--- �P�ƃ��C���̗��[�ɔ�������ꍇ ---*/
   if ( kisyuten.renzoku == 1){
      /*--- ���C�� ---*/
      for( i=0; i<num; i++){
         mdlElement_read( &ele, MASTERFILE, Pos[i]);
         mdlLinear_extract( Poi, &nv, &ele, MASTERFILE);
         del_doubled3( &nv, Poi, 1);

         /*--- �N�I�_�L������ ---*/
         create_cell( nv, Poi);
      }
   }

   /*--- �P�Ȃ���̗��[�ɔ�������ꍇ ---*/
   else{
      del = (int *)calloc( num, sizeof( int));

      /*--- ���C�� ---*/
      for( i=0; i<num; i++){
        if ( del[i] != 0){ continue;}

         mdlElement_read( &ele, MASTERFILE, Pos[i]);
         mdlLinear_extract( Poi, &nv, &ele, MASTERFILE);

         renNum = 1;   renPos[0] = Pos[i];
         get_renzoku( num, Pos, &nv, Poi, &renNum, renPos);
         del_doubled3( &nv, Poi, 1);

         delete_flag( del, num, Pos, renNum, renPos);

         /*--- �N�I�_�L������ ---*/
         create_cell( nv, Poi);
      }
      free( del);
   }

   free( Poi);
   free( Pos);

   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	���C������
------------------------------------------------------------------------*/
Private  int   scan_line( ULong *Pos)
{
MSElementDescr	*dscr;
ExtScanList		scanlist;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				i, j, sw2, num=0;
ELMSYMA			tmpElm;

    mdlScan_initScanlist( &scanlist);
    mdlScan_noRangeCheck( &scanlist);

    eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
    mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
    scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
    scanlist.extendedType = FILEPOS;
    scanlist.clasmsk      = 0xFFF1;

    for( i=0; i<8; i++)	scanlist.typmask[i] = 0x0000;
    scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING | TMSK0_CMPLX_STRING;

    for( i=0; i<8; i++){
        scanlist.levmask[i] = 0xFFFF;
    }

    /*----- �X�L�����J�n -----*/
    sw2 = 0;
    mdlScan_initialize( MASTERFILE, &scanlist);
    filepos   = 0L;
    scanwords = sizeof( elemAddr)/sizeof( short);
    while( 1){
        status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
        numAddr = scanwords/sizeof( short);
        for( i=0; i<numAddr; i++){
            if ( elemAddr[i] >= eofpos){ sw2++;   break;}

            mdlElmdscr_read( &dscr, elemAddr[i], MASTERFILE, FALSE, NULL);
            mdlElmdscr_operation( dscr, (PFElemOperation)elmsymAbChka, &tmpElm, ELMD_ELEMENT);

            /*----- ���C���[�`�F�b�N -----*/
            for( j=0; j<tblICnt; j++){
               if ( ( tmpElm.lv == tblI.line[j].lv || tblI.line[j].lv == -1)
                 && ( tmpElm.co == tblI.line[j].co || tblI.line[j].co == -1)
                 && ( tmpElm.wt == tblI.line[j].wt || tblI.line[j].wt == -1)
                 && ( tmpElm.st == tblI.line[j].st || tblI.line[j].st == -1)){
                  Pos[num] = elemAddr[i];
                  num++;
               }
            }
            mdlElmdscr_freeAll( &dscr);
        }

        if ( sw2 > 0){ break;}
        if ( status != BUFF_FULL){ break;}
    }

    return( num);
}


/*-----------------------------------------------------------------------
**	�N�I�_�L������
-----------------------------------------------------------------------*/
Private  void	create_cell( int nv, Dpoint3d *Poi)
{
RotMatrix	 rMatrix;
double		 angle;
UInt32       ret;
UInt32       cmpcnt;
Dpoint3d     origin;
WChar        tenmei[256];
int i;
   /*----- �N�I�_�L������ -----*/

   for (i = 0; i < 2; i++) {
	   if (i == 0) {
		   angle = atan2(Poi[1].y - Poi[0].y, Poi[1].x - Poi[0].x);
		   memcpy(&origin, &Poi[0], sizeof(Dpoint3d));
		   swprintf(tenmei, L"%ls", kisyuten.kiten);
	   }
	   else {
		   angle = atan2(Poi[nv - 1].y - Poi[nv - 2].y, Poi[nv - 1].x - Poi[nv - 2].x);
		   memcpy(&origin, &Poi[nv - 1], sizeof(Dpoint3d));
		   swprintf(tenmei, L"%ls", kisyuten.syuten);
	   }
	   mdlRMatrix_fromAngle(&rMatrix, angle);
	   ret = mdlCell_placeCell(&origin, NULL, TRUE, &rMatrix, NULL, 0, false, 0, 0, tenmei, NULL);
	   
	   mdlElmdscr_read(&elmDP, ret, MASTERFILE, 0, NULL);
	   mdlElmdscr_duplicate(&NelmDP, elmDP);
	   mdlElmdscr_setSymbology(&NelmDP, &kisyuten.co, &kisyuten.st, &kisyuten.wt, NULL);
	   if (mdlModelRef_is3D(MASTERFILE)) {
		   cmpcnt = elmDP->el.cell_3d.componentCount;
	   }
	   else {
		   cmpcnt = elmDP->el.cell_2d.componentCount;
	   }
	   elmdscrModify(NelmDP, cmpcnt);
	   mdlCell_setRange(NelmDP, ACTIVEMODEL);
	   mdlElmdscr_rewrite(NelmDP, elmDP, ret);
	   mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
	   mdlElmdscr_freeAll(&elmDP);
	   mdlElmdscr_freeAll(&NelmDP);
	   
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^�� �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_end_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          mdlSystem_exit( exitStatus, 1);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*-----------------------------------------------------------------------
**	���C���[�擾
-----------------------------------------------------------------------*/
Private int elmsymAbChka
(
MSElement		*el,
ELMSYMA      	*tmpElm,
ULong			offset,
MSElementDescr	*eld
)
{
	mdlElement_getProperties( &tmpElm->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, el);
    mdlElement_getSymbology( &tmpElm->co, &tmpElm->wt, &tmpElm->st, el);

	return( 1);
}
/*-----------------------------------------------------------------------
**	�v�f�L�q�q�̍\���v�f�̓��e��ύX����
-----------------------------------------------------------------------*/
void  elmdscrModify(MSElementDescr *TelmDP, UInt32 cmpcnt)
{
	while (cmpcnt != 0)
	{
		if (TelmDP->h.isHeader) {
			elmdscrModify(TelmDP->h.firstElem, cmpcnt);
			return;

		}
		else
		{
			TelmDP->el.ehdr.level = (LevelId)kisyuten.lv;
				cmpcnt--;
			elmdscrModify(TelmDP->h.next, cmpcnt);
			return;
		}
	}
	return;
}

/*-----------------------------------------------------------------------
**	�f���[�g�t���O
-----------------------------------------------------------------------*/
Private void	delete_flag( int *del, int chknum, ULong *chkPos, int rennum, ULong *renPos)
{
int		ii, jj;

   for( ii=0; ii<rennum; ii++){
      for( jj=0; jj<chknum; jj++){
         if ( renPos[ii] == chkPos[jj]){
            del[jj]++;
            break;
         }
      }
   }
}
