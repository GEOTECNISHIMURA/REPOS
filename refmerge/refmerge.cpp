#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <DgnPlatform\ScanCriteria.h>
#include <DgnPlatform\DgnModel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "refmerge.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

int			        exitStatus;
MSDialog            *db_p, *db_p2;
REFMERGE_INFO   	refmerge;
UInt32              cpyElemCnt;
UInt32              refElemCnt;
WChar               cBarString[512];


typedef struct IDX_INFO{
	WChar  master[512];
	WChar  ref[15][512];
	int    fcnt;
}IDX;

IDX_INFO   tblI[5000];

int     tblICnt;

/*-    �t�b�N�֐�		-*/
void	delele_dialog_hook(DialogMessage *dmP);
void	toggle_bat_hook(DialogItemMessage *dimP);
void	toggle_mode_hook(DialogItemMessage *dimP);
void	push_bat_hook(DialogItemMessage *dimP);
void	push_ok_hook(DialogItemMessage *dimP);
void	push_end_hook(DialogItemMessage *dimP);

DialogHookInfo	uHooks[] = {
   { HOOKID_REFMERGE_DIALOG, (PFDialogHook)delele_dialog_hook},
   { HOOKID_TOGGLE_BAT,      (PFDialogHook)toggle_bat_hook   },
   { HOOKID_TOGGLE_MODE,     (PFDialogHook)toggle_mode_hook  },
   { HOOKID_PUSH_BAT,	     (PFDialogHook)push_bat_hook     },
   { HOOKID_PUSH_OK,	     (PFDialogHook)push_ok_hook      },
   { HOOKID_PUSH_END,	     (PFDialogHook)push_end_hook     },
};

/*-	�v���g�^�C�v	-*/

bool unload(UnloadProgramReason unloadType);
int  read_Tbl(WChar *tblName);
void syori(void);
int copyFromReference(DgnModelRefP refModel);
int scanCallback(MSElementDescr* edP, CallbackArgP arg, ScanCriteriaP scP);
void    scan_element_all(DgnModelRefP refModel);
/*-----------------------------------------------------------------------
**	���C��
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	FILE		    *fp;
	RscFileHandle   Rsc;
	SymbolSet		*setP;
	BeFileName  fname;

	/*--------- ���\�[�X�t�@�C���̃I�[�v�� -------------*/
	if (mdlResource_openFile(&Rsc, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"delele�̃��\�[�X�t�@�C�����J���܂���");
		mdlSystem_exit(exitStatus, 1);
		return;
	}

	/*--- �A�����[�h�֐��Z�b�g ---*/
	//mdlSystem_setFunction( SYSTEM_UNLOAD_PROGRAM, unload);
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	/*--------- �t�b�N�֐��̎g�p�錾 -----------*/
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	/*--------- C����`�����ݒ� -------------*/
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, "REFMERGE_INFO", "refmerge", &refmerge);

	/*--------- �ݒ�l��ǂݍ��� -------------*/

	mdlFile_find(&fname, L"refmerge.tbl", L"MS_DEF", NULL);
	if ((fp = _wfopen(fname.GetWCharCP(), L"r")) != NULL) {
		fwscanf(fp, L"%d\n", &refmerge.tog_bat);
		fwscanf(fp, L"%d\n", &refmerge.mode);
		fwscanf(fp, L"%ls\n", refmerge.bat);
		fclose(fp);
	}
	else {
		refmerge.tog_bat = 1;
		refmerge.mode = 0;
		wcscpy(refmerge.bat, L"c:\\");
	}


	/*-------- �_�C�A���O�{�b�N�X�I�[�v�� --------*/
	db_p = mdlDialog_open(NULL, DIALOGID_REFMERGE);
	if (db_p == NULL) {
		mdlOutput_error(L"REFMERGE�_�C�A���O�{�b�N�X���J���܂���");
	}

	/*--- �P�t�@�C���̏ꍇ ---*/
	if (refmerge.tog_bat == 0) {
		mdlDialog_itemSetEnabledState(db_p, 1, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 2, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 3, FALSE, FALSE);
		mdlDialog_itemSetEnabledState(db_p, 7, FALSE, FALSE);
	}

}
/*-----------------------------------------------------------------------
**	�A�����[�h�֐�
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{

	return(SUCCESS);
}

/*-----------------------------------------------------------------------
**	�_�C�A���O�t�b�N�֐�
-----------------------------------------------------------------------*/
void	delele_dialog_hook(DialogMessage *dmP)
{
	DialogBoxRsc	*dbr_P;

	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType) {
	case DIALOG_MESSAGE_CREATE:
		dbr_P = dmP->u.create.dialogBoxRP;
		dmP->u.create.interests.updates = TRUE;
		dmP->u.create.interests.mouses = TRUE;
		dmP->u.create.interests.dialogFocuses = TRUE;
		dmP->u.create.interests.itemFocuses = TRUE;
		break;

	case DIALOG_MESSAGE_DESTROY:
		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD,
			mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
		break;

	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}


