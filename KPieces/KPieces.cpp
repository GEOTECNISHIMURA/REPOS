#define	DEBUG	0
#define LOCK 0
/*- 0:�A�����b�N,  1:���b�N -*/
/*----------------------------------------------------------------------+
|									|
|    $Workfile:   KPieces.mc  $
|    $Revision:   1.0  $  $Date:   2000/10/2  $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
| Copyright (C) 1991   Bentley Systems, Inc., All rights reserved.	|
|									|
| "MicroStation", "MDL", and "MicroCSL" are trademarks of Bentley	|
|  Systems, Inc.							|
|									|
| Limited permission is hereby granted to reproduce and modify this	|
| copyrighted material provided that the resulting code is used only in	|
| conjunction with Bentley Systems products under the terms of the	|
| license agreement provided therein, and that this notice is retained	|
| in its entirety in any such reproduction or modification.		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   KPieces.mc -  MDL example to show creation & use of KPieces dialog box|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>

#include "KPieces.h"	    /* KPieces dialog box example constants & structs */
#include "KPiecestxt.h"    /* KPieces dialog box command numbers */
#include "../CommonCE/naka8.cpp"
//#include <lok.mc>
//#include "include/err_s.h"
#include "include/kansuu.h"
//#include <standard.h>
#include "../CommonCE/check_kigen8.cpp"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT
/*----------------------------------------------------------------------+
|									|
|   Local defines							|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Local type definitions						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Private Global variables						|
|									|
+----------------------------------------------------------------------*/
KpiecesGlobals	kpiecesGlobals;
ActiveParam		activeParam;

#ifndef Private
#define Private
#endif

MSElement	GellBuf;

/*----------------------------------------------------------------------+
|									|
|   Local function declarations 					|
|									|
+----------------------------------------------------------------------*/


void	KPieces_Practice( DialogItemMessage *dmP);
void	KPieces_FileIdx( DialogItemMessage *dmP);
void	KPieces_CkBat( DialogItemMessage *dmP);
void	KPieces_Syori( DialogItemMessage *dmP);
void	KPieces_Jiko( DialogItemMessage *dmP);
void	Workspace( DialogItemMessage *dmP);
void	set_levels( DialogItemMessage *dmP);
void	Levels_Ok( DialogItemMessage *dmP);
void	Levels_chk( DialogItemMessage *dmP);
void	KPieces_FileTbl( DialogItemMessage *dmP);
void	KPieces_LogB(DialogItemMessage *dmP);

void	KPieces_Apply( WChar *jstring);
void	KPieces_Bunkai(	ULong	setFilepos);
int		check_Design( WChar *fileName);
void    tmpelmdsp(MSElement elm, Dpoint3d *pts);

Private void userState_get( void);									/* �c�������̎擾	      */
Private void userState_clean( void);								/* �c��������Clean	      */
Private void start_sin( void);
Private void set_JustOrigin( void);
Private int  chk_level( int lv, int co, int wt, int sy);
Private int  chk_font( int font);
Private void SetTLevels( void);
Private void Set_ini( void);
Private void Get_ini( void);
int Text_ExtractShape_a(MSElementP elmP, DPoint3d *points);

Private DialogHookInfo uHooks[] =
{
	{ HOOKITEMID_Practice,	(PFDialogHook)KPieces_Practice},
	{ HOOKITEMID_FileIdx,	(PFDialogHook)KPieces_FileIdx},
	{ HOOKITEMID_CkBat,		(PFDialogHook)KPieces_CkBat},
	{ HOOKITEMID_Syori,		(PFDialogHook)KPieces_Syori},
	{ HOOKITEMID_Jiko,		(PFDialogHook)KPieces_Jiko},
	{ HOOKITEMID_Workspace,	(PFDialogHook)Workspace},
	{ HOOKITEMID_Levels,	(PFDialogHook)set_levels},
	{ HOOKITEMID_LevelsOk,	(PFDialogHook)Levels_Ok},
	{ HOOKITEMID_Levelschk,	(PFDialogHook)Levels_chk},
	{ HOOKITEMID_FileTbl,	(PFDialogHook)KPieces_FileTbl},
	{ HOOKITEMID_Logbottn,	(PFDialogHook)KPieces_LogB},
};

int			dgntype = 0;
MSDialog	*MdbP;				/* a ptr to a dialog box */




// mdlText_extract �錾
Dpoint3d		origin, userOrigin, setOrigin, maxOrigin, minOrigin, points[101];
int				numVerts;
WString			string;
RotMatrix		rMatrix;
TextStyleInfo	textstyleInfo; 
TextSizeParam   textSize;
TextParamWide	textParam;
TextEDParam     textEDParam;
MSTextSize      tileSize;

//
int				textByte, textMoj;
double			textMatrix, Matrix, angle;

TextElementJustification just;
//

// mdlText_extractWide �錾
//Dpont3d		Worigin, WuserOrigin;
//RotMatrix	WrMatrix;
TextSizeParam	WtextSize;
TextParamWide	WtextParamWide;
TextEDParam		WedPatam;

// mdlText_create �錾
//TextParam		textParam;

// mdlText_createWide �錾
TextParamWide	textParamWide;

// Text ����
UShort			vertical;
WChar			stringb[3];
WChar			stringmoj[300];
WChar   		stringwb[3];
WChar           stringwmoj[300];
FILE			*flog;

int exitStatus;
/*----------------------------------------------------------------------+
|									|
| name		main							|
|									|
| author	BSI     				12/90		|
|									|
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
	SymbolSet		*setP;		/* a ptr to a "C expression symbol set" */
    RscFileHandle   rscFileH;	/* a resource file handle */
	char            structName[128];
	char            variableName[128];
/*
#if LOCK == 1
    main_lok( );
#endif
*/
	chk_kigen( );



    /* open the resource file that we came out of */
    mdlResource_openFile( &rscFileH, NULL, 0);

    /* set up variables that will be evaluated within C expression strings */
	sprintf(structName, "kpiecesglobals");
	sprintf(variableName, "kpiecesGlobals");

    setP = mdlCExpression_initializeSet( VISIBILITY_CALCULATOR|VISIBILITY_DIALOG_BOX, 0, TRUE);
    mdlDialog_publishComplexVariable( setP, structName,
		                              variableName, &kpiecesGlobals);
	kpiecesGlobals.idxggle = -1;
	kpiecesGlobals.syori   = 1;
	kpiecesGlobals.jiko    = 1;
	kpiecesGlobals.just    = (int)TextElementJustification::LeftBaseline;
	kpiecesGlobals.tjust   = (int)TextElementJustification::LeftTop;
	kpiecesGlobals.ft      = 0;
	kpiecesGlobals.levelschk = -1;
	kpiecesGlobals.ftchk     = -1;
	kpiecesGlobals.tbli      = -1;
	kpiecesGlobals.log       = -1;

	Get_ini();
    /* publish our hook functions */
    mdlDialog_hookPublish( sizeof( uHooks)/sizeof(DialogHookInfo), uHooks);

    /* open the "Dlg" dialog box */
    if ( (MdbP = mdlDialog_open (NULL, DIALOGID_MainDialog)) == NULL){
//	errorPrint (2);

    }
}

