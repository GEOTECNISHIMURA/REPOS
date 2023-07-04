#define	DEBUG	0
#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
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

Private void userState_get( void);									/* Ｄａｔａの取得	      */
Private void userState_clean( void);								/* ＤａｔａのClean	      */
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




// mdlText_extract 宣言
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

// mdlText_extractWide 宣言
//Dpont3d		Worigin, WuserOrigin;
//RotMatrix	WrMatrix;
TextSizeParam	WtextSize;
TextParamWide	WtextParamWide;
TextEDParam		WedPatam;

// mdlText_create 宣言
//TextParam		textParam;

// mdlText_createWide 宣言
TextParamWide	textParamWide;

// Text 分解
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
		|    ボタン等を使用可能・不可能(3-103)		 |
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
      /*--- インデックスファイルオープン ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	ファイル名の構成要素を抽出する。(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	名前の構成要素に基づいてファイル名を作成する。(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  ファイルのＯＰＥＮ(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileOpen( fileName, NULL, 0, kpiecesGlobals.idxname, L"*.idx", directory, L"インデックスファイル");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.idxname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   ダイアログのアップデート(3-106)	    |
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
      /*--- インデックスファイルオープン ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	ファイル名の構成要素を抽出する。(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	名前の構成要素に基づいてファイル名を作成する。(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  ファイルのＯＰＥＮ(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileOpen( fileName, NULL, 0, kpiecesGlobals.tblname, L"*.tbl", directory, L"ＴＢＬファイル");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.tblname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   ダイアログのアップデート(3-106)	    |
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
      /*--- インデックスファイルオープン ---*/
			WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	ファイル名の構成要素を抽出する。(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	名前の構成要素に基づいてファイル名を作成する。(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		/*----------------------------------------------+
		|  ファイルのＯＰＥＮ(25-23)			|
		+----------------------------------------------*/
		err = mdlDialog_fileCreate( fileName, NULL, 0, kpiecesGlobals.logname, L"*.log", directory, L"ログファイル");
		if ( err == 0 ){
			wcscpy( kpiecesGlobals.logname, fileName.GetWCharCP());
			/*------------------------------------------+
			|   ダイアログのアップデート(3-106)	    |
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
			|    ボタン等を使用可能・不可能(3-103)		 |
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
			|   ダイアログのアップデート(3-106)	    |
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
		|   ダイアログのclose					|
		+--------------------------------------------------------------*/
			WdbP = mdlDialog_find( DIALOGID_Levels, NULL );
	  		mdlDialog_closeCommandQueue( WdbP );	
			/*------------------------------------------+
			|   ダイアログのアップデート(3-106)	    |
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
	mdlOutput_error(L"２次元ファイルです");
	dgntype = 2;
	}
	else{
	mdlOutput_error(L"３次元ファイルです");
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
				//左下原点左回りを左上原点右周りにする（V7に合わせる）
				memcpy(&tmppnt, &points[0], sizeof(Dpoint3d));
				memcpy(&points[0], &points[3], sizeof(Dpoint3d));
				memcpy(&points[3], &tmppnt, sizeof(Dpoint3d));
				memcpy(&tmppnt, &points[1], sizeof(Dpoint3d));
				memcpy(&points[1], &points[2], sizeof(Dpoint3d));
				memcpy(&points[2], &tmppnt, sizeof(Dpoint3d));

			}

			if( kpiecesGlobals.viewIndep == 0 ){
				WtextParamWide.viewIndependent = 0;	//ビュー依存
			}
			else{
				WtextParamWide.viewIndependent = 1;	//向き一定
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
			|	要素のファイル位置を取得(5-34)			|
			+------------------------------------------------------*/
			filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &pModelRef);

			/*------------------------------------------------------+
			|	デザインファイルから要素を取り込む(5-33)	|
			+------------------------------------------------------*/
			if( pModelRef == MASTERFILE ){
				err = mdlElement_read( &element, pModelRef, filePos );
				if( err == SUCCESS ){
					/*------------------------------------------------------+
					|	要素から要素のタイプを抽出(5-36)		|
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
				|	要素指定カーソルにする(7-16)			|
				+------------------------------------------------------*/
				mdlLocate_setCursor( );
				/*------------------------------------------------------+
				|	呼び出したいユーザー関数を指定する(7-17)	|
				+------------------------------------------------------*/
				LocateCallback::SetPostLocateFunction((LocateFunc_LocateFilter)userState_get );
				/*------------------------------------------------------+
				|	修正コマンドを開始(1-9)				|
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
				if( kpiecesGlobals.idxggle == -1 ){			/* デザインファイルのセット */
					if( ! wcscmp( kpiecesGlobals.idxname, L"" ) || kpiecesGlobals.tbli == -1 && ! wcscmp( kpiecesGlobals.tblname, L"" ) || kpiecesGlobals.log == -1 && ! wcscmp( kpiecesGlobals.logname, L"" )){
							mdlDialog_openInfoBox( L"作業環境を確認してください" );
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
								|    デザインファイルのＯＰＥＮ(4-8)		|
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
							mdlDialog_openInfoBox( L"作業環境を確認してください" );
					}
					else{
						wcscpy( strbuf, L"");
						wcscpy( strbuf, tcb->dgnfilenm);		/* アクティブファイル名を取得 */
						check_Design( strbuf );
						wcscpy( jstring, L"" );
						wcscpy( jstring, L"処理中");
						KPieces_Apply( jstring );
					}
				}
			}
			else{
					wcscpy( strbuf, L"");
					wcscpy( strbuf, tcb->dgnfilenm);		/* アクティブファイル名を取得 */
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
| name		分解                                                   |
|									|
+----------------------------------------------------------------------*/
void	KPieces_Bunkai(	ULong	setFilepos)
{
  int		i, j,err;
  int		moj, Error;
  FILE	    *fin;
  int		cSpace1;
  long		cSsize;
// レベル等
UInt32	lv, co, wt;
int st;
int	tbllv, tblco, tblwt, tblsy;
int	tblft, tateyoko;
int	tblth, tbltw, tbltp;
WChar	tbljtf[10], buf[500];
MSElement	 ellBuf, OutellBuf;

			/*--------------------------------------------------------------+
			|    ＴＥＸＴの取得						|
			+--------------------------------------------------------------*/
			/*------------------------------------------------------+
			|	デザインファイルから要素を取り込む(5-33)	|
			+------------------------------------------------------*/
			err = mdlElement_read( &ellBuf, MASTERFILE, setFilepos );

				/*------------------------------------------------------+
				|   レベルの取得 (5-38) mdlElement_getProperties	|
				+------------------------------------------------------*/
				mdlElement_getProperties( &lv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ellBuf);
				/*------------------------------------------------------+
				|   レベル以外の取得 (5-36) mdlElement_getSymbology	|
				+------------------------------------------------------*/
				mdlElement_getSymbology( &co, &wt, &st, &ellBuf);

#if DEBUG
printf(" level  = %d\n", lv);
printf(" color  = %d\n weight = %d\n style  = %d\n", co, wt, st);
printf(" tbli : %d\n", kpiecesGlobals.tbli );
printf(" ellBuf.ehdr.complex : %d\n", ellBuf.ehdr.complex);
#endif


				/*------------------------------------------------------+
				|   ＴＥＸＴ情報取得 (28-57) mdlText_extractWide	|
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
				mdlDialog_openInfoBox( L"ＴＢＬフィル Ｏｐｅｎ　Ｅｒｒｏｒ" );
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
				|   文字バイト数					|
				+------------------------------------------------------*/
				if( dgntype == 2 ){
					//textByte = ellBuf.text_2d.numWChars;			// ２次元用
					textByte = ellBuf.text_2d.numchars;
				}
				if( dgntype == 3 ){
					//textByte = ellBuf.text_3d.numWChars;         // ３次元用
					textByte = ellBuf.text_3d.numchars;
				}
#if DEBUG
printf("Byte = %d\n", textByte);
#endif
				/*--------------------------------------------------------------+
				|    文字要素から情報を抽出する (5-20) mdlText_extract		|
				+--------------------------------------------------------------*/
				mdlText_extract( &string, &origin, &userOrigin, &rMatrix, &textSize, NULL,
								    &textEDParam, &ellBuf);
				memcpy( &tileSize, &textSize.size, sizeof(mstextsize));
#if DEBUG
printf(" 左下座標　   x = %12.4lf  y = %12.4lf  z = %12.4lf\n", origin.x/1000, origin.y/1000, origin.z/1000);
printf(" スナップ座標 x = %12.4lf  y = %12.4lf  z = %12.4lf\n", userOrigin.x/1000, userOrigin.y/1000, userOrigin.z/1000);
//printf(" 文字入力欄の数 = %d\n", numEdfields);
printf(" 文字           = %s\n", string );
#endif
				textMoj = wcslen( string.GetWCharCP() );

#if DEBUG
printf("文字数 : %d\n", textMoj);
#endif
				if( textByte != textMoj ){
					textMoj = (textByte-2)/2;
				}
				else {
					//tileSize.width = tileSize.width / 2;
					tileSize.width = tileSize.width;
				}
				/*--------------------------------------------------------+
				|   回転 Matrix から角度へ変換 (20-10) mdlMatrix_toAngle  |
				+--------------------------------------------------------*/
				textMatrix = mdlRMatrix_toAngle( &rMatrix )*fc_180overpi;
				/*----------------------------------------------+
				|   文字要素に外接する短形(5-21)		|
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
			|     要素の分解								 	|
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
					|	１文字抽出				  |
					+------------------------------------------------*/
					wcscpy( stringb, L"");
					if( textByte == textMoj ){
						wcsncat(stringb, i+string.GetWCharCP(), 1 );
						if( ! wcscmp( stringb, L"1" ) || ! wcscmp( stringb, L"１") ){
							cSpace1 ++ ;
						}
					}
					else{
						wcsncat(stringb, i-j+string.GetWCharCP(), 1);
						if( ! wcscmp( stringb, L"1" ) || ! wcscmp( stringb, L"１") ){
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
					//-------- ＥＮＤ　１文字抽出 ---------------
					if( ! wcscmp( stringb, L" ") || ! wcscmp( stringb, L"　")){
						if( kpiecesGlobals.syori == 2 ){
							/*----------------------------------------------+
							|    文字要素を作成 (28-56) mdlText_createWide	|
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
							|    要素を追加 (5-28) mdlElement_add		|
							+----------------------------------------------*/
							mdlElement_add( &OutellBuf );
							wcscpy( stringmoj, L"" ); moj = 0;
						}
					}
					else{
						if( kpiecesGlobals.syori == 1 ){
							/*----------------------------------------------+
							|    文字要素を作成 (28-56) mdlText_createWide	|
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
							|    要素を追加 (5-28) mdlElement_add		|
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
					|    文字要素を作成 (28-56) mdlText_createWide	|
					+----------------------------------------------*/
					//mbstowcs(stringwmoj, (char*)stringmoj, moj+1 );
					WtextParamWide.flags.vertical = vertical;
					mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
					Text_ExtractShape_a(&OutellBuf, points);
					Text_Extract(&OutellBuf, NULL, NULL, &setOrigin);

				    set_JustOrigin();
					mdlText_create( &OutellBuf, &ellBuf, stringmoj, &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
					/*----------------------------------------------+
					|    要素を追加 (5-28) mdlElement_add		|
					+----------------------------------------------*/
					mdlElement_add( &OutellBuf );
					wcscpy( stringmoj, L"" ); moj = 0;

				}
				/*--------------------------------------------------------------------------------------+
				|     要素のデリート (5-30) mdlElement_undoableDelete (5-32) mdlElement_igdsSize	|
				+--------------------------------------------------------------------------------------*/
				mdlElement_undoableDelete( &ellBuf, setFilepos, TRUE );
				/*------------------------------------------------------+
				|    全てのビューに表示	(5-29) mdlElement_display	|
				+------------------------------------------------------*/
				mdlElement_display( &OutellBuf, DRAW_MODE_Normal);
			}
			else{

				set_JustOrigin();
				/*----------------------------------------------+
				|    文字要素を作成 (28-56) mdlText_createWide	|
				+----------------------------------------------*/
				//mbstowcs(stringwmoj, (char*)&string, textMoj+1);
				WtextParamWide.flags.vertical = vertical;
				mdlText_create( &OutellBuf, &ellBuf, string.GetWCharCP(), &setOrigin, NULL, NULL, &WtextParamWide, NULL, MASTERFILE);
				/*----------------------------------------------+
				|    要素を追加 (5-28) mdlElement_add		|
				+----------------------------------------------*/
				mdlElement_add( &OutellBuf );

				/*--------------------------------------------------------------------------------------+
				|     要素のデリート (5-30) mdlElement_undoableDelete (5-32) mdlElement_igdsSize	|
				+--------------------------------------------------------------------------------------*/
				mdlElement_undoableDelete( &ellBuf, setFilepos, TRUE );
				/*------------------------------------------------------+
				|    全てのビューに表示	(5-29) mdlElement_display	|
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


// 進行状況表示
MSDialog 	*dbp;

	MSElement    	 ellBuf;

		if( kpiecesGlobals.backup == -1 ){
			WChar	fileName[1000], devP[5], dirP[1000], name[100], exp[10];

			/*------------------------------------------------------+
			|	ファイル名の構成要素を抽出する。(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	名前の構成要素に基づいてファイル名を作成する。(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( fileName, devP, dirP, name, L"bak" );
			/*--------------------------------------------------------------+
			|	ファイルをコピーする。(25-12)				|
			+--------------------------------------------------------------*/
			mdlFile_copy( fileName, tcb->dgnfilenm );

		}

		if( kpiecesGlobals.CompressDesign == -1 ){
			/*----------------------------------------------+
			|	デザインファイルの圧縮(4-19)		|
			+----------------------------------------------*/
			mdlSystem_compressDgnFile();
		}
		/*----------------------------------------------------------------------+
		| 検索条件を設定 				                        |
		+----------------------------------------------------------------------*/
		/*----------------------------------------------------------------------+
		| スキャンリストの初期化等 			                        |
		+----------------------------------------------------------------------*/
		  mdlScan_initScanlist( &scanList );		/* スキャンリストを初期化する */
		  mdlScan_setDrawnElements( &scanList );	/* 表示可能なMicroStation要素を全て戻すように、スキャンリストのタイプマスクを設定する */
		  mdlScan_noRangeCheck( &scanList );		/* デザイン平面全てをスキャンするように、スキャンリストの範囲を設定する */

		/*----------------------------------------------------------------------+
		| scanListの各メンバーに以下をセットする。 			        |
		+----------------------------------------------------------------------*/
		  scanList.scantype |= (unsigned short)ELEMDATA;	// if true, store data 
		  scanList.scantype |= (unsigned short)ONEELEM;		// if true, get only one element
		  scanList.scantype |= (unsigned short)ELEMTYPE;	// if true, compare on element type
//		  scanList.scantype |= (unsigned short)GRPHGRP;		// if true, compare on graphics group

		  scanList.typmask[0] |= TMSK0_TEXT_NODE;
		  scanList.typmask[0] |= TMSK1_TEXT;

		/*----------------------------------------------------------------------+
		| スキャン実行。 				                        |
		+----------------------------------------------------------------------*/
		  if (mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
			mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
			mdlOutput_message(L"アンロードしました。");
			mdlSystem_exit( exitStatus, 1);
		  }
		/*----------------------------------------------------------------------+
		|   要素のファイル位置を取得 (5-34) mdlElement_getFilePos		|
		+----------------------------------------------------------------------*/
		eoffilePos = mdlElement_getFilePos( FILEPOS_EOF, 0);
		/*------------------------------------------------------------------------------+
		|   進行状況表示ウインドウをオープンする (3-77) mdlDialog_openCompletionBar	|
		+------------------------------------------------------------------------------*/
		dbp = mdlDialog_completionBarOpen( jstring );
		/*----------------------------------------------------------------------+
		|   ＬＯＧファイルの作成						|
		+----------------------------------------------------------------------*/
		if( kpiecesGlobals.log == -1 && kpiecesGlobals.idxggle != -1 ){
			flog = _wfopen( kpiecesGlobals.logname, L"w" );
		}
		if( kpiecesGlobals.log == -1 ){
				fwprintf( flog, L"デザインファイル名 : %ls\n", tcb->dgnfilenm );
		}

		/*----------------------------------------------------------------------+
		|   データの取得							|
		+----------------------------------------------------------------------*/
		  while(TRUE){
			/*--------------------------------------------------------------+
			|    デザインファイルをスキャンする (7-7) mdlScan_file		|
			+--------------------------------------------------------------*/
			Error = mdlScan_file(&ellBuf, &getSize, sizeof(ellBuf), &filePos);
#if DEBUG
printf("Err = %d   filePos = %d  type = %d  acceptedsize = %d\n", Error, filePos, ellBuf.ehdr.type, getSize );
#endif
			/*--------------------------------------------------------------+
			|   Ｅｎｄ　ｏｂ　Ｆｉｌｅ　の処理				|
			+--------------------------------------------------------------*/
			if( filePos > eoffilePos ){
				break;
			}
			/*--------------------------------------------------------------+
			|   エラーの処理						|
			+--------------------------------------------------------------*/
			if(getSize <= 0){
				break;
			}
			if(Error >= 65){
				break;
			}
			if(Error == 10){
				mdlOutput_error(L"ファイルの終わりなのでアンロードしました。");
				mdlSystem_exit(exitStatus, 1);
			}

			/*------------------------------------------------------------------------------+
			|   ＴＥＸＴ情報取得 								|
			+------------------------------------------------------------------------------*/
			  if( ellBuf.ehdr.type == TEXT_ELM || ellBuf.ehdr.type == TEXT_NODE_ELM ){
#if DEBUG
printf("****** 文字情報 取得  タイプ = %d  FilePos = %d ******\n", ellBuf.ehdr.type, filePos );
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
					|    文字要素から情報を抽出する (5-20) mdlText_extract		|
					+--------------------------------------------------------------*/
					//						mdlText_extract( NULL, &logOrigin, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &ellBuf);
											/*------------------------------------------------------+
											|   ＴＥＸＴ情報取得 (28-57) mdlText_extractWide	|
											+------------------------------------------------------*/
											//						mdlText_extractWide( NULL, NULL, &logOrigin, NULL, NULL, NULL, NULL, &ellBuf);

					
											//						fprintf( flog,"\t文字ノードを検出 (%4d ) : %11ld   %11.3lf   %11.3lf\n", nodetype, setfilePos, ellBuf.origin.x/1000, logOrigin.y/1000);
					if (kpiecesGlobals.log == -1) {
						fwprintf(flog, L"\t文字ノードを検出 (%4d ) : %11ld\n", nodetype, setfilePos);
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
			|   ＥＮＤ ＴＥＸＴ情報								|
			+------------------------------------------------------------------------------*/

			/*------------------------------------------------------------------------+
			|     進行状況表示ウインドウを更新 (3-78) mdlDialog_updateCompletionBar	  |
			+------------------------------------------------------------------------*/
			  mdlDialog_completionBarUpdate( dbp, jstring, filePos*100/eoffilePos );

		  }
		if( kpiecesGlobals.log == -1 && kpiecesGlobals.idxggle != -1 ){
			fclose( flog );
		}

		/*------------------------------------------------------------------------------+
		|     進行状況表示ウインドウをクローズ (3-78) mdlDialog_closeCompletionBar	|
		+------------------------------------------------------------------------------*/
		mdlDialog_completionBarClose( dbp );

}

void tmpelmdsp(MSElement elm, Dpoint3d *pts)
{
	/*------------------------------------------------------+
	|	連続線分要素を作成(5-5)				|
	+------------------------------------------------------*/
	mdlShape_create(&elm, NULL, pts, 5, -1);
	/*------------------------------------------------------+
	|	デザインファイルへ追加			|
	+------------------------------------------------------*/
	mdlElement_add(&elm);
	/*------------------------------------------------------+
	|    全てのビューに表示	(5-29) mdlElement_display	|
	+------------------------------------------------------*/
	mdlElement_display(&elm, DRAW_MODE_Normal);





}


/// <summary>
/// テキスト要素の四隅の座標を得る
/// </summary>
/// <param name="elmP"></param>
/// <param name="points">左下から左回りで、起点を含めた5点</param>
/// <returns>SUCCESS or ERROR</returns>
int Text_ExtractShape_a(MSElementP elmP, DPoint3d *points)
{
	// テキスト・テキストノードの場合
	if (elmP->ehdr.type == TEXT_ELM || elmP->ehdr.type == TEXT_NODE_ELM)
	{
		// 幅、高さ、寄せ位置の取得
		EditElementHandle eh(elmP, MASTERFILE);
		TextBlockPtr textBlock = TextHandlerBase::GetFirstTextPartValue(eh);
		if (textBlock.IsValid())
		{
			DPoint3d userOrigin = textBlock->GetUserOrigin();
			DRange3d range = textBlock->GetNominalRange();
			RotMatrix rotMatrix = textBlock->GetOrientation();
			DVec3d vec;
			double angle = rotMatrix.GetRotationAngleAndVector(vec);	// atan2と違って右回りがプラスになる

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

			// 起点を原点にする
			for (int i = 0; i < 5; i++)
				points[i].Subtract(shift);

			// 回転させる
			for (int i = 0; i < 5; i++)
				points[i].RotateXY(-angle);

			// 実座標へ移動
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

