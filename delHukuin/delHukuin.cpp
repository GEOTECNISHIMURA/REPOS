#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*----------------------------------------------------------------------*/
/*-		DELHUKUIN.MC						*/
/*----------------------------------------------------------------------*/
#include <stdio.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <RmgrTools\Tools\msstrlst.h>
#include <string.h>
#include <math.h>

#include <time.h>
#include "../CommonCE/naka8.cpp"
#include "../CommonCE/check_kigen8.cpp"
#include "delHukuin.h"

#define HUKUIN  1
#define KUKAN   2

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE		*fp;
MSDialog	*db_p;
BeFileName  full;
int			exitStatus;
DELHUKUIN  	delHukuin;
/*
typedef struct {
	UInt32		lv, co, wt;
	int         st, gg;
} ELMSYM;
*/
#ifndef Private
#define Private
#endif
/*-    �t�b�N�֐�   -*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	toggle_idx_hook( DialogItemMessage *dimP);
Private void	push_idx_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[] = {
   { HOOKID_DELHUKUIN_DIALOG, (PFDialogHook)dialog_hook    },
   { HOOKID_TOGGLE_IDX,       (PFDialogHook)toggle_idx_hook},
   { HOOKID_PUSH_IDX,         (PFDialogHook)push_idx_hook  },
   { HOOKID_PUSH_OK,          (PFDialogHook)push_ok_hook   },
   { HOOKID_PUSH_END,         (PFDialogHook)push_end_hook  },
};

Private bool    unload(UnloadProgramReason unloadType);
Private int 	syori( void);
Private	int 	scan_ele( int *num, ULong *Pos, int tp);
int elmSymbChk(MSElement *el, void *param, int operation, UInt32 offset, MSElementDescr *eld);


/*-----------------------------------------------------------------------
**	MAIN
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
//#pragma			resourceID	DELHUKUIN_ID
RscFileHandle	Rsc;
SymbolSet		*setP;
char            structName[128];
char            variableName[128];
/*
#if LOCK == 1
	main_lok( );
#endif
*/
	chk_kigen( );


   /*--- ���\�[�X�t�@�C�����J���� ---*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
      mdlDialog_openInfoBox( L"DELHUKUIN �̃��\�[�X�t�@�C�����J���܂���");
      mdlSystem_exit( exitStatus, 1);
   }

   /*--- �A�����[�h�֐��̃Z�b�g ---*/
   SystemCallback::SetUnloadProgramFunction(unload);
   mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

   /*--- �t�b�N�֐��̎g�p�錾 ---*/
   mdlDialog_hookPublish( sizeof( uHooks)/sizeof( DialogHookInfo), uHooks);

   /*--- C����`�������ݒ� ---*/
   sprintf(structName, "DELHUKUIN_INFO");
   sprintf(variableName, "delHukuin");
   setP = mdlCExpression_initializeSet( VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
   mdlDialog_publishComplexVariable( setP, structName, variableName, &delHukuin);

   /*--- �e�[�u���t�@�C���I�[�v�� -----*/
   mdlFile_find( &full, L"delHukuin.tbl", L"MS_DEF", NULL);
   if ((fp = _wfopen( full.GetWCharCP(), L"r")) != NULL){
      fwscanf( fp, L"%d\n", &delHukuin.tog_idx);
      fwscanf( fp, L"%ls\n", delHukuin.idxName);
      fwscanf( fp, L"%d\n", &delHukuin.hu_lv);
      fwscanf( fp, L"%d\n", &delHukuin.hu_co);
      fwscanf( fp, L"%d\n", &delHukuin.hu_st);
      fwscanf( fp, L"%d\n", &delHukuin.hu_wt);
      fwscanf( fp, L"%d\n", &delHukuin.ku_lv);
      fwscanf( fp, L"%d\n", &delHukuin.ku_co);
      fwscanf( fp, L"%d\n", &delHukuin.ku_st);
      fwscanf( fp, L"%d\n", &delHukuin.ku_wt);
      fwscanf( fp, L"%lf\n", &delHukuin.kyoyouti);
	  fclose(fp);
   }
   else{
      delHukuin.tog_idx    = 0;
	  swprintf( delHukuin.idxName, L"c:\\\0");
      delHukuin.hu_lv      =  55;
      delHukuin.hu_co      =  15;
      delHukuin.hu_st      =   0;
      delHukuin.hu_wt      =   1;
      delHukuin.ku_lv      =  52;
      delHukuin.ku_co      = 197;
      delHukuin.ku_st      =   0;
      delHukuin.ku_wt      =   3;
      delHukuin.kyoyouti   = 0.01;

   }
   

   /*--- �_�C�A���O�{�b�N�X�I�[�v�� ---*/
   db_p = mdlDialog_open( NULL, DIALOGID_DELHUKUIN);
   if ( db_p == NULL){
      mdlDialog_openInfoBox( L"DELHUKUIN �_�C�A���O�{�b�N�X���J���܂���");
   }

   /*--- �P�t�@�C���̏ꍇ ---*/
   
   if (delHukuin.tog_idx != -1) {
	   mdlDialog_itemSetEnabledState(db_p, 1, FALSE, FALSE);
	   mdlDialog_itemSetEnabledState(db_p, 2, FALSE, FALSE);
   }
   else
   {
	   mdlDialog_itemSetEnabledState(db_p, 1, TRUE, FALSE);
	   mdlDialog_itemSetEnabledState(db_p, 2, TRUE, FALSE);
   }

}