/*----------------------------------------------------------------------+
|                                                                       |
| name		CkBat                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_CkBat(DialogItemMessage *dmP)
{
	if( kpiecesGlobals.idxggle == -1 ){
		/*-----------------------------------------------+
		|    �{�^�������g�p�\�E�s�\(3-103)		 |
		+-----------------------------------------------*/
//			mdlDialog_itemSetEnabledState( MdbP, 3, TRUE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 4, TRUE, FALSE );
	}
	else{
//			mdlDialog_itemSetEnabledState( MdbP, 3, FALSE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 4, FALSE, FALSE );

	}

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		Workspace                                                   |
|									|
+----------------------------------------------------------------------*/
Private void Workspace(DialogItemMessage *dmP)
{

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{
	    if ( (MdbP = mdlDialog_open (NULL, DIALOGID_Workspace)) == NULL){
//	errorPrint (2);

	    }
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}


}
/*----------------------------------------------------------------------+
|                                                                       |
| name		FileIdx                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_FileIdx(DialogItemMessage *dmP)
{

	BeFileName	 fileName;
	int	err;
	MSDialog	*WdbP;

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{
      /*--- �C���f�b�N�X�t�@�C���I�[�v�� ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	�t�@�C�����̍\���v�f�𒊏o����B(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	���O�̍\���v�f�Ɋ�Â��ăt�@�C�������쐬����B(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  �t�@�C���̂n�o�d�m(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileOpen( fileName, NULL, 0, kpiecesGlobals.idxname, L"*.idx", directory, L"�C���f�b�N�X�t�@�C��");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.idxname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   �_�C�A���O�̃A�b�v�f�[�g(3-106)	    |
			+------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Workspace, NULL );
				mdlDialog_itemsSynch( WdbP );
		}
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}
#if DEBUG
printf("IDX Name = %s\n", kpiecesGlobals.idxname );
#endif

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		FileTbl                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_FileTbl(DialogItemMessage *dmP)
{

	BeFileName	 fileName;
	int	err;
	MSDialog	*WdbP;

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{
      /*--- �C���f�b�N�X�t�@�C���I�[�v�� ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	�t�@�C�����̍\���v�f�𒊏o����B(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	���O�̍\���v�f�Ɋ�Â��ăt�@�C�������쐬����B(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  �t�@�C���̂n�o�d�m(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileOpen( fileName, NULL, 0, kpiecesGlobals.tblname, L"*.tbl", directory, L"�s�a�k�t�@�C��");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.tblname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   �_�C�A���O�̃A�b�v�f�[�g(3-106)	    |
			+------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Workspace, NULL );
				mdlDialog_itemsSynch( WdbP );
		}
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		LogButton                                               |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_LogB(DialogItemMessage *dmP)
{
	BeFileName	 fileName;
	int	err;
	MSDialog	*WdbP;

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{
      /*--- �C���f�b�N�X�t�@�C���I�[�v�� ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	�t�@�C�����̍\���v�f�𒊏o����B(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	���O�̍\���v�f�Ɋ�Â��ăt�@�C�������쐬����B(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  �t�@�C���̂n�o�d�m(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileCreate( fileName, NULL, 0, kpiecesGlobals.logname, L"*.log", directory, L"���O�t�@�C��");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.logname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   �_�C�A���O�̃A�b�v�f�[�g(3-106)	    |
			+------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Workspace, NULL );
				mdlDialog_itemsSynch( WdbP );
		}
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		Jiko                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_Jiko(DialogItemMessage *dmP)
{

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_STATECHANGED:
	{
		if( kpiecesGlobals.jiko == 2 ){
			kpiecesGlobals.idxggle = 0;
		}

		if( kpiecesGlobals.idxggle == -1 ){
			/*-----------------------------------------------+
			|    �{�^�������g�p�\�E�s�\(3-103)		 |
			+-----------------------------------------------*/
//			mdlDialog_itemSetEnabledState( MdbP, 3, TRUE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 4, TRUE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 5, TRUE, FALSE );
		}
		else{
//			mdlDialog_itemSetEnabledState( MdbP, 3, FALSE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 4, FALSE, FALSE );
//			mdlDialog_itemSetEnabledState( MdbP, 5, FALSE, FALSE );

		}
		
			/*------------------------------------------+
			|   �_�C�A���O�̃A�b�v�f�[�g(3-106)	    |
			+------------------------------------------*/
				mdlDialog_itemsSynch( MdbP );

		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}


