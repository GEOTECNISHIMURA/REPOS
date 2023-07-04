/************************************************************************
*									*
*	Element_get.h							*
*									*
************************************************************************/
/*----------------------------------------------------------------------+
|                                                                       |
| name		Level_Symbol		                                |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_Level 	                                        |
+----------------------------------------------------------------------*/
Private int	Get_Level( MSElement *element );
Private int	Get_Level( MSElement *element )
{
	UInt32	level;

		/*------------------------------------------------------+
		|	要素からプロパティ情報を抽出(5-38)		|
		+------------------------------------------------------*/
		mdlElement_getProperties( &level, NULL, NULL, NULL, NULL, NULL, NULL, NULL, element );
		return( level );
 
}
/*----------------------------------------------------------------------+
| name		Get_Symbol	                                       |
+----------------------------------------------------------------------*/
Private void	Get_Symbol( MSElement *ellBuf, UInt32 *color, UInt32 *weight, int *style );
Private void	Get_Symbol( MSElement *ellBuf, UInt32 *color, UInt32 *weight, int *style )
{
#if DENUG
printf("----- Get Line Symbol ------\n");
#endif
		/*------------------------------------------------------+
		|	要素から線属性情報を抽出(5-36)			|
		+------------------------------------------------------*/
		mdlElement_getSymbology( color, weight, style, ellBuf );
}

/*----------------------------------------------------------------------+
|                                                                       |
|		CELL_LIB_ELM 	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CELL_HEADER_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Cell_Level 	                                       |
+----------------------------------------------------------------------*/
Private void	Cell_Level( MSElement *element, UInt32 *level );
Private void	Cell_Level( MSElement *element, UInt32 *level )
{
	/*------------------------------------------------------+
	|	要素からプロパティ情報を抽出(5-38)		|
	+------------------------------------------------------*/
		mdlElement_getProperties( level, NULL, NULL, NULL, NULL, NULL, NULL, NULL, element );
 
}

/*----------------------------------------------------------------------+
| name		Get_Cell_Level 	                                        |
+----------------------------------------------------------------------*/
Private UInt32	Get_Cell_Level( ULong filePos, DgnModelRefP modelRef  );
Private UInt32	Get_Cell_Level( ULong filePos, DgnModelRefP modelRef)
{
	MSElementDescr	*elemDescr;
	UInt32		rfilePos, level;
	int		error;

			//5-48
			error = mdlElmdscr_read( &elemDescr, filePos, modelRef, FALSE, &rfilePos );

			mdlElmdscr_operation( elemDescr, (PFElemOperation)Cell_Level, &level, ELMD_ELEMENT );


 	       mdlElmdscr_freeAll (&elemDescr);
		return( level );
}
/*----------------------------------------------------------------------+
| name		Get_Cell_Symbol	                                       |
+----------------------------------------------------------------------*/
Private void	Get_Cell_Symbol( ULong filePos, DgnModelRefP modelRef, UInt32 *color, UInt32 *weight, int *style );
Private void	Get_Cell_Symbol( ULong filePos, DgnModelRefP modelRef, UInt32 *color, UInt32 *weight, int *style )
{
	MSElement    ellBuf;
#if DENUG
printf("----- Get Cell Symbol ------\n");
#endif
		/*----------------------------------------------------------------------+
		|    デザインファイルから要素を読みとる (5-33) mdlElement_read		|
		+----------------------------------------------------------------------*/
		mdlElement_read( &ellBuf, modelRef, filePos);
		/*------------------------------------------------------+
		|	要素から線属性情報を抽出(5-36)			|
		+------------------------------------------------------*/
		mdlElement_getSymbology( color, weight,	style, &ellBuf );
}

/*----------------------------------------------------------------------+
| name		Get_Cell_Name 	                                       |
+----------------------------------------------------------------------*/
Private void	Get_Cell_Name( ULong filePos, DgnModelRefP modelRef,
				 Dpoint3d *origin, Dpoint3d *shape, RotMatrix *rMatrix,
				 Dpoint3d *scale, WChar cellName[] );
