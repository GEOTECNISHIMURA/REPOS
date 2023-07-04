#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-	�|���S�����̃e�L�X�g��MSLINK�Ƃ��ė^����	-*/
/*-											    -*/
/*-	POLLINK.MC								    -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "pollink.h"

#define     POLYGON     1
#define     TEXT_A        2
#define     LINE        3

FILE			*fp;
RscFileHandle   Rsc;
MSDialog       *db_p;
int				exitStatus;
POLLINK			pollink;

int             layNum;
int             pol_lv[100],    pol_co[100],       pol_st[100],    pol_wt[100];
int             txt_lv[100][2], txt_co[100][2],    txt_st[100][2], txt_wt[100][2];
int             lin_lv[100],    lin_co[100],       lin_st[100],    lin_wt[100];
WChar            table[100][50], field[100][2][50];
int             check;
int             polID;
ULong           polPos, txtPos;
typedef struct {
	UInt32	lv, co, wt;
	Int32	st;
} ELMSYMA;


#ifndef Private
#define Private
#endif

/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_tbl_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_select_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_POLLINK_DIALOG, 	 (PFDialogHook)dialog_hook     },
   { HOOKID_TOGGLE_POLLINK_IDX,	 (PFDialogHook)toggle_idx_hook },
   { HOOKID_PUSH_POLLINK_IDX,	 (PFDialogHook)push_idx_hook   },
   { HOOKID_PUSH_POLLINK_TBL,	 (PFDialogHook)push_tbl_hook   },
   { HOOKID_PUSH_POLLINK_OK,	 (PFDialogHook)push_ok_hook    },
   { HOOKID_PUSH_POLLINK_SELECT, (PFDialogHook)push_select_hook},
   { HOOKID_PUSH_POLLINK_END,	 (PFDialogHook)push_end_hook   },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int 	new_design_file( void);
Private int 	read_table( void);
Private void 	startModify_polygon( void);
Private void 	startModify_text( void);
Private void    syori_single( Dpoint3d *Point, int view);
Private void	syori( void);
Private void	syori_link( int txtNum, ULong *txtPos, int polNv, Dpoint3d *polPoi, ULong *polPos, WChar *table, WChar *field1, WChar *field2, int linkCount, int *del, int layID);
Private void	scan_ele( int *num, ULong *Pos, int lv, int co, int st, int wt, int type);
Private int     elmSymChk( MSElement *el, ELMSYMA *tmpElm, ULong offset, MSElementDescr *eld);
Private int     lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber);
Private int 	scan_nearPoint( WChar *str, Dpoint3d *point, int lv, int co, int st, int wt, int type);
Private void	get_scan_rangea( ExtScanList *scanlist, Dpoint3d *min, Dpoint3d *max);
int _PointInside(Dpoint3d *ply, int pntNum, Dpoint3d *pnt);
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

    chk_kigen( );


   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlOutput_error( L"POLLINK �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
	  return;
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "POLLINK_INFO");
   sprintf(variableName, "pollink");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &pollink);

   /*--- �t�F���X������ɂ��� -----*/
   mdlInput_sendKeyin( L"lock fence inside", 1, INPUTQ_EOQ, NULL);

   /*--- �e�[�u���t�@�C����ǂݍ��� -------------*/
   mdlFile_find( &full, L"pollink.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%ls\n",  pollink.dbsource);
      fwscanf( fp, L"%d\n",  &pollink.tog_all);
      fwscanf( fp, L"%d\n",  &pollink.tog_idx);
      fwscanf( fp, L"%ls\n",  pollink.idxName);
      fwscanf( fp, L"%ls\n",  pollink.tblName);
      fwscanf( fp, L"%d\n",  &pollink.tolerance);
      fwscanf( fp, L"%d\n",  &pollink.err_lv);
      fwscanf( fp, L"%d\n",  &pollink.err_co);
      fwscanf( fp, L"%d\n",  &pollink.err_st);
      fwscanf( fp, L"%d\n",  &pollink.err_wt);
	  fclose(fp);
   }
   else{
	  swprintf(pollink.dbsource, L"dbsource\0");
      pollink.tog_all     = 0;
      pollink.tog_idx     = 0;
	  swprintf(pollink.idxName, L"c:\\\0");
	  swprintf(pollink.tblName, L"c:\\\0");
      pollink.tolerance   = 1;
      pollink.err_lv      = 63;
      pollink.err_co      =  0;
      pollink.err_st      =  0;
      pollink.err_wt      =  1;
   }


   /*--- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_POLLINK);
   if ( db_p == NULL){
      mdlOutput_error( L"POLLINK�_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �o�b�`�łȂ��ꍇ ---*/
   if ( pollink.tog_idx != -1){
      mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
      mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
   }

   /*--- �V�����f�U�C���t�@�C�����J������ -----*/
   SystemCallback::SetNewDesignFileFunction((SystemFunc_NewDesignFile)new_design_file);
}