/*----------------------------------------------------------------------+
|                                                                       |
| name		Syori                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_Syori(DialogItemMessage *dmP)
{

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_STATECHANGED:
	{
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		LevelsChk                                               |
|									|
+----------------------------------------------------------------------*/
Private void Levels_chk(DialogItemMessage *dmP)
{

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_STATECHANGED:
	{
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}


/*----------------------------------------------------------------------+
|                                                                       |
| name		set_levels                                              |
|									|
+----------------------------------------------------------------------*/
Private void set_levels(DialogItemMessage *dmP)
{

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{
	    if ( (MdbP = mdlDialog_open (NULL, DIALOGID_Levels)) == NULL){
//	errorPrint (2);

	    }
		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}
/*----------------------------------------------------------------------+
|                                                                       |
| name		levelsOk                                              |
|									|
+----------------------------------------------------------------------*/
Private void Levels_Ok(DialogItemMessage *dmP)
{
	MSDialog	*WdbP;

	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
	case DITEM_MESSAGE_BUTTON:
	{

			SetTLevels();
			kpiecesGlobals.levelschk = -1;
		/*--------------------------------------------------------------+
		|   �_�C�A���O��close					|
		+--------------------------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Levels, NULL );
	  		mdlDialog_closeCommandQueue( WdbP );	
			/*------------------------------------------+
			|   �_�C�A���O�̃A�b�v�f�[�g(3-106)	    |
			+------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Workspace, NULL );
				mdlDialog_itemsSynch( WdbP );

		  break;
	};
	default:
		dmP -> msgUnderstood = TRUE;
		break;
	}

}

/*----------------------------------------------------------------------+
|                                                                       |
| name		SetTLevels                                             |
|									|
+----------------------------------------------------------------------*/
Private void SetTLevels( void )
{
	int  i, j;
	WChar mask_bit = 0x1;

		for( i = 0 ; i < 4 ; i++ ){
			for( j = 0 ; j < 16 ; j++ ){
				if( 0 != ( kpiecesGlobals.levels[i] & ( mask_bit << j )) ){
					kpiecesGlobals.setlevels[i*16+j+1] = TRUE;
				}
				else{
					kpiecesGlobals.setlevels[i*16+j+1] = FALSE;
				}
#if DEBUG
printf("Levels  %d : %d\n", i*16+j, kpiecesGlobals.setlevels[i*16+j]);
#endif
			}
		}
}
/*----------------------------------------------------------------------+
|                                                                       |
| name		check_Design                                            |
|									|
+----------------------------------------------------------------------*/
int	check_Design( WChar *fileName )
{
/*	int	err;
	boolean	isThreeD;

	err = mdlFile_checkDesignFile( &isThreeD, fileName );
	if( err != 0){
		if( isThreeD == TRUE ){
				dgntype = 3;
		}
		else{
				dgntype = 2;
		}
	}
	return( err);
*/
  if(mdlModelRef_is3D(MASTERFILE) != true)
	{
	mdlOutput_error(L"�Q�����t�@�C���ł�");
	dgntype = 2;
	}
	else{
	mdlOutput_error(L"�R�����t�@�C���ł�");
	dgntype = 3;
	}
#if DEBUG
printf("dgntype = %d\n", dgntype );
#endif
	return( 0 );


}



/*----------------------------------------------------------------------+
|									|
| name		Get_ini							|
|									|
+----------------------------------------------------------------------*/
Private	void	Get_ini()
{
	FILE	*fin;
	BeFileName	innameP;
	WChar	batname[10000], tblname[10000], logname[10000];


		if( SUCCESS ==  mdlFile_find( &innameP, L"KPieces.ini", L"MS_DEF", NULL)){
#if DEBUG
printf(" ------------ Get_ini -- %s ----------------\n", innameP );
#endif
			if( NULL != (fin = _wfopen( innameP.GetWCharCP(), L"r" )) ){
				fwscanf( fin, L"%d %ls %d %ls %d %d %d %d %d %d %d %d %d %d %ls %d %d %d %d\n",
						&kpiecesGlobals.tbli, tblname, &kpiecesGlobals.idxggle, batname,
							 &kpiecesGlobals.backup, &kpiecesGlobals.CompressDesign, &kpiecesGlobals.levelschk, &kpiecesGlobals.ftchk,
								&kpiecesGlobals.ft, &kpiecesGlobals.levels[0], &kpiecesGlobals.levels[1],
									&kpiecesGlobals.levels[2], &kpiecesGlobals.levels[3],
										&kpiecesGlobals.log, logname, &kpiecesGlobals.syori, &kpiecesGlobals.jiko,
											&kpiecesGlobals.just, &kpiecesGlobals.tjust );

					if( ! wcscmp( batname, L"NULL" )) {
						wcscpy( kpiecesGlobals.idxname, L"" );
					}
					else{
						wcscpy( kpiecesGlobals.idxname, batname );
					}
					if( ! wcscmp( tblname, L"NULL" )) {
						wcscpy( kpiecesGlobals.tblname, L"" );
					}
					else{
						wcscpy( kpiecesGlobals.tblname, tblname );
					}
					if( ! wcscmp( logname, L"NULL" )) {
						wcscpy( kpiecesGlobals.logname, L"" );
					}
					else{
						wcscpy( kpiecesGlobals.logname, logname );
					}
				SetTLevels();
				fclose(fin);
			}
			else{
#if DEBUG
printf(" open err \n");
#endif
			}
			
		}
#if DEBUG
printf(" levels chk : %d\n", kpiecesGlobals.levelschk);
#endif
}

/*----------------------------------------------------------------------+
|									|
| name		Set_ini							|
|									|
+----------------------------------------------------------------------*/
Private	void	Set_ini()
{
	FILE	*fout;
	BeFileName	outnameP;
	WChar	batname[10000], tblname[10000], logname[10000];

		 mdlFile_find( &outnameP, L"KPieces.ini", L"MS_DEF", NULL);
#if DEBUG
printf(" ------------ Set_ini ------------------\n");
#endif
			if( ! wcscmp( kpiecesGlobals.idxname, L"" )){
				wcscpy( batname, L"NULL" );
			}
			else{
				wcscpy( batname,  kpiecesGlobals.idxname  );
			}
			if( ! wcscmp( kpiecesGlobals.tblname, L"" )){
				wcscpy( tblname, L"NULL" );
			}
			else{
				wcscpy( tblname,  kpiecesGlobals.tblname  );
			}
			if( ! wcscmp( kpiecesGlobals.logname, L"" )){
				wcscpy( logname, L"NULL" );
			}
			else{
				wcscpy( logname,  kpiecesGlobals.logname  );
			}
			fout = _wfopen( outnameP, L"w" );
				fwprintf( fout, L"%d %ls %d %ls %d %d %d %d %d %d %d %d %d %d %ls %d %d %d %d\n",
						kpiecesGlobals.tbli, tblname, kpiecesGlobals.idxggle, batname,
							 kpiecesGlobals.backup, kpiecesGlobals.CompressDesign, kpiecesGlobals.levelschk, kpiecesGlobals.ftchk,
								kpiecesGlobals.ft, kpiecesGlobals.levels[0], kpiecesGlobals.levels[1],
									kpiecesGlobals.levels[2], kpiecesGlobals.levels[3],
										kpiecesGlobals.log, logname, kpiecesGlobals.syori, kpiecesGlobals.jiko,
											kpiecesGlobals.just, kpiecesGlobals.tjust );

			fclose( fout );
}


Private int  chk_level( int lv, int co, int wt, int sy )
{

	kpiecesGlobals.chkley.lv = -1;

	switch( kpiecesGlobals.jiko )
	{
		case	1:
			if( kpiecesGlobals.levelschk == -1 ){
				if( kpiecesGlobals.setlevels[lv] == TRUE ){
					kpiecesGlobals.chkley.lv = 0;

				}
			}
			else{
				kpiecesGlobals.chkley.lv = 0;
			}
			break;
		case	2:
				kpiecesGlobals.chkley.lv = 0;
			break;
		default:
				kpiecesGlobals.chkley.lv = -1;
			break;
	}


	return( kpiecesGlobals.chkley.lv );
}


Private int  chk_font( int font )
{

	kpiecesGlobals.chkley.ft = -1;

	switch( kpiecesGlobals.jiko )
	{
		case	1:
			if( kpiecesGlobals.ftchk == -1 ){
				if( kpiecesGlobals.ft == font ){
					kpiecesGlobals.chkley.ft = 0;
				}
			}
			else{
				kpiecesGlobals.chkley.ft = 0;
			}
			break;
		case	2:
				kpiecesGlobals.chkley.ft = 0;
			break;
		default:
				kpiecesGlobals.chkley.ft = -1;
			break;
	}


	return( kpiecesGlobals.chkley.ft );
}


/*===========================================================================*/

Private void set_JustOrigin( void  )
{
	Dpoint3d     tmppnt;

	int	i;

			if( vertical == 0 ){
				if( kpiecesGlobals.just != (int)TextElementJustification::Invalid){
					WtextParamWide.just = (TextElementJustification)kpiecesGlobals.just;
				}
				else{
					WtextParamWide.just = just;

				}
			}
			if( vertical == 1 ){

				if( kpiecesGlobals.tjust != (int)TextElementJustification::Invalid){
					WtextParamWide.just = (TextElementJustification)kpiecesGlobals.tjust;
				}
				else{
					WtextParamWide.just = just;
				}
				//�������_���������㌴�_�E����ɂ���iV7�ɍ��킹��j
				memcpy(&tmppnt, &points[0], sizeof(Dpoint3d));
				memcpy(&points[0], &points[3], sizeof(Dpoint3d));
				memcpy(&points[3], &tmppnt, sizeof(Dpoint3d));
				memcpy(&tmppnt, &points[1], sizeof(Dpoint3d));
				memcpy(&points[1], &points[2], sizeof(Dpoint3d));
				memcpy(&points[2], &tmppnt, sizeof(Dpoint3d));

			}

			if( kpiecesGlobals.viewIndep == 0 ){
				WtextParamWide.viewIndependent = 0;	//�r���[�ˑ�
			}
			else{
				WtextParamWide.viewIndependent = 1;	//�������
			}

			switch( WtextParamWide.just )
			{
			case	TextElementJustification::LeftTop:
					if( vertical == 0 ){
						setOrigin.x = points[3].x;
						setOrigin.y = points[3].y;
					}
					else{
						setOrigin.x = points[0].x;
						setOrigin.y = points[0].y;
					}
					break;
				case	TextElementJustification::LeftMiddle:
					setOrigin.x =  ( points[0].x + points[3].x ) / 2;
					setOrigin.y =  ( points[0].y + points[3].y ) / 2;
					break;
				case	TextElementJustification::LeftBaseline:
					if( vertical == 0 ){
						setOrigin.x = points[0].x;
						setOrigin.y = points[0].y;
					}
					else{
						setOrigin.x = points[3].x;
						setOrigin.y = points[3].y;
					}
					break;
				case	TextElementJustification::CenterTop:
					if( vertical == 0 ){
						setOrigin.x = ( points[2].x + points[3].x ) / 2;
						setOrigin.y = ( points[2].y + points[3].y ) / 2;
					}
					else{
						setOrigin.x = ( points[0].x + points[1].x ) / 2;
						setOrigin.y = ( points[0].y + points[1].y ) / 2;
					}
					break;
				case	TextElementJustification::CenterMiddle:
					maxOrigin.x = minOrigin.x = points[0].x;
					maxOrigin.y = minOrigin.y = points[0].y;
					for( i = 0 ; i < 4 ; i++ ){
						if( maxOrigin.x < points[i].x ){	maxOrigin.x = points[i].x;	}
						if( minOrigin.x > points[i].x ){	minOrigin.x = points[i].x;	}
						if( maxOrigin.y < points[i].y ){	maxOrigin.y = points[i].y;	}
						if( minOrigin.y > points[i].y ){	minOrigin.y = points[i].y;	}
					}
					setOrigin.x = ( maxOrigin.x + minOrigin.x ) / 2;
					setOrigin.y = ( maxOrigin.y + minOrigin.y ) / 2;
					break;
				case	TextElementJustification::CenterBaseline:
					if( vertical == 0 ){
						setOrigin.x = ( points[0].x + points[1].x ) / 2;
						setOrigin.y = ( points[0].y + points[1].y ) / 2;
					}
					else{
						setOrigin.x = ( points[2].x + points[3].x ) / 2;
						setOrigin.y = ( points[2].y + points[3].y ) / 2;
					}
					break;
				case	TextElementJustification::RightTop:
					if( vertical == 0 ){
						setOrigin.x = points[2].x;
						setOrigin.y = points[2].y;
					}
					else{
						setOrigin.x = points[1].x;
						setOrigin.y = points[1].y;
					}
					break;
				case	TextElementJustification::RightMiddle:
					setOrigin.x =  ( points[1].x + points[2].x ) / 2;
					setOrigin.y =  ( points[1].y + points[2].y ) / 2;
					break;
				case	TextElementJustification::RightBaseline:
					if( vertical == 0 ){
						setOrigin.x = points[1].x;
						setOrigin.y = points[1].y;
					}
					else{
						setOrigin.x = points[2].x;
						setOrigin.y = points[2].y;
					}
					break;
				default:
					break;
			}
}

/*===========================================================================*/






/*----------------------------------------------------------------------+
|                                                                       |
| name		userState_get                                          |
|									|
+----------------------------------------------------------------------*/
void	userState_get()
{
	MSElement	element;
	int		err;
	DgnModelRefP pModelRef;
	ULong		filePos;
	int		type;

			/*------------------------------------------------------+
			|	�v�f�̃t�@�C���ʒu���擾(5-34)			|
			+------------------------------------------------------*/
			filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &pModelRef);

			/*------------------------------------------------------+
			|	�f�U�C���t�@�C������v�f����荞��(5-33)	|
			+------------------------------------------------------*/
			if( pModelRef == MASTERFILE ){
				err = mdlElement_read( &element, pModelRef, filePos );
				if( err == SUCCESS ){
					/*------------------------------------------------------+
					|	�v�f����v�f�̃^�C�v�𒊏o(5-36)		|
					+------------------------------------------------------*/
					type = mdlElement_getType( &element );
					if( type == 17 || type == 7 ){
						KPieces_Bunkai( filePos  );
					}
				}
			}
		start_sin();


}
/*----------------------------------------------------------------------+
|                                                                       |
| name		userState_clean                                         |
|									|
+----------------------------------------------------------------------*/
void	userState_clean()
{
	mdlSelect_freeAll();
	mdlState_clear();


}


Private void start_sin( )
{
				/*------------------------------------------------------+
				|	�v�f�w��J�[�\���ɂ���(7-16)			|
				+------------------------------------------------------*/
				mdlLocate_setCursor( );
				/*------------------------------------------------------+
				|	�Ăяo���������[�U�[�֐����w�肷��(7-17)	|
				+------------------------------------------------------*/
				LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)userState_get );
				/*------------------------------------------------------+
				|	�C���R�}���h���J�n(1-9)				|
				+------------------------------------------------------*/
				mdlState_startModifyCommand((StateFunc_Reset)userState_clean, (MdlFunc_Accept)userState_get, NULL, NULL, NULL, NULL, NULL, TRUE, 0 );
}
/*----------------------------------------------------------------------+
|                                                                       |
| name		Practice                                                   |
|									|
+----------------------------------------------------------------------*/
Private void KPieces_Practice(DialogItemMessage *dmP)
{
	FILE *fin;
	int	i, nnIdx, err;
	WChar	strbuf[MAXFILELENGTH];
	WChar	jstring[100];


	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
		case DITEM_MESSAGE_BUTTON:
		{


			Set_ini();

			if( kpiecesGlobals.jiko != 2 ){
				if( kpiecesGlobals.idxggle == -1 ){			/* �f�U�C���t�@�C���̃Z�b�g */
					if( ! wcscmp( kpiecesGlobals.idxname, L"" ) || kpiecesGlobals.tbli == -1 && ! wcscmp( kpiecesGlobals.tblname, L"" ) || kpiecesGlobals.log == -1 && ! wcscmp( kpiecesGlobals.logname, L"" )){
							mdlDialog_openInfoBox( L"��Ɗ����m�F���Ă�������" );
					}
					else{
						fin = _wfopen( kpiecesGlobals.idxname, L"r" );
						nnIdx = 0;
						while( NULL != fgetws( strbuf, 256, fin)){
							nnIdx++;
						}
						fclose( fin );
						fin = _wfopen( kpiecesGlobals.idxname, L"r" );
							if( kpiecesGlobals.log == -1 ){
								flog = _wfopen( kpiecesGlobals.logname, L"w" );
							}
						i = 1;
						while( EOF != fwscanf(fin,L"%ls", strbuf)){
							wcscpy( jstring, L"" );
							swprintf( jstring, L"%d/%d > %ls", i, nnIdx, strbuf);
							err = check_Design( strbuf );
							if( err == 0 ){
								/*----------------------------------------------+
								|    �f�U�C���t�@�C���̂n�o�d�m(4-8)		|
								+----------------------------------------------*/
									mdlSystem_newDesignFile( strbuf );
#if DEBUG
printf("Open File Name = %s\n", string );
#endif
								KPieces_Apply( jstring );
							}
						
							i++;
						}
						fclose( fin );
						if( kpiecesGlobals.log == -1 ){
							fclose( flog );
						}
					}
				}
				else{
					if(  kpiecesGlobals.tbli == -1 && !  wcscmp( kpiecesGlobals.tblname, L"" ) || kpiecesGlobals.log == -1 && ! wcscmp( kpiecesGlobals.logname, L"" )){
							mdlDialog_openInfoBox( L"��Ɗ����m�F���Ă�������" );
					}
					else{
						wcscpy( strbuf, L"");
						wcscpy( strbuf, tcb->dgnfilenm);		/* �A�N�e�B�u�t�@�C�������擾 */
						check_Design( strbuf );
						wcscpy( jstring, L"" );
						wcscpy( jstring, L"������");
						KPieces_Apply( jstring );
					}
				}
			}
			else{
					wcscpy( strbuf, L"");
					wcscpy( strbuf, tcb->dgnfilenm);		/* �A�N�e�B�u�t�@�C�������擾 */
					check_Design( strbuf );

					start_sin();

			}

		  break;
		};
		default:
			dmP -> msgUnderstood = TRUE;
			break;
		}

}