/*--------------------------------------------------------------+
|       �g�O���{�^���i�o�b�`�j�@�t�b�N�֐�      |
+--------------------------------------------------------------*/
void   toggle_bat_hook(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType) {
	case DITEM_MESSAGE_STATECHANGED:
		/*--- �o�b�` ON �̏ꍇ --------*/
		if (refmerge.tog_bat == -1) {
			mdlDialog_itemSetEnabledState(db_p, 1, TRUE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 2, TRUE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 3, TRUE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 7, TRUE, FALSE);
		}

		/*--- �o�b�` OFF �̏ꍇ --------*/
		else {
			mdlDialog_itemSetEnabledState(db_p, 1, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 2, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 3, FALSE, FALSE);
			mdlDialog_itemSetEnabledState(db_p, 7, FALSE, FALSE);
		}
		mdlDialog_itemsSynch(db_p);
		break;

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


/*-----------------------------------------------------------------------
**	  �g�O���{�^���i�A�^�b�`���[�h�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
void	toggle_mode_hook(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType) {
	case DIALOG_MESSAGE_CREATE:
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i�o�b�`�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
void	push_bat_hook(DialogItemMessage *dimP)
{
WChar   filter[32], defaultDirec[MAXDIRLENGTH], title[128], devP[MAXDEVICELENGTH], dirP[MAXDIRLENGTH];
BeFileName      filePath;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType) {
	case DITEM_MESSAGE_CREATE:
		break;

	case DITEM_MESSAGE_BUTTON:
		swprintf(filter, L"*.idx");
		swprintf(title, L"idx�t�@�C����I��");

		mdlFile_parseName(refmerge.bat, devP, dirP, NULL, NULL);
		swprintf(defaultDirec, L"%ls:%ls", devP, dirP);

		mdlDialog_fileOpen(filePath, NULL, NULL, NULL, filter, defaultDirec, title);
		if (wcslen(filePath.GetWCharCP()) != 0) {
			swprintf(refmerge.bat, L"%ls", filePath.GetWCharCP());
			mdlDialog_itemsSynch(db_p);
		}
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^���i���s�j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
void	push_ok_hook(DialogItemMessage *dimP)
{
FILE		*fp;
BeFileName  fname;
int         ii, jj, sw,elemcnt, viewIndex;
DgnModelRefP modelrefP;
DgnModelP activeModel;
//ViewFlags       oViewFlags,flgs;
bool         lw;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType) {
	case DITEM_MESSAGE_CREATE:
		break;

	case DITEM_MESSAGE_BUTTON:

		viewIndex = 0;

		/*--------- �e�[�u���t�@�C���ɏ������� -------------*/
		mdlFile_find(&fname, L"refmerge.tbl", L"MS_DEF", NULL);
		if ((fp = _wfopen(fname.GetWCharCP(), L"w")) != NULL) {
			fwprintf(fp, L"%d\n", refmerge.tog_bat);
			fwprintf(fp, L"%d\n", refmerge.mode);
			fwprintf(fp, L"%ls\n", refmerge.bat);
			fclose(fp);
		}

		if (refmerge.tog_bat == 0) {
			//lw = mdlView_getDisplayControl(VIEWCONTROL_LINE_WEIGHTS, viewIndex);
			//syori();
			printf(" ����\n");
			//mdlView_setDisplayControl(VIEWCONTROL_LINE_WEIGHTS, viewIndex, lw);
		}
		else {
			/*--- �e�[�u���ǂݍ��� -----*/
			if (read_Tbl(refmerge.bat) == 0) {
				mdlOutput_error(L"refmerge_idx�t�@�C���ǂݍ��݂Ɏ��s");
				return;
			}
			ii = 0;
			while (ii < tblICnt) {
				sw = 0;
				for (jj = 0;jj < tblI[ii].fcnt;jj++) {
					if (jj == 0) {
						// sinki
						if (mdlSystem_newDesignFile(tblI[ii].master) == SUCCESS) {
							if (refmerge.mode != 0) break;
						}
						else
						{
							sw = 1;
						}
					}
					else {
						// attach
						if (mdlRefFile_attach( &modelrefP, tblI[ii].ref[jj-1], NULL, NULL, NULL, NULL, 1, NULL
							, 0, NULL, REF_FILE_LEVEL_DISPLAY_DEFAULT, TRUE, TRUE) == SUCCESS) {
						}
						if (jj == tblI[ii].fcnt - 1) {
							//syori
							syori();
						}
					}
				}
				if (refmerge.mode != 0 && sw == 0) {
					// attach
					syori();
				}
				mdlSystem_fileDesign();
				ii++;
			}
		}

		break;

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


/*-----------------------------------------------------------------------
**	�v�b�V���{�^��(�I���j�@�t�b�N�֐�
-----------------------------------------------------------------------*/
void	push_end_hook(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType) {
	case DITEM_MESSAGE_CREATE:
		break;

	case DITEM_MESSAGE_BUTTON:

		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD,
			mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
		break;

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}



/*----------------------------------------------------------------------+
|       �e�[�u���ǂݍ���                        |
+----------------------------------------------------------------------*/
int  read_Tbl(WChar *tblName)
{
FILE       *fp;
WChar      s_buf[512],msg[512];
int        rtn = 0, ii,sw = 0;
BeFileName full;


	tblICnt = 0;
	if (NULL == (fp = _wfopen(tblName, L"rt"))) {
		mdlOutput_error(L"tblFile OPEN ERROR!");
		return(rtn);
	}

	while (1) {
		fgetws(s_buf, 512, fp);
		if (feof(fp) || ferror(fp)) {
			rtn = TRUE;
			break;
		}
		if (0 == wcsncmp(s_buf, L";", 1)) {
			continue;
		}
		tblI[tblICnt].fcnt = swscanf(s_buf, L"%ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls %ls",
			  &tblI[tblICnt].master, &tblI[tblICnt].ref[0], &tblI[tblICnt].ref[1], &tblI[tblICnt].ref[2],
			  &tblI[tblICnt].ref[3], &tblI[tblICnt].ref[4], &tblI[tblICnt].ref[5], &tblI[tblICnt].ref[6],
			  &tblI[tblICnt].ref[7], &tblI[tblICnt].ref[8], &tblI[tblICnt].ref[9], &tblI[tblICnt].ref[10],
			  &tblI[tblICnt].ref[11], &tblI[tblICnt].ref[12], &tblI[tblICnt].ref[13], &tblI[tblICnt].ref[14]);

		for( ii=0;ii < tblI[tblICnt].fcnt;ii++){
			if (ii == 0) {
				swprintf(s_buf, L"%ls", tblI[tblICnt].master);
			}
			else {
				swprintf(s_buf, L"%ls", tblI[tblICnt].ref[ii-1]);
			}
			if ( mdlFile_find(&full, s_buf, NULL, NULL) != SUCCESS) {
				swprintf(msg, L"idx�t�@�C��\n�s�ԍ� %d\n�t�@�C���� %ls\n ��������܂���\n���f���܂�", tblICnt +1, s_buf);

				mdlDialog_openInfoBox(msg);
				sw = 1;
				break;
			}
			if (ii == 0) {
				swprintf(tblI[tblICnt].master, L"%ls", full.GetWCharCP());
			}
			else {
				swprintf(tblI[tblICnt].ref[ii - 1], L"%ls", full.GetWCharCP());
			}

		};
		if (sw == 1) break;
		tblICnt++;
	}
	fclose(fp);
	return(rtn);
}

void syori(void)
{
DgnModelRefP		modelRefP;
DgnModelP           modelP;
ModelRefIteratorP	iterator;
DgnFileP            fileP;
DgnAttachmentArrayP  dgnAaryP;
//DgnAttachmentP       dgnAP;
//int                   cnt,ii;

//bool                  ret;
//WChar  wcs[512];

    mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		modelP = modelRefP->GetDgnModelP();
		if ( modelP != NULL) {

			refElemCnt = modelP->GetElementCount(DgnModelSections::All);
			fileP = modelRefP->GetDgnFileP();
			swprintf(cBarString, L"������\n%ls", fileP->GetFileName().GetWCharCP());

			//dgnAaryP = modelP->GetDgnAttachmentsP();
			/*
			if (dgnAaryP != NULL) {
				cnt = dgnAaryP->size();
				for (ii = 0;ii < cnt;ii++) {
					dgnAP = dgnAaryP->at(ii);
					ret = dgnAP->IsMissingFile();
				}
			}
			*/
			db_p2 = mdlDialog_completionBarOpen(cBarString);
			scan_element_all(modelRefP);
			//copyFromReference( modelRefP);
			mdlDialog_completionBarClose(db_p2);

		}
	}
	mdlRefFile_detach(REFERENCE_All_References);
	return;
}


