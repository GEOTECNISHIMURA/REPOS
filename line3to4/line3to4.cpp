/*-----------------------------------------------------------------------*/
/*-	    �v���O���� 					-*/
/*-									-*/
/*-	        LINE3TO4.MC				-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>

#include <ctype.h>
#include <string.h>

#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "line3to4.h"

FILE			*fp;
RscFileHandle   Rsc;
MSDialog       *db_p;
int				exitStatus;
LINE3TO4   		line3to4;

typedef struct {
    int   lv, co, st, wt;
} TABLEINFO;
int         tblInum;
TABLEINFO   tblI[100];

#ifndef Private
#define Private
#endif

/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_MAIN_DIALOG, (PFDialogHook)dialog_hook    },
   { HOOKID_TOGGLE_IDX,	 (PFDialogHook)toggle_idx_hook},
   { HOOKID_PUSH_IDX,	 (PFDialogHook)push_idx_hook  },
   { HOOKID_PUSH_TABLE,	 (PFDialogHook)push_table_hook},
   { HOOKID_PUSH_OK,	 (PFDialogHook)push_ok_hook   },
   { HOOKID_PUSH_END,	 (PFDialogHook)push_end_hook  },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int	    read_table( WChar *tblName);
Private void	scan_ele( void);

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
      mdlOutput_error( L"LINE3TO4�̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 -----------*/
   mdlDialog_hookPublish( sizeof( uHooks)/ sizeof( DialogHookInfo), uHooks);

   /*--- C����`�����ݒ� -------------*/
   sprintf(structName, "LINE3TO4_INFO");
   sprintf(variableName, "line3to4");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable(setP, structName, variableName, &line3to4);

   /*--- �e�[�u���t�@�C����ǂݍ��� -------------*/
   mdlFile_find( &full, L"line3to4.tbl", L"MS_DEF", NULL);
   if ( ( fp = _wfopen(full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &line3to4.tog_idx);
      fwscanf( fp, L"%ls\n", line3to4.idxName);
      fwscanf( fp, L"%ls\n", line3to4.tblName);
	  fclose(fp);
   }
   else{
      line3to4.tog_idx    = 0;
	  swprintf(line3to4.idxName, L"c:\\\0");
	  swprintf(line3to4.tblName, L"c:\\\0");
   }


   /*--- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
   db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
   if ( db_p == NULL){
      mdlOutput_error( L"LINE3TO4 �_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �P�t�@�C���̏ꍇ ---*/
   if ( line3to4.tog_idx != -1){
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
	 	 mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD,
                                   mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
         dmP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�g�O���{�^���iIDX�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	toggle_idx_hook( DialogItemMessage *dimP)
{


   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �P�t�@�C���̏ꍇ ---*/
         if ( line3to4.tog_idx != -1){
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


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���iIDX�j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_idx_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.idx");
         swprintf( title,  L"�e�[�u���t�@�C����I��");

         mdlFile_parseName( line3to4.idxName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf( line3to4.idxName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�e�[�u���j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName		filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         swprintf( filter, L"*.tbl");
         swprintf( title,  L"�e�[�u���t�@�C����I��");

         mdlFile_parseName( line3to4.tblName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);

	     mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(line3to4.tblName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	�e�[�u���t�@�C���̓ǂݍ���
-----------------------------------------------------------------------*/
Private  int	read_table( WChar *tblName)
{
FILE	*fp_tbl;
WChar	s_buf[512], info[256];

   /*--- ���C���e�[�u���t�@�C�����J�� -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"���C���e�[�u���t�@�C�� %ls ���ǂ߂܂��� ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      tblInum = 0;
      while( 1){
         fgetws( s_buf, 511, fp_tbl);
         if ( feof( fp_tbl) || ferror( fp_tbl)){ break;}
         if ( 0 == wcsncmp( s_buf, L";", 1)){  continue;}
         if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

         swscanf( s_buf, L"%d %d %d %d"
                      , &tblI[tblInum].lv, &tblI[tblInum].co, &tblI[tblInum].st, &tblI[tblInum].wt);
         tblInum++;
      }
   }
   fclose( fp_tbl);

   return( SUCCESS);
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
         /*--- �ݒ�l�ۑ� ---*/
         mdlFile_find( &full, L"line3to4.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full.GetWCharCP(), L"w");
            fwprintf( fp, L"%d\n", line3to4.tog_idx);
            fwprintf( fp, L"%ls\n", line3to4.idxName);
            fwprintf( fp, L"%ls\n", line3to4.tblName);
         fclose( fp);

         /*--- �e�[�u���t�@�C����ǂݍ��� ---*/
         if ( read_table( line3to4.tblName) == ERROR){
            break;
         }

         /*--- �P�t�@�C���̏ꍇ ---*/
         if ( line3to4.tog_idx != -1){
            scan_ele( );
         }

         /*--- �o�b�`�̏ꍇ ---*/
         else{
            /*----- ���̓t�@�C�����X�g���J�� -----*/
	        if ( NULL == ( fp = _wfopen( line3to4.idxName, L"r"))){
		       swprintf( info, L"idx�t�@�C�� %ls ���ǂ߂܂���", line3to4.idxName);
               mdlDialog_openInfoBox( info);
		       break;
            }

            /*----- ���̓t�@�C�����X�g��ǂ� -----*/
            while( 1){
		       fgetws( s_buf, 512, fp);
               if ( feof( fp) || ferror( fp)){ break;}

               if ( 0 == wcsncmp( s_buf, L";", 1)){ continue;}
               tok = wcstok( s_buf, L"\n");

               /*----- dgn���J�� -----*/
               if ( mdlSystem_newDesignFile( s_buf) == SUCCESS){
                  scan_ele( );
               }
            }
            fclose( fp);
         }
         mdlDialog_openInfoBox( L"�I�����܂���");
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�����̌���
-----------------------------------------------------------------------*/
Private  void	scan_ele( )
{
ExtScanList	scanlist;
MSElement	oele, nele;
ULong		eofpos, filepos, elemAddr[256];
int			ii, jj, st, lnv, scanwords, status, numAddr;
UInt        lv, co, wt;
Dpoint3d	lPoi[101];

   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE;

   /*--- �X�L�����J�n -----*/
   mdlScan_initialize( MASTERFILE, &scanlist);
   filepos   = 0L;
   scanwords = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElement_read( &oele, MASTERFILE, elemAddr[ii]);
         get_layer( &lv, &co, &st, &wt, NULL, &oele);

         for( jj=0; jj<tblInum; jj++){
            if ( ( lv == tblI[jj].lv || tblI[jj].lv == -1)
              && ( co == tblI[jj].co || tblI[jj].co == -1)
              && ( st == tblI[jj].st || tblI[jj].st == -1)
              && ( wt == tblI[jj].wt || tblI[jj].wt == -1)){
               mdlLinear_extract( lPoi, &lnv, &oele, MASTERFILE);
               mdlLineString_create( &nele, &oele, lPoi, lnv);

               mdlElement_rewrite( &nele, &oele, elemAddr[ii]);
               break;
            }
         }
      }
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
         mdlDialog_cmdNumberQueue( FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID( ), INPUTQ_EOQ);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}