/*-----------------------------------------------------------------------
**	�V�����f�U�C���t�@�C�����J������
-----------------------------------------------------------------------*/
Private int     new_design_file( void)
{

   /*--- �f�[�^�x�[�X�ɐڑ� -----*/
   mdlDB_activeDeleteMode( FALSE);
   mdlDB_activeLinkageMode( L"duplicate");

   if ( wcscmp( pollink.dbsource, L"") == 0){
      mdlDialog_openInfoBox( L"�f�[�^�x�[�X�ɐڑ��ł��܂���");
      return( ERROR);
   }
   if (mdlDB_changeDatabase(DATABASESERVERID_ODBC, pollink.dbsource) != SUCCESS){
      mdlDialog_openInfoBox( L"�f�[�^�x�[�X�ɐڑ��ł��܂���");
      return( ERROR);
   }

   return( SUCCESS);
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
		 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^���i�o�b�`�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_idx_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- �o�b�`�̏ꍇ ---*/
         if ( pollink.tog_idx == -1){
            mdlDialog_itemSetEnabledState( db_p, 3, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, TRUE, FALSE);
         }
         else{
            mdlDialog_itemSetEnabledState( db_p, 3, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 4, FALSE, FALSE);
         }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title,  L"idx�t�@�C����I��");

         mdlFile_parseName( pollink.idxName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 if (wcslen(filePath.GetWCharCP()) != 0) {
			 swprintf(pollink.idxName, L"%ls", filePath.GetWCharCP());
			 mdlDialog_itemsSynch(db_p);
		 }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���iTBL�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_tbl_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.tbl");
         swprintf( title,  L"tbl�t�@�C����I��");

         mdlFile_parseName( pollink.tblName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 if (wcslen(filePath.GetWCharCP()) != 0){
			 swprintf(pollink.tblName, L"%ls", filePath.GetWCharCP());
			 mdlDialog_itemsSynch(db_p);
		 }
         break;

      default:
		 dimP->msgUnderstood = FALSE;
		 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_select_hook( DialogItemMessage *dimP)
{

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �f�[�^�x�[�X�Ƃ̐ڑ� -------------*/
         if ( new_design_file( ) == ERROR){ break;}

         /*--- �e�[�u���t�@�C����ǂ� -------------*/
         if ( read_table( ) == ERROR){ break;}

         startModify_polygon( );
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	startModify�i�|���S���I���j
-----------------------------------------------------------------------*/
Private void	startModify_polygon( )
{
static int searchType[] = { SHAPE_ELM, CMPLX_SHAPE_ELM};

   check = POLYGON;
   mdlLocate_noElemNoLocked( );
   mdlLocate_setElemSearchMask( sizeof( searchType) / sizeof( int), searchType);

   mdlState_startModifyCommand( (StateFunc_Reset)startModify_polygon, (MdlFunc_Accept)startModify_text, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);

   /*----- �I��v�f�����F���邩���ۂ��邩�𔻒肷�� ------*/
   //mdlLocate_setFunction( LOCATE_POSTLOCATE, lineCheck);
   LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)lineCheck);

   /*----- ̧�ق̐擪���猟�� -----*/
   mdlLocate_init( );
   mdlOutput_error( L"�|���S����I�����ĉ������B");
}


/*-----------------------------------------------------------------------
**	startModify�i�e�L�X�g�I���j
-----------------------------------------------------------------------*/
Private void	startModify_text( )
{
static int searchType[] = { TEXT_ELM};

   check = TEXT_A;
   mdlLocate_noElemNoLocked( );
   mdlLocate_setElemSearchMask( sizeof( searchType) / sizeof( int), searchType);

   mdlState_startModifyCommand((StateFunc_Reset)startModify_text, (MdlFunc_Accept)syori_single, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);

   /*----- �I��v�f�����F���邩���ۂ��邩�𔻒肷�� ------*/
   LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)lineCheck);

   /*----- ̧�ق̐擪���猟�� -----*/
   mdlLocate_init( );
   mdlOutput_error( L"�e�L�X�g��I�����ĉ������B");
}


/*-----------------------------------------------------------------------
**	�P�Ə���
-----------------------------------------------------------------------*/
Private void    syori_single( Dpoint3d *Point, int view)
{
MSElementDescr  *dscr;
MSElement       ele;
int             linkCount;
WChar            str[100], str2[100], query[512];
Dpoint3d        txtPoi;
UInt32           mslink;
DatabaseLink    *link;
WString         wstr;

   /*--- �|���S�� -------------*/
   mdlElmdscr_read( &dscr, polPos, MASTERFILE, FALSE, NULL);
   mdlDB_extractLinkages( &link, &linkCount, &dscr->el);

   /*--- MSLINK�t�����ďo�͂�OFF�ŁA����MSLINK���t���v�f��I�񂾏ꍇ�iMSLINK�͊����̂܂܁j -------------*/
   if ( pollink.tog_all != -1 && linkCount > 0){
      mdlDialog_openInfoBox( L"�|���S���Ɋ���MSLINK�����Ă��܂�");
   }
   /*--- ����ȊO�̏ꍇ�iMSLINK�́AMAX�{1�j -------------*/
   else{
      mdlDB_largestMslink( &mslink, table[polID]);
      mslink++;

      /*--- MSLINK��DB�ɏo�� ---*/
      swprintf( query, L"insert into %ls (MSLINK) values (%ld)"
                    , table[polID], mslink);
      process_SQL( query);

      /*--- �|���S����MSLINK�t�� ---*/
      attach_mslink_filePos( table[polID], mslink, &polPos);

      /*--- �e�L�X�g -------------*/
      mdlElement_read( &ele, MASTERFILE, txtPos);
      mdlText_extract( &wstr, NULL, &txtPoi, NULL, NULL, NULL, NULL, &ele);
	  swprintf( str, L"%ls", wstr.GetWCharCP());

      /*--- �e�L�X�g��DB�ɏo�� ---*/
      mdlDB_writeColumn( table[polID], mslink, field[polID][0], str);

      /*--- �e�L�X�g�t�߂̃��C���̌��� ---*/
      scan_nearPoint( str2, &txtPoi, lin_lv[polID], lin_co[polID], lin_st[polID], lin_wt[polID], LINE);

      /*--- ���C���łȂ��������Α��̃e�L�X�g���� ---*/
      scan_nearPoint( str2, &txtPoi, txt_lv[polID][1], txt_co[polID][1], txt_st[polID][1], txt_wt[polID][1], TEXT_A);

      /*--- ���C���łȂ��������Α��̃e�L�X�g��DB�ɏo�� ---*/
      mdlDB_writeColumn( table[polID], mslink, field[polID][1], str2);
      mdlElmdscr_freeAll( &dscr);
   }

   mdlState_clear( );
   startModify_polygon( );
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_ok_hook( DialogItemMessage *dimP)
{

WChar    *tok, s_buf[512], info[512];
BeFileName		full;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �e�[�u���t�@�C���ɏ������� -------------*/
		 mdlFile_find(&full, L"pollink.tbl", L"MS_DEF", NULL);
         fp = _wfopen(full.GetWCharCP(), L"w");
            fwprintf( fp, L"%ls\n", pollink.dbsource);
            fwprintf( fp, L"%d\n", pollink.tog_all);
            fwprintf( fp, L"%d\n", pollink.tog_idx);
            fwprintf( fp, L"%ls\n", pollink.idxName);
            fwprintf( fp, L"%ls\n", pollink.tblName);
            fwprintf( fp, L"%d\n", pollink.tolerance);
            fwprintf( fp, L"%d\n", pollink.err_lv);
            fwprintf( fp, L"%d\n", pollink.err_co);
            fwprintf( fp, L"%d\n", pollink.err_st);
            fwprintf( fp, L"%d\n", pollink.err_wt);
         fclose( fp);

         /*--- �f�[�^�x�[�X�Ƃ̐ڑ� -------------*/
         //if ( new_design_file( ) == ERROR){ break;}

         /*--- �e�[�u���t�@�C����ǂ� -------------*/
         if ( read_table( ) == ERROR){ break;}



if ( mdlDialog_openMessageBox( DIALOGID_MsgBoxOKCancel, L"�{���ɑS���������܂����H", MessageBoxIconType::Question) == ACTIONBUTTON_OK){
         /*--- 1�t�@�C�� ---*/
         if ( pollink.tog_idx != -1){
            syori( );
         }
         /*--- �o�b�` ---*/
         else{
            /*--- ���̓t�@�C�����X�g���J���Ȃ��ꍇ -----*/
            if ( ( fp = _wfopen( pollink.idxName, L"r")) == NULL){
               swprintf( info, L"idx�t�@�C�� %ls ���J���܂���", pollink.idxName);
               mdlDialog_openInfoBox( info);
               break;
            }

            /*----- ���̓t�@�C�����X�g���J�� -----*/
            else{
               while( 1){
                  fgetws( s_buf, 512, fp);
                  if ( feof( fp) || ferror( fp)){ break;}

                  if ( 0 == wcsncmp( s_buf, L";", 1)){ continue;}
                  tok = wcstok( s_buf, L"\n");

                  /*----- dgn���J�� -----*/
                  if ( mdlSystem_newDesignFile( s_buf) == SUCCESS){
                     syori( );
                  }
               }
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
**	�e�[�u���t�@�C����ǂ�
-----------------------------------------------------------------------*/
Private int     read_table( )
{
WChar		buf[256], info[512];

   /*--- ���C���[�e�[�u�� ---*/
   layNum = 0;
   if ( ( fp = _wfopen( pollink.tblName, L"r")) != NULL){
      while( fgetws( buf, 256, fp) != NULL){
         /*--- 1�����ڂ�;�ȊO�̏ꍇ ---*/
         if ( wmemcmp( &buf[0] , L";", 1) != 0){
            swscanf( buf, L"%d %d %d %d %ls %d %d %d %d %ls %d %d %d %d %ls %d %d %d %d"
                       , &pol_lv[layNum], &pol_co[layNum], &pol_st[layNum], &pol_wt[layNum]
                       , table[layNum]
                       , &txt_lv[layNum][0], &txt_co[layNum][0], &txt_st[layNum][0], &txt_wt[layNum][0]
                       , field[layNum][0]
                       , &txt_lv[layNum][1], &txt_co[layNum][1], &txt_st[layNum][1], &txt_wt[layNum][1]
                       , field[layNum][1]
                       , &lin_lv[layNum], &lin_co[layNum], &lin_st[layNum], &lin_wt[layNum]);
            layNum++;
         }
      }
   }

   else{
      swprintf( info, L"%ls ��������܂���", pollink.tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }
   fclose( fp);

   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	syori
-----------------------------------------------------------------------*/
Private void	syori( )
{
MSElementDescr  *dscr, *nDscr;
MSElement       ele;
Dpoint3d        *polPoi, orig;
int             ii, jj, linkCount;
int             polNum, txtNum1, polNv, *del1;
ULong           *polaPos, *txtPos1, *linPos;
DatabaseLink    *link;

   polaPos  = (ULong*)malloc( 100000 * sizeof( ULong));
   txtPos1 = (ULong*)malloc( 100000 * sizeof( ULong));
   linPos  = (ULong*)malloc( 100000 * sizeof( ULong));
   del1    = (int*)calloc( 100000, sizeof( int));

   /*--- ���C�� -------------*/
   for( ii=0; ii<layNum; ii++){
      /*--- �|���S������ -------------*/
      polNum  = 0;
      scan_ele( &polNum, polaPos, pol_lv[ii], pol_co[ii], pol_st[ii], pol_wt[ii], POLYGON);

      /*--- �e�L�X�g���� -------------*/
      txtNum1 = 0;
      scan_ele( &txtNum1, txtPos1, txt_lv[ii][0], txt_co[ii][0], txt_st[ii][0], txt_wt[ii][0], TEXT_A);

      /*--- �|���S�� -------------*/
      for( jj=0; jj<polNum; jj++){
         mdlElmdscr_read( &dscr, polaPos[jj], MASTERFILE, FALSE, NULL);
         mdlElmdscr_stroke( &polPoi, &polNv, dscr, 0.001);
		 printf("%d\n", polaPos[jj]);
         /*--- MSLINK�t�����ďo�͂̏ꍇ -------------*/
         if ( pollink.tog_all == -1){
            /*--- ��UMSLINK�͂��� -------------*/
            mdlElmdscr_duplicate( &nDscr, dscr);
            mdlDB_detachAttributesDscr( &nDscr);
            polaPos[jj] = mdlElmdscr_rewrite( nDscr, dscr, polaPos[jj]);
         }
         mdlElement_read( &ele, MASTERFILE, polaPos[jj]);
         mdlDB_extractLinkages( &link, &linkCount, &ele);

         /*--- �����N���� -------------*/
		 syori_link(txtNum1, txtPos1, polNv, polPoi, &polaPos[jj], table[ii], field[ii][0], field[ii][1], linkCount, del1, ii);
		 dlmSystem_mdlFree( polPoi);
         mdlElmdscr_freeAll( &dscr);
      }
   }

   /*--- �e�L�X�g -------------*/
   for( ii=0; ii<txtNum1; ii++){
      /*--- �ǂ̃|���S���ɂ������Ă��Ȃ��e�L�X�g -------------*/
      if ( del1[ii] == 0){
         mdlElement_read( &ele, MASTERFILE, txtPos1[ii]);
         mdlText_extract( NULL, &orig, NULL, NULL, NULL, NULL, NULL, &ele);
         create_errCircle( pollink.err_lv, pollink.err_co, pollink.err_st, pollink.err_wt, 1000, &orig);
      }
   }
   free( del1);
   free( linPos);
   free( txtPos1);
   free( polaPos);
}


/*-----------------------------------------------------------------------
**	�����N�t������
-----------------------------------------------------------------------*/
Private void	syori_link( int txtNum, ULong *txtaPos, int polNv, Dpoint3d *polPoi, ULong *polaPos, WChar *tablea, WChar *field1, WChar *field2, int linkCount, int *del, int layID)
{
MSElementDescr  *dscr, *nDscr;
MSElement       ele;
int             flag, ii, jj;
WChar           str[100],  column[10][256], column2[10][256], query[512];
UInt32          mslink=0;
Dpoint3d        txtPoi;
WString         wstr;

   /*--- �e�L�X�g ---*/
   flag = 0;
   for( ii=0; ii<txtNum; ii++){
      mdlElement_read( &ele, MASTERFILE, txtaPos[ii]);
      mdlText_extract( &wstr, &txtPoi,NULL, NULL, NULL, NULL, NULL, &ele);
	  swprintf(str, L"%ls", wstr.GetWCharCP());
      /*--- �e�L�X�g���|���S�������ǂ����̔��� ---*/ 
	  if ( judge_inout( &polNv, polPoi, &txtPoi) == 1){
	  //if (_PointInside( polPoi, polNv, &txtPoi) == 2){
		 printf("txtpos%d\n", txtaPos[ii]);
         wcscpy( column[flag], str);

         /*--- �e�L�X�g2���s�v�ȏꍇ ---*/
         if ( txt_lv[layID][1] == 0){
            flag++;
         }
         /*--- �e�L�X�g2���K�v�ȏꍇ ---*/
         else{
            /*--- �e�L�X�g�t�߂̃��C���̌��� ---*/
            if ( scan_nearPoint( column2[flag], &txtPoi, lin_lv[layID],    lin_co[layID],    lin_st[layID],    lin_wt[layID],    LINE) != SUCCESS){
               create_errCircle( pollink.err_lv, pollink.err_co, pollink.err_st, pollink.err_wt, 1000, &txtPoi);
            }
            else{
               /*--- ���C���łȂ��������Α��̃e�L�X�g���� ---*/
               if ( scan_nearPoint( column2[flag], &txtPoi, txt_lv[layID][1], txt_co[layID][1], txt_st[layID][1], txt_wt[layID][1], TEXT_A) != SUCCESS){
                  create_errCircle( pollink.err_lv, pollink.err_co, pollink.err_st, pollink.err_wt, 1000, &txtPoi);
               }
               else{
                  flag++;
               }
            }
         }
         del[ii]++;
      }
   }

   /*--- MSLINK�t���ĂȂ����̂̂ݏ��� -------------*/
   if ( pollink.tog_all == -1 || linkCount == 0){
      //mdlDB_largestMslink( &mslink, tablea);
      mslink++;

      /*--- �e�L�X�g���P�ȏ㑶�݂���ꍇ ---*/
      if ( flag >= 1){
         /*--- DB�o�� ---*/
         swprintf( query, L"insert into %ls (MSLINK) values (%ld)"
                       , tablea, mslink);
         //process_SQL( query);
         //mdlDB_writeColumn( tablea, mslink, field1, column[0]);
         //mdlDB_writeColumn( tablea, mslink, field2, column2[0]);

         /*--- MSLINK�t�� ---*/
         //*polaPos = attach_mslink_filePos( tablea, mslink, polaPos);
         mslink++;

         for( jj=1; jj<flag; jj++){
            /*--- DB�o�� ---*/
            swprintf( query, L"insert into %ls (MSLINK) values (%ld)"
                          , tablea, mslink);
            //process_SQL( query);
            //mdlDB_writeColumn( tablea, mslink, field1, column[jj]);
            //mdlDB_writeColumn( tablea, mslink, field2, column2[jj]);

            mdlElmdscr_read( &dscr, *polaPos, MASTERFILE, FALSE, NULL);
            mdlElmdscr_duplicate( &nDscr, dscr);

            /*--- MSLINK�t�� ---*/
            //attach_mslink_dscr( tablea, mslink, nDscr, 0);

            mdlElmdscr_freeAll( &dscr);
            mdlElmdscr_freeAll( &nDscr);
            mslink++;
         }
      }

      /*--- �e�L�X�g���Ȃ��ꍇ ---*/
      else{
         /*--- �|���S�����Ȃ��� ---*/
         lineString_add( &pollink.err_lv, &pollink.err_co, &pollink.err_st, &pollink.err_wt, NULL, polPoi, &polNv);
      }
   }
}


/*-----------------------------------------------------------------------
**	�v�f�̃X�L����
-----------------------------------------------------------------------*/
Private void	scan_ele( int *num, ULong *Pos, int lv, int co, int st, int wt, int type)
{
ExtScanList		scanlist;
MSElementDescr	*dscr;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, nv, typ, sw2;
Dpoint3d        *poi;
ELMSYMA			tmpElm;


   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   if ( type == POLYGON){ scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING | TMSK0_CMPLX_STRING | TMSK0_SHAPE | TMSK0_CMPLX_SHAPE;}
   else{                  scanlist.typmask[1] = TMSK1_TEXT;}

   /*--- �X�L�����J�n -----*/
   sw2       = 0;
   filepos   = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         if ( elemAddr[ii] >= eofpos){ sw2++;   break;}

         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);

         /*--- ���C������v����ꍇ -----*/
         if ( tmpElm.lv == lv && tmpElm.co == co && tmpElm.st == st && tmpElm.wt == wt){
            typ = mdlElement_getType( &dscr->el);
            if ( type==POLYGON && ( typ==3 || typ==4 || typ==12)){
               mdlElmdscr_stroke( &poi, &nv, dscr, 0.001);

               /*--- ���C���n�v�f���Ȃ��� ---*/
               lineString_add( &pollink.err_lv, &pollink.err_co, &pollink.err_st, &pollink.err_wt, NULL, poi, &nv);
			   dlmSystem_mdlFree( poi);
            }
            else{
               Pos[*num] = elemAddr[ii];
               *num      = *num + 1;
            }
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( sw2 > 0){ break;}
      if ( status != BUFF_FULL){ break;}
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�I���j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_end_hook( DialogItemMessage *dimP)
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
Private int elmSymChk( MSElement *el, ELMSYMA *tmpElm, ULong offset, MSElementDescr *eld)
{
   mdlElement_getProperties( &tmpElm->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, el);
   mdlElement_getSymbology( &tmpElm->co, &tmpElm->wt, &tmpElm->st, el);

   return( 1);
}


/*----------------------------------------------------------------------+
| 	�w���v�f��̧���߼޼�݁A�n�_�ʒu���擾				|
+----------------------------------------------------------------------*/
Private int     lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber)
{
int            rtn = LOCATE_ELEMENT_REJECT, sw, j;
UInt32          readFilePos;
MSElementDescr *edP = NULL;
ELMSYMA         tmpElm;

    /*----- �I�������v�f��ǂ� -----*/
    mdlElmdscr_read( &edP, filePosition, MASTERFILE, FALSE, &readFilePos);
    sw = 0;

    /*----- �I�������v�f�̼���ۼނ�ǂ� -----*/
    mdlElmdscr_operation( edP, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

    /*----- �|���S����I�������ꍇ -----*/
    if ( check == POLYGON){
       for( j=0; j<layNum; j++){
          if ( pol_lv[j] == tmpElm.lv && pol_co[j] == tmpElm.co
            && pol_wt[j] == tmpElm.wt && pol_st[j] == tmpElm.st){
             polID = j;
             sw    = 1;
             break;
          }
       }
    }
    /*----- �e�L�X�g��I�������ꍇ -----*/
    else{
       if ( txt_lv[polID][0] == tmpElm.lv && txt_co[polID][0] == tmpElm.co
         && txt_wt[polID][0] == tmpElm.wt && txt_st[polID][0] == tmpElm.st){
          sw    = 1;
       }
    }

    /*----- �I�������v�f�̼���ۼނ�ð��قƈ�v���Ȃ��ꍇ�i���ہj -----*/
    if ( !sw){
        if ( check == POLYGON){ mdlDialog_openInfoBox( L"�|���S���̃��C�����Ⴂ�܂�");}
        else{                   mdlDialog_openInfoBox( L"�e�L�X�g�̃��C�����Ⴂ�܂�");}
        mdlElmdscr_freeAll( &edP);
        return( rtn);
    }

    /*----- �I�������v�f��̧���߼޼�� -----*/
    if ( check == POLYGON){ polPos = filePosition;}
    else{                   txtPos = filePosition;}

    if ( NULL != edP){ mdlElmdscr_freeAll( &edP);}

    /*----- ���F -----*/
    rtn = LOCATE_ELEMENT_ACCEPT;
    return( rtn);
}


/*-----------------------------------------------------------------------
**	�w�肵�����W�t�߂̗v�f����
-----------------------------------------------------------------------*/
Private int 	scan_nearPoint( WChar *str, Dpoint3d *point, int lv, int co, int st, int wt, int type)
{
MSElementDescr	*dscr;
ExtScanList		scanlist;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, sw, sw2, nv;
Dpoint3d        *poi, orig;
WChar            tmpstr[21];
ELMSYMA			tmpElm;
WString         wstr;

   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   /*----- �X�L���� -----*/
   get_scan_rangea( &scanlist, point, point);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   if ( type == LINE){ scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING;}
   else{               scanlist.typmask[1] = TMSK1_TEXT;}

   /*--- �X�L�����J�n -----*/
   sw2 = 0;
   sw  = 1;
   filepos   = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         if ( elemAddr[ii] >= eofpos){ sw2++;   break;}

         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);

         /*--- ���C������v����ꍇ -----*/
         if ( tmpElm.lv == lv && tmpElm.co == co && tmpElm.st == st && tmpElm.wt == wt){
            mdlElmdscr_stroke( &poi, &nv, dscr, 0.001);

            /*--- ���L�ƒ��L�����Ԑ��̏ꍇ -----*/
            if ( type == LINE){
               if ( get_leng( &poi[0], point) <= pollink.tolerance){
                  memcpy( point, &poi[nv-1], sizeof( Dpoint3d));
                  sw = 0;
                  sw2++;
               }
               else if ( get_leng( &poi[nv-1], point) <= pollink.tolerance){
                  memcpy( point, &poi[0], sizeof( Dpoint3d));
                  sw = 0;
                  sw2++;
               }
            }

            /*--- ���L2�̏ꍇ -----*/
            else{
               mdlText_extract( &wstr, NULL, &orig, NULL, NULL, NULL, NULL, &dscr->el);
			   swprintf(tmpstr, L"%ls", wstr.GetWCharCP());
               if ( get_leng( &orig, point) <= pollink.tolerance){
                  wcscpy( str, tmpstr);
                  sw = 0;
                  sw2++;
               }
            }
			dlmSystem_mdlFree( poi);
         }
         mdlElmdscr_freeAll( &dscr);
         if ( sw2 > 0){ break;}
      }
      if ( sw2 > 0){ break;}
      if ( status != BUFF_FULL){ break;}
   }
   return( sw);
}


/*-----------------------------------------------------------------------
**	�����͈͐ݒ�
-----------------------------------------------------------------------*/
Private void	get_scan_rangea( ExtScanList *scanlist, Dpoint3d *min, Dpoint3d *max)
{
Point3d			ipoint;
Dpoint3d        dpoint, trpoint;

    dpoint.x          = min->x - 10.0;
    dpoint.y          = min->y - 10.0;
	mdlCurrTrans_transformPointArray(&trpoint, &dpoint, 1);
	mdlCnv_DPointToIPoint(&ipoint, &trpoint);/* �����̍��W		*/
	scanlist->xlowlim = ipoint.x;
	scanlist->ylowlim = ipoint.y;

    dpoint.x           = max->x + 10.0;
    dpoint.y           = max->y + 10.0;
	mdlCurrTrans_transformPointArray(&trpoint, &dpoint, 1);
	mdlCnv_DPointToIPoint(&ipoint, &trpoint);/* �E��̍��W		*/
    scanlist->xhighlim = ipoint.x;
    scanlist->yhighlim = ipoint.y;
}


// �|���S�����Ƀ|�C���g�����邩���ׂ�
// -1:�G���[, 1:�O��, 0:�|���S����, 2:����
int _PointInside(Dpoint3d *ply, int pntNum, Dpoint3d *pnt)
{
	double		*tmpPoly, tmpPnt[5];
	int			j, ret;
	WChar		msg[256];

	// �������m��
	if ((tmpPoly = (double*)malloc(sizeof(double) * (pntNum * 2 + 2))) == NULL)
	{
		wcscpy(msg, L"�������m�ۃG���[");
		//Log(msg);
		return -1;
	}

	for (j = 0; j < pntNum; j++)
	{
		tmpPoly[j * 2] = ply[j].x;
		tmpPoly[j * 2 + 1] = ply[j].y;
	}

	// pts���܂�ł���|���S�����`�F�b�N
	tmpPnt[0] = pnt->x;
	tmpPnt[1] = pnt->y;
	ret = mdlPolygon_pointInside(tmpPnt, tmpPoly, pntNum, TRUE, 2, 0.001); //<= 0 if outside, -1 of on, 1 if inside
	//sprintf( msg, "mdlPolygon_pointInside= %d", ret );
	//Log(msg);
	if (-1 < ret && ret <= 0)
		ret = 0;

	free(tmpPoly);
	return ret + 1;
}