int copyFromReference (DgnModelRefP refModel)
{

   cpyElemCnt = 0;
   ElementCopyContextP  context = mdlCopyContext_create ();
   ScanCriteriaP sc = mdlScanCriteria_create ();
   mdlScanCriteria_setReturnType(sc, MSSCANCRIT_RETURN_ELEMENTDATA, FALSE, TRUE);
   //mdlScanCriteria_setDrawnElements(sc);
   mdlScanCriteria_setModel (sc, refModel);
   mdlScanCriteria_setElmDscrCallback (sc, (PFScanElemDscrCallback)scanCallback, context);
   mdlScanCriteria_scan(sc, NULL, NULL, NULL);
   mdlScanCriteria_free (sc);
   mdlCopyContext_free (&context);
   return cpyElemCnt;
}

// Callback function
int scanCallback (MSElementDescr* edP, CallbackArgP arg, ScanCriteriaP scP)
{

   ElementCopyContextP context = (ElementCopyContextP)arg;
   DgnModelRefP destination = mdlModelRef_getActive ();
   DgnModelRefP source      = mdlScanCriteria_getModel (scP);
   MSElementDescrP duplicate = NULL;
   mdlElmdscr_duplicate (&duplicate, edP);
   mdlElmdscr_copy (&duplicate, source, destination, context);
   mdlElmdscr_freeAll (&duplicate);
   //cpyElemCnt++;
   //mdlDialog_completionBarUpdate(db_p2, cBarString, 100 * cpyElemCnt / refElemCnt);
   return SUCCESS;
}


