#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*-----------------------------------------------------------------------*/
/*-	        delPointNew.MC				-*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "../CommonCE/naka8.cpp"
//#include <lok.mc> 
#include "../CommonCE/check_kigen8.cpp"
#include "delPointNew.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT


FILE			*fp;
RscFileHandle   Rsc;
MSDialog        *db_p;

ULong           renPos[100];
Dpoint3d        staP, endP;
int				num, exitStatus;
int             renNum, renNv;
Dpoint3d        renPoi[10000],cenPoi[10000];
DELPOINTNEW   	delPointNew;
MSElementDescr  *Tdscr;
UInt32          Tpos,Tflg;
DgnModelRefP	modelRefP, outModelRefP;


/*
typedef struct {
	UInt32		lv, co, wt, gg;
	int         st;
} ELMSYM;
*/
/*-	�e�[�u���ݒ�	-*/
typedef struct {
    int     lv, co, wt, st;
} TABLEINFO;
int         tblInum, cenInum;
TABLEINFO   tblI[100], cenI[100], kuI;

/*-	�ŏ��ɑI���������C��	-*/
typedef struct {
    ULong   Pos;
	UInt32     lv, co, wt;
	int        st;
} LINEINFO;
LINEINFO  linI;

#ifndef Private
#define Private
#endif
/*-	�t�b�N�֐�		-*/
Private void	dialog_hook( DialogMessage *dmP);
Private void	push_table_hook( DialogItemMessage *dimP);
Private void	push_table_kukan_hook( DialogItemMessage *dimP);
Private void	push_ok_hook( DialogItemMessage *dimP);
Private void	push_kukan_hook( DialogItemMessage *dimP);
Private void	push_end_hook( DialogItemMessage *dimP);

Private DialogHookInfo	uHooks[]= {
   { HOOKID_MAIN_DIALOG,     (PFDialogHook)dialog_hook          },
   { HOOKID_PUSH_TABLE,	     (PFDialogHook)push_table_hook      },
   { HOOKID_PUSH_TABLE_KUKAN,(PFDialogHook)push_table_kukan_hook},
   { HOOKID_PUSH_OK,	     (PFDialogHook)push_ok_hook         },
   { HOOKID_PUSH_KUKAN,	     (PFDialogHook)push_kukan_hook      },
   { HOOKID_PUSH_END,	     (PFDialogHook)push_end_hook        },
};

/*-	�v���g�^�C�v	-*/
Private bool    unload(UnloadProgramReason unloadType);
Private int	    read_table( WChar *tblName);
Private int	    read_table_kukan( WChar *tblName);
Private void    startModify( void);
Private	void	get_point( Dpoint3d *pt, int view);
Private void    cleanFunc(void);
Private	int 	syori( void);
Private void	scan_ele( int *num, ULong *Pos, UInt32 lv, UInt32 co, int st, UInt32 wt);
Private void	syori_kukan( void);
Private void	scan_ele_kukan( int *num, ULong *Pos, int type);
Private int     lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber);
Private void	lineString_add_zokusei( MSElement *oele, UInt32 *lv, UInt32 *co, int *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);
Private int     elmSymbChk( MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr *eld);
Private int  	linkage_extract( LinkData *linkageP, LINKSET *linkage);
Private void    create_batu( double angle, Dpoint3d *Poi, DgnDrawMode drawMode);
Private void    create_batu_2(double angle, Dpoint3d *Poi, DgnDrawMode drawMode);
int get_closest_point_a(int *lnv, Dpoint3d *lPoi, Dpoint3d *orig, Dpoint3d *nearPoi, int *segnum, double *leng);
/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	//CExprResult     result;
	//CExprValue      value;
	//#pragma		    resourceID	DELPOINTNEW_ID
	const WChar     REFNM[20] = L"delPointNew_Ref.dgn";
	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];

	DgnModelP           modelP;
	ModelRefIteratorP	iterator;
	DgnFileP            fileP;
	WChar               sBuf[512];
	BeFileName          full;
	int                 cnta;
	WString             cfgVal;
	Int32               ret;
	/*
	#if LOCK == 1
		main_lok( );
	#endif
	*/
	chk_kigen();



	/*--- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"DELPOINTNEW �̃��\�[�X�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
	}
	/*--- �A�����[�h�֐��Z�b�g ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	/*--- �t�b�N�֐��̎g�p�錾 -----------*/
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	/*--- C����`�����ݒ� -------------*/
	sprintf(structName, "DELPOINTNEW_INFO");
	sprintf(variableName, "delPointNew");
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName, &delPointNew);

	/*--- �e�[�u���t�@�C����ǂݍ��� -------------*/
	mdlFile_find(&full, L"delPointNew.tbl", L"MS_DEF", NULL);
	if ((fp = _wfopen(full.GetWCharCP(), L"r")) != NULL) {
		fwscanf(fp, L"%ls\n", delPointNew.tblName);
		fwscanf(fp, L"%ls\n", delPointNew.kukanName);
		fwscanf(fp, L"%lf\n", &delPointNew.rr);
		fclose(fp);
	}
	else {
		swprintf(delPointNew.tblName, L"c:\\\0");
	}

    mdlFile_find(&full, REFNM, L"MS_DGNOUT", NULL);
	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		mdlRefFile_getStringParameters(sBuf, sizeof(sBuf), REFERENCE_ATTACHNAME, modelRefP);

		if (wmemcmp(sBuf, REFNM, wcslen(REFNM)) == 0) {
			mdlRefFile_detach(modelRefP);
			break;
		}
	}

	ret = mdlFile_find(&full, REFNM, L"MS_DGNOUT", NULL);
	swprintf(sBuf,L"%ls", full.GetWCharCP());
	if (ret == 0) {
		mdlFile_delete(full.GetWCharCP());
	}
	if (mdlFile_find(&full, L"2D Metric Design.dgn", L"MS_SEEDFILES", NULL) == SUCCESS) {

		mdlFile_copy(sBuf, full.GetWCharCP());
	}
	else {
		mdlOutput_error(L"seed �t�@�C��������܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
	}
	//���̕ӂ���x�����Ă����Ȃ��ƂȂ����ŏ��̃A�^�b�`�̎��x���Ȃ�
	DgnPlatform::DgnFileFormatType  pFormat;
	bool            pThreeD;
	mdlWorkDgn_openFile(&modelRefP, &pFormat, &pThreeD, full.GetWCharCP(), NULL, false);
	mdlWorkDgn_saveChanges(modelRefP);
	mdlRefFile_attach(&outModelRefP, full.GetWCharCP(), NULL, NULL, NULL, NULL, 10, NULL, 0, NULL, REF_FILE_LEVEL_DISPLAY_DEFAULT, TRUE, TRUE);
	mdlRefFile_detach(outModelRefP);
	mdlWorkDgn_closeFile(modelRefP);
/*
	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);

	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		modelRefP = mdlModelRefIterator_getFirst(iterator);
		mdlModelRef_getFileName(modelRefP,sBuf,512);

		modelP = modelRefP->GetDgnModelP();
		if (modelP != NULL) {

			fileP = modelRefP->GetDgnFileP();
			ret = wmemcmp(fileP->GetFileName().GetWCharCP(), full.GetWCharCP(), size_t( wcslen(fileP->GetFileName().GetWCharCP())));
			if (ret==0){
				mdlRefFile_detach(modelRefP);
				break;
			}
		}
	}
*/   
	/*--- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
	db_p = mdlDialog_open( NULL, DIALOGID_MAIN);
	if ( db_p == NULL){
		mdlOutput_error( L"DELPOINTNEW �_�C�A���O�{�b�N�X���J���܂���");
	}

	Tpos = NULL;

}


/*-----------------------------------------------------------------------
**	�A�����[�h�֐� 
-----------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
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
**	�v�b�V���{�^���i�e�[�u���j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_table_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
		 swprintf( filter, L"*.tbl");
		 swprintf( title,  L"�e�[�u���t�@�C����I��");

         mdlFile_parseName( delPointNew.tblName, devP, dirP, NULL, NULL);
		 swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
		 filePath.SetName( delPointNew.tblName);
	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(delPointNew.tblName, L"%ls", filePath.GetWCharCP());
         mdlDialog_itemsSynch( db_p);
         break;

      default:
	 	 dimP->msgUnderstood = FALSE;
	 	 break;
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i��ԃe�[�u���j�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_table_kukan_hook( DialogItemMessage *dimP)
{
WChar	filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName	filePath;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
		 swprintf(filter, L"*.tbl");
		 swprintf(title, L"�e�[�u���t�@�C����I��");

         mdlFile_parseName( delPointNew.kukanName, devP, dirP, NULL, NULL);
         swprintf( defaultDirec, L"%ls:%ls", devP, dirP);
		 filePath.SetName( delPointNew.kukanName);
	     mdlDialog_fileOpen( filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		 swprintf(delPointNew.kukanName, L"%ls", filePath.GetWCharCP());
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
WChar	s_buf[256], info[256];


   /*--- ���C���e�[�u���t�@�C�����J�� -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"���C���e�[�u���t�@�C�� %s ���ǂ߂܂��� ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      tblInum = 0;
      while( 1){
         fgetws( s_buf, 255, fp_tbl);
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
**	�e�[�u���t�@�C���̓ǂݍ���
-----------------------------------------------------------------------*/
Private  int	read_table_kukan( WChar *tblName)
{
FILE	*fp_tbl;
WChar	s_buf[256], info[256];
int     cnt;

   /*--- ���C���e�[�u���t�@�C�����J�� -----*/
   if ( NULL == ( fp_tbl = _wfopen( tblName, L"r"))){
      swprintf( info, L"���C���e�[�u���t�@�C�� %s ���ǂ߂܂��� ", tblName);
      mdlDialog_openInfoBox( info);
      return( ERROR);
   }

   else{
      cnt     = 0;
      cenInum = 0;
      while( 1){
         fgetws( s_buf, 255, fp_tbl);
         if ( feof( fp_tbl) || ferror( fp_tbl)){ break;}
         if ( 0 == wcsncmp( s_buf, L";", 1)){  continue;}
         if ( 0 == wcsncmp( s_buf, L"\n", 1)){ continue;}

         if ( cnt == 0){
            swscanf( s_buf, L"%d %d %d %d"
                         , &kuI.lv, &kuI.co, &kuI.st, &kuI.wt);
         }
         else{
            swscanf( s_buf, L"%d %d %d %d"
                         , &cenI[cenInum].lv, &cenI[cenInum].co, &cenI[cenInum].st, &cenI[cenInum].wt);
            cenInum++;
         }
         cnt++;
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
BeFileName      full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �ݒ�l�ۑ� ---*/
         mdlFile_find( &full, L"delPointNew.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%ls\n", delPointNew.tblName);
            fwprintf( fp, L"%ls\n", delPointNew.kukanName);
            fwprintf( fp, L"%lf\n", delPointNew.rr);
         fclose( fp);

         /*--- �e�[�u���t�@�C����ǂݍ��� ---*/
         if ( read_table( delPointNew.tblName) == ERROR){
            break;
         }
		
         num = 0;
         startModify( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*------------------------------------------------------------------------
**	�v�f�I��
------------------------------------------------------------------------*/
Private  void   startModify( void)
{
static	int	searchType[] = { LINE_ELM, LINE_STRING_ELM};

   /*--- MASTERFILE�̐ݒ�v�f�^�C�v�̂ݑI���\ ---*/
   mdlLocate_noElemNoLocked( );
   mdlLocate_setElemSearchMask( sizeof( searchType)/sizeof( int), searchType);

   mdlState_startModifyCommand((StateFunc_Reset)startModify,
	   (MdlFunc_Accept)get_point, NULL, NULL, NULL, NULL, NULL, FALSE, FALSE);

   /*----- �I��v�f�����F���邩���ۂ��邩�𔻒肷�� ------*/
   LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)lineCheck);

   /*----- �ʃR�}���h���s���̗v�f�L�q�q�N���A ------*/
   StateCallback::SetCommandCleanupFunction((StateFunc_CommandCleanup)cleanFunc);

   /*----- �t�@�C���̐擪���猟�� -----*/
   mdlLocate_init( );

   if ( num == 0){
      mdlOutput_error( L"1�_�ڂ��X�i�b�v���Ă�������");

   }
   else{
      mdlOutput_error( L"2�_�ڂ��X�i�b�v���Ă�������");
   }
}


/*----------------------------------------------------------------------+
**	�N���b�N�����_�̍��W�擾
+----------------------------------------------------------------------*/
Private	void	get_point( Dpoint3d *pt, int view)
{
MSElement   ele, u;
int         ii, jj, lNum = 0;
double      angle, angle1, angle2;
ULong       *lPos;
UInt32      co, wt;
int         lc;

DgnPlatform::DgnFileFormatType  pFormat;
bool            pThreeD;
BeFileName      full;
int ret;

   Tflg = 0;
   /*--- 1�_�� ---*/
   if ( num == 0){

      staP.x = statedata.dPointStack[0].x;
      staP.y = statedata.dPointStack[0].y;
      staP.z = 0;
	  
	  mdlCell_create(&u, NULL, NULL, FALSE);
	  ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
	  ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
	  ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);

	  mdlElement_setSymbology(&u, &co, &wt, &lc);
	  mdlElmdscr_new(&Tdscr, NULL, &u);
	  
      lPos = (ULong*)malloc( 50000 * sizeof( ULong));

      /*--- 1�_�ڗv�f�̃��C�� ---*/
      mdlElement_read( &ele, MASTERFILE, linI.Pos);
      get_layer( &linI.lv, &linI.co, &linI.st, &linI.wt, NULL, &ele);

      /*--- �ŏ��ɑI�������v�f�Ɠ����C���̗v�f������ -----*/
      scan_ele( &lNum, lPos, linI.lv, linI.co, linI.st, linI.wt);

      /*--- �ŏ��ɑI�������v�f�ƘA������v�f������ -----*/
      renNum = 1;
      renPos[0] = linI.Pos;
      mdlLinear_extract( renPoi, &renNv, &ele, MASTERFILE);
      get_renzoku( lNum, lPos, &renNv, renPoi, &renNum, renPos);

      /*--- �_�̕\�� ---*/


	  mdlFile_find(&full, L"delPointNew_Ref.dgn", L"MS_DGNOUT", NULL);
	  ret = mdlWorkDgn_openFile( &modelRefP, &pFormat, &pThreeD, full.GetWCharCP(), NULL, false);
//printf("ret1 %d\n", ret);

	  /*--- 1�_�� ---*/
	  angle = atan2( renPoi[1].y - renPoi[0].y, renPoi[1].x - renPoi[0].x);
	  create_batu( angle, &renPoi[0], DRAW_MODE_Normal);

	  /*--- ���ԓ_ ---*/
	  for( jj=1; jj<renNv-1; jj++){
	     angle1 = atan2( renPoi[jj+1].y - renPoi[jj].y,   renPoi[jj+1].x - renPoi[jj].x);
		 angle2 = atan2( renPoi[jj].y   - renPoi[jj-1].y, renPoi[jj].x   - renPoi[jj-1].x);
		 angle  = ( angle1 + angle2)/2;
		 create_batu( angle, &renPoi[jj], DRAW_MODE_Normal);
	  }

	  /*--- �Ō�̓_ ---*/
	  angle = atan2( renPoi[renNv-1].y - renPoi[renNv-2].y, renPoi[renNv-1].x - renPoi[renNv-2].x);
	  create_batu( angle, &renPoi[renNv-1], DRAW_MODE_Normal);
      
      num++;

	  Tpos = mdlWorkDgn_write(Tdscr, -1, modelRefP);
//printf("%d\n", mdlErrno);
	  mdlWorkDgn_saveChanges(modelRefP);
      mdlRefFile_attach( &outModelRefP,full.GetWCharCP(),NULL,NULL,NULL,NULL,10,NULL,0,NULL,REF_FILE_LEVEL_DISPLAY_DEFAULT,TRUE,TRUE);
	  mdlLevel_setDisplayPriority(MASTERFILE, linI.lv,-1);
      free( lPos);
      startModify( );
	  Tflg = 1;
	  
   }

   /*--- 2�_�� ---*/
   else{

      endP.x = statedata.dPointStack[0].x;
      endP.y = statedata.dPointStack[0].y;
      endP.z = 0;

      /*--- �_�̍폜���� ---*/
      syori( );

	  Tflg = 1;
	  cleanFunc();
	  Tflg = 0;

      num--;
      startModify( );
   }
}


/*------------------------------------------------------------------------
**	syori
------------------------------------------------------------------------*/
Private  int   syori( )
{
LINKSET		linkage0, linkage;
MSElement   ele, oele;
int         newNv, rosen0, rosen;
int         siti, eiti, ii;
double      sleng, eleng;
Dpoint3d    *newPoi, nearPoi;

   newPoi = (Dpoint3d*)malloc( 10000 * sizeof( Dpoint3d));

   /*--- �ŏ��ɑI�������v�f -----*/
   mdlElement_read( &ele, MASTERFILE, linI.Pos);

   /*--- 2�_�ڂ̈ʒu -----*/
   if (get_closest_point_a(&renNv, renPoi, &endP, &nearPoi, &eiti, &eleng) == ERROR) {
	   free(newPoi);
	   mdlDialog_openInfoBox(L"1�_�ڂ�2�_�ڂ��Ȃ����Ă��܂���");
	   return(ERROR);
   }
   
   if ( get_leng( &endP, &nearPoi) >= 1){
      mdlDialog_openInfoBox( L"1�_�ڂ�2�_�ڂ��Ȃ����Ă��܂���");
   }

   else{
      /*--- 1�_�ڂ̈ʒu -----*/
      if (get_closest_point_a(&renNv, renPoi, &staP, &nearPoi, &siti, &sleng) == ERROR) {
		  free(newPoi);
		  return(ERROR);
	  }

      /*--- �n�_����S �I�_����E�Ƃ��� -----*/
      if ( siti > eiti || ( siti == eiti && sleng > eleng)){
         exchangei( &siti,  &eiti);
         exchanged( &sleng, &eleng);
         exchanged3( &staP, &endP);
      }

      /*--- �L�[�|�C���g�ŉ������Ă���ꍇ -----*/
      if ( get_leng( &staP, &renPoi[siti+1]) == 0){
         siti++;
      }

      /*--- 1�_�ڂ�2�_�ڂ̊Ԃ̓_���Ȃ� -----*/
      /*--- 1�_�ڈȑO�̓_ -----*/
      for( ii=0; ii<=siti; ii++){
         memcpy( &newPoi[ii], &renPoi[ii], sizeof( Dpoint3d));
      }

      /*--- 2�_�ڈȍ~�̓_ -----*/
      for( ii=eiti+1; ii<renNv; ii++){
         memcpy( &newPoi[ii+siti-eiti], &renPoi[ii], sizeof( Dpoint3d));
      }
      newNv = renNv + siti - eiti;

      /*--- ���v�f�̃��[�U�f�[�^�����N�i�H���ԍ��j -----*/
      mdlElement_read( &oele, MASTERFILE, renPos[0]);
      mdlLinkage_extractFromElement( NULL, &oele, 16, RSCID_DataDef_Link, NULL
                                   , (LinkageFunc)linkage_extract, &linkage0);
      rosen0 = atoi( linkage0.rosen);

      for( ii=1; ii<renNum; ii++){
         mdlElement_read( &oele, MASTERFILE, renPos[ii]);
         mdlLinkage_extractFromElement( NULL, &oele, 16, RSCID_DataDef_Link, NULL
                                      , (LinkageFunc)linkage_extract, &linkage);
         rosen = atoi( linkage.rosen);

         /*--- �H���ԍ�����v���Ȃ��ꍇ -----*/
         if ( rosen != rosen0){
            mdlDialog_openInfoBox( L"�H���ԍ���������v���܂���");
            free( newPoi);
            return( ERROR);
         }
      }

      /*--- ���v�f���폜 -----*/
      for( ii=0; ii<renNum; ii++){
         mdlElement_read( &oele, MASTERFILE, renPos[ii]);
         mdlElement_undoableDelete( &oele, renPos[ii], TRUE);
      }
	  	  
      /*--- �_���Ԉ����Ĕz�u -----*/
      lineString_add_zokusei( &ele, &linI.lv, &linI.co, &linI.st, &linI.wt, NULL, newPoi, &newNv);
   }

  
   free( newPoi);
   return( SUCCESS);
}


/*-----------------------------------------------------------------------
**	�����̌���
-----------------------------------------------------------------------*/
Private  void	scan_ele( int *numidx, ULong *Pos, UInt32 lv, UInt32 co, int st, UInt32 wt)
{
ExtScanList	    scanlist;
MSElementDescr	*dscr;
ULong		    eofpos, filepos, elemAddr[256];
int			    ii, scanwords, status, numAddr;
ELMSYM          tmpElm;

   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     = NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING;

   /*--- �X�L�����J�n -----*/
   mdlScan_initialize(MASTERFILE, &scanlist);
   filepos    = 0L;
   scanwords  = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

         /*--- ���C������v����ꍇ -----*/
         if ( tmpElm.lv == lv && tmpElm.co == co
           && tmpElm.st == st && tmpElm.wt == wt){
            Pos[*numidx] = elemAddr[ii];
            *numidx      = *numidx + 1;
         }
         mdlElmdscr_freeAll( &dscr);
      }
      if ( status != BUFF_FULL){ break;}
   }
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i��ԁj�t�b�N�֐�
-----------------------------------------------------------------------*/
Private void	push_kukan_hook( DialogItemMessage *dimP)
{
BeFileName      full;
   dimP->msgUnderstood = TRUE;
   switch( dimP->messageType){
      case DITEM_MESSAGE_BUTTON:
         /*--- �ݒ�l�ۑ� ---*/
		 mdlFile_find(&full, L"kukan.tbl", L"MS_DEF", NULL);
         fp = _wfopen( full, L"w");
            fwprintf( fp, L"%ls\n", delPointNew.tblName);
            fwprintf( fp, L"%ls\n", delPointNew.kukanName);
            fwprintf( fp, L"%lf\n", delPointNew.rr);
         fclose( fp);

         /*--- �e�[�u���t�@�C����ǂݍ��� ---*/
         if ( read_table_kukan( delPointNew.kukanName) == ERROR){
            break;
         }

         syori_kukan( );
         break;

      default:
         dimP->msgUnderstood = FALSE;
         break;
   }
}


/*-----------------------------------------------------------------------
**	�����i��ԁj
-----------------------------------------------------------------------*/
Private void	syori_kukan( )
{
MSElementDescrP  kuDscr, cenDscr, oldDscr;
//MSElementDescrCP  oldDscr;
MSElement       ele;
int             ii, jj, kk, ll, index, kuNum, cenNum, cenNv, delNum;
ULong           *kuPos, *cenPos;
Dpoint3d        *cenPoi1, isPnt1, isPnt2;

   kuPos  = (ULong*)malloc( 10000 * sizeof( ULong));
   cenPos = (ULong*)malloc( 10000 * sizeof( ULong));
   

   /*--- ��Ԋ����E���S������ -----*/
   kuNum = cenNum = 0;
   scan_ele_kukan( &kuNum, kuPos, 1);
   scan_ele_kukan( &cenNum, cenPos, 2);

   /*--- ���S�� ---*/
   for( ii=0; ii<cenNum; ii++){
      mdlElmdscr_read( &cenDscr, cenPos[ii], MASTERFILE, 0, NULL);
      mdlElmdscr_duplicate( &oldDscr, cenDscr);
      mdlElement_read( &ele, MASTERFILE, cenPos[ii]);
      mdlElmdscr_stroke( &cenPoi1, &cenNv, cenDscr, 0.001);
	  if ( cenNv != 0) {
		  for (jj = 0; jj < cenNv; jj++) {
			  memcpy(&cenPoi[jj], &cenPoi1[jj], sizeof(Dpoint3d));
		  }
		  //free(cenPoi1);
		  dlmSystem_mdlFree(cenPoi1);
	  }

      /*--- ��Ԋ��� ---*/
      delNum = 0;
      for( jj=0; jj<kuNum; jj++){
         mdlElmdscr_read( &kuDscr, kuPos[jj], MASTERFILE, 0, NULL);

         if ( mdlIntersect_closestBetweenElms( &isPnt1, &isPnt2, &index, cenDscr, kuDscr, NULL, &cenPoi[0], 1) == SUCCESS){
            /*--- ���S���̊e�_ ---*/
            for( kk=1; kk<cenNv-1; kk++){
               if ( get_leng( &isPnt1, &cenPoi[kk]) < 5){
                  mdlElmdscr_freeAll( &cenDscr);
                  for( ll=kk; ll<cenNv-1; ll++){
                     memcpy( &cenPoi[ll], &cenPoi[ll+1], sizeof( Dpoint3d));
                  }
                  delNum++;
                  cenNv--;
                  mdlElmdscr_createFromVertices( &cenDscr, &ele, cenPoi, cenNv, FALSE, 0);
                  break;
               }
            }
         }

         mdlElmdscr_freeAll( &kuDscr);
      }
      if ( delNum > 0){
         mdlElmdscr_rewrite( cenDscr, oldDscr, cenPos[ii]);
      }

	  //free(cenPoi1);
      mdlElmdscr_freeAll( &oldDscr);
      mdlElmdscr_freeAll( &cenDscr);
   }
   free( cenPos);
   free( kuPos);
}


/*-----------------------------------------------------------------------
**	�����̌���
-----------------------------------------------------------------------*/
Private  void	scan_ele_kukan( int *numidx, ULong *Pos, int type)
{
ExtScanList	    scanlist;
MSElementDescr	*dscr;
ULong		    eofpos, filepos, elemAddr[256];
int			    ii, jj, scanwords, status, numAddr;
ELMSYM          tmpElm;

   /*--- �X�L�����ݒ� -----*/
   mdlScan_initScanlist( &scanlist);
   mdlScan_noRangeCheck( &scanlist);

   eofpos = mdlElement_getFilePos( FILEPOS_EOF, NULL);
   mdlScan_setDrawnElements( &scanlist);		/* �\���\�v�f�w��	*/
   scanlist.scantype     = NESTCELL | PROPCLAS | ELEMTYPE;
   scanlist.extendedType = FILEPOS;
   scanlist.clasmsk      = 0xFFFF;

   for( ii=0; ii<8; ii++) scanlist.typmask[ii] = 0x0000;
   scanlist.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING;

   /*--- �X�L�����J�n -----*/
   mdlScan_initialize(MASTERFILE, &scanlist);
   filepos    = 0L;
   scanwords  = sizeof( elemAddr)/sizeof( short);
   while( 1){
      status  = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filepos);
      numAddr = scanwords/sizeof( short);

      for( ii=0; ii<numAddr; ii++){
         mdlElmdscr_read( &dscr, elemAddr[ii], MASTERFILE, FALSE, NULL);
         mdlElmdscr_operation( dscr, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

         /*--- ��Ԋ��� -----*/
         if ( type == 1){
            /*--- ���C������v����ꍇ -----*/
            if ( tmpElm.lv == kuI.lv && tmpElm.co == kuI.co
              && tmpElm.st == kuI.st && tmpElm.wt == kuI.wt){
               Pos[*numidx] = elemAddr[ii];
               *numidx      = *numidx + 1;
            }
         }

         /*--- ���S�� -----*/
         else{
            for( jj=0; jj< cenInum; jj++){
               /*--- ���C������v����ꍇ -----*/
               if ( tmpElm.lv == cenI[jj].lv && tmpElm.co == cenI[jj].co
                 && tmpElm.st == cenI[jj].st && tmpElm.wt == cenI[jj].wt){
                  Pos[*numidx] = elemAddr[ii];
                  *numidx      = *numidx + 1;
               }
            }
         }
         mdlElmdscr_freeAll( &dscr);
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


/*----------------------------------------------------------------------+
| 	�w���v�f��̧���߼޼�݁A�n�_�ʒu���擾				|
+----------------------------------------------------------------------*/
Private int lineCheck( int preLocate, MSElement *elementP, int fileNumber, unsigned long filePosition, Point3d *pointP, int viewNumber)
{
int            rtn = LOCATE_ELEMENT_REJECT, sw, ii;
UInt32          readFilePos;
MSElementDescr *edP = NULL;
ELMSYM         tmpElm;

   /*--- �I�������v�f��ǂ� -----*/
   mdlElmdscr_read( &edP, filePosition, MASTERFILE, FALSE, &readFilePos);

   /*--- �I�������v�f�̼���ۼނ�ǂ� -----*/
   mdlElmdscr_operation( edP, (PFElemOperation)elmSymbChk, &tmpElm, ELMD_ELEMENT);

   sw = 0;

   /*--- 1�_�� -----*/
   if ( num == 0){
      /*--- �e�[�u�� -----*/
      for( ii=0; ii<tblInum; ii++){
         /*--- ���C������v����ꍇ -----*/
         if ( ( tblI[ii].lv == -1 || tmpElm.lv == tblI[ii].lv)
           && ( tblI[ii].co == -1 || tmpElm.co == tblI[ii].co)
           && ( tblI[ii].st == -1 || tmpElm.st == tblI[ii].st)
           && ( tblI[ii].wt == -1 || tmpElm.wt == tblI[ii].wt)){
            sw++;
            break;
         }
      }
   }

   /*--- 2�_�� -----*/
   else{
      if ( tmpElm.lv == linI.lv && tmpElm.co == linI.co
        && tmpElm.st == linI.st && tmpElm.wt == linI.wt){
         sw++;
      }
   }

   /*--- �I�������v�f�̼���ۼނ�ð��قƈ�v���Ȃ��ꍇ�i���ہj -----*/
   if ( !sw){
      mdlElmdscr_freeAll( &edP);

      /*--- 1�_�� -----*/
      if ( num == 0){
         mdlDialog_openInfoBox( L"�ݒ背�C���̃��C����I��ŉ�����");
      }

      /*--- 2�_�� -----*/
      else{
         mdlDialog_openInfoBox( L"1�_�ڂƓ����C���̃��C����I��ŉ�����");
      }
      return( rtn);
   }

   /*--- �I�������v�f��̧���߼޼�� -----*/
   if ( num == 0){
      linI.Pos = filePosition;
   }

   if ( NULL != edP)    mdlElmdscr_freeAll( &edP);

   /*--- ���F -----*/
   rtn = LOCATE_ELEMENT_ACCEPT;
   return( rtn);
}


/*------------------------------------------------------------------------------------------------*/
/*--- �������C���X�g�����O�̔z�u�i������ێ������܂܁j -----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
Private void	lineString_add_zokusei( MSElement *oele, UInt32 *lv, UInt32 *co, int *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv)
{
MSElement	ele;
int		    ii, lnv;

   lnv  = *nv;
   while( 1){
      if ( lnv > 80){
         mdlLineString_create( &ele, oele, lPoi, 80);
         mdlElement_setProperties( &ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &ele, co, wt, lc);
         mdlElement_add( &ele);
         mdlElement_display( &ele, DgnDrawMode::DRAW_MODE_Normal);

         for( ii=0; ii<lnv-79; ii++){
            memcpy( &lPoi[ii], &lPoi[ii+79], sizeof( Dpoint3d));
         }
         lnv = lnv - 79;
      }
      else{
         mdlLineString_create( &ele, oele, lPoi, lnv);
         mdlElement_setProperties( &ele, lv, ggnum, NULL, NULL, NULL, NULL, NULL, NULL);
         mdlElement_setSymbology( &ele, co, wt, lc);
         mdlElement_add( &ele);
         mdlElement_display( &ele, DgnDrawMode::DRAW_MODE_Normal);
         break;
      }
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


/*-----------------------------------------------------------------------
**	UDLINK��ǂ�
-----------------------------------------------------------------------*/
Private int  	linkage_extract( LinkData  *linkageP, LINKSET *linkage)
{
   /*--- �H���ԍ� ---*/
   if ( linkageP->linkage.rosen != 0){
      strcpy( linkage->rosen, linkageP->linkage.rosen);
   }

   /*--- ��Ԕԍ� ---*/
   if ( linkageP->linkage.kukan != 0){
      linkage->kukan = linkageP->linkage.kukan;
   }

   /*--- ������ ---*/
   if ( linkageP->linkage.lleng != 0){
      linkage->lleng = linkageP->linkage.lleng;
   }

   /*--- �E���� ---*/
   if ( linkageP->linkage.rleng != 0){
      linkage->rleng = linkageP->linkage.rleng;
   }

   /*--- ���� ---*/
   if ( linkageP->linkage.huku != 0){
      linkage->huku = linkageP->linkage.huku;
   }

   /*--- ���̑����� ---*/
   if ( linkageP->linkage.hoka != 0){
      linkage->hoka = linkageP->linkage.hoka;
   }

   /*--- ���a�����H�ɕt���Ă��邩�A�����ɕt���Ă��邩 ---*/
   if ( linkageP->linkage.inout != 0){
      linkage->inout = linkageP->linkage.inout;
   }

   return( SUCCESS);
}


/*----------------------------------------------------------------------+
| 	�~��̔���				|
+----------------------------------------------------------------------*/
Private void    create_batu( double angle, Dpoint3d *Poi, DgnDrawMode drawMode)
{
MSElement   ele;
MSElementDescr	*dscr;
double      leng;
Dpoint3d    batuPoi[2];

UInt32      lv,co, wt;
int         lc;


   leng         = 500 * delPointNew.rr;
   batuPoi[0].z = batuPoi[1].z = 0;

   batuPoi[0].x = Poi->x + leng * cos( angle + fc_piover4);
   batuPoi[0].y = Poi->y + leng * sin( angle + fc_piover4);
   batuPoi[1].x = Poi->x - leng * cos( angle + fc_piover4);
   batuPoi[1].y = Poi->y - leng * sin( angle + fc_piover4);
   mdlLine_create( &ele, NULL, batuPoi);
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   lv = 0;
   mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   //ele.ehdr.locked = 1;
   ele.hdr.dhdr.props.b.s = 1;
   mdlElmdscr_appendElement(Tdscr, &ele);


   /*
   ele.ehdr.locked = 0;
   ele.hdr.dhdr.props.b.s = 0;
   ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
   ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
   ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
   mdlElement_setSymbology(&ele, &co, &wt, &lc);
   lv = 3;
   mdlElement_setProperties(&ele, &lv,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   mdlElmdscr_new(&dscr, NULL, &ele);
   Tpos = mdlElmdscr_addByModelRef(dscr, modelRefP);
   printf("%d\n", mdlErrno);
   mdlElmdscr_display(dscr, modelRefP, DRAW_MODE_Normal);
   mdlElmdscr_freeAll(&dscr);
   */

   batuPoi[0].x = Poi->x + leng * cos( angle - fc_piover4);
   batuPoi[0].y = Poi->y + leng * sin( angle - fc_piover4);
   batuPoi[1].x = Poi->x - leng * cos( angle - fc_piover4);
   batuPoi[1].y = Poi->y - leng * sin( angle - fc_piover4);
   mdlLine_create( &ele, NULL, batuPoi);
   ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
   lv = 0;
   mdlElement_setProperties(&ele, &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   //ele.ehdr.locked = 1;
   ele.hdr.dhdr.props.b.s = 1;
   mdlElmdscr_appendElement(Tdscr, &ele);



}
/*----------------------------------------------------------------------+
| 	�~��̔���				|
+----------------------------------------------------------------------*/
Private void    create_batu_2(double angle, Dpoint3d *Poi, DgnDrawMode drawMode)
{
	MSElement   ele;
	MSElementDescr	*dscr;
	double      leng;
	Dpoint3d    batuPoi[2];


	leng = 500 * delPointNew.rr;
	batuPoi[0].z = batuPoi[1].z = 0;

	batuPoi[0].x = Poi->x + leng;
	batuPoi[0].y = Poi->y + leng;
	batuPoi[1].x = Poi->x - leng;
	batuPoi[1].y = Poi->y - leng;
	mdlLine_create(&ele, NULL, batuPoi);

	msTransientElmP = mdlTransient_addElement(NULL, &ele, 0, 0xffff, drawMode, false, false, true);

	batuPoi[0].x = Poi->x - leng;
	batuPoi[0].y = Poi->y + leng;
	batuPoi[1].x = Poi->x + leng;
	batuPoi[1].y = Poi->y - leng;
	mdlLine_create(&ele, NULL, batuPoi);
	msTransientElmP = mdlTransient_addElement(NULL, &ele, 0, 0xffff, drawMode, false, false, true);


}
/*------------------------------------------------------------------------------------------------*/
/*--- ����_����1�ԋ߂����C����̓_�Ƃ��̓_�̃��C����ł̈ʒu �@�i�X�i�b�v�ʒu�ɕύX�j----------*/
/*------------------------------------------------------------------------------------------------*/
int get_closest_point_a(int *lnv, Dpoint3d *lPoi, Dpoint3d *orig, Dpoint3d *nearPoi, int *segnum, double *leng)
{
	MSElement	ele;
	DSegment3d	seg;
	Dpoint3d	*tmpPoi, tmpnearPoi;
	int 		ii, jj, tmpnv, tmpseg;
	double		tmpleng, minleng;

	int ret;
	

	tmpnv = *lnv;
	tmpPoi = (DPoint3d*)malloc(50000 * sizeof(DPoint3d));

	/*--- ���̃��C���̍��W��ۑ� ---*/
	for (ii = 0; ii < tmpnv; ii++) {
		memcpy(&tmpPoi[ii], &lPoi[ii], sizeof(Dpoint3d));
	}

	minleng = 9999999;
	tmpseg = 0;
	while (1) {

		/*--- �ŒZ�����̓_ ---*/
		mdlLineString_create(&ele, NULL, tmpPoi, tmpnv);
		memcpy(&tmpnearPoi, orig, sizeof(Dpoint3d));
		//		ret = mdlLinear_getClosestSegment(&seg, &tmpnearPoi, &ele);
		for (jj = 0; jj < tmpnv; jj++) {
			if (orig->x == lPoi[jj].x && orig->y == lPoi[jj].y) {
				ret = jj - 1;
				if (ret <= 0) { ret = 0; }
				break;
			}
		}
		if (jj >= tmpnv) {
		    free(tmpPoi);
		    return(ERROR);
	    }

		/*--- �ŒZ�����̓_�̈ʒu ---*/
		tmpleng = sqrt(pow(orig->x - tmpnearPoi.x, 2) + pow(orig->y - tmpnearPoi.y, 2));
		if (tmpleng < minleng) {
			minleng = tmpleng;
			memcpy(nearPoi, &tmpnearPoi, sizeof(Dpoint3d));
			for (jj = 0; jj < tmpnv; jj++) {
				if (sqrt(pow(tmpPoi[jj].x - lPoi[ret].x, 2) + pow(tmpPoi[jj].y - lPoi[ret].y, 2)) < 1) {
					*segnum = tmpseg + jj;
					memcpy(nearPoi, &tmpnearPoi, sizeof(Dpoint3d));
					*leng = sqrt(pow(nearPoi->x - lPoi[ret].x, 2) + pow(nearPoi->y - lPoi[ret].y, 2));
					break;
				}
			}
		}
		break;
	}
	free(tmpPoi);
	return(SUCCESS);
}


Private void    cleanFunc(void)
{
	int ret;
	if (Tpos != NULL && Tflg == 1) {

		mdlRefFile_detach(outModelRefP);
	    ret = mdlWorkDgn_delete(Tdscr, Tpos -1, modelRefP);
//printf("ret2 %d\n", ret);
		mdlElmdscr_freeAll(&Tdscr);
		Tpos = NULL;
		mdlWorkDgn_saveChanges(modelRefP);
		mdlWorkDgn_closeFile(modelRefP);
		mdlLevel_setDisplayPriority(MASTERFILE, linI.lv, 0);

	};
}