Private void	Get_Cell_Name( ULong filePos, DgnModelRefP modelRef,
				 Dpoint3d *origin, Dpoint3d *shape, RotMatrix *rMatrix,
				 Dpoint3d *scale, WChar cellName[] )
{
	MSElement ellBuf;
#if DENUG
printf("----- Get Cell Name ------\n");
#endif
		/*----------------------------------------------------------------------+
		|    デザインファイルから要素を読みとる (5-33) mdlElement_read		|
		+----------------------------------------------------------------------*/
		mdlElement_read( &ellBuf, modelRef, filePos);
		/*----------------------------------------------+
		|	セルヘッダ要素から情報を抽出(5-24)	|
		+----------------------------------------------*/
		mdlCell_extract( origin, shape, rMatrix, scale, cellName, MAX_CELLNAME_LENGTH, &ellBuf );
}

/*----------------------------------------------------------------------+
|                                                                       |
|		LINE_ELM	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_Line_Dpoint						|
+----------------------------------------------------------------------*/
Private int	Get_Line_Dpoint( MSElement *ellBuf, DgnModelRefP modelRef, Dpoint3d *points, int *numVerts );
Private int	Get_Line_Dpoint( MSElement *ellBuf, DgnModelRefP modelRef, Dpoint3d *points, int *numVerts )
{
	int	Error;
		/*----------------------------------------------+
		|    線要素から一連の座標値を抽出(5-18)		|
		+----------------------------------------------*/
		Error = mdlLinear_extract( points, numVerts, ellBuf, modelRef);

		return( Error );

}

/*----------------------------------------------------------------------+
|                                                                       |
|		LINE_STRING_ELM	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_LineString_Dpoint					|
+----------------------------------------------------------------------*/
Private int	Get_LineString_Dpoint( MSElement *ellBuf, DgnModelRefP modelRef, Dpoint3d *points, int *numVerts );
Private int	Get_LineString_Dpoint( MSElement *ellBuf, DgnModelRefP modelRef, Dpoint3d *points, int *numVerts )
{
	int	Error;
		/*----------------------------------------------+
		|    線要素から一連の座標値を抽出(5-18)		|
		+----------------------------------------------*/
		Error = mdlLinear_extract( points, numVerts, ellBuf, modelRef);

		return( Error );

}

/*----------------------------------------------------------------------+
|                                                                       |
|		GROUP_DATA_ELM	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		SHAPE_ELM	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		TEXT_NODE_ELM	                                        |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_TextNode                                             |
+----------------------------------------------------------------------*/
Private void	Get_TextNode( ULong filePos, DgnModelRefP modelRef);
Private void	Get_TextNode( ULong filePos, DgnModelRefP modelRef)
{
	int		type, err;
	MSElement	element;

// mdlText_extract 宣言
	Dpoint3d	origin, userOrigin;
	WChar		string[100];
	RotMatrix	rMatrix;
	TextSizeParam   *textSize;
	DgnPlatform::TextParamWide *textParam;
	TextEDParamP  edParam;
	WStringP  wString;
//

			/*------------------------------------------------------+
			|	デザインファイルから要素を取り込む(5-33)	|
			+------------------------------------------------------*/
			err = mdlElement_read( &element, modelRef, filePos );
			if( err == SUCCESS ){
				/*------------------------------------------------------+
				|	要素から要素のタイプを抽出(5-36)		|
				+------------------------------------------------------*/
				type = mdlElement_getType( &element );
				/*--------------------------------------------------------------+
				|    文字要素から情報を抽出する (5-20) mdlText_extract		|
				+--------------------------------------------------------------*/
				mdlText_extract( wString, &origin, &userOrigin, &rMatrix, textSize, textParam, edParam, &element);
				swprintf(string, L"%ls", wString->GetWCharCP());


			}

}
/*----------------------------------------------------------------------+
|                                                                       |
|		DIG_SETDATA_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		DGNFIL_HEADER_ELM                                       |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		LEV_SYM_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CURVE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CMPLX_STRING_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CONIC_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CMPLX_SHAPE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		ELLIPSE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		ARC_ELM							|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		TEXT_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_Text                                             |
+----------------------------------------------------------------------*/
Private void	Get_Text( ULong filePos, DgnModelRefP modelRef);
Private void	Get_Text( ULong filePos, DgnModelRefP modelRef)
{
	int		type, err;
	MSElement	element;

// mdlText_extract 宣言
//

			/*------------------------------------------------------+
			|	デザインファイルから要素を取り込む(5-33)	|
			+------------------------------------------------------*/
			err = mdlElement_read( &element, modelRef, filePos );
			if( err == SUCCESS ){
				/*------------------------------------------------------+
				|	要素から要素のタイプを抽出(5-36)		|
				+------------------------------------------------------*/
				type = mdlElement_getType( &element );
				/*--------------------------------------------------------------+
				|    文字要素から情報を抽出する (5-20) mdlText_extract		|
				+--------------------------------------------------------------*/
				//mdlText_extract( &origin, &userOrigin, &numEdfields, &edFields, string,
				//				 &rMatrix, &textstyleInfo, &just, &tileSize, &textSize, &element);

			}

}

