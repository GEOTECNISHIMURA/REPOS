#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-	        ���C��														-*/
/*-	        DOUKAN2.MC													-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <string.h>
#include <stdio.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "doukan2.h"

#define MAXBUF			512
#define MAX_LAYERCNT	1000
#define TEST			1
#define BUNKATU			2
#define GG				3
#define KANRI			4
#define LINE			5

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE			*fp, *fp_log;
RscFileHandle   Rsc;
MSDialog        *db_p;
int	        	exitStatus;
ULong			selectPos;
DOUKAN2			doukan2;


/*-    �e�[�u���t�@�C�����   -*/
typedef struct {
	int		p65Num, bunNum, linNum, ggNum, kanriNum;
	ELMSYM	p65[100], p65err[100], bun[100], bunerr[100], lin[100];
	ELMSYM	gg[100], ggerr[100], kanri[100], kanrierr[100];
} TABLEINFO;
TABLEINFO	tblI;


#ifndef Private
#define Private
#endif

/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_tbl_hook( DialogItemMessage *dimP);
Private void	push_65_hook( DialogItemMessage *dimP);
Private void	push_test_hook( DialogItemMessage *dimP);
Private void	push_bunkatu_hook( DialogItemMessage *dimP);
Private void	push_gg_hook( DialogItemMessage *dimP);
Private void	push_kanri_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
	{ HOOKID_DOUKAN2_DIALOG, (PFDialogHook)dialog_hook      },
	{ HOOKID_TOGGLE_IDX,     (PFDialogHook)toggle_idx_hook  },
	{ HOOKID_PUSH_IDX,       (PFDialogHook)push_idx_hook    },
	{ HOOKID_PUSH_TBL,       (PFDialogHook)push_tbl_hook    },
	{ HOOKID_PUSH_65,        (PFDialogHook)push_65_hook     },
	{ HOOKID_PUSH_TEST,      (PFDialogHook)push_test_hook   },
	{ HOOKID_PUSH_BUNKATU,   (PFDialogHook)push_bunkatu_hook},
	{ HOOKID_PUSH_GG,        (PFDialogHook)push_gg_hook     },
	{ HOOKID_PUSH_KANRI,     (PFDialogHook)push_kanri_hook  },
	{ HOOKID_PUSH_END,       (PFDialogHook)push_end_hook    },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int		read_layer( WChar *datName);
Private int		syori( int mode);

Private void    syori_65( void);
Private void    startModify_polygon( void);
Private void    syori_test( void);
Private void    syori_bunkatu( void);
Private void    bunkatu( int polNum, ULong *polPos, ELMSYM *chg_layer, int flag, UInt32 ggmax);
Private void    polygon_bunkatu( MSElementDescr *pDscr, Dpoint3d *kouPoi, ELMSYM *chg_layer, UInt32 *ggmax, ULong polPos, ULong *polPos1, ULong *polPos2);
Private void    syori_gg( void);
Private void    syori_kanri( void);

Private int		scan_element( ULong *Pos, ELMSYM *err_layer, int flag);
Private int		scan_gg( UInt32 *ggnum, ULong *Pos, ELMSYM *err_layer, int tblID);
Private int		scan_kanri( WChar **str, ULong *Pos, ELMSYM *err_layer, int tblID);
Private void	layer_change( MSElementDescr *dscr, ULong filePos, UInt32 lv, UInt32 co, UInt32 wt, int st);
Private int 	elmSymChk( MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr	*eld);
Private int		selectElemCheck( int preLocate, MSElement *elementP, int fileNum, unsigned long filePos, Point3d *pointP, int viewNumber);
Private void	save_settei( void);

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
	BeFileName		full;