/*-----------------------------------------------------------------------
**  �I��v�f�Ɠ����C���[�̗v�f������
-----------------------------------------------------------------------*/
void    scan_element_all(DgnModelRefP refModel)
{
ExtScanList scanList;
ULong       filePos;
int         getSize, Error;
UInt32       elemAddr[1];
MSElementDescrP duplicate = NULL, edP;
ElementCopyContextP  context;
	/*--- �X�L�������X�g�̏����� ---*/
	mdlScan_initScanlist(&scanList);

	/*--- �����͈͂̐ݒ� ---*/
	mdlScan_noRangeCheck(&scanList);

	scanList.scantype |= (unsigned short)ONEELEM;
	scanList.scantype |= (unsigned short)ELEMTYPE;

	scanList.typmask[0] |= (unsigned short)TMSK0_CELL_HEADER;
	scanList.typmask[0] |= (unsigned short)TMSK0_LINE;
	scanList.typmask[0] |= (unsigned short)TMSK0_LINE_STRING;
	scanList.typmask[0] |= (unsigned short)TMSK0_SHAPE;
	scanList.typmask[0] |= (unsigned short)TMSK0_TEXT_NODE;
	scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_STRING;
	scanList.typmask[0] |= (unsigned short)TMSK0_CMPLX_SHAPE;
	scanList.typmask[0] |= (unsigned short)TMSK0_ELLIPSE;
	scanList.typmask[0] |= (unsigned short)TMSK0_ARC;
	scanList.typmask[1] |= (unsigned short)TMSK1_TEXT;
	/*--- �X�L�������s ---*/
    filePos = 0;
	if (mdlScan_initialize(refModel, &scanList) != SUCCESS) {
		mdlDialog_openInfoBox(L"�X�L�������X�g���󂯓�����܂���ł����B");
		mdlSystem_exit(exitStatus, 1);
		return;
	}
	cpyElemCnt = 0;
	context = mdlCopyContext_create();
	while (TRUE) {
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);
		if (Error == 10) { break; }
		if (Error == 65) { mdlDialog_openInfoBox(L"�����ȃt�@�C���ԍ�"); }
		if (Error == 66) { mdlDialog_openInfoBox(L"EOF�G���["); }
		if (Error == 67) { mdlDialog_openInfoBox(L"�X�L�������X�g�Ɍ��"); }
		if (Error == 68) { mdlDialog_openInfoBox(L"�����ȗv�f"); }
		if (Error >= 65) { mdlSystem_exit(exitStatus, 1);return; }

		
		mdlElmdscr_read(&edP, elemAddr[0], refModel, FALSE, NULL);
		mdlElmdscr_duplicate(&duplicate, edP);
		mdlElmdscr_copy(&duplicate, refModel, MASTERFILE, context);
		mdlElmdscr_freeAll(&duplicate);
		mdlElmdscr_freeAll(&edP);
		cpyElemCnt++;
        mdlDialog_completionBarUpdate(db_p2, cBarString, 100 * cpyElemCnt / refElemCnt);
	}
	mdlCopyContext_free(&context);
}