/*----------------------------------------------------------------------+
|                                                                       |
|		SURFACE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		SOLID_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSPLINE_POLE_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		POINT_STRING_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		CONE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSPLINE_SURFACE_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSURF_BOUNDARY_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSPLINE_KNOT_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSPLINE_CURVE_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BSPLINE_WEIGHT_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		DIMENSION_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		SHAREDCELL_DEF_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		MULTILINE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		ATTRIBUTE_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		BREP_ELM						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		MICROSTATION_ELM					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		RASTER_HDR						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		RASTER_COMP						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|		RASTER_REFERENCE_ELM					|
|									|
+----------------------------------------------------------------------*/

Private void	refer( void );
Private void	refer( void )
{
	FILE	*fin;
	WChar	buffer[256];
	WChar	string[256];
	WChar	z[102], at[102], cd[102], kisai[100], cell[102];
	WChar	zk[102], hn[102];
	int	lv, co, wt, lc;
	int	Error, i;
	int cnt;
//	WChar	spe[] = " ";
	WChar    stringp[256];
	WChar	*token;


#if DEBUG
printf("---------- start refer ---------------\n");
#endif

		Error = -1;
		cnt = 0;
		if( NULL != (fin = _wfopen( setRefer.idxname, L"r" )) ){
			while( NULL != fgetws( buffer, 256, fin )){

				if (!wcsncmp(buffer, L";", 1)) {
					Error = -1;
				}

				else{
					token = wcstok(buffer, L" ");
					wcscpy(stringp, L"");
					wcscpy(z, token);
					for( i=0 ; i < 14 ; i++ ){
						token = wcstok( NULL, L" " );
						switch( i ){
							case	0:
								wcscpy( at, token );
								break;
							case	1:
								wcscpy( cd, token );
								break;
							case	2:
								wcscpy( zk, token );
								break;
							case	3:
								wcscpy( hn, token );
								break;
							case	4:
								wcscpy( kisai, token );
								break;
							case	5:
								lv = _wtoi( token );
								break;
							case	6:
								co = _wtoi( token );
								break;
							case	7:
								wt = _wtoi( token );
								break;
							case	8:
								lc = _wtoi( token );
								break;
							case	13:
								wcscpy( cell, token );
								break;
							default:
								break;
						}
						wcscat(stringp,L" ");
					    wcscat(stringp, token);
					}
//					sscanf( stringp, "%s %s %s %s %s %s %d %d %d %d %s %s %s %s %s",
//							z, at, cd, zk, hn, kisai, &lv, &co, &wt, &lc, wk, pen, wht, wk, cell );

					
					if( setRefer.symb.level == lv && setRefer.symb.color == co &&
							setRefer.symb.weight == wt && setRefer.symb.style == lc ){
#if DEBUG
printf("-------- %s %s %s %d %d %d %d %s -----------\n", z, at, kisai, lv, co, wt, lc, cell );
#endif
						if( setRefer.type == 2 && ! wcscmp( at, L"S") ){
							if( ! wcscmp( setRefer.cellName, cell )){
								wcscpy( string, cd );
								wcscat( string, L"  " );
								wcscat( string, zk );
								wcscat( string, L"  " );
								wcscat( string, hn );
								wcscat( string, L"  " );
								wcscat( string, kisai );
								Error = 0;
								break;
							}
						}
						else{
								wcscpy( string, cd );
								wcscat( string, L"  " );
								wcscat( string, zk );
								wcscat( string, L"  " );
								wcscat( string, hn );
								wcscat( string, L"  " );
								wcscat( string, kisai );
							Error = 0;
							break;
						}
					}
				}
				cnt++;
			}
			fclose( fin );

			if( Error == 0 ){
				mdlDialog_openInfoBox( string );
				mdlOutput_prompt( string );
			}
			else{
				mdlDialog_openInfoBox( L"該当無し" );
				mdlOutput_prompt( L"該当無し" );
			}
		}
		
}
/*----------------------------------------------------------------------+
|                                                                       |
| name		Get_HRdata                                              |
|									|
+----------------------------------------------------------------------*/
Private void	Get_HRdata( ULong filePos, DgnModelRefP modelRef);
Private void	Get_HRdata( ULong filePos, DgnModelRefP modelRef)
{

	MSElement	ellBuf;
	int	style;
	UInt32 color, weight;
//------- Cell ----------
	Dpoint3d  origin;
	Dpoint3d  shape[8];
	RotMatrix rMatrix;
	Dpoint3d  scale;

/*
typedef struct setelement
    {
	int 	type;
	Symbol	symb;

    } SetElement;
*/


#if DEBUG
printf("---------- start Get_HRdata ---------------\n");
printf(" filePos : %ld  fileNumber : %d\n", filePos, fileNumber );
#endif
	/*----------------------------------------------------------------------+
	|   要素のファイル位置を取得 (5-34) mdlElement_getFilePos		|
	+----------------------------------------------------------------------*/
//	filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &fileNumber);
//		if( fileNumber == 0 ){
			/*----------------------------------------------------------------------+
			|    デザインファイルから要素を読みとる (5-33) mdlElement_read		|
			+----------------------------------------------------------------------*/
			mdlElement_read( &ellBuf, modelRef, filePos);
				/*------------------------------------------------------+
				|	要素から要素のタイプを抽出(5-36)		|
				+------------------------------------------------------*/
				setRefer.type = mdlElement_getType( &ellBuf );
#if DEBUG
printf("setRefer.type : %d\n", setRefer.type );
#endif
				switch( setRefer.type ){
					//case	CELL_LIB_ELM:
						//break;
					case	CELL_HEADER_ELM:
						setRefer.symb.level = Get_Cell_Level( filePos, modelRef);
						Get_Cell_Symbol(  filePos, modelRef, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						//Get_Cell_Name( filePos, modelRef, &origin, &shape, &rMatrix, &scale, setRefer.cellName );
						mdlElement_read(&ellBuf, modelRef, filePos);
						//mdlCell_extract(&origin, &shape, &rMatrix, &scale, setRefer.cellName, MAX_CELLNAME_LENGTH, &ellBuf);
						mdlCell_extract(&origin, shape, &rMatrix, &scale, setRefer.cellName, MAX_CELLNAME_LENGTH, &ellBuf);
						break;
					case	LINE_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	LINE_STRING_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	GROUP_DATA_ELM:
						break;
					case	SHAPE_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	TEXT_NODE_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	DIG_SETDATA_ELM:
						break;
					case	DGNFIL_HEADER_ELM:
						break;
					case	LEV_SYM_ELM:
						break;
					case	CURVE_ELM:
						break;
					case	CMPLX_STRING_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;

					case	CMPLX_SHAPE_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	ELLIPSE_ELM:
						break;
					case	ARC_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	TEXT_ELM:
						setRefer.symb.level = Get_Level( &ellBuf );
						Get_Symbol(  &ellBuf, &color, &weight, &style );
						setRefer.symb.color = color;
						setRefer.symb.weight = weight;
						setRefer.symb.style = style;
						break;
					case	SURFACE_ELM:
						break;
					case	SOLID_ELM:
						break;
					case	BSPLINE_POLE_ELM:
						break;
					case	POINT_STRING_ELM:
						break;
					case	CONE_ELM:
						break;
					case	BSPLINE_SURFACE_ELM:
						break;
					case	BSURF_BOUNDARY_ELM:
						break;
					case	BSPLINE_KNOT_ELM:
						break;
					case	BSPLINE_CURVE_ELM:
						break;
					case	BSPLINE_WEIGHT_ELM:
						break;
					case	DIMENSION_ELM:
						break;
					case	SHAREDCELL_DEF_ELM:
						break;
					case	SHARED_CELL_ELM:
						break;
					case	MULTILINE_ELM:
						break;
					case	ATTRIBUTE_ELM:
						break;
//					case	BREP_ELM:
//						break;
					case	MICROSTATION_ELM:
						break;
					case	RASTER_HDR:
						break;
					case	RASTER_COMP:
						break;
					case	RASTER_REFERENCE_ELM:
						break;
					default:
						break;
				}
#if DEBUG
printf("symb %d %d %d %d\n", setRefer.symb.level, setRefer.symb.color, setRefer.symb.weight, setRefer.symb.style );
#endif
			refer();

//		}

		/*------------------------------------------------------------------------------+
		|	MicroStationが「一回きり」操作モードかどうかをチェックする。(1-6)	|
		+------------------------------------------------------------------------------*/
		mdlState_checkSingleShot();
#if DEBUG
printf("---------- end Get_HRdata ---------------\n");
#endif

}

Private void	userState_clean( void );
/*----------------------------------------------------------------------+
|                                                                       |
| name		userState_clean                                         |
|									|
+----------------------------------------------------------------------*/
Private void	userState_clean()
{
	mdlSelect_freeAll();
	mdlState_clear();
#if DEBUG
printf(" userState_clean   !\n" );
#endif


}



Private void	Get_data( void );
Private void	userState_getData( void );
/*----------------------------------------------------------------------+
|                                                                       |
| name		userState_set                                          |
|									|
+----------------------------------------------------------------------*/
Private void	userState_getData()
{
	ULong	filePos;
	DgnModelRefP modelRef;
	/*----------------------------------------------------------------------+
	|   要素のファイル位置を取得 (5-34) mdlElement_getFilePos		|
	+----------------------------------------------------------------------*/
	filePos = mdlElement_getFilePos( FILEPOS_CURRENT, &modelRef);
#if DEBUG
printf("filePos : %ld   fileNumber : %d\n", filePos, fileNumber);
#endif



        Get_HRdata( filePos, modelRef);



		Get_data();
}


/*----------------------------------------------------------------------+
|                                                                       |
| name		Get_data                                              |
|									|
+----------------------------------------------------------------------*/
Private void	Get_data( void )
{
#if DEBUG
printf("----- 要素指定 -------\n");
#endif
			/*------------------------------------------------------+
			|	要素指定カーソルにする(7-16)			|
			+------------------------------------------------------*/
			mdlLocate_setCursor( );
			/*------------------------------------------------------+
			|	呼び出したいユーザー関数を指定する(7-17)	|
			+------------------------------------------------------*/
//			mdlLocate_setFunction( LOCATE_POSTLOCATE, userState_getData );
			/*------------------------------------------------------+
			|	修正コマンドを開始(1-9)				|
			+------------------------------------------------------*/
			mdlState_startModifyCommand( (StateFunc_Reset)userState_clean, (MdlFunc_Accept)userState_getData, NULL, NULL, NULL, NULL, NULL, TRUE, 0 );
mdlLocate_init();
mdlLocate_allowLocked(  );

}