/*
#if LOCK == 1
   main_lok( );
#endif
*/
	chk_kigen( );



   /*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"polygon64 �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "DOUKAN2_INFO");
   sprintf(variableName, "doukan2");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &doukan2);

   /*--- �e�[�u���t�@�C���I�[�v�� -----*/
   mdlFile_find( &full, L"doukan2.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &doukan2.tog_idx);
      fwscanf( fp, L"%ls\n", doukan2.idxName);
      fwscanf( fp, L"%ls\n", doukan2.tblName);
	  fclose(fp);
   }
   else{
      doukan2.tog_idx    = -1;
	  swprintf( doukan2.idxName, L"c:\\\0");
	  swprintf( doukan2.tblName, L"c:\\\0");
   }


   /*--- �_�C�A���O�{�b�N�X ------------*/
   db_p = mdlDialog_open( NULL, DIALOGID_DOUKAN2);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"DOUKAN2 �_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �P�t�@�C���̏ꍇ ------------*/
   if ( doukan2.tog_idx != -1){
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


/*-----------------------------------------------------------------------
**	�g�O���{�^��(IDX�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_idx_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_STATECHANGED:
         /*--- �o�b�`�̏ꍇ ------------*/
         if ( doukan2.tog_idx == -1){
            mdlDialog_itemSetEnabledState( db_p, 1, TRUE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, TRUE, FALSE);
         }

         /*--- �P�t�@�C���̏ꍇ ------------*/
         else{
            mdlDialog_itemSetEnabledState( db_p, 1, FALSE, FALSE);
            mdlDialog_itemSetEnabledState( db_p, 2, FALSE, FALSE);
         }
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(IDX�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title,  L"idx�t�@�C����I��");

         mdlFile_parseName( doukan2.idxName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf( doukan2.idxName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(TBL�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_tbl_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.tbl");
         swprintf( title,  L"tbl�t�@�C����I��");

         mdlFile_parseName( doukan2.tblName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf( doukan2.tblName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	     dimP->msgUnderstood = FALSE;
	     break;
   }
}


/*------------------------------------------------------------------------
**	���C���e�[�u����ǂ�
------------------------------------------------------------------------*/
Private  int  read_layer( WChar *datName)
{

WChar	s_buf[MAXBUF], info[MAXBUF];
int		sw = 0, p65Num = 0, bunNum = 0, linNum = 0, ggNum = 0, kanriNum = 0;

    /*----- ð���̧�ق��J�� -----*/
	if ( NULL == ( fp = _wfopen( datName, L"rt"))){
		swprintf( info, L"ð���̧�� %ls ���ǂ߂܂���", datName);
        mdlDialog_openInfoBox( info);
		return( ERROR);
	}

	while( 1){
		fgetws( s_buf, 512, fp);
		if ( feof( fp) || ferror( fp)){ break;}

		if ( 0 == wcsncmp( s_buf, L";", 1)){	 continue;}
		if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

		if ( 0 == wcsncmp( s_buf, L"#65",      wcslen( L"#65"))){      sw = 1;   continue;}
		if ( 0 == wcsncmp( s_buf, L"#BUNKATU", wcslen( L"#BUNKATU"))){ sw = 2;   continue;}
		if ( 0 == wcsncmp( s_buf, L"#LINE",    wcslen( L"#LINE"))){    sw = 3;   continue;}
		if ( 0 == wcsncmp( s_buf, L"#GG",      wcslen( L"#GG"))){      sw = 4;   continue;}
		if ( 0 == wcsncmp( s_buf, L"#KANRI",   wcslen( L"#KANRI"))){   sw = 5;   continue;}

		switch( sw){
			/*--- #65 ---*/
			case 1:
				swscanf( s_buf, L"%d %d %d %d %d %d %d %d"
                      , &tblI.p65[p65Num].lv,    &tblI.p65[p65Num].co,    &tblI.p65[p65Num].wt,    &tblI.p65[p65Num].st
                      , &tblI.p65err[p65Num].lv, &tblI.p65err[p65Num].co, &tblI.p65err[p65Num].wt, &tblI.p65err[p65Num].st);
                p65Num++;
				break;

			/*--- #BUNKATU ---*/
			case 2:
				swscanf( s_buf, L"%d %d %d %d %d %d %d %d"
                      , &tblI.bun[bunNum].lv,    &tblI.bun[bunNum].co,    &tblI.bun[bunNum].wt,    &tblI.bun[bunNum].st
                      , &tblI.bunerr[bunNum].lv, &tblI.bunerr[bunNum].co, &tblI.bunerr[bunNum].wt, &tblI.bunerr[bunNum].st);
                bunNum++;
				break;

			/*--- #LINE ---*/
			case 3:
				swscanf( s_buf, L"%d %d %d %d"
                      , &tblI.lin[linNum].lv, &tblI.lin[linNum].co, &tblI.lin[linNum].wt, &tblI.lin[linNum].st);
                linNum++;
				break;

			/*--- #GG ---*/
			case 4:
				swscanf( s_buf, L"%d %d %d %d %d %d %d %d"
                      , &tblI.gg[ggNum].lv,    &tblI.gg[ggNum].co,    &tblI.gg[ggNum].wt,    &tblI.gg[ggNum].st
                      , &tblI.ggerr[ggNum].lv, &tblI.ggerr[ggNum].co, &tblI.ggerr[ggNum].wt, &tblI.ggerr[ggNum].st);
                ggNum++;
				break;

			/*--- #KANRI ---*/
			case 5:
				swscanf( s_buf, L"%d %d %d %d %d %d %d %d"
                      , &tblI.kanri[kanriNum].lv,    &tblI.kanri[kanriNum].co
                      , &tblI.kanri[kanriNum].wt,    &tblI.kanri[kanriNum].st
                      , &tblI.kanrierr[kanriNum].lv, &tblI.kanrierr[kanriNum].co
                      , &tblI.kanrierr[kanriNum].wt, &tblI.kanrierr[kanriNum].st);
                kanriNum++;
				break;
        }
	}
	fclose( fp);

    tblI.p65Num   = p65Num;
    tblI.bunNum   = bunNum;
    tblI.linNum   = linNum;
    tblI.ggNum    = ggNum;
    tblI.kanriNum = kanriNum;

    return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i65�_�ȏ�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_65_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         save_settei( );

         syori( 65);
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i64�_�ȓ��ŕ����\���e�X�g�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_test_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         save_settei( );

         /*--- �e�[�u���ǂݍ��� ---*/
         if ( read_layer( doukan2.tblName) == ERROR){ break;}

         /*--- �|���S���I�� ---*/
         startModify_polygon( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�����j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_bunkatu_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         save_settei( );

         syori( BUNKATU);
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���iGG�����j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_gg_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         save_settei( );

         syori( GG);
         break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�Ǘ��ԍ������j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_kanri_hook( DialogItemMessage *dimP)
{
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         save_settei( );

         syori( KANRI);
         break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j �t�b�N�֐�
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


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  int   syori( int mode)
{
WChar	info[MAXBUF], dgnName[512];


   /*--- �e�[�u���ǂݍ��� ---*/
   if ( read_layer( doukan2.tblName) == ERROR){ return( ERROR);}

   /*--- �P�t�@�C���̏ꍇ ---*/
   if ( doukan2.tog_idx != -1){
      if ( mode == 65){      syori_65( );     } // 65�_�ȏ�
      if ( mode == BUNKATU){ syori_bunkatu( );} // ����
      if ( mode == GG){      syori_gg( );     } // GG
      if ( mode == KANRI){   syori_kanri( );  } // �Ǘ��ԍ�
   }

   /*--- �o�b�`�̏ꍇ ---*/
   else{
      /*----- ���̓t�@�C�����X�g���J�� -----*/
	  if ( NULL == ( fp = _wfopen( doukan2.idxName, L"r"))){
	     swprintf( info, L"idx�t�@�C�� %ls ���J���܂���", doukan2.idxName);
         mdlDialog_openInfoBox( info);
      }

      /*----- ���̓t�@�C�����X�g��ǂ� -----*/
      while( 1){
	     fwscanf( fp, L"%ls", dgnName);
         if ( feof( fp) || ferror( fp)){ break;}

         if ( 0 == wcsncmp( dgnName, L";", 1)){ continue;}
	     if ( 0 == wcsncmp( dgnName, L"\n", 1)){ continue;}

         if ( mdlSystem_newDesignFile( dgnName) == SUCCESS){
            /*--- �����̎��s ---*/
            if ( mode == 65){      syori_65( );     } // 65�_�ȏ�
            if ( mode == BUNKATU){ syori_bunkatu( );} // ����
            if ( mode == GG){      syori_gg( );     } // GG
            if ( mode == KANRI){   syori_kanri( );  } // �Ǘ��ԍ�
         }
		
      }
	  fclose(fp);
      
   }
   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	65�_�ȏ�|���S���̌���
------------------------------------------------------------------------*/
Private  void   syori_65( )
{
MSElementDescr	*dscr;
int				ii, polNum, nv;
ULong			*polPos;
Dpoint3d		*points;
ELMSYM			*err_layer;

   polPos    = (ULong*)calloc( 10000, sizeof( ULong));
   err_layer = (ELMSYM*)malloc( 10000 * sizeof( ELMSYM));

   /*----- �|���S���v�f�̌��� -----*/
   polNum = scan_element( polPos, err_layer, 65);

   /*----- �|���S���v�f -----*/
   for( ii=0; ii<polNum; ii++){
      /*----- �|���S���̓_�� -----*/
      mdlElmdscr_read( &dscr, polPos[ii], MASTERFILE, FALSE, NULL);
      mdlElmdscr_stroke( &points, &nv, dscr, 0.01);

      /*----- 65�_�ȏ�̓G���[ -----*/
      if ( nv >= 65){
         layer_change( dscr, polPos[ii], err_layer[ii].lv, err_layer[ii].co, err_layer[ii].wt, err_layer[ii].st);
      }
      //free( points);
	  dlmSystem_mdlFree(points);
      mdlElmdscr_freeAll( &dscr);
   }

   free( err_layer);
   free( polPos);
}


/*-----------------------------------------------------------------------
**	�|���S���I��
-----------------------------------------------------------------------*/
Private void	startModify_polygon( void)
{
   /*--- �A�N�e�B�u�t�@�C���̂ݑI���\ ---*/
   mdlLocate_normal( );
   mdlState_startModifyCommand((StateFunc_Reset)startModify_polygon, (MdlFunc_Accept)syori_test, NULL, NULL, NULL, 2, 1, TRUE, 0);

   /*--- �I��v�f�����F���邩���ۂ��邩�𔻒肷�� ------*/
   LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)selectElemCheck);

   /*--- �t�@�C���̐擪���猟�� -----*/
   mdlLocate_init( );
}


/*-----------------------------------------------------------------------
**	�����e�X�g
-----------------------------------------------------------------------*/
Private void	syori_test( void)
{
MSElementDescr	*pDscr;
int				polNum, ii;
UInt32          ggnum, ggmax;
ULong			*polPos;
ELMSYM			*chg_layer;

   polPos    = (ULong*)calloc( 10000, sizeof( ULong));
   chg_layer = (ELMSYM*)malloc( 10000 * sizeof( ELMSYM));

   /*----- �v�f�̌��� -----*/
   polNum = scan_element( polPos, chg_layer, BUNKATU);

   /*----- �ő�O���[�v�ԍ����� -----*/
   ggmax = 0;
   for( ii=0; ii<polNum; ii++){
      mdlElmdscr_read( &pDscr, polPos[ii], MASTERFILE, FALSE, NULL);
      mdlElement_getProperties( NULL, &ggnum, NULL, NULL, NULL, NULL, NULL, NULL, &pDscr->el);
      if ( ggnum > ggmax){ ggmax = ggnum;}
      mdlElmdscr_freeAll( &pDscr);
      if ( polPos[ii] == selectPos){
         memcpy( &chg_layer[0], &chg_layer[ii], sizeof( ELMSYM));
      }
   }
   ggmax++;

   /*----- �I�������v�f -----*/
   polNum    = 1;
   polPos[0] = selectPos;

   /*----- �������� -----*/
   bunkatu( polNum, polPos, chg_layer, TEST, ggmax);

   free( chg_layer);
   free( polPos);
   startModify_polygon( );
}


/*------------------------------------------------------------------------
**	�|���S���̕���
------------------------------------------------------------------------*/
Private  void   syori_bunkatu( )
{
MSElementDescr	*pDscr;
int				polNum, ii;
UInt32          ggnum, ggmax;
ULong			*polPos;
ELMSYM			*chg_layer;

   polPos    = (ULong*)calloc( 10000, sizeof( ULong));
   chg_layer = (ELMSYM*)malloc( 10000 * sizeof( ELMSYM));

   /*----- �v�f�̌��� -----*/
   polNum = scan_element( polPos, chg_layer, BUNKATU);

   /*----- �ő�O���[�v�ԍ����� -----*/
   ggmax = 0;
   for( ii=0; ii<polNum; ii++){
      mdlElmdscr_read( &pDscr, polPos[ii], MASTERFILE, FALSE, NULL);
      mdlElement_getProperties( NULL, &ggnum, NULL, NULL, NULL, NULL, NULL, NULL, &pDscr->el);
      if ( ggnum > ggmax){ ggmax = ggnum;}
      mdlElmdscr_freeAll( &pDscr);
   }
   ggmax++;

   /*----- �������� -----*/
   bunkatu( polNum, polPos, chg_layer, BUNKATU, ggmax);

   free( chg_layer);
   free( polPos);
}


/*------------------------------------------------------------------------
**	����
------------------------------------------------------------------------*/
Private void    bunkatu( int polNum, ULong *polPos, ELMSYM *chg_layer, int flag, UInt32 ggmax)
{
MSElementDescr	*pDscr, *lDscr;
int				ii, jj, linNum, ispnum, *lin_del;
ULong			*linPos, polPos1, polPos2;
Dpoint3d		isPnt1[10], isPnt2[10];
ELMSYM			*tmp;

   linPos    = (ULong*)calloc( 10000, sizeof( ULong));
   lin_del   = (int*)calloc( 10000, sizeof( int));
   tmp       = (ELMSYM*)malloc( sizeof( ELMSYM));

   /*----- �v�f�̌��� -----*/
   linNum = scan_element( linPos, tmp, LINE);

   /*----- �|���S���v�f -----*/
   for( ii=0; ii<polNum; ii++){
      mdlElmdscr_read( &pDscr, polPos[ii], MASTERFILE, FALSE, NULL);

      /*----- ���C���v�f -----*/
      for( jj=0; jj<linNum; jj++){
         if ( lin_del[jj] == 1){ continue;} // �g�p�ς݃��C���͖���

         mdlElmdscr_read( &lDscr, linPos[jj], MASTERFILE, FALSE, NULL);
         ispnum = mdlIntersect_allBetweenElms( isPnt1, isPnt2, 10, pDscr, lDscr, NULL, 0.01);
         del_doubled3( &ispnum, isPnt1, 0.1);

         /*----- ���C�����|���S����2��_�����ꍇ -----*/
         if ( ispnum == 2){
            polygon_bunkatu( pDscr, isPnt1, &chg_layer[ii], &ggmax, polPos[ii], &polPos1, &polPos2);
            polPos[polNum]   = polPos1;
            memcpy( &chg_layer[polNum], &chg_layer[ii], sizeof( ELMSYM));
            polPos[polNum+1] = polPos2;
            memcpy( &chg_layer[polNum+1], &chg_layer[ii], sizeof( ELMSYM));
            polNum           = polNum + 2;
            lin_del[jj]++;
            mdlElmdscr_freeAll( &lDscr);
            break;
         }
         mdlElmdscr_freeAll( &lDscr);
      }
      mdlElmdscr_freeAll( &pDscr);
   }

   free( tmp);
   free( lin_del);
   free( linPos);
}


/*------------------------------------------------------------------------
**	�|���S���̕���
------------------------------------------------------------------------*/
Private void    polygon_bunkatu( MSElementDescr *pDscr, Dpoint3d *kouPoi, ELMSYM *chg_layer, UInt32 *ggmax, ULong polPos, ULong *polPos1, ULong *polPos2)
{
MSElementDescr	*dscr;
int				ii,  pnv, pnv1, pnv2, iti[2];
int				st;
UInt32          lv, co, wt, ggnum; 
double			leng[2];
Dpoint3d		*tmpPoi, *pPoi, *pPoi1, *pPoi2, nearPoi;

   /*----- ���̃|���S���̃��C�� -----*/
   get_layer( &lv, &co, &st, &wt, &ggnum, &pDscr->el);
   mdlElmdscr_stroke( &tmpPoi, &pnv, pDscr, 0.01);
   pPoi  = (Dpoint3d*)malloc( ( pnv+2) * sizeof( Dpoint3d));
   pPoi1 = (Dpoint3d*)malloc( ( pnv+2) * sizeof( Dpoint3d));
   pPoi2 = (Dpoint3d*)malloc( ( pnv+2) * sizeof( Dpoint3d));

   /*----- 1�ڂ̌�_�̃|���S����̈ʒu -----*/
   get_closest_point( &pnv, tmpPoi, &kouPoi[0], &nearPoi, &iti[0], &leng[0]);

   /*----- 1�ڂ̌�_���|���S����1�_�ڂƂ��ă|���S�����W���\�[�g -----*/
   memcpy( &pPoi[0], &kouPoi[0], sizeof( Dpoint3d));
   for( ii=iti[0]+1; ii<pnv; ii++){
      memcpy( &pPoi[ii-iti[0]], &tmpPoi[ii], sizeof( Dpoint3d));
   }
   for( ii=0; ii<=iti[0]; ii++){
      memcpy( &pPoi[pnv-iti[0]+ii], &tmpPoi[ii], sizeof( Dpoint3d));
   }

   pnv++;
   del_doubled3( &pnv, pPoi, 0.1);

   memcpy( &pPoi[pnv], &kouPoi[0], sizeof( Dpoint3d));
   pnv++;

   /*----- 2�ڂ̌�_�̃|���S����̈ʒu -----*/
   get_closest_point( &pnv, pPoi, &kouPoi[1], &nearPoi, &iti[1], &leng[1]);

   /*----- 1�ڂ̕����|���S���쐬 -----*/
   for( ii=0; ii<=iti[1]; ii++){
      memcpy( &pPoi1[ii], &pPoi[ii], sizeof( Dpoint3d));
   }
   memcpy( &pPoi1[iti[1]+1], &kouPoi[1], sizeof( Dpoint3d));
   pnv1 = iti[1] + 2;
   del_doubled3( &pnv1, pPoi1, 0.1);
   memcpy( &pPoi1[pnv1], &kouPoi[0], sizeof( Dpoint3d));
   pnv1++;

   /*----- 1�ڂ̕����|���S����65�_�ȏ�̏ꍇ�i���̃��C���̂܂܁j -----*/
   if ( pnv1 >= 65){
      polygon_create( &lv, &co, &st, &wt, &ggnum, pPoi1, pnv1, &dscr);
   }

   /*----- 1�ڂ̕����|���S����64�_�ȉ��̏ꍇ�i�����ς݃��C���ɕύX�j -----*/
   else{
      polygon_create( &chg_layer->lv, &chg_layer->co, &chg_layer->st, &chg_layer->wt, &ggnum, pPoi1, pnv1, &dscr);
   }

   /*----- ���̃|���S����1�ڂ̕����|���S���ɏ������� -----*/
   *polPos1 = mdlElmdscr_rewrite( dscr, pDscr, polPos);
   mdlElmdscr_display( pDscr, MASTERFILE, DRAW_MODE_Erase);
   mdlElmdscr_display( dscr, MASTERFILE, DRAW_MODE_Normal);
   mdlElmdscr_freeAll( &dscr);

   /*----- 2�ڂ̕����|���S���쐬 -----*/
   memcpy( &pPoi2[0], &kouPoi[1], sizeof( Dpoint3d));
   for( ii=iti[1]+1; ii<pnv; ii++){
      memcpy( &pPoi2[ii-iti[1]], &pPoi[ii], sizeof( Dpoint3d));
   }
   pnv2 = pnv - iti[1];
   del_doubled3( &pnv2, pPoi2, 0.1);
   memcpy( &pPoi2[pnv2], &kouPoi[1], sizeof( Dpoint3d));
   pnv2++;

   /*----- 2�ڂ̕����|���S����65�_�ȏ�̏ꍇ�i���̃��C���̂܂܁j -----*/
   if ( pnv2 >= 65){
      polygon_create( &lv, &co, &st, &wt, ggmax, pPoi2, pnv2, &dscr);
   }

   /*----- 2�ڂ̕����|���S����64�_�ȉ��̏ꍇ�i�����ς݃��C���ɕύX�j -----*/
   else{
      polygon_create( &chg_layer->lv, &chg_layer->co, &chg_layer->st, &chg_layer->wt, ggmax, pPoi2, pnv2, &dscr);
   }

   /*----- 2�ڂ̕����|���S����z�u -----*/
   *polPos2 = mdlElmdscr_add( dscr);
   mdlElmdscr_display( dscr, MASTERFILE, DRAW_MODE_Normal);
   mdlElmdscr_freeAll( &dscr);
   *ggmax = *ggmax + 1;

   free( pPoi2);
   free( pPoi1);
   free( pPoi);
   dlmSystem_mdlFree(tmpPoi);
}


/*------------------------------------------------------------------------
**	GG����
------------------------------------------------------------------------*/
Private  void   syori_gg( )
{
MSElementDescr	*dscr;
int				ii, jj, kk, num;
UInt32          *ggnum;
ULong			*Pos;
ELMSYM			*err_layer;

   Pos       = (ULong*)malloc( 50000 * sizeof( ULong));
   ggnum     = (UInt32*)malloc( 50000 * sizeof(UInt32));
   err_layer = (ELMSYM*)malloc( 50000 * sizeof( ELMSYM));

   /*----- GG�������C�� -----*/
   for( ii=0; ii<tblI.ggNum; ii++){
      num = scan_gg( ggnum, Pos, err_layer, ii);

      /*----- GG�����v�f -----*/
      for( jj=0; jj<num-1; jj++){
         for( kk=jj+1; kk<num; kk++){
            /*----- ����GG�����������ꍇ -----*/
            if ( ggnum[kk] == ggnum[jj]){
               mdlElmdscr_read( &dscr, Pos[jj], MASTERFILE, FALSE, NULL);
               layer_change( dscr, Pos[jj], err_layer[jj].lv, err_layer[jj].co, err_layer[jj].wt, err_layer[jj].st);
               mdlElmdscr_freeAll( &dscr);

               mdlElmdscr_read( &dscr, Pos[kk], MASTERFILE, FALSE, NULL);
               layer_change( dscr, Pos[kk], err_layer[kk].lv, err_layer[kk].co, err_layer[kk].wt, err_layer[kk].st);
               mdlElmdscr_freeAll( &dscr);
            }
         }
      }

      /*----- GG�����v�f -----*/
      for( jj=0; jj<num; jj++){
         /*----- ����GG�����������ꍇ -----*/
         if ( ggnum[jj] == 0){
            mdlElmdscr_read( &dscr, Pos[jj], MASTERFILE, FALSE, NULL);
            layer_change( dscr, Pos[jj], err_layer[jj].lv, err_layer[jj].co, err_layer[jj].wt, err_layer[jj].st);
            mdlElmdscr_freeAll( &dscr);
         }
      }
   }

   free( err_layer);
   free( ggnum);
   free( Pos);
}


/*------------------------------------------------------------------------
**	�Ǘ��ԍ�����
------------------------------------------------------------------------*/
Private  void   syori_kanri( )
{
MSElement		oele, nele;
int				ii, jj, kk, num, iKukan1, iKukan2;
ULong			*Pos;
WChar			**str, kukan1[6], kukan2[6];
ELMSYM			*err_layer;

   Pos = (ULong*)malloc( 10000 * sizeof( ULong));

   str = ( WChar **)malloc( 10000 * sizeof( WChar *));
   for( ii=0; ii<10000; ii++){ str[ii] = ( WChar *) malloc( 30 * sizeof(WChar *));}

   err_layer = (ELMSYM*)malloc( 50000 * sizeof( ELMSYM));

   /*----- �Ǘ��ԍ��������C�� -----*/
   for( ii=0; ii<tblI.kanriNum; ii++){
      num = scan_kanri( str, Pos, err_layer, ii);

      /*----- �Ǘ��ԍ��v�f�̃\�[�g -----*/
      for( jj=0; jj<num-1; jj++){
         for( kk=jj+1; kk<num; kk++){
            /*----- ����GG�����������ꍇ -----*/
            if ( wcscmp( str[kk], str[jj]) < 0){
               exchangechar( str[kk],           str[jj]);
               exchangeul(   &Pos[kk],          &Pos[jj]);
               exchangei(    (int*)&err_layer[kk].lv, (int*)&err_layer[jj].lv);
               exchangei(    (int*)&err_layer[kk].co, (int*)&err_layer[jj].co);
               exchangei(    (int*)&err_layer[kk].wt, (int*)&err_layer[jj].wt);
               exchangei(    &err_layer[kk].st, &err_layer[jj].st);
            }
         }
      }

      /*----- �Ǘ��ԍ��v�f -----*/
      swprintf( kukan1, L"%c%c%c%c%c"
             , str[0][17], str[0][18], str[0][19], str[0][20], str[0][21]);
      iKukan1 = _wtoi( kukan1);
      for( jj=1; jj<num; jj++){
         /*----- �P�O�̗v�f�Ɠ���H���̏ꍇ -----*/
		 if ( wcsncmp( str[jj], str[jj-1], 17) == 0){
            swprintf( kukan2, L"%c%c%c%c%c"
                   , str[jj][17], str[jj][18], str[jj][19], str[jj][20], str[jj][21]);
            iKukan2 = _wtoi( kukan2);

            /*----- ���̔ԍ��Ɠ������� -----*/
            if ( iKukan2 - iKukan1 == 0){
               /*----- �G���[���C���[�ɕύX -----*/
               mdlElement_read( &oele, MASTERFILE, Pos[jj-1]);
               memcpy( &nele, &oele, sizeof( MSElement));
               set_layer( &err_layer[jj-1].lv, &err_layer[jj-1].co, &err_layer[jj-1].st, &err_layer[jj-1].wt, NULL, &nele);
               mdlElement_display( &oele, DRAW_MODE_Erase);
               mdlElement_display( &nele, DRAW_MODE_Normal);
               mdlElement_rewrite( &nele, &oele, Pos[jj-1]);
            }

            /*----- �O�̔ԍ��Ɠ����܂��͌��Ԃ�����ꍇ -----*/
            if ( iKukan2 - iKukan1 != 1){
               /*----- �G���[���C���[�ɕύX -----*/
               mdlElement_read( &oele, MASTERFILE, Pos[jj]);
               memcpy( &nele, &oele, sizeof( MSElement));
               set_layer( &err_layer[jj].lv, &err_layer[jj].co, &err_layer[jj].st, &err_layer[jj].wt, NULL, &nele);
               mdlElement_display( &oele, DRAW_MODE_Erase);
               mdlElement_display( &nele, DRAW_MODE_Normal);
               mdlElement_rewrite( &nele, &oele, Pos[jj]);
            }

            wcscpy( kukan1, kukan2);
            iKukan1 = iKukan2;
         }

         /*----- �P�O�̗v�f�ƕʘH���̏ꍇ -----*/
		 else{
            swprintf( kukan1, L"%c%c%c%c%c"
                   , str[jj][17], str[jj][18], str[jj][19], str[jj][20], str[jj][21]);
            iKukan1 = _wtoi( kukan1);
            continue;
         }
      }
   }

   for( ii=0; ii<10000; ii++){ free( str[ii]);}
   free( err_layer);
   free( str);
   free( Pos);
}


/*------------------------------------------------------------------------
**	�|���S���v�f�̌���
------------------------------------------------------------------------*/
Private int		scan_element( ULong *Pos, ELMSYM *err_layer, int flag)
{
ExtScanList		scanlist;
MSElementDescr	*dscr;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, jj, sw2, num = 0;
ELMSYM			tmpElm;

   /*----- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++){ scanlist.typmask[ii] = 0x0000;}

   /*----- �|���S���̏ꍇ -----*/
   if ( flag == 65 || flag == BUNKATU){
      scanlist.typmask[0] |= (unsigned short)TMSK0_SHAPE;
      scanlist.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;
   }

   /*----- ���C���̏ꍇ -----*/
   if ( flag == LINE){
      scanlist.typmask[0] |= (unsigned short)TMSK0_LINE;
      scanlist.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   }

   for( ii=0; ii<8; ii++){ scanlist.levmask[ii] = 0xFFFF;}

   /*----- �X�L�����J�n -----*/
   sw2       = 0;
   filepos   = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);
      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);

         /*----- 65�_�ȏ�|���S���̏ꍇ -----*/
         if ( flag == 65){
            for( jj=0; jj<tblI.p65Num; jj++){
               if (  ( tmpElm.lv == tblI.p65[jj].lv || tblI.p65[jj].lv == -1)
                  && ( tmpElm.co == tblI.p65[jj].co || tblI.p65[jj].co == -1)
                  && ( tmpElm.wt == tblI.p65[jj].wt || tblI.p65[jj].wt == -1)
                  && ( tmpElm.st == tblI.p65[jj].st || tblI.p65[jj].st == -1)){
                  Pos[num] = elemAddr[ii];
                  if ( tblI.p65err[jj].lv == -1){ err_layer[num].lv = tmpElm.lv;}
                  else{                           err_layer[num].lv = tblI.p65err[jj].lv;}
                  if ( tblI.p65err[jj].co == -1){ err_layer[num].co = tmpElm.co;}
                  else{                           err_layer[num].co = tblI.p65err[jj].co;}
                  if ( tblI.p65err[jj].wt == -1){ err_layer[num].wt = tmpElm.wt;}
                  else{                           err_layer[num].wt = tblI.p65err[jj].wt;}
                  if ( tblI.p65err[jj].st == -1){ err_layer[num].st = tmpElm.st;}
                  else{                           err_layer[num].st = tblI.p65err[jj].st;}
                  num++;
                  break;
               }
            }
         }

         /*----- �����̏ꍇ -----*/
         if ( flag == BUNKATU){
            for( jj=0; jj<tblI.bunNum; jj++){
               if (  ( tmpElm.lv == tblI.bun[jj].lv || tblI.bun[jj].lv == -1)
                  && ( tmpElm.co == tblI.bun[jj].co || tblI.bun[jj].co == -1)
                  && ( tmpElm.wt == tblI.bun[jj].wt || tblI.bun[jj].wt == -1)
                  && ( tmpElm.st == tblI.bun[jj].st || tblI.bun[jj].st == -1)){
                  Pos[num] = elemAddr[ii];
                  if ( tblI.bunerr[jj].lv == -1){ err_layer[num].lv = tmpElm.lv;}
                  else{                           err_layer[num].lv = tblI.bunerr[jj].lv;}
                  if ( tblI.bunerr[jj].co == -1){ err_layer[num].co = tmpElm.co;}
                  else{                           err_layer[num].co = tblI.bunerr[jj].co;}
                  if ( tblI.bunerr[jj].wt == -1){ err_layer[num].wt = tmpElm.wt;}
                  else{                           err_layer[num].wt = tblI.bunerr[jj].wt;}
                  if ( tblI.bunerr[jj].st == -1){ err_layer[num].st = tmpElm.st;}
                  else{                           err_layer[num].st = tblI.bunerr[jj].st;}
                  num++;
                  break;
               }
            }
         }

         /*----- ���C���̏ꍇ -----*/
         if ( flag == LINE){
            for( jj=0; jj<tblI.linNum; jj++){
               if (  ( tmpElm.lv == tblI.lin[jj].lv || tblI.lin[jj].lv == -1)
                  && ( tmpElm.co == tblI.lin[jj].co || tblI.lin[jj].co == -1)
                  && ( tmpElm.wt == tblI.lin[jj].wt || tblI.lin[jj].wt == -1)
                  && ( tmpElm.st == tblI.lin[jj].st || tblI.lin[jj].st == -1)){
                  Pos[num] = elemAddr[ii];
                  num++;
                  break;
               }
            }
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( status != BUFF_FULL){ break;}
   }
   return( num);
}


/*------------------------------------------------------------------------
**	�v�f�̌����iGG�d�������p�j
------------------------------------------------------------------------*/
Private int		scan_gg( UInt32 *ggnum, ULong *Pos, ELMSYM *err_layer, int tblID)
{
ExtScanList		scanlist;
MSElementDescr	*dscr;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, sw2, num = 0;
ELMSYM			tmpElm;

   /*----- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++){ scanlist.typmask[ii] = 0x0000;}
   scanlist.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;
   scanlist.typmask[0] |= (unsigned short)TMSK0_LINE;
   scanlist.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
   scanlist.typmask[0] |= (unsigned short)TMSK0_SHAPE;
   scanlist.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;
   scanlist.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;
   scanlist.typmask[1] |= (unsigned short)TMSK1_TEXT;

   for( ii=0; ii<8; ii++){ scanlist.levmask[ii] = 0xFFFF;}

   /*----- �X�L�����J�n -----*/
   sw2       = 0;
   filepos   = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);
      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);

         /*----- ���C���[������ -----*/
         if (  ( tmpElm.lv == tblI.gg[tblID].lv || tblI.gg[tblID].lv == -1)
            && ( tmpElm.co == tblI.gg[tblID].co || tblI.gg[tblID].co == -1)
            && ( tmpElm.wt == tblI.gg[tblID].wt || tblI.gg[tblID].wt == -1)
            && ( tmpElm.st == tblI.gg[tblID].st || tblI.gg[tblID].st == -1)){

            if ( tblI.ggerr[tblID].lv == -1){ err_layer[num].lv = tmpElm.lv;}
            else{                             err_layer[num].lv = tblI.ggerr[tblID].lv;}
            if ( tblI.ggerr[tblID].co == -1){ err_layer[num].co = tmpElm.co;}
            else{                             err_layer[num].co = tblI.ggerr[tblID].co;}
            if ( tblI.ggerr[tblID].wt == -1){ err_layer[num].wt = tmpElm.wt;}
            else{                             err_layer[num].wt = tblI.ggerr[tblID].wt;}
            if ( tblI.ggerr[tblID].st == -1){ err_layer[num].st = tmpElm.st;}
            else{                             err_layer[num].st = tblI.ggerr[tblID].st;}
            Pos[num] = elemAddr[ii];
	        mdlElement_getProperties( NULL, &ggnum[num], NULL, NULL, NULL, NULL, NULL, NULL, &dscr->el);
            num++;
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( status != BUFF_FULL){ break;}
   }
   return( num);
}


/*------------------------------------------------------------------------
**	�v�f�̌����i�Ǘ��ԍ������p�j
------------------------------------------------------------------------*/
Private int		scan_kanri( WChar **str, ULong *Pos, ELMSYM *err_layer, int tblID)
{
ExtScanList		scanlist;
MSElementDescr	*dscr;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, sw2, num = 0;
ELMSYM			tmpElm;
WString         wstr;

   /*----- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++){ scanlist.typmask[ii] = 0x0000;}
   scanlist.typmask[1] |= (unsigned short)TMSK1_TEXT;

   for( ii=0; ii<8; ii++){ scanlist.levmask[ii] = 0xFFFF;}

   /*----- �X�L�����J�n -----*/
   sw2       = 0;
   filepos   = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);
      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);

         /*----- ���C���[������ -----*/
         if (  ( tmpElm.lv == tblI.kanri[tblID].lv || tblI.kanri[tblID].lv == -1)
            && ( tmpElm.co == tblI.kanri[tblID].co || tblI.kanri[tblID].co == -1)
            && ( tmpElm.wt == tblI.kanri[tblID].wt || tblI.kanri[tblID].wt == -1)
            && ( tmpElm.st == tblI.kanri[tblID].st || tblI.kanri[tblID].st == -1)){

            if ( tblI.kanrierr[tblID].lv == -1){ err_layer[num].lv = tmpElm.lv;}
            else{                                err_layer[num].lv = tblI.kanrierr[tblID].lv;}
            if ( tblI.kanrierr[tblID].co == -1){ err_layer[num].co = tmpElm.co;}
            else{                                err_layer[num].co = tblI.kanrierr[tblID].co;}
            if ( tblI.kanrierr[tblID].wt == -1){ err_layer[num].wt = tmpElm.wt;}
            else{                                err_layer[num].wt = tblI.kanrierr[tblID].wt;}
            if ( tblI.kanrierr[tblID].st == -1){ err_layer[num].st = tmpElm.st;}
            else{                                err_layer[num].st = tblI.kanrierr[tblID].st;}
            Pos[num] = elemAddr[ii];

			mdlText_extractString( wstr, &dscr->el);
			swprintf(str[num], L"%ls", wstr.GetWCharCP());

            num++;
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( status != BUFF_FULL){ break;}
   }
   return( num);
}


