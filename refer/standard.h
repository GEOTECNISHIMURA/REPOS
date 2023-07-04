/************************************************************************
*									*
*	standard.h							*
*									*
************************************************************************/
Private void	Update_Dialog( int DialogName);
/*----------------------------------------------------------------------+
|									|
|	Update_Dialog							|
|									|
+----------------------------------------------------------------------*/
Private void	Update_Dialog( int DialogName )
{
	MSDialog	*dbP;

	/*--------------------------------------------------------------+
	|	�_�C�A���O�̃|�C���^���擾(3-87)			|
	+--------------------------------------------------------------*/
		dbP = mdlDialog_find( DialogName, NULL );
	/*--------------------------------------------------------------+
	|   �_�C�A���O�̃A�b�v�f�[�g(3-106)				|
	+--------------------------------------------------------------*/
		mdlDialog_itemsSynch( dbP );
}

Private void	View_Update( int viewNo );
/*----------------------------------------------------------------------+
|                                                                       |
| name		View_Update                                             |
|									|
+----------------------------------------------------------------------*/
Private void View_Update( int viewNo )
{
	int	i, Error;
	bool	view;

#if MSVERSION >= 0x790
	bool	viewList[8];
#else
	short	viewList[8];
#endif
	if( viewNo == 0 ){
		/*------------------------------------------------------+
		|	�r���[�̃A�N�e�B�u���`�F�b�N(5-122)		|
		+------------------------------------------------------*/
		for( i = 0 ; i < 8 ; i++ ){
			view = mdlView_isActive( i );
			if( view == TRUE ){	 viewList[i] = TRUE;	}
			else{			 viewList[i] = FALSE;	}
		}
		/*------------------------------------------------------+
		|   �u�������̍ĕ\��(5-109)				|
		+------------------------------------------------------*/
		mdlView_updateMulti( viewList, FALSE, DRAW_MODE_Normal, NULL, TRUE );
	}
	else{
		if( SUCCESS != ( Error = mdlView_updateSingle( viewNo-1 )) ){
#if MSVERSION == 0x570
			Put_Error( Error, NULL );
#endif
		}
	}

}

Private int	DgnFile_Chk( void );
/*----------------------------------------------------------------------+
|									|
|name		DgnFile_Chk						|
|									|
+----------------------------------------------------------------------*/
Private int	DgnFile_Chk(  )
{
	int	Error;
	int	dgnType;
	DgnFileFormatType *pFormat;
	int *pMajorVersion;
	int *pMinorVersion;
	byte **ppThumbnail;
	int *pThumbnailSize;


	bool     pThreeD;    /* <= TRUE if this is a 3D design file */


	mdlFile_validDesignFile ( &pThreeD, pFormat, pMajorVersion, pMinorVersion, ppThumbnail, pThumbnailSize, tcb->dgnfilenm );

		if( pThreeD == TRUE ){
			dgnType = 3;
		}
		else{
			dgnType = 2;
		}


	return( dgnType );
}

Private void 	Set_FileName( WChar dgnFileName[], WChar name[], WChar ext[], WChar fileName[] );
/*----------------------------------------------------------------------+
|									|
|name		Set_FileName						|
|		�f�o�b�O�t�@�C���쐬					|
|		return fileName						|
+----------------------------------------------------------------------*/
Private void	Set_FileName( WChar dgnFileName[], WChar name[], WChar ext[], WChar fileName[] )
{
	WChar	devP[5], dirP[1000];

		/*------------------------------------------------------+
		|	�t�@�C�����̍\���v�f�𒊏o����B(25-4)		|
		+------------------------------------------------------*/
		mdlFile_parseName( dgnFileName, devP, dirP, NULL, NULL );
		/*--------------------------------------------------------------+
		|	���O�̍\���v�f�Ɋ�Â��ăt�@�C�������쐬����B(25-4)	|
		+--------------------------------------------------------------*/
		mdlFile_buildName( fileName, devP, dirP, name, ext );

}





Private	void	app_Start( void );
/*----------------------------------------------------------------------+
|									|
| name		app_Start						|
|									|
+----------------------------------------------------------------------*/
Private	void	app_Start( )
{
	int	Error;
	UInt32	*filePoss, filePos;
	int	*fileNums, fileNumber;
	int	i, numSelected;
	DgnModelRefP  *modelRefArray;


	/*--------------------------------------------------------------+
	|	�I���Z�b�g���A�N�e�C�u���ǂ����𒲂ׂ�(7-29)		|
	+--------------------------------------------------------------*/
	if( TRUE == mdlSelect_isActive () ){
		//(7-29)
		mdlSelect_returnPositions( &filePoss, &modelRefArray, &numSelected );
#if DEBUG
printf(" numSelected = %d\n", numSelected);
for( i = 0 ; i < numSelected ; i++ ){
printf(" filePos = %ld    fileNum = %d\n", filePoss[i], fileNums[i]);
}
#endif
		for( i = 0 ; i < numSelected ; i++ ){
			Get_HRdata( filePoss[i], modelRefArray[i] );
		}
		//(7-30)
		mdlSelect_freeAll();
	}
	else{
//�X�^�[�g�v���O����
			Get_data();
		mdlSelect_freeAll();
	}
}
Private	void	Start_app( void );
/*----------------------------------------------------------------------+
|									|
| name		Start_app						|
|									|
+----------------------------------------------------------------------*/
Private	void	Start_app( void )
{

			app_Start(  );
}