/*------------------------------------------------------------------------
**	�A�����[�h�֐�
------------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
{
   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
------------------------------------------------------------------------*/
Private void   dialog_hook( DialogMessage *dmP)
{
DialogBoxRsc	*dbr_P;

   dmP->msgUnderstood = TRUE;
   switch( dmP->messageType){
      case DIALOG_MESSAGE_CREATE:
         dbr_P = dmP->u.create.dialogBoxRP;
         dmP->u.create.interests.updates	   = TRUE;
         dmP->u.create.interests.mouses		   = TRUE;
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
          if ( delHukuin.tog_idx != -1){
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
BeFileName	filePath;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
          swprintf( filter, L"*.idx");
          swprintf( title,  L"idx�t�@�C����I��");

          mdlFile_parseName( delHukuin.idxName, devP, dirP, NULL, NULL);
          swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
		  filePath.SetName( delHukuin.idxName);
	      mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		  swprintf(delHukuin.idxName, L"%ls", filePath.GetWCharCP());
          mdlDialog_itemsSynch( db_p);
          break;

      default:
          dimP->msgUnderstood = FALSE;
          break;
   }
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void   push_ok_hook( DialogItemMessage *dimP)
{
WChar    info[512], s_buf[512], *tok;

   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
		  /*--------- �e�[�u���t�@�C���ɏ������� -------------*/
		  mdlFile_find(&full, L"delHukuin.tbl", L"MS_DEF", NULL);
		  if ((fp = _wfopen(full.GetWCharCP(), L"w")) != NULL) {
			  fwprintf(fp, L"%d\n", delHukuin.tog_idx);
			  fwprintf(fp, L"%ls\n", delHukuin.idxName);
			  fwprintf(fp, L"%d\n", delHukuin.hu_lv);
			  fwprintf(fp, L"%d\n", delHukuin.hu_co);
			  fwprintf(fp, L"%d\n", delHukuin.hu_st);
			  fwprintf(fp, L"%d\n", delHukuin.hu_wt);
			  fwprintf(fp, L"%d\n", delHukuin.ku_lv);
			  fwprintf(fp, L"%d\n", delHukuin.ku_co);
			  fwprintf(fp, L"%d\n", delHukuin.ku_st);
			  fwprintf(fp, L"%d\n", delHukuin.ku_wt);
			  fwprintf(fp, L"%f\n", delHukuin.kyoyouti);
			  fclose(fp);
		  }
         /*--- �o�b�` ---*/
         if ( delHukuin.tog_idx == -1){
            /*--- ���̓t�@�C�����X�g���J���Ȃ��ꍇ ---*/
            if ( ( fp = _wfopen( delHukuin.idxName, L"r")) == NULL){
               swprintf( info, L"idx�t�@�C�� %ls ���J���܂���", delHukuin.idxName);
               mdlDialog_openInfoBox( info);
            }

            /*----- ���̓t�@�C�����X�g���J�� -----*/
            else{
               while( 1){
	              fgetws( s_buf, 512, fp);
                  if ( feof( fp) || ferror( fp)){ break;}

                  if ( 0 == wcscmp( s_buf, L";")){ continue;}
                  tok = wcstok( s_buf, L"\n");

                  /*----- dgn���J�� -----*/
                  if ( mdlSystem_newDesignFile( s_buf) == SUCCESS){
                     syori( );
                  }
               }
            }
         }

         /*--- 1�t�@�C�� ---*/
         else{
            syori( );
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
Private  int  syori( )
{
MSElement       hukuEle, kukanEle;
double          kyoyouti;
int             huku2, huku3, kukanS, kukanE;
int             ii, jj, del, hukuNum, kukanNum, hukuNv, kukanNv;
ULong           *hukuPos, *kukanPos;
Dpoint3d        hukuPoi[101], kukanPoi[101];

   hukuPos  = (ULong*)malloc( 50000 * sizeof( ULong));
   kukanPos = (ULong*)malloc( 50000 * sizeof( ULong));

   kyoyouti = 1000 * delHukuin.kyoyouti;

   /*--- ���C�����������݂̏ꍇ ---*/
   hukuNum = kukanNum = 0;
   scan_ele( &hukuNum,  hukuPos,  HUKUIN);
   scan_ele( &kukanNum, kukanPos, KUKAN);

   /*--- ���� ---*/
   for( ii=0; ii<hukuNum; ii++){
      mdlElement_read( &hukuEle, MASTERFILE, hukuPos[ii]);
      mdlLinear_extract( hukuPoi, &hukuNv, &hukuEle, MASTERFILE);

      /*--- ��Ԋ��� ---*/
      del = 0;
      for( jj=0; jj<kukanNum; jj++){
         mdlElement_read( &kukanEle, MASTERFILE, kukanPos[jj]);
         mdlLinear_extract( kukanPoi, &kukanNv, &kukanEle, MASTERFILE);

         huku2 = judge_point_nearline( &kukanNv, kukanPoi, &hukuPoi[1], kyoyouti); // ������2�_�ڂ���Ԋ����ォ�ǂ���
         huku3 = judge_point_nearline( &kukanNv, kukanPoi, &hukuPoi[2], kyoyouti); // ������3�_�ڂ���Ԋ����ォ�ǂ���

         /*--- ��������2�_�ڂ�3�_�ڂ���Ԋ�����i�����Ƌ�Ԋ�����v�A���́A��Ԋ��������j ---*/
         if ( huku2 == 1 && huku3 == 1){
            del++;
            break;
         }

         kukanS = judge_point_nearline( &hukuNv, hukuPoi, &kukanPoi[0],         kyoyouti); // ��Ԋ����̎n�_���������ォ�ǂ���
         kukanE = judge_point_nearline( &hukuNv, hukuPoi, &kukanPoi[kukanNv-1], kyoyouti); // ��Ԋ����̏I�_���������ォ�ǂ���

         /*--- ��Ԋ����̎n�_�ƏI�_����������i�����������j ---*/
         if ( kukanS == 1 && kukanE == 1){
            del++;
            break;
         }

         /*--- ��Ԋ����ƕ�����������ďd�Ȃ�i�i���ӏ��ɃI�[�v�����a�̏ꍇ�Ȃǁj ---*/
         /*--- ��Ԋ����n�_����������A������2�_�ڂ���Ԋ����� ---*/
         if ( kukanS == 1 && huku2 == 1){
            if ( get_leng( &kukanPoi[0],         &hukuPoi[1]) > kyoyouti){
               del++;
               break;
            }
         }

         /*--- ��Ԋ����n�_����������A������3�_�ڂ���Ԋ����� ---*/
         if ( kukanS == 1 && huku3 == 1){
            if ( get_leng( &kukanPoi[0],         &hukuPoi[2]) > kyoyouti){
               del++;
               break;
            }
         }

         /*--- ��Ԋ����I�_����������A������2�_�ڂ���Ԋ����� ---*/
         if ( kukanE == 1 && huku2 == 1){
            if ( get_leng( &kukanPoi[kukanNv-1], &hukuPoi[1]) > kyoyouti){
               del++;
               break;
            }
         }

         /*--- ��Ԋ����I�_����������A������3�_�ڂ���Ԋ����� ---*/
         if ( kukanE == 1 && huku3 == 1){
            if ( get_leng( &kukanPoi[kukanNv-1], &hukuPoi[2]) > kyoyouti){
               del++;
               break;
            }
         }
      }

      /*--- �폜�̏ꍇ ---*/
      if ( del == 1){
         mdlElement_undoableDelete( &hukuEle, hukuPos[ii], TRUE);
      }
   }

   free( kukanPos);
   free( hukuPos);

   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�v�f����
------------------------------------------------------------------------*/
Private	int     scan_ele( int *num, ULong *Pos, int tp)
{
ExtScanList		scanlist;
MSElementDescr	*dscr;
ULong			eofpos, filepos, elemAddr[256];
int				scanwords, status, numAddr;
int				ii, sw2;
ELMSYM			tmpElm;

   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos                = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     =  NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING;

   /*--- �X�L�����J�n -----*/
   sw2        = 0;
   filepos    = 0L;
   mdlScan_initialize( MASTERFILE, &scanlist);
   scanwords  = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         if ( elemAddr[ii] >= eofpos){ sw2++;   break;}

         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, elmSymbChk, &tmpElm, ELMD_ELEMENT);

         /*--- ���� -----*/
         if ( tp == HUKUIN){
            if ( tmpElm.lv == delHukuin.hu_lv
              && tmpElm.co == delHukuin.hu_co
              && tmpElm.st == delHukuin.hu_st
              && tmpElm.wt == delHukuin.hu_wt){
               Pos[*num] = elemAddr[ii];
               *num      = *num + 1;
            }
         }

         /*--- ��Ԋ��� -----*/
         if ( tp == KUKAN){
            if ( tmpElm.lv == delHukuin.ku_lv
              && tmpElm.co == delHukuin.ku_co
              && tmpElm.st == delHukuin.ku_st
              && tmpElm.wt == delHukuin.ku_wt){
               Pos[*num] = elemAddr[ii];
               *num      = *num + 1;
            }
         }

         mdlElmdscr_freeAll( &dscr);
      }
      if ( sw2 > 0){ break;}
      if ( status != BUFF_FULL){ break;}
   }
   return( SUCCESS);
}


/*------------------------------------------------------------------------
**	�v�b�V���{�^���i�I���j �t�b�N�֐�
------------------------------------------------------------------------*/
Private  void  push_end_hook( DialogItemMessage *dimP)
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
int elmSymbChk
(
	MSElement		*el,
	void      		*param,
	int				operation,
	UInt32			offset,
	MSElementDescr	*eld
)
{
	ELMSYM *tmpElm = (ELMSYM*)param;
	mdlElement_getProperties(&tmpElm->lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, el);
	mdlElement_getSymbology(&tmpElm->co, &tmpElm->wt, &tmpElm->st, el);

	return(1);
}