/*------------------------------------------------------------------------
**	���C���[�ύX
------------------------------------------------------------------------*/
Private  void  layer_change( MSElementDescr *dscr, ULong filePos, UInt32 lv, UInt32 co, UInt32 wt, int st)
{
MSElementDescr	*nDscr, *oDscr;
int				type;

   mdlElmdscr_duplicate( &nDscr, dscr);
   type = mdlElement_getType( &nDscr->el);

   /*----- ���C���E���C���X�g�����O�E�V�F�C�v�E�e�L�X�g�̏ꍇ -----*/
   if ( type == 3 || type == 4 || type == 6 || type == 17){
      mdlElement_setProperties( &nDscr->el, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      mdlElement_setSymbology( &nDscr->el, &co, &wt, &st);
      mdlElmdscr_rewrite( nDscr, dscr, filePos);
      mdlElmdscr_display( dscr, MASTERFILE, DRAW_MODE_Erase);
      mdlElmdscr_display( nDscr, MASTERFILE, DRAW_MODE_Normal);
   }

   /*----- �Z���E�����A���E�����}�`�̏ꍇ -----*/
   else if ( type == 2 || type == 12 || type == 14){
      mdlElmdscr_duplicate( &oDscr, dscr);

      /*----- �S�̂̃w�b�_ -----*/
      mdlElement_setProperties( &oDscr->el, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      mdlElement_setSymbology( &oDscr->el, &co, &wt, &st);
      mdlElmdscr_new( &nDscr, NULL, &oDscr->el);
      oDscr = oDscr->h.firstElem;
      do{
         /*----- �w�b�_���̃w�b�_ -----*/
         if ( oDscr->h.isHeader){
            mdlElement_setProperties( &oDscr->el, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            mdlElement_setSymbology( &oDscr->el, &co, &wt, &st);
            mdlElmdscr_appendElement( nDscr, &oDscr->el);
            oDscr = oDscr->h.firstElem;
         }

         /*----- �w�b�_���̍\���v�f -----*/
         else{
            /*----- �Ō�̍\���v�f -----*/
            if ( !oDscr->h.next){
               mdlElement_setProperties( &oDscr->el, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
               mdlElement_setSymbology( &oDscr->el, &co, &wt, &st);
               mdlElmdscr_appendElement( nDscr, &oDscr->el);
               oDscr = oDscr->h.myHeader;
               oDscr = oDscr->h.next;
            }

            /*----- �Ō�łȂ��\���v�f -----*/
            else{
               mdlElement_setProperties( &oDscr->el, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
               mdlElement_setSymbology( &oDscr->el, &co, &wt, &st);
               mdlElmdscr_appendElement( nDscr, &oDscr->el);
               oDscr = oDscr->h.next;
            }
         }
      }while ( oDscr);

      mdlElmdscr_rewrite( nDscr, dscr, filePos);
      mdlElmdscr_display( dscr, MASTERFILE, DRAW_MODE_Erase);
      mdlElmdscr_display( nDscr, MASTERFILE, DRAW_MODE_Normal);
      mdlElmdscr_freeAll( &oDscr);
      mdlElmdscr_freeAll( &nDscr);
   }
}


/*-----------------------------------------------------------------------
**	���C���[�擾
-----------------------------------------------------------------------*/
Private int elmSymChk( MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr	*eld)
{
	mdlElement_getProperties( &tmpElm->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, el);
    mdlElement_getSymbology( &tmpElm->co, &tmpElm->wt, &tmpElm->st, el);

	return( 1);
}


/*----------------------------------------------------------------------+
** 	�I��v�f�̃��C���[�`�F�b�N
+----------------------------------------------------------------------*/
Private int		selectElemCheck( int preLocate, MSElement *elementP, int fileNum, unsigned long filePos, Point3d *pointP, int viewNumber)
{
MSElementDescr *edP = NULL;
ELMSYM         tmpElm;
int            ii, type;

    /*----- �I�������v�f��ǂ� -----*/
    mdlElmdscr_read( &edP, filePos, MASTERFILE, FALSE, NULL);

    /*----- �v�f�^�C�v -----*/
    type = mdlElement_getType( &edP->el);
    if ( type != 6 && type != 14){
       mdlDialog_openInfoBox( L"�|���S����I�����ĉ�����");
       return( LOCATE_ELEMENT_REJECT);
    }

    /*----- �I�������v�f�̼���ۼނ�ǂ� -----*/
    mdlElmdscr_operation( edP, (PFElemOperation)elmSymChk, &tmpElm, ELMD_ELEMENT);
    mdlElmdscr_freeAll( &edP);

    /*----- �I�������v�f�̼���ۼނ�ǂ� -----*/
    for( ii=0; ii<tblI.bunNum; ii++){
       if (  ( tmpElm.lv == tblI.bun[ii].lv || tblI.bun[ii].lv == -1)
          && ( tmpElm.co == tblI.bun[ii].co || tblI.bun[ii].co == -1)
          && ( tmpElm.wt == tblI.bun[ii].wt || tblI.bun[ii].wt == -1)
          && ( tmpElm.st == tblI.bun[ii].st || tblI.bun[ii].st == -1)){
          selectPos = filePos;
          return( LOCATE_ELEMENT_ACCEPT);
       }
    }

    /*----- �I�������v�f���ݒ背�C���łȂ��ꍇ -----*/
    mdlDialog_openInfoBox( L"�ݒ肵�����C���ł͂���܂���");

    return( LOCATE_ELEMENT_REJECT);
}


/*-----------------------------------------------------------------------
**	���C���[�擾
-----------------------------------------------------------------------*/
Private void	save_settei( )
{
	BeFileName	full;

   /*--- �e�[�u���t�@�C���I�[�v�� -----*/
   mdlFile_find( &full, L"doukan2.tbl", L"MS_DEF", NULL);
   fp = _wfopen(full.GetWCharCP(), L"w");
      fwprintf( fp, L"%d\n", doukan2.tog_idx);
      fwprintf( fp, L"%ls\n", doukan2.idxName);
      fwprintf( fp, L"%ls\n", doukan2.tblName);
   fclose( fp);
}
