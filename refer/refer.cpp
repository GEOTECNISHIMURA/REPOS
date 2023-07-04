#define	DEBUG		0
#define BAT			1
#define PRGNAME		"refer"
#define INIEXT		".ini"
#define DGNTYPE		0
#define LOCK 0
/*- 0:アンロック,  1:ロック -*/
/*----------------------------------------------------------------------+
|  プログラム名 :main.mc                                            	|
|  Vision       :	|
|  目的		:ＭＦＣ内の検索						|
|		:	|
|  日付		:                                           	|
|  作者		:T.Akamatsu			                 	|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include	"include/includef.h"
//#include    "include/err_s.h"
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include "../CommonCE/check_kigen8.cpp"


#include    "refer.h"	    /* txreso dialog box example constants & structs */
#include    "refertxt.h"    /* txreso dialog box command numbers */

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

/*----------------------------------------------------------------------+
|	Defines								|
+----------------------------------------------------------------------*/

#define		REFER_PAR_CLASS		'RePr'

#ifndef Private
#define Private
#endif
/*----------------------------------------------------------------------+
|グローバル変数の定義		                                    	|
+----------------------------------------------------------------------*/
//#include    "globals.h"
SetRefer	 setRefer;
//AcTive		acTive;

MSElement	 GellBuf;
Private int 	exitStatus;


SetRefer	referPar = {
		L"",			//MFC名
		 };

RscFileHandle	rFileH;

/*----------------------------------------------------------------------+
|ユーザー定義関数のプロトタイプ宣言                                    	|
+----------------------------------------------------------------------*/
#include    "private.h"
#include    "include/kansuu.h"
#include    "element_get.h"
#include    "standard.h"


/*----------------------------------------------------------------------+
|	users 変数の定義		                               	|
+----------------------------------------------------------------------*/
MSDialog	    *MdbP;	/* main a ptr to a dialog box */
FILE			*flog;

#if DEBUG
   FILE	*ftmp;
   WChar	tmpFileName[256];
#endif


time_t	ltime;
WChar	strtime[100];
WChar	timetuk[10], timenen[100], timew[100];


/*----------------------------------------------------------------------+
|									|
|	アンロード関数							|
|									|
+----------------------------------------------------------------------*/
Private	bool	unload(UnloadProgramReason unloadType);
Private	bool	unload(UnloadProgramReason unloadType)
{
	RscFileHandle	userPrefsH;
	void			*rscP;

#if DEBUG
   fclose( ftmp);
#endif


    mdlDialog_userPrefFileOpen(&userPrefsH, TRUE);
//	rscP = mdlResource_load(userPrefsH, REFER_PAR_CLASS, 1);
//	if (rscP != NULL) {
		mdlResource_delete(userPrefsH, REFER_PAR_CLASS, 1);
		mdlResource_add(userPrefsH, REFER_PAR_CLASS, 1, &setRefer, sizeof(SetRefer), NULL);
//	}
//	else
//	{
//		mdlResource_add(userPrefsH, REFER_PAR_CLASS, 1, &referPar, sizeof(referPar), NULL);
//	}
	mdlResource_closeFile(userPrefsH);

	return( SUCCESS);
}


Private	void	chk_dgn( void);
/*----------------------------------------------------------------------+
|                                                                       |
| name		chk_dgn                                                 |
|									|
+----------------------------------------------------------------------*/
Private	void	chk_dgn( void)
{
	int	error;

	if ( DGNTYPE != 0){
		error = DgnFile_Chk( );
#if DEBUG
   printf(" DGNTYPE : %d\n", error);
#endif
		switch( error){
			case	2:
				mdlDialog_openInfoBox( ERR_DGN_TYPE2);
				mdlSystem_exit( NULL, 1);
				break;

			case	3:
				mdlDialog_openInfoBox( ERR_DGN_TYPE3);
				mdlSystem_exit( NULL, 1);
				break;

			default:
				break;
		}
	}
}