/*----------------------------------------------------------------------+
|                                                                       |
| name		����                                                   |
|									|
+----------------------------------------------------------------------*/
void	KPieces_Bunkai(	ULong	setFilepos)
{
  int		i, j,err;
  int		moj, Error;
  FILE	    *fin;
  int		cSpace1;
  long		cSsize;
// ���x����
UInt32	lv, co, wt;
int st;
int	tbllv, tblco, tblwt, tblsy;
int	tblft, tateyoko;
int	tblth, tbltw, tbltp;
WChar	tbljtf[10], buf[500];
MSElement	 ellBuf, OutellBuf;

			/*--------------------------------------------------------------+
			|    �s�d�w�s�̎擾						|
			+--------------------------------------------------------------*/
			/*------------------------------------------------------+
			|	�f�U�C���t�@�C������v�f����荞��(5-33)	|
			+------------------------------------------------------*/
			err = mdlElement_read( &ellBuf, MASTERFILE, setFilepos );

				/*------------------------------------------------------+
				|   ���x���̎擾 (5-38) mdlElement_getProperties	|
				+------------------------------------------------------*/
				mdlElement_getProperties( &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ellBuf);
				/*------------------------------------------------------+
				|   ���x���ȊO�̎擾 (5-36) mdlElement_getSymbology	|
				+------------------------------------------------------*/
				mdlElement_getSymbology( &co, &wt, &st, &ellBuf);

#if DEBUG
printf(" level  = %d\n", lv);
printf(" color  = %d\n weight = %d\n style  = %d\n", co, wt, st);
printf(" tbli : %d\n", kpiecesGlobals.tbli );
printf(" ellBuf.ehdr.complex : %d\n", ellBuf.ehdr.complex);
#endif


				/*------------------------------------------------------+
				|   �s�d�w�s���擾 (28-57) mdlText_extractWide	|
				+------------------------------------------------------*/
                mdlText_extract( NULL, NULL, NULL, NULL, &WtextSize, &WtextParamWide, NULL, &ellBuf);
				vertical = WtextParamWide.flags.vertical;
#if DEBUG
printf("WtextParamWide viewIndependent : %d\n", WtextParamWide.viewIndependent );
#endif
			err = -1;
		if( kpiecesGlobals.tbli == -1 ){

			if( NULL != (fin = _wfopen( kpiecesGlobals.tblname, L"r" )) ){
				while( NULL != fgetws( buf, 128, fin ) ){
					if( ! wcsncmp( buf, L";", 1 ) ){
					}
					else{
						swscanf( buf, L" %d  %d %d %d   %d  %d %d %d %d %ls", &tbllv, &tblco, &tblwt, &tblsy,
									&tateyoko, &tblft, &tblth, &tbltw, &tbltp, tbljtf);
						if( lv == tbllv ){
							err = 0;
							if( tblco >= 0  && co != tblco ){
								err = 1;
							}
							else{
								if( tblwt >= 0 && wt != tblwt ){
									err = 1;
								}
								else{
									if( tblsy >= 0 && st != tblsy ){
										err = 1;
									}
								}
							}
							if( ! wcsncmp( tbljtf, L"-", 1 ) ){
							}
							else{
								TextElementJustification setjust;
								TextElementJustification Invalid = TextElementJustification::Invalid;
								
								if( ! wcscmp( tbljtf,L"LT" ) ){		setjust = TextElementJustification::LeftTop;		}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"LC" ) ){		setjust = TextElementJustification::LeftMiddle;		}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"LB" ) ){		setjust = TextElementJustification::LeftBaseline;   }else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"CT" ) ){		setjust = TextElementJustification::CenterTop;		}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"CC" ) ){		setjust = TextElementJustification::CenterMiddle;	}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"CB" ) ){		setjust = TextElementJustification::CenterBaseline;	}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"RT" ) ){		setjust = TextElementJustification::RightTop;		}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"RC" ) ){		setjust = TextElementJustification::RightMiddle;	}else{ setjust = Invalid; }
								if( ! wcscmp( tbljtf,L"RB" ) ){		setjust = TextElementJustification::RightBaseline;  }else{ setjust = Invalid; }

									kpiecesGlobals.tjust = kpiecesGlobals.tjust = (int)setjust;
							}
							break;
						}
						else{
							err = -1;
						}
					}
				}
			}
			else{
				mdlDialog_openInfoBox( L"�s�a�k�t�B�� �n�������@�d��������" );
			}
			fclose( fin );
#if DEBUG
printf("tbl file close\n");
#endif
		}
		else{
			if( 0  == chk_level( lv, co, wt, st ) ){
				err = 0;
				if( kpiecesGlobals.ftchk == TRUE ){
					if( kpiecesGlobals.ft == chk_font(  WtextParamWide.font ) ){
						err = 0;
					}
					else{
						err = -1;
					}
				}
			}
			else{
				err = -1;
			}
		}


		if( err == 0 ){
#if DEBUG
printf("levelschk : %d    chk_level : %d\n", kpiecesGlobals.levelschk, err );
#endif
				/*------------------------------------------------------+
				|   �����o�C�g��					|
				+------------------------------------------------------*/
				if( dgntype == 2 ){
					//textByte = ellBuf.text_2d.numWChars;			// �Q�����p
					textByte = ellBuf.text_2d.numchars;
				}
				if( dgntype == 3 ){
					//textByte = ellBuf.text_3d.numWChars;         // �R�����p
					textByte = ellBuf.text_3d.numchars;
				}
#if DEBUG
printf("Byte = %d\n", textByte);
#endif
				/*--------------------------------------------------------------+
				|    �����v�f������𒊏o���� (5-20) mdlText_extract		|
				+--------------------------------------------------------------*/
				mdlText_extract( &string, &origin, &userOrigin, &rMatrix, &textSize, NULL,
								    &textEDParam, &ellBuf);
				memcpy( &tileSize, &textSize.size, sizeof(mstextsize));
#if DEBUG
printf(" �������W�@   x = %12.4lf  y = %12.4lf  z = %12.4lf\n", origin.x/1000, origin.y/1000, origin.z/1000);
printf(" �X�i�b�v���W x = %12.4lf  y = %12.4lf  z = %12.4lf\n", userOrigin.x/1000, userOrigin.y/1000, userOrigin.z/1000);
//printf(" �������͗��̐� = %d\n", numEdfields);
printf(" ����           = %s\n", string );
#endif
				textMoj = wcslen( string.GetWCharCP() );

#if DEBUG
printf("������ : %d\n", textMoj);
#endif
				if( textByte != textMoj ){
					textMoj = (textByte-2)/2;
				}
				else {
					//tileSize.width = tileSize.width / 2;
					tileSize.width = tileSize.width;
				}
				/*--------------------------------------------------------+
				|   ��] Matrix ����p�x�֕ϊ� (20-10) mdlMatrix_toAngle  |
				+--------------------------------------------------------*/
				textMatrix = mdlRMatrix_toAngle( &rMatrix )*fc_180overpi;
				/*----------------------------------------------+
				|   �����v�f�ɊO�ڂ���Z�`(5-21)		|
				+----------------------------------------------*/
				Error = mdlText_extractShape(points, &userOrigin, &ellBuf, FALSE, FALSE);
				if( vertical == 0 ){
					angle = dkisan( points[0].x, points[0].y, points[1].x, points[1].y );
				}
				else{
					Text_ExtractShape_a(&ellBuf, points);

					angle = dkisan( points[0].x, points[0].y, points[3].x, points[3].y );
				}

		if( textMoj > 0 ){


			/*--------------------------------------------------------------------------------------+
			|     �v�f�̕���								 	|
			+--------------------------------------------------------------------------------------*/
			wcscpy( stringmoj, L"" ); moj = 0;
			cSpace1 = 0;
			if( kpiecesGlobals.syori != 3 ){
				j = 0;
				wcscpy( stringb, L"" );
				for( i=0 ; i < textMoj ; i++ ){
					if( i == 0 ){
						userOrigin.x = points[0].x;
						userOrigin.y = points[0].y;
						if (vertical != 0) {
							userOrigin.x = points[3].x;
							userOrigin.y = points[3].y;

						}
						if( vertical == 0 ){
							WtextParamWide.just = TextElementJustification::LeftBaseline;
						}
						if( vertical == 1 ){
							WtextParamWide.just = TextElementJustification::LeftTop;
						}

					}
					else{
						if( vertical == 0 ){
							WtextParamWide.just = TextElementJustification::LeftBaseline;
							if( cSpace1 > 0 ){
//									cSsize = tileSize.width / 2 * cSpace1-tileSize.width;
							}
cSsize = 0;
							userOrigin.x = x_kis( userOrigin.x, angle, tileSize.width+WtextParamWide.characterSpacing+cSsize);
							userOrigin.y = y_kis( userOrigin.y, angle, tileSize.width+WtextParamWide.characterSpacing+cSsize );
						}
						else{
							WtextParamWide.just = TextElementJustification::LeftTop;
							userOrigin.x = x_kis( userOrigin.x, angle, tileSize.height+WtextParamWide.characterSpacing);
							userOrigin.y = y_kis( userOrigin.y, angle, (tileSize.height*-1)+WtextParamWide.characterSpacing*-1 );
						}
					}
					if( moj == 0 ){
						setOrigin.x = userOrigin.x;
						setOrigin.y = userOrigin.y;
					}


					/*------------------------------------------------+
					|	�P�������o				  |
					+------------------------------------------------*/
					wcscpy( stringb, L"");
					if( textByte == textMoj ){
						wcsncat(stringb, i+string.GetWCharCP(), 1 );
						if( ! wcscmp( stringb, L"1" ) || ! wcscmp( stringb, L"�P") ){
							cSpace1 ++ ;
						}
					}
					else{
						wcsncat(stringb, i-j+string.GetWCharCP(), 1);
						if( ! wcscmp( stringb, L"1" ) || ! wcscmp( stringb, L"�P") ){
							cSpace1 ++ ;
						}
						if( ! wcscmp( stringb, L" ")){
							j++;
						}
						else{
							wcscpy( stringb, L"");
							//wcsncat(stringb, i*2-j+string.GetWCharCP(), 2);
							wcsncat(stringb, i - j + string.GetWCharCP(), 1);
						}
					}
					//-------- �d�m�c�@�P�������o ---------------
					if( ! wcscmp( stringb, L" ") || ! wcscmp( stringb, L"�@")){
						if( kpiecesGlobals.syori == 2 ){
							/*----------------------------------------------+
							|    �����v�f���쐬 (28-56) mdlText_createWide	|
							+----------------------------------------------*/
								//mbstowcs(stringwmoj, (char*)stringmoj, moj+1);
							wcscpy(stringwb, stringb);
							WtextParamWide.flags.vertical = vertical;
							mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
                            Text_ExtractShape_a(&OutellBuf, points);
							Text_Extract(&OutellBuf, NULL, NULL, &setOrigin);
				            set_JustOrigin();
							mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
							/*----------------------------------------------+
							|    �v�f��ǉ� (5-28) mdlElement_add		|
							+----------------------------------------------*/
							mdlElement_add( &OutellBuf );
							wcscpy( stringmoj, L"" ); moj = 0;
						}
					}
					else{
						if( kpiecesGlobals.syori == 1 ){
							/*----------------------------------------------+
							|    �����v�f���쐬 (28-56) mdlText_createWide	|
							+----------------------------------------------*/
							//mbstowcs(stringwb, (char*)stringb, 2);
							wcscpy(stringwb, stringb);

							WtextParamWide.flags.vertical = vertical;
							mdlText_create( &OutellBuf, &ellBuf, stringwb, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
							Text_ExtractShape_a(&OutellBuf, points);
							Text_Extract(&OutellBuf, NULL, NULL, &userOrigin);
				            set_JustOrigin();
							mdlText_create( &OutellBuf, &ellBuf, stringwb, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
							/*----------------------------------------------+
							|    �v�f��ǉ� (5-28) mdlElement_add		|
							+----------------------------------------------*/
							mdlElement_add( &OutellBuf );
						}
						else{
							wcscat( stringmoj, stringb );
							moj++;
						}
					}
				}

				if( kpiecesGlobals.syori == 2 && moj != 0 ){
					/*----------------------------------------------+
					|    �����v�f���쐬 (28-56) mdlText_createWide	|
					+----------------------------------------------*/
					//mbstowcs(stringwmoj, (char*)stringmoj, moj+1 );
					WtextParamWide.flags.vertical = vertical;
					mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
					Text_ExtractShape_a(&OutellBuf, points);
					Text_Extract(&OutellBuf, NULL, NULL, &setOrigin);

				    set_JustOrigin();
					mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
					/*----------------------------------------------+
					|    �v�f��ǉ� (5-28) mdlElement_add		|
					+----------------------------------------------*/
					mdlElement_add( &OutellBuf );
					wcscpy( stringmoj, L"" ); moj = 0;

				}
				/*--------------------------------------------------------------------------------------+
				|     �v�f�̃f���[�g (5-30) mdlElement_undoableDelete (5-32) mdlElement_igdsSize	|
				+--------------------------------------------------------------------------------------*/
				mdlElement_undoableDelete( &ellBuf, setFilepos, TRUE );
				/*------------------------------------------------------+
				|    �S�Ẵr���[�ɕ\��	(5-29) mdlElement_display	|
				+------------------------------------------------------*/
				mdlElement_display( &OutellBuf, DRAW_MODE_Normal);
			}
			else{

				set_JustOrigin();
				/*----------------------------------------------+
				|    �����v�f���쐬 (28-56) mdlText_createWide	|
				+----------------------------------------------*/
				//mbstowcs(stringwmoj, (char*)&string, textMoj+1);
				WtextParamWide.flags.vertical = vertical;
				mdlText_create( &OutellBuf, &ellBuf, string.GetWCharCP(), &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
				/*----------------------------------------------+
				|    �v�f��ǉ� (5-28) mdlElement_add		|
				+----------------------------------------------*/
				mdlElement_add( &OutellBuf );

				/*--------------------------------------------------------------------------------------+
				|     �v�f�̃f���[�g (5-30) mdlElement_undoableDelete (5-32) mdlElement_igdsSize	|
				+--------------------------------------------------------------------------------------*/
				mdlElement_undoableDelete( &ellBuf, setFilepos, TRUE );
				/*------------------------------------------------------+
				|    �S�Ẵr���[�ɕ\��	(5-29) mdlElement_display	|
				+------------------------------------------------------*/
				mdlElement_display( &OutellBuf, DRAW_MODE_Normal);

			}
		}
		}
}

/*----------------------------------------------------------------------+
|                                                                       |
| name		Apply                                                   |
|									|
+----------------------------------------------------------------------*/
void	KPieces_Apply( WChar *jstring)
{
  int		getSize, Error;
  ULong		filePos, eoffilePos, setfilePos;
  ExtScanList	scanList;


// �i�s�󋵕\��
MSDialog 	*dbp;

	MSElement    	 ellBuf;

		if( kpiecesGlobals.backup == -1 ){
			WChar	fileName[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	�t�@�C�����̍\���v�f�𒊏o����B(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	���O�̍\���v�f�Ɋ�Â��ăt�@�C�������쐬����B(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( fileName, devP, dirP, name, L"bak" );
			/*--------------------------------------------------------------+
			|	�t�@�C�����R�s�[����B(25-12)				|
			+--------------------------------------------------------------*/
			mdlFile_copy( fileName, tcb->dgnfilenm );

		}

		if( kpiecesGlobals.CompressDesign == -1 ){
			/*----------------------------------------------+
			|	�f�U�C���t�@�C���̈��k(4-19)		|
			+----------------------------------------------*/
			mdlSystem_compressDgnFile();
		}
		/*----------------------------------------------------------------------+
		| ����������ݒ� 				                        |
		+----------------------------------------------------------------------*/
		/*----------------------------------------------------------------------+
		| �X�L�������X�g�̏������� 			                        |
		+----------------------------------------------------------------------*/
		  mdlScan_initScanlist( &scanList );		/* �X�L�������X�g������������ */
		  mdlScan_setDrawnElements( &scanList );	/* �\���\��MicroStation�v�f��S�Ė߂��悤�ɁA�X�L�������X�g�̃^�C�v�}�X�N��ݒ肷�� */
		  mdlScan_noRangeCheck( &scanList );		/* �f�U�C�����ʑS�Ă��X�L��������悤�ɁA�X�L�������X�g�͈̔͂�ݒ肷�� */

		/*----------------------------------------------------------------------+
		| scanList�̊e�����o�[�Ɉȉ����Z�b�g����B 			        |
		+----------------------------------------------------------------------*/
		  scanList.scantype |= (unsigned short)ELEMDATA;	// if true, store data 
		  scanList.scantype |= (unsigned short)ONEELEM;		// if true, get only one element
		  scanList.scantype |= (unsigned short)ELEMTYPE;	// if true, compare on element type
//		  scanList.scantype |= (unsigned short)GRPHGRP;		// if true, compare on graphics group

		  scanList.typmask[0] |= TMSK0_TEXT_NODE;
		  scanList.typmask[0] |= TMSK1_TEXT;

		/*----------------------------------------------------------------------+
		| �X�L�������s�B 				                        |
		+----------------------------------------------------------------------*/
		  if (mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
			mdlOutput_prompt(L"�X�L�������X�g���󂯓�����܂���ł����B");
			mdlOutput_message(L"�A�����[�h���܂����B");
			mdlSystem_exit( exitStatus, 1);
		  }
		/*----------------------------------------------------------------------+
		|   �v�f�̃t�@�C���ʒu���擾 (5-34) mdlElement_getFilePos		|
		+----------------------------------------------------------------------*/
		eoffilePos = mdlElement_getFilePos( FILEPOS_EOF, 0);
		/*------------------------------------------------------------------------------+
		|   �i�s�󋵕\���E�C���h�E���I�[�v������ (3-77) mdlDialog_openCompletionBar	|
		+------------------------------------------------------------------------------*/
		dbp = mdlDialog_completionBarOpen( jstring );
		/*----------------------------------------------------------------------+
		|   �k�n�f�t�@�C���̍쐬						|
		+----------------------------------------------------------------------*/
		if( kpiecesGlobals.log == -1 && kpiecesGlobals.idxggle != -1 ){
			flog = _wfopen( kpiecesGlobals.logname, L"w" );
		}
		if( kpiecesGlobals.log == -1 ){
				fwprintf( flog, L"�f�U�C���t�@�C���� : %ls\n", tcb->dgnfilenm );
		}

		/*----------------------------------------------------------------------+
		|   �f�[�^�̎擾							|
		+----------------------------------------------------------------------*/
		  while(TRUE){
			/*--------------------------------------------------------------+
			|    �f�U�C���t�@�C�����X�L�������� (7-7) mdlScan_file		|
			+--------------------------------------------------------------*/
			Error = mdlScan_file(&ellBuf, &getSize, sizeof(ellBuf), &filePos);
#if DEBUG
printf("Err = %d   filePos = %d  type = %d  acceptedsize = %d\n", Error, filePos, ellBuf.ehdr.type, getSize );
#endif
			/*--------------------------------------------------------------+
			|   �d�����@�����@�e�������@�̏���				|
			+--------------------------------------------------------------*/
			if( filePos > eoffilePos ){
				break;
			}
			/*--------------------------------------------------------------+
			|   �G���[�̏���						|
			+--------------------------------------------------------------*/
			if(getSize <= 0){
				break;
			}
			if(Error >= 65){
				break;
			}
			if(Error == 10){
				mdlOutput_error(L"�t�@�C���̏I���Ȃ̂ŃA�����[�h���܂����B");
				mdlSystem_exit(exitStatus, 1);
			}

			/*------------------------------------------------------------------------------+
			|   �s�d�w�s���擾 								|
			+------------------------------------------------------------------------------*/
			  if( ellBuf.ehdr.type == TEXT_ELM || ellBuf.ehdr.type == TEXT_NODE_ELM ){
#if DEBUG
printf("****** ������� �擾  �^�C�v = %d  FilePos = %d ******\n", ellBuf.ehdr.type, filePos );
#endif
				//setfilePos = filePos - (ULong) mdlElement_igdsSize( &ellBuf, 0 )*2;
                setfilePos = filePos - 1;
				if (ellBuf.ehdr.type == TEXT_NODE_ELM) {
					MSElementDescr	*elemDscrPP;
					int		 nodetype;

					//5-48
					mdlElmdscr_read(&elemDscrPP, setfilePos, MASTERFILE, 0, NULL);

					//for( ; elemDscrPP != NULL ; elemDscrPP = elemDscrPP->h.next ){

					nodetype = mdlElement_getType(&elemDscrPP->el);
					/*--------------------------------------------------------------+
					|    �����v�f������𒊏o���� (5-20) mdlText_extract		|
					+--------------------------------------------------------------*/
					//						mdlText_extract( NULL, &logOrigin, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ellBuf);
											/*------------------------------------------------------+
											|   �s�d�w�s���擾 (28-57) mdlText_extractWide	|
											+------------------------------------------------------*/
											//						mdlText_extractWide( NULL, NULL, &logOrigin, NULL, NULL, NULL, NULL, &ellBuf);

					
											//						fprintf( flog,"\t�����m�[�h�����o (%4d ) : %11ld   %11.3lf   %11.3lf\n", nodetype, setfilePos, ellBuf.origin.x/1000, logOrigin.y/1000);
					if (kpiecesGlobals.log == -1) {
						fwprintf(flog, L"\t�����m�[�h�����o (%4d ) : %11ld\n", nodetype, setfilePos);
					}
				    //}
					mdlElmdscr_freeAll(&elemDscrPP);
//					mdlElmdscr_undoableDelete( &elemDscrPP, setfilePos, TRUE );

				}

				if( ellBuf.ehdr.type == TEXT_ELM && ellBuf.ehdr.complex != 1 ){
					KPieces_Bunkai( setfilePos  );
				}
			  }
			/*------------------------------------------------------------------------------+
			|   �d�m�c �s�d�w�s���								|
			+------------------------------------------------------------------------------*/

			/*------------------------------------------------------------------------+
			|     �i�s�󋵕\���E�C���h�E���X�V (3-78) mdlDialog_updateCompletionBar	  |
			+------------------------------------------------------------------------*/
			  mdlDialog_completionBarUpdate( dbp, jstring, filePos*100/eoffilePos );

		  }
		if( kpiecesGlobals.log == -1 && kpiecesGlobals.idxggle != -1 ){
			fclose( flog );
		}

		/*------------------------------------------------------------------------------+
		|     �i�s�󋵕\���E�C���h�E���N���[�Y (3-78) mdlDialog_closeCompletionBar	|
		+------------------------------------------------------------------------------*/
		mdlDialog_completionBarClose( dbp );

}

void tmpelmdsp(MSElement elm, Dpoint3d *pts)
{
	/*------------------------------------------------------+
	|	�A�������v�f���쐬(5-5)				|
	+------------------------------------------------------*/
	mdlShape_create(&elm, NULL, pts, 5, -1);
	/*------------------------------------------------------+
	|	�f�U�C���t�@�C���֒ǉ�			|
	+------------------------------------------------------*/
	mdlElement_add(&elm);
	/*------------------------------------------------------+
	|    �S�Ẵr���[�ɕ\��	(5-29) mdlElement_display	|
	+------------------------------------------------------*/
	mdlElement_display(&elm, DRAW_MODE_Normal);





}


/// <summary>
/// �e�L�X�g�v�f�̎l���̍��W�𓾂�
/// </summary>
/// <param name="elmP"></param>
/// <param name="points">�������獶���ŁA�N�_���܂߂�5�_</param>
/// <returns>SUCCESS or ERROR</returns>
int Text_ExtractShape_a(MSElementP elmP, DPoint3d *points)
{
	// �e�L�X�g�E�e�L�X�g�m�[�h�̏ꍇ
	if (elmP->ehdr.type == TEXT_ELM || elmP->ehdr.type == TEXT_NODE_ELM)
	{
		// ���A�����A�񂹈ʒu�̎擾
		EditElementHandle eh(elmP, MASTERFILE);
		TextBlockPtr textBlock = TextHandlerBase::GetFirstTextPartValue(eh);
		if (textBlock.IsValid())
		{
			DPoint3d userOrigin = textBlock->GetUserOrigin();
			DRange3d range = textBlock->GetNominalRange();
			RotMatrix rotMatrix = textBlock->GetOrientation();
			DVec3d vec;
			double angle = rotMatrix.GetRotationAngleAndVector(vec);	// atan2�ƈ���ĉE��肪�v���X�ɂȂ�

			points[0] = range.low;
			points[1] = range.low; points[1].x = range.high.x;
			points[2] = range.high;
			points[3] = range.high; points[3].x = range.low.x;
			points[4] = range.low;


			DPoint3d shift = DPoint3d::From(0, 0, 0);
			CaretPtr caret = textBlock->CreateStartCaret();
			TextElementJustification just = caret->GetCurrentParagraphCP()->GetProperties().GetJustification();
			switch (just)
			{
			case Bentley::DgnPlatform::TextElementJustification::LeftTop:
				shift = points[3];
				break;
			case Bentley::DgnPlatform::TextElementJustification::LeftMiddle:
				get_tyuuten(&shift, &points[0], &points[3]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::LeftBaseline:
				shift = points[0];
				break;
				//case Bentley::DgnPlatform::TextElementJustification::LeftDescender:
				//	break;
			case Bentley::DgnPlatform::TextElementJustification::CenterTop:
				get_tyuuten(&shift, &points[2], &points[3]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::CenterMiddle:
				DPoint3d ct, cb;
				get_tyuuten(&ct, &points[2], &points[3]);
				get_tyuuten(&cb, &points[0], &points[1]);
				get_tyuuten(&shift, &ct, &cb);
				break;
			case Bentley::DgnPlatform::TextElementJustification::CenterBaseline:
				get_tyuuten(&shift, &points[0], &points[1]);
				break;
				//case Bentley::DgnPlatform::TextElementJustification::CenterDescender:
				//	break;
			case Bentley::DgnPlatform::TextElementJustification::RightTop:
				shift = points[2];
				break;
			case Bentley::DgnPlatform::TextElementJustification::RightMiddle:
				get_tyuuten(&shift, &points[1], &points[2]);
				break;
			case Bentley::DgnPlatform::TextElementJustification::RightBaseline:
				shift = points[1];
				break;

			default:
				return ERROR;
				break;
			}

			// �N�_�����_�ɂ���
			for (int i = 0; i < 5; i++)
				points[i].Subtract(shift);

			// ��]������
			for (int i = 0; i < 5; i++)
				points[i].RotateXY(-angle);

			// �����W�ֈړ�
			for (int i = 0; i < 5; i++)
				points[i].Add(userOrigin);

			//test
			double angle2 = atan2(points[1].y - points[0].y, points[1].x - points[0].x);


			//test
			//MSElement ele;
			//mdlLineString_create(&ele, NULL, points, 5);
			////mdlElement_setSymbology(&ele, co, wt, lc);
			//mdlElement_add(&ele);
			//mdlElement_display(&ele, DgnDrawMode::DRAW_MODE_Normal);

		}
		else
		{
			return(ERROR);
		}
	}
	else
	{
		return(ERROR);
	}

	return(SUCCESS);
}