Private	void	Set_Button( int dialogBox, int dialogItem, WChar *Dstring, WChar *string, int moj );
/*----------------------------------------------------------------------+
|                                                                       |
| name		Set_Button                                             |
|									|
+----------------------------------------------------------------------*/
Private	void	Set_Button( int dialogBox, int dialogItem, WChar *Dstring, WChar *string, int moj )
{
		int	err, itemIndex;
		WChar	fileName[1000];
		WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];
		MSDialog  *WdbP;
		DialogItem *WitemP;

				/*------------------------------------------+
				|   ダイアログのポインタ(3-87)		    |
				+------------------------------------------*/
				WdbP = mdlDialog_find( dialogBox, NULL );
				/*------------------------------------------+
				|   (3-98)		    |
				+------------------------------------------*/
				WitemP = mdlDialog_itemGetByIndex( WdbP, dialogItem );
#if DEBUG
printf("WdbP : %d   WitmP : %d\n", WdbP, WitemP );
#endif
				itemIndex = (int)WitemP;

				if( ! wcscmp( string, L"" )){
					/*------------------------------------------+
					|   アイテムのラベルを設定(3-104)	    |
					+------------------------------------------*/
					mdlDialog_itemSetLabel( WdbP, itemIndex, Dstring );
				}
				else{
					wcscpy( fileName, string );
					mdlFile_abbreviateName( fileName, moj );
					
					mdlDialog_itemSetLabel( WdbP, itemIndex, fileName );
				}
				/*------------------------------------------+
				|   ダイアログのアップデート(3-106)	    |
				+------------------------------------------*/
					mdlDialog_itemsSynch( WdbP );

}
#if BAT
Private	void	Set_BatName( void );
/*----------------------------------------------------------------------+
|                                                                       |
| name		Set_BatName                                             |
|									|
+----------------------------------------------------------------------*/
Private	void	Set_BatName( void )
{
	int	err, itemIndex;
	BeFileName filePath;
	WChar	fileName[1000];
	WChar	batName[1000];
	WChar	directory[1000], devP[5], dirP[1000], name[100], exp[10];

		if( ! wcscmp( setRefer.idxname, L"" )){
			/*------------------------------------------------------+
			|	ファイル名の構成要素を抽出する。(25-4)		|
			+------------------------------------------------------*/
			mdlFile_parseName( tcb->dgnfilenm, devP, dirP, name, exp );
			/*--------------------------------------------------------------+
			|	名前の構成要素に基づいてファイル名を作成する。(25-4)	|
			+--------------------------------------------------------------*/
			mdlFile_buildName( directory, devP, dirP, name, NULL  );
		}
		else{
			wcscpy( directory, setRefer.idxname );
		}
		/*----------------------------------------------+
		|  ファイルのＯＰＥＮ(25-23)			|
		+----------------------------------------------*/
		if( 0 == mdlDialog_fileOpen( filePath, NULL, 0, directory, L"*.mfc", directory, L"mfc ファイル") ){
			swprintf(fileName, L"%ls", filePath.GetWCharCP());
			wcscpy( setRefer.idxname, fileName );

			mbstowcs(batName, TXT_PBTN_BatName, sizeof(TXT_PBTN_BatName));
			Set_Button( DIALOGID_MainDialog, PUSHBTNID_BatName, batName, fileName, 30 );

		}
}
#endif

Private	void	Mdl_Exit( void);
/*----------------------------------------------------------------------+
|                                                                       |
| name		Mdl_Exit                                                 |
|									|
+----------------------------------------------------------------------*/
Private	void	Mdl_Exit( void)
{
			mdlSystem_exit( NULL, 1);
}
/*----------------------------------------------------------------------+
|                                                                       |
| name		Main_Exit                                                 |
|									|
+----------------------------------------------------------------------*/
Private void Main_Exit(DialogItemMessage *dmP)
{
	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
		case DIALOG_MESSAGE_DESTROY:
		{
			Mdl_Exit();
		  break;
		};
		case DITEM_MESSAGE_BUTTON:
		{
			Mdl_Exit();
		  break;
		};
		default:
			dmP -> msgUnderstood = TRUE;
			break;
	}

}
/*----------------------------------------------------------------------+
|                                                                       |
| name		Bat_Name                                               |
|									|
+----------------------------------------------------------------------*/
#if BAT
Private void Bat_Name(DialogItemMessage *dmP)
{
	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
		case DITEM_MESSAGE_BUTTON:
		{

			Set_BatName();
		  break;
		};
		default:
			dmP -> msgUnderstood = TRUE;
			break;
	}

}
#endif

/*----------------------------------------------------------------------+
|                                                                       |
| name		Jikou_Ok                                                 |
|									|
+----------------------------------------------------------------------*/
Private void Jikou_Ok(DialogItemMessage *dmP)
{
	dmP -> msgUnderstood = TRUE;
	switch( dmP -> messageType ){
		case DITEM_MESSAGE_BUTTON:
		{

			Start_app(   );

		  break;
		};
		default:
			dmP -> msgUnderstood = TRUE;
			break;
	}

}
/*------------------------------------------------------------------------
**  ダイアログフック関数
------------------------------------------------------------------------*/
Private void  dialog_hook(DialogMessage *dmP)
{
	DialogBoxRsc    *dbr_P;

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
		mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), INPUTQ_EOQ);
		break;

	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}



/*======================================================================================================+
|													|
|	ｍａｉｎ											|
|													|
+======================================================================================================*/
/*----------------------------------------------------------------------+
|									|
| name		main							|
|									|
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
    
    RscFileHandle   rscFileH, userPrefsH;	/* a resource file handle */
    int				exitStatus, fileNum, getSize;
	int				i;
	short			viewList[8];
	int				exitG;
	SymbolSet		*setP;
	char            structName[128];
	char            variableName[128];
	WChar	        batName[1000];

	BeFileName		full;
	FILE        *fp;
	void			*referPtr = NULL;
/*----------------------------------------------------------------------+
|	ロックの認識							|
+----------------------------------------------------------------------*/


	chk_kigen( );



/*--- リソースファイルを開ける ---*/
	if (mdlResource_openFile(&rFileH, NULL, RSC_READONLY) != SUCCESS) {
		mdlOutput_error(L"FFREF のリソースファイルが開きません");
		mdlSystem_exit(exitStatus, 1);
	}
	//mdlResource_openFile( &rFileH, NULL, RSC_READWRITE );

   /*--- アンロード関数セット ---------------*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

	//---	フック関数使用宣言	---//
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	// ユーザーリソースの読み込み
	mdlDialog_userPrefFileOpen(&userPrefsH, TRUE);
	if (userPrefsH)
		referPtr = mdlResource_load(userPrefsH, REFER_PAR_CLASS, 1);

	if (referPtr != NULL) {
		setRefer = *(SetRefer*)referPtr;
	}
	else {
		setRefer = referPar;
		mdlResource_add(userPrefsH, REFER_PAR_CLASS, 1, &referPar, sizeof(referPar), NULL);
	}

	if (userPrefsH)
		mdlResource_closeFile(userPrefsH);


	//---	Ｃ式定義初期設定	---//
	sprintf(structName, "setrefer");
	sprintf(variableName, "setRefer");
	setP = mdlCExpression_initializeSet (VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR, 0, TRUE);
	mdlDialog_publishComplexVariable(setP, structName, variableName, &setRefer );




/*----------------------------------------------------------------------+
|	デザインファイルのチェック					|
+----------------------------------------------------------------------*/
	chk_dgn();

/*----------------------------------------------------------------------+
|	Main Dialog open						|
+----------------------------------------------------------------------*/
    if ( (MdbP = mdlDialog_open (NULL, DIALOGID_MainDialog)) == NULL){
		mdlDialog_openInfoBox( L"Ｍａｉｎ　ダイアログボックスを開けません");
		mdlSystem_exit(NULL, 1);
		return;

    }
	mbstowcs(batName, TXT_PBTN_BatName, sizeof(TXT_PBTN_BatName));
	Set_Button( DIALOGID_MainDialog, PUSHBTNID_BatName, batName, setRefer.idxname, 30 );

	return;
}
/*======================================================================================================+
|	ｍｉｎａ　ＥＮＤ										|
+======================================================================================================*/
