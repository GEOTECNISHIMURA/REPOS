/*-----------------------------------------------------------------------*/
/*- ライン、セル、テキストのレイヤーなどを変える            -*/
/*- LAY.MC                                                              -*/
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include "../CommonCE/naka8.cpp"

#include "laychg.h"

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

FILE                    *fp, *fp2;
LAYSET                  *layset;
CExprResult             result;
CExprValue              value;
RscFileHandle           Rsc;
MSDialog                *db_p;
MSElementDescr          *elmDP, *elmDP2, *NelmDP, *DelmDP;
double                  th, tp;
WChar                   full[80], buf[128], file[MAXFILELENGTH], filename[128], info[128], logname[MAXDIRLENGTH];
ExtScanList             scanList;
int                     ii, jj, just, fileNum, Error, getSize, type, linkCount, chgnum;
UInt32                  ggnum;
DatabaseLink            *dblink;
ULong                   filePos, filePos2, celfilePos,  cPos, endpos, filendpos, complexPos;
ULong                   cellPos;
MSElement               ele, ele2, elece, cele, elm;
int                     ce, len, lo, exitStatus, numelems;
Dpoint3d                scale, origin, center;
WChar                   string[255], ins[256];
RotMatrix               rMatrix;
//size_t                  size;
WChar                   typ[2];
long                    ll, ss, tt;         /*- テーブル中のL,S,T の数 -*/
UInt32                  lv, co, wt;     /*- 読み込んだ要素のレベル、カラー、スタイル、ウエイト -*/
int                     lc;
UInt32                  clv, cco, cwt; /*- チェックサークルのレベル、カラー、スタイル、ウエイト -*/
int                     clc;
UInt32                  lvn, con, wtn; /*- 新要素のレベル、カラー、スタイル、ウエイト -*/
int                     lcn;
WChar                 MSWcell[MAX_CELLNAME_LENGTH];
WChar                    cell[MAX_CELLNAME_BYTES];
int                     moji;
/*--- ライン ---*/
struct  ltable{
	UInt32        lvo, coo,  wto;
	int           lco;
	UInt32        lvn, con,  wtn;
	int           lcn;
};
struct  ltable  ltbl[100000];

/*--- セル ---*/
struct  ctable{
   UInt32     lvo, coo,  wto;
   int        lco;
   WChar      celo[10];
   UInt32     lvn, con,  wtn;
   int        lcn;
   double  scale, rot;
   WChar    celn[10];
   int     ten;
};
struct  ctable  ctbl[100000];

/*--- テキスト ---*/
struct  ttable{
   UInt32     lvo, coo,  wto;
   int        lco;
   UInt32     lvn, con,  wtn;
   int        lcn;
   WChar    stro[100], strn[100];
   double  thn, tpn, rot;
   int     ft;
};
struct  ttable  ttbl[100000];

/*- プロトタイプ    -*/
bool   unload(UnloadProgramReason unloadType);
void   kensaku( void);
void   read_table( void);
void   kensaku_line( void);
void   kensaku_cell( void);
void   kensaku_text( void);
void   create_circle( void);

int      cnv_level_name2IDa(LevelId *lvID, WChar *wlvName);
void appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt);
/*-----------------------------------------------------------------------
**  メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{


   /*--- リソースファイルのオープン -------------*/
   if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"LAYのリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
    }

    /*--- 広域変数の取得 -------------*/
    if ( mdlCExpression_getValue( &value, &result, "&layset",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlOutput_error( L"広域変数が取得できません");
        
    }
    layset = (LAYSET*)value.val.valPointer;

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    read_table( );

    mdlView_updateSingle( 0);
    mdlSystem_exit( exitStatus, 1);


}


/*-----------------------------------------------------------------------
**  アンロード関数 
-----------------------------------------------------------------------*/
bool unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  テーブルファイルを読み込む
-----------------------------------------------------------------------*/
void    read_table( )
{
WChar   devP[64], dirP[256], nameP[256], bufa[256];
int		oi,i,j,cnt,flg,leng,flg2;
int		status;
LevelId	tmplv;

    /*--- テーブルファイルの読み込み ---*/
	
    if ( ( fp = _wfopen( layset->file, L"r")) != NULL){
        ll = 0;   ss = 0;   tt = 0;
        while( fgetws( buf, 128, fp) != NULL){
            /*--- ";"を読み飛ばす ---*/
            if ( wmemcmp( buf, L";", 1) == 0){
                continue;
            }

            /*--- ライン、シェイプ、サークル、複合連結、複合図形の場合 ---&ltbl[ll].lvo*/
            if ( wmemcmp( &buf[0], L"L", 1) == 0){

				i = 0;
            	oi = 0;
            	cnt = 0;
            	flg = 0;
            	flg2 = 0;
            	tmplv = 0;
            	while (TRUE){
            		
            		if ((buf[i] == 0x20 && flg == 1 && ( flg2 == 0 || flg2 == 2)) || buf[i] == '\0') {
            			
            			if (flg2 == 0){
            				leng  = i - oi;
            			}
            			else{
            				leng  = i - oi - 2;
            				oi++;
            			}
            			for (j=0;j<leng;j++){
            				bufa[j] = buf[oi + j];
            			}
            			
            			bufa[j] = '\0';

            			
            			switch (cnt){
            				case 0:
            					swprintf(typ,L"%ls",bufa);
            					break;
            				case 1:
	            				if (flg2 == 0){
	            					ltbl[ll].lvo = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ltbl[ll].lvo = tmplv;
	            				}
            					
            					break;
            				case 2:
            					ltbl[ll].coo = _wtoi(bufa);
            					break;
            				case 3:
            					ltbl[ll].lco = _wtoi(bufa);
            					break;
            				case 4:
            					ltbl[ll].wto = _wtoi(bufa);
            					break;
            				case 5:
            					if (flg2 == 0){
	            					ltbl[ll].lvn = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ltbl[ll].lvn = tmplv;
	            				}
            					break;
            				case 6:
            					ltbl[ll].con = _wtoi(bufa);
            					break;
            				case 7:
            					ltbl[ll].lcn = _wtoi(bufa);
            					break;
            				case 8:
            					ltbl[ll].wtn = _wtoi(bufa);
            					break;
            				default:
            					break;
            			}
            			cnt++;
            			flg = 0;
            			flg2 = 0;
            			
            		}
            		else if (buf[i] == 0x22 && flg2 == 1){

            			flg2 = 2;
            		}
            		else if (buf[i] != 0x20 && flg == 0){
            			if (buf[i] == 0x22) { flg2 = 1;}
            			oi = i;
            			flg = 1;
            		}
            		else {
            			
            		}
            		if (buf[i] == L'\0') {break;}
            		i++;
            	}
                ll++;
            }

            /*--- セルの場合 ---*/
            else if ( wmemcmp( &buf[0], L"S", 1) == 0){

            	i = 0;
            	oi = 0;
            	cnt = 0;
            	flg = 0;
            	flg2 = 0;
            	tmplv = 0;
            	while (TRUE){
            		
            		if ((buf[i] == 0x20 && flg == 1 && ( flg2 == 0 || flg2 == 2)) || buf[i] == L'\0') {
            			
            			if (flg2 == 0){
            				leng  = i - oi;
            			}
            			else{
            				leng  = i - oi - 2;
            				oi++;
            			}
            			for (j=0;j<leng;j++){
            				bufa[j] = buf[oi + j];
            			}
            			
            			bufa[j] = '\0';
            			switch (cnt){
            				case 0:
            					swprintf(typ,L"%ls",bufa);
            					break;
            				case 1:
	            				if (flg2 == 0){
	            					ctbl[ss].lvo = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ctbl[ss].lvo = tmplv;
	            				}
            					
            					break;
            				case 2:
            					ctbl[ss].coo = _wtoi(bufa);
            					break;
            				case 3:
            					ctbl[ss].lco = _wtoi(bufa);
            					break;
            				case 4:
            					ctbl[ss].wto = _wtoi(bufa);
            					
            					break;
            				case 5:
								swprintf(ctbl[ss].celo,L"%ls",bufa);
            					break;
            				case 6:
	            				if (flg2 == 0){
	            					ctbl[ss].lvn = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ctbl[ss].lvn = tmplv;
	            				}
            					
            					break;
            				case 7:
            					ctbl[ss].con = _wtoi(bufa);
            					break;
            				case 8:
            					ctbl[ss].lcn = _wtoi(bufa);
            					break;
            				case 9:
            					ctbl[ss].wtn = _wtoi(bufa);
            					break;
            				case 10:
            					ctbl[ss].scale = _wtof(bufa);
            					break;
            			    case 11:
            					ctbl[ss].rot = _wtof(bufa);
            					break;
            				case 12:
            					swprintf(ctbl[ss].celn,L"%ls",bufa);
            					break;
            				case 13:
            					ctbl[ss].ten = _wtoi(bufa);
            					break;
            				default:
            					break;
            			}
            			cnt++;
            			flg = 0;
            			flg2 = 0;
            			
            		}
            		else if (buf[i] == 0x22 && flg2 == 1){

            			flg2 = 2;
            		}
            		else if (buf[i] != 0x20 && flg == 0){
            			if (buf[i] == 0x22) { flg2 = 1;}
            			oi = i;
            			flg = 1;
            		}
            		else {
            			
            		}
            		if (buf[i] == L'\0') {break;}
            		i++;
            	}

                ss++;
            }

            /*--- テキストの場合 ---*/
            else if ( wmemcmp( &buf[0], L"T", 1) == 0){

            	i = 0;
            	oi = 0;
            	cnt = 0;
            	flg = 0;
            	flg2 = 0;
            	tmplv = 0;
            	while (TRUE){
            		
            		if ((buf[i] == 0x20 && flg == 1 && ( flg2 == 0 || flg2 == 2)) || buf[i] == L'\0') {
            			
            			if (flg2 == 0){
            				leng  = i - oi;
            			}
            			else{
            				leng  = i - oi - 2;
            				oi++;
            			}
            			for (j=0;j<leng;j++){
            				bufa[j] = buf[oi + j];
            			}
            			
            			bufa[j] = L'\0';
            			switch (cnt){
            				case 0:
            					swprintf(typ,L"%ls",bufa);
            					break;
            				case 1:
	            				if (flg2 == 0){
	            					ttbl[tt].lvo = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ttbl[tt].lvo = tmplv;
	            				}
            					
            					break;
            				case 2:
            					ttbl[tt].coo = _wtoi(bufa);
            					break;
            				case 3:
            					ttbl[tt].lco = _wtoi(bufa);
            					break;
            				case 4:
            					ttbl[tt].wto = _wtoi(bufa);
            					
            					break;
            				case 5:
								swprintf(ttbl[tt].stro,L"%ls",bufa);
            					break;
            				case 6:
	            				if (flg2 == 0){
	            					ttbl[tt].lvn = _wtoi(bufa);
	            				}
	            				else{
	            					status = cnv_level_name2IDa(&tmplv, bufa);
	            					ttbl[tt].lvn = tmplv;
	            				}
            					
            					break;
            				case 7:
            					ttbl[tt].con = _wtoi(bufa);
            					break;
            				case 8:
            					ttbl[tt].lcn = _wtoi(bufa);
            					break;
            				case 9:
            					ttbl[tt].wtn = _wtoi(bufa);
            					break;
            				case 10:
            					swprintf(ttbl[tt].strn,L"%ls",bufa);
            					break;
            			    case 11:
            					ttbl[tt].thn = _wtof(bufa);
            					break;
            				case 12:
            					ttbl[tt].tpn = _wtof(bufa);
            					break;
            				case 13:
            					ttbl[tt].rot = _wtof(bufa);
            					break;
            				case 14:
            					ttbl[tt].ft = _wtoi(bufa);
            					break;
            				default:
            					break;
            			}
            			cnt++;
            			flg = 0;
            			flg2 = 0;
            			
            		}
            		else if (buf[i] == 0x22 && flg2 == 1){

            			flg2 = 2;
            		}
            		else if (buf[i] != 0x20 && flg == 0){
            			if (buf[i] == 0x22) { flg2 = 1;}
            			oi = i;
            			flg = 1;
            		}
            		else {
            			
            		}
            		if (buf[i] == L'\0') {break;}
            		i++;
            	}
//               sscanf( buf, "%ls %d %d %d %d %ls %d %d %d %d %ls %lf %lf %lf %d\n", typ,
//                             &ttbl[tt].lvo, &ttbl[tt].coo, &ttbl[tt].lco, &ttbl[tt].wto, ttbl[tt].stro,
//                             &ttbl[tt].lvn, &ttbl[tt].con, &ttbl[tt].lcn, &ttbl[tt].wtn, ttbl[tt].strn,
//                             &ttbl[tt].thn, &ttbl[tt].tpn, &ttbl[tt].rot, &ttbl[tt].ft);
               tt++;
            }
        }
    }
    else{
        mdlDialog_openInfoBox(L"テーブルファイルが読み込めません");
        mdlSystem_exit( exitStatus, 1);
    }
    fclose( fp);

    lo = 0;

    /*--- バッチファイル ON の場合 ---*/
    if ( layset->tbat == -1){
        fp2 = _wfopen( layset->bfile, L"r");
        ii = 0;
        while( fwscanf( fp2, L"%ls\n", filename) != EOF){
            if ( memcmp( &filename[0], ";", 1) == 0){
                continue;
            }
            mdlSystem_newDesignFile( filename);

            /*--- ログファイル名 ---*/
            mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
            swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
            fp = _wfopen( logname, L"w");

            kensaku( );

            fclose( fp);
        }
        fclose( fp2);
    }

    /*--- バッチファイル OFF の場合 ---*/
    else{
        /*--- ログファイル名 ---*/
        mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
        swprintf( logname, L"%ls:%ls%ls.log", devP, dirP, nameP);
        fp = _wfopen( logname, L"w");

        kensaku( );

        fclose( fp);
    }
}


/*-----------------------------------------------------------------------
**  検索
-----------------------------------------------------------------------*/
void    kensaku( )
{
	BeFileName	filePath, Cfile;
    DgnModelRefP pModelRef;
    
    /*--- EOF の ファイルポジション ---*/
    pModelRef = NULL;
    filendpos =mdlElement_getFilePos( FILEPOS_EOF, NULL);

    /*--- セルライブラリのアタッチ ---*/
	Cfile.SetName(layset->cfile);
    mdlCell_attachLibrary( filePath, &Cfile, NULL, TRUE);

    chgnum = 0;
    fwprintf( fp, L"%ls\n", tcb->dgnfilenm);
	mdlCell_getLibraryName(filePath);
    if (wcslen(filePath.GetWCharCP()) == 0){
        mdlDialog_openInfoBox( L"セルライブラリにアタッチして下さい");
        mdlSystem_exit( exitStatus, 1);
    }

    kensaku_line( );
    kensaku_cell( );
    kensaku_text( );

    mdlView_updateSingle( 0);

    if ( chgnum == 0){
        fwprintf( fp, L"    変更なし\n");
    }
}


/*-----------------------------------------------------------------------
|  モジュール名：kensaku_line   スキャン（ライン、シェイプ、サークル）  |
-----------------------------------------------------------------------*/
void    kensaku_line( )
{
Dpoint3d    lPoi[5000];
int         lnv;
/*元int         word;*/
ULong       elemAddr[1];
int         scanwords;
int 		chkflg;
int			i;
    fileNum = 0;
    numelems = 0;

    /*--- 検索条件を設定 ---*/
    /*--- スキャンリストの初期化等 ---*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

    /*--- scanListの各メンバーに以下をセットする ---*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;*/
    /*scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS ;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;

    /*--- 検索対象の決定 ---*/
    for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
    scanList.typmask[0] = TMSK0_LINE | TMSK0_LINE_STRING | TMSK0_CMPLX_STRING | TMSK0_CMPLX_SHAPE | TMSK0_SHAPE | TMSK0_ELLIPSE;
    
	for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--- ｽｷｬﾝﾘｽﾄの読み込み ---*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
    filePos   = 0L;
    scanwords = sizeof(elemAddr)/sizeof( short);

    /*--- ｽｷｬﾝ実行 ---*/
    while( TRUE){
        Error = mdlScan_file( elemAddr, &scanwords, sizeof(elemAddr), &filePos);
	    
        if ( filePos > filendpos){ break;}
        /*if ( Error == 10){ break;}*/
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
        mdlElement_read( &ele, MASTERFILE, elemAddr[0]);

        
        /*--- 要素にｼﾝﾎﾞﾛｼﾞを与える ---*/

        type = mdlElement_getType( &ele);

        /*--- 旧ラインのレイヤーを取得 ---*/
        get_layer( &lv, &co, &lc, &wt, NULL, &ele);


        /*--- 範囲内の要素を検索 --------*/
        for( ii=0; ii<ll; ii++){
        	chkflg = 0;
        	while ( TRUE){
	            /*--- 旧要素のレイヤーが設定値と違う場合 ---*/
	            if ( lv != ltbl[ii].lvo && ltbl[ii].lvo != -1){ break;}
	            if ( co != ltbl[ii].coo && ltbl[ii].coo != -1){ break;}
	            if ( lc != ltbl[ii].lco && ltbl[ii].lco != -1){ break;}
	            if ( wt != ltbl[ii].wto && ltbl[ii].wto != -1){ break;}

	            memcpy( &ele2, &ele, sizeof( MSElement));

	            filePos2 = elemAddr[0];
	            /*--- 新要素のレイヤー ---*/
	            lvn = ltbl[ii].lvn;   con = ltbl[ii].con;   lcn = ltbl[ii].lcn;   wtn = ltbl[ii].wtn;
	            if ( ltbl[ii].lvn == -1){ lvn = lv;}
	            if ( ltbl[ii].con == -1){ con = co;}
	            if ( ltbl[ii].lcn == -1){ lcn = lc;}
	            if ( ltbl[ii].wtn == -1){ wtn = wt;}

	            if( type == 12 || type == 14){
					mdlElmdscr_read(&elmDP, filePos2, MASTERFILE, FALSE, NULL);
					mdlElmdscr_duplicate(&NelmDP, elmDP);
					//   			memcpy(&NelmDP, &elmDP, sizeof(elmDP));
					mdlElmdscr_setProperties(&NelmDP, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
					mdlElmdscr_setSymbology(&NelmDP, &con, &lcn, &wtn, NULL);
					mdlElmdscr_rewrite(NelmDP, elmDP, filePos2);
					mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
					mdlElmdscr_freeAll(&NelmDP);
					mdlElmdscr_freeAll(&elmDP);
	            }
	            else{
	                mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	                mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	                mdlElement_rewrite( &ele2, &ele, filePos2);
	            }

	            if ( type == 3 || type == 4 || type == 6){
	                mdlLinear_extract( lPoi, &lnv, &ele, MASTERFILE);
	                center.x = ( lPoi[0].x + lPoi[1].x)/2;
	                center.y = ( lPoi[0].y + lPoi[1].y)/2;
	                center.z = ( lPoi[0].z + lPoi[1].z)/2;
	            }
	            else if ( type == 12 || type == 14){

	                complexPos = filePos2;
	                mdlElement_read( &elm, MASTERFILE, complexPos);
	                mdlLinear_extract( lPoi, &lnv, &elm, MASTERFILE);
	                center.x = ( lPoi[0].x + lPoi[1].x)/2;
	                center.y = ( lPoi[0].y + lPoi[1].y)/2;
	                center.z = ( lPoi[0].z + lPoi[1].z)/2;
	            }
	            else if( type == 15){
	                if ( mdlModelRef_is3D(MASTERFILE)){
	                    center.x = (double)ele2.ellipse_3d.origin.x;
	                    center.y = (double)ele2.ellipse_3d.origin.y;
	                    center.z = (double)ele2.ellipse_3d.origin.z;
	                }
	                else{
	                    center.x = (double)ele2.ellipse_2d.origin.x;
	                    center.y = (double)ele2.ellipse_2d.origin.y;
	                    center.z = 0;
	                }
	            }
	            create_circle( );
	            chgnum++;
	            fwprintf( fp, L"%6d %12.3lf %12.3lf %12.3lf\n",
	                          chgnum, center.x/1000, center.y/1000, center.z/1000);
        		chkflg = 1;
	            break;
        	}
        	if (chkflg == 1 ){ break;}
        }
    if ( Error == 10){ break;}
    }
}


/*-----------------------------------------------------------------------
|  モジュール名：kensaku_cell  スキャン（セル）                         |
-----------------------------------------------------------------------*/
void    kensaku_cell( )
{
    MSElement           Ncel;
	MSElementDescr      *TelmDP;
    ULong               celendPos, elemAddr[1];
    bool                solid;
    /*int               word;*/
    /*LinkData          setULink;*/

    int                 length;
    UInt16              attrP[MAX_ATTRIBSIZE];
    int                 scanwords;
    double              transform2d[2][2];
    double              transform3d[3][3];
	UInt32              cmpcnt;
	int 				i, chkflg;
    
    fileNum = 0;
    length = 0;
/*----------------------------------------------------------------------+
| 検索条件を設定                                        |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| スキャンリストの初期化等                                  |
+----------------------------------------------------------------------*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

/*----------------------------------------------------------------------+
| scanListの各メンバーに以下をセットする。                  |
+----------------------------------------------------------------------*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;*/
    /*scanList.scantype |= (unsigned short)ONEELEM;*/
    /*scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;

/*----------------------------------------------------------------------+
| 検索対象の決定。                              |
+----------------------------------------------------------------------*/
	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
    scanList.typmask[0] |= TMSK0_CELL_HEADER /* 対象はｾﾙ */;
	for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }

/*----------------------------------------------------------------------+
| ｽｷｬﾝﾘｽﾄの読み込み。                                       |
+----------------------------------------------------------------------*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
    filePos   = 0L;
    scanwords = sizeof( elemAddr)/sizeof( short);
/*----------------------------------------------------------------------+
| ｽｷｬﾝ実行。                                                            |
+----------------------------------------------------------------------*/
	filendpos =mdlElement_getFilePos( FILEPOS_EOF, NULL);
	while( TRUE){
		
        Error = mdlScan_file( elemAddr, &scanwords, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
        mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
    	
/*----------------------------------------------------------------------+
| 要素にｼﾝﾎﾞﾛｼﾞを与える。                                               |
+----------------------------------------------------------------------*/
        type = mdlElement_getType( &ele);

        /*--- 範囲内の要素を検索 ---*/
        switch( type){
            case CELL_HEADER_ELM:
                ce = 1;

                celfilePos = elemAddr[0];
                memcpy( &elece, &ele, sizeof( MSElement));

                /*--- 旧セルの基準点、rMatrix、スケール、セル名を取得 ---*/
                mdlCell_extract( &origin, NULL, &rMatrix, &scale, MSWcell, MAX_CELLNAME_LENGTH, &ele);

                /*--- リンケージ（UDLINK、MSLINK）取得 ---*/
                
                mdlElement_extractAttributes( &length, attrP, &ele);
 
                /*--- セルのレイヤーを取得 ---*/

				mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, FALSE, NULL);
				get_layer(&lv, &co, &lc, &wt, NULL, &elmDP->h.firstElem->el);
				mdlElmdscr_freeAll(&elmDP);
                for( ii = 0; ii < ss; ii++){
                	chkflg = 0;
                	while( TRUE){
	                    /*--- 旧セルのレイヤーが設定値と一致しない場合 ---*/
	                    if ( lv != ctbl[ii].lvo && ctbl[ii].lvo != -1){ break;}
	                    if ( co != ctbl[ii].coo && ctbl[ii].coo != -1){ break;}
	                    if ( lc != ctbl[ii].lco && ctbl[ii].lco != -1){ break;}
	                    if ( wt != ctbl[ii].wto && ctbl[ii].wto != -1){ break;}
	                    if ( wcscmp( MSWcell, ctbl[ii].celo) != 0 && wcscmp( ctbl[ii].celo, L"-1") != 0){
	                        break;
	                    }

	                    /*--- 新セルのセルライブラリ中のポジションを取得 ---*/
						
	                    if ( wcscmp( ctbl[ii].celn, L"-1") == 0){
	                    	
	                    }
	                    else{
	                        swprintf(MSWcell, L"%ls",ctbl[ii].celn);
	                    }
						
						cellPos = mdlCell_existsInLibrary( MSWcell);
                		
	                    /*--- セルがセルライブラリ中に存在しない場合 ---*/
	                    if ( cellPos == FALSE){
	                        lo++;
	                        if ( wcscmp( ctbl[ii].celn, L"-1") == 0){
	                            fwprintf( fp, L"%lsがセルライブラリ中に存在しません。\n", MSWcell);
	                        }
	                        else{
	                            fwprintf( fp, L"%lsがセルライブラリ中に存在しません。\n", ctbl[ii].celn);
	                        }
	                        chkflg = 1;
	                        break;
	                    }

	                    /*--- 新要素のレイヤー ---*/
	                    lvn = ctbl[ii].lvn;   con = ctbl[ii].con;   lcn = ctbl[ii].lcn;   wtn = ctbl[ii].wtn;
	                    if ( ctbl[ii].lvn == -1){ lvn = lv;}
	                    if ( ctbl[ii].con == -1){ con = co;}
	                    if ( ctbl[ii].lcn == -1){ lcn = lc;}
	                    if ( ctbl[ii].wtn == -1){ wtn = wt;}

	                    /*--- スケール ---*/
	                    if ( ctbl[ii].scale >= 0){
	                        scale.x = ctbl[ii].scale;
	                        scale.y = ctbl[ii].scale;
	                    }

	                    /*--- 角度の設定（-1は元の角度と同じ） ---*/
	                    if ( ctbl[ii].rot >= 0){
	                        mdlRMatrix_fromAngle( &rMatrix, ctbl[ii].rot * fc_pi/180);
	                    }

	                    /*--- 新セルの配置 ---*/
						ActiveParams::GetValue(lv, ACTIVEPARAM_LEVEL);
						ActiveParams::GetValue(co, ACTIVEPARAM_COLOR);
						ActiveParams::GetValue(wt, ACTIVEPARAM_LINEWEIGHT);
						ActiveParams::GetValue(lc, ACTIVEPARAM_LINESTYLE);
						ActiveParams::SetValue(lvn, ACTIVEPARAM_LEVEL);
						ActiveParams::SetValue(con, ACTIVEPARAM_COLOR);
						ActiveParams::SetValue(wtn, ACTIVEPARAM_LINEWEIGHT);
						ActiveParams::SetValue(lcn, ACTIVEPARAM_LINESTYLE);
	                    if ( wcscmp( ctbl[ii].celn, L"-1") == 0){
	                        cellPos = mdlCell_existsInLibrary( MSWcell);
	                        cPos = mdlCell_placeCell(  &origin, &scale, FALSE, &rMatrix,
	                                                  NULL, ggnum, TRUE, lvn, 0, MSWcell, NULL);
							ActiveParams::SetValue(lv, ACTIVEPARAM_LEVEL);
							ActiveParams::SetValue(co, ACTIVEPARAM_COLOR);
							ActiveParams::SetValue(wt, ACTIVEPARAM_LINEWEIGHT);
							ActiveParams::SetValue(lc, ACTIVEPARAM_LINESTYLE);


	                        center.x = origin.x;   center.y = origin.y;   center.z = origin.z;
	                        create_circle( );
	                        chgnum++;
	                        fwprintf( fp, L"%6d %12.3lf %12.3lf %12.3lf\n",
	                                      chgnum, center.x/1000, center.y/1000, center.z/1000);

	                        /*--- 新セルヘッダ作成（図形セルの場合） ---*/
	                        if ( ctbl[ii].ten == 0){
	                            mdlCell_create( &Ncel, MSWcell, &origin, FALSE);
	                        }

	                        /*--- 新セルヘッダ作成（点セルの場合） ---*/
	                        if ( ctbl[ii].ten == 1){
	                            mdlCell_create( &Ncel, MSWcell, &origin, TRUE);
	                        }

	                        /*--- 新セルヘッダ作成（セルタイプは元のままの場合） ---*/
	                        if ( ctbl[ii].ten == -1){
	                            /*--- 旧セルヘッダが点セルの場合 ---*/
	                            if ( mdlCell_isPointCell( &elece) == TRUE){
	                                mdlCell_create( &Ncel, MSWcell, &origin, TRUE);
	                            }
	                            /*--- 旧セルヘッダが図形セルの場合 ---*/
	                            if ( mdlCell_isPointCell( &elece) == FALSE){
	                                mdlCell_create( &Ncel, MSWcell, &origin, FALSE);
	                            }
	                        }
	                    }
	                    else{
							swprintf(MSWcell, L"%ls", ctbl[ii].celn);
	                        cellPos = mdlCell_existsInLibrary( MSWcell);
	                        cPos = mdlCell_placeCell( &origin, &scale, FALSE, &rMatrix,
	                                                   NULL, ggnum, TRUE, lvn, 0, MSWcell, NULL);
							ActiveParams::SetValue(lv, ACTIVEPARAM_LEVEL);
							ActiveParams::SetValue(co, ACTIVEPARAM_COLOR);
							ActiveParams::SetValue(wt, ACTIVEPARAM_LINEWEIGHT);
							ActiveParams::SetValue(lc, ACTIVEPARAM_LINESTYLE);

	                        center.x = origin.x;   center.y = origin.y;   center.z = origin.z;
	                        create_circle( );
	                        chgnum++;
	                        fwprintf( fp, L"%6d %12.3lf %12.3lf %12.3lf\n",
	                                      chgnum, center.x/1000, center.y/1000, center.z/1000);

	                        /*--- 新セルヘッダ作成（図形セルの場合） ---*/
	                        if ( ctbl[ii].ten == 0){
	                            mdlCell_create( &Ncel, MSWcell, &origin, FALSE);
	                        }

	                        /*--- 新セルヘッダ作成（点セルの場合） ---*/
	                        if ( ctbl[ii].ten == 1){
	                            mdlCell_create( &Ncel, MSWcell, &origin, TRUE);
	                        }

	                        /*--- 新セルヘッダ作成（セルタイプは元のままの場合） ---*/
	                        if ( ctbl[ii].ten == -1){
	                            /*--- 旧セルヘッダが点セルの場合 ---*/
	                            if ( mdlCell_isPointCell( &elece) == TRUE){
	                                mdlCell_create( &Ncel, MSWcell, &origin, TRUE);
	                            }
	                            /*--- 旧セルヘッダが図形セルの場合 ---*/
	                            if ( mdlCell_isPointCell( &elece) == FALSE){
	                                mdlCell_create( &Ncel, MSWcell, &origin, FALSE);
	                            }
	                        }
	                    }
	                    /*--- スナップ可能にする ---*/
	                    if ( mdlModelRef_is3D(MASTERFILE)){ Ncel.cell_3d.dhdr.props.b.s = 0;}
	                    else{                       Ncel.cell_2d.dhdr.props.b.s = 0;}

	                    /*--- ソリッドにする ---*/
	                    solid = 0;
	                    mdlElement_setProperties( &Ncel, NULL, &ggnum, NULL, NULL, NULL, NULL, NULL, &solid);
	                    /*--- 色、線種、線幅の設定 ---*/
						mdlElement_setSymbology( &Ncel, &con, &wtn, &lcn);
	                    mdlElement_read( &ele, MASTERFILE, cPos);
	                    if ( mdlModelRef_is3D(MASTERFILE)){
	                        memcpy( Ncel.cell_3d.transform, ele.cell_3d.transform, sizeof( transform3d));
	                    }
	                    else{
	                        memcpy( Ncel.cell_2d.transform, ele.cell_2d.transform, sizeof( transform2d));
	                    }
	                    mdlElmdscr_new(&NelmDP, NULL, &Ncel);
						

	                    /*--- 変換前のリンケージ（UDLINK、MSLINK）を与える ---*/
	                    mdlElmdscr_appendAttributes(&NelmDP, length, attrP);

						/*--- 新セルヘッダに要素を追加 ---*/
						mdlElmdscr_read(&TelmDP, cPos, MASTERFILE, 0, NULL);
						if (mdlModelRef_is3D(MASTERFILE)) {
							cmpcnt = TelmDP->el.cell_3d.componentCount;
						}
						else {
							cmpcnt = TelmDP->el.cell_2d.componentCount;
						}
						appendElemtoDP(TelmDP, cmpcnt);

						/*--- 元のセル削除 ---*/
						mdlElmdscr_undoableDelete(TelmDP, cPos, TRUE);
						mdlElmdscr_freeAll(&TelmDP);

						mdlElmdscr_setSymbology(&NelmDP, &con, &lcn, &wtn, 0);

						/*--- 新セルの配置 ---*/
						mdlElmdscr_read(&elmDP, celfilePos, MASTERFILE, 0, NULL);
						mdlElmdscr_validate(NelmDP, 0);
						mdlCell_setRange(NelmDP, ACTIVEMODEL);
						mdlLinkage_deleteStringLinkage(&NelmDP->el, 1, 1);
						mdlElmdscr_rewrite(NelmDP, elmDP, celfilePos);
						mdlElmdscr_display(NelmDP, MASTERFILE, DRAW_MODE_Normal);
						mdlElmdscr_freeAll(&elmDP);
						mdlElmdscr_freeAll(&NelmDP);
                		chkflg = 1;
	                    break;
                	}
        			if (chkflg == 1){break;}
                }
            break;
        }
    }
}


/*-----------------------------------------------------------------------
|  モジュール名:kensaku_text  スキャン（テキスト）                      |
-----------------------------------------------------------------------*/
void    kensaku_text( )
{
TextSizeParam       textSize;
TextParamWide       txtParamWide;
WChar               wcs[256];
/*元int                 word;*/
WChar                stro[256], strn[256];
int                 scanwords;
ULong               elemAddr[1];
int 				i, chkflg;
WString             wstr;
    
    fileNum = 0;
    
    /*--- 検索条件を設定 ---*/
    /*--- スキャンリストの初期化等 ---*/
    mdlScan_initScanlist( &scanList);
    mdlScan_noRangeCheck( &scanList);

    /*--- scanListの各メンバーに以下をセットする。 ---*/
    /*scanList.scantype |= (unsigned short)ELEMDATA;
    scanList.scantype |= (unsigned short)ONEELEM;
    scanList.scantype |= (unsigned short)ELEMTYPE;*/
	mdlScan_setDrawnElements( &scanList);		/* 表示可能要素指定	*/
	scanList.scantype     = NESTCELL | PROPCLAS | ELEMTYPE | LEVELS | ONEELEM;
    scanList.extendedType = FILEPOS;
    scanList.clasmsk      = 0xFFF1;
	
    /*--- 検索対象の決定 ---*/
	for( i=0; i<8; i++)	scanList.typmask[i] = 0x0000;
    scanList.typmask[1] = TMSK1_TEXT;        /* 対象はﾃｷｽﾄ */
    for( i=0; i<4; i++){
        scanList.levmask[i] = 0xFFFF;
    }
    /*--- ｽｷｬﾝﾘｽﾄの読み込み ---*/
    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt(L"スキャンリストが受け入れられませんでした。");
        mdlSystem_exit( exitStatus, 1);
    }
    filePos   = 0L;
    scanwords = sizeof( elemAddr)/sizeof( short);
    /*--- ｽｷｬﾝ実行 ---*/
	
    while( TRUE){
        Error = mdlScan_file( elemAddr, &getSize, sizeof( elemAddr), &filePos);
		
        if ( filePos > filendpos){ break;}
        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox(L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox(L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox(L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox(L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}
        mdlElement_read( &ele, MASTERFILE, elemAddr[0]);
        /*--- 要素にｼﾝﾎﾞﾛｼﾞを与える ---*/
        type = mdlElement_getType( &ele);

        /*--- 旧テキストのレイヤーを取得 ---*/
        get_layer( &lv, &co, &lc, &wt, &ggnum, &ele);
        /*word     = mdlElement_igdsSize( &ele, 0);*/

        /*--- 範囲内の要素を検索 ---*/
        switch( type){
           case TEXT_ELM:
              for( ii=0; ii<tt; ii++){
              	chkflg = 0;

              	while( TRUE){
	                 /*--- 旧要素のレイヤーが設定値と違う場合 ---*/
	                 if ( lv != ttbl[ii].lvo && ttbl[ii].lvo != -1){ break;}
	                 if ( co != ttbl[ii].coo && ttbl[ii].coo != -1){ break;}
	                 if ( lc != ttbl[ii].lco && ttbl[ii].lco != -1){ break;}
	                 if ( wt != ttbl[ii].wto && ttbl[ii].wto != -1){ break;}

	                 /*--- 文字列、基準点、角度、文字サイズ、文字間隔、フォント、基準点の位置を取得 ---*/
	                 textSize.mode = TXT_BY_TILE_SIZE;
	                 mdlText_extract( &wstr, NULL, &origin, &rMatrix, &textSize, &txtParamWide, NULL, &ele);
					 swprintf(wcs, L"%ls", wstr.GetWCharCP());

	                 /*--- 旧要素の文字列が設定値と違う場合 ---*/
	                 if ( wcscmp( ttbl[ii].stro, wcs) != 0 && wcscmp( ttbl[ii].stro, L"-1") != 0){
	                    break;
	                 }

	                 filePos2 = elemAddr[0];

	                 /*--- 新要素のレイヤー ---*/
	                 lvn = ttbl[ii].lvn;   con = ttbl[ii].con;   lcn = ttbl[ii].lcn;   wtn = ttbl[ii].wtn;
	                 if ( ttbl[ii].lvn == -1){ lvn = lv;}
	                 if ( ttbl[ii].con == -1){ con = co;}
	                 if ( ttbl[ii].lcn == -1){ lcn = lc;}
	                 if ( ttbl[ii].wtn == -1){ wtn = wt;}

	                 /*--- 新要素の文字列 ---*/
	                 if ( wcscmp( ttbl[ii].strn, L"-1") == 0){ }         // 旧要素と同じ
	                 else{                                    wcscpy( wcs, ttbl[ii].strn);}// 変更
	                 

	                 /*--- 文字サイズの設定 ------*/
	                 if ( ttbl[ii].thn != -1){
	                    textSize.size.height = ttbl[ii].thn * 1000;
	                    textSize.size.width  = ttbl[ii].thn * 1000;
	                 }

	                 /*--- 文字間隔の設定 ------*/
	                 if ( ttbl[ii].tpn != -1){
	                    txtParamWide.characterSpacing = ttbl[ii].tpn * 1000;
	                 }

	                 /*--- 角度の設定（-1は元の角度と同じ） ------*/
	                 if ( ttbl[ii].rot != -1){
	                    mdlRMatrix_fromAngle( &rMatrix, ttbl[ii].rot * fc_piover180);
	                 }

	                 /*--- フォント（-1は元の角度と同じ）の設定 ------*/
	                 if ( ttbl[ii].ft != -1){
	                    txtParamWide.font = ttbl[ii].ft;
	                 }

	                 /*--- テキスト配置 ---*/
	                 txtParamWide.flags.interCharSpacing = TRUE;
	                 if ( txtParamWide.characterSpacing == 0){
	                    txtParamWide.flags.interCharSpacing = FALSE; // 文字間を使わない（こうしないと文字間0でもユーザデータリンクがついてしまう）
	                 }
              		
	                 mdlText_create( &ele2, &ele, wcs, &origin, &rMatrix, &textSize, &txtParamWide, NULL, MASTERFILE);
	                 mdlElement_setProperties( &ele2, &lvn, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	                 mdlElement_setSymbology( &ele2, &con, &wtn, &lcn);
	                 mdlElement_rewrite( &ele2, &ele, filePos2);
	                 center.x = origin.x;
	                 center.y = origin.y;
	                 center.z = origin.z;
	                 create_circle( );
	                 chgnum++;
	                 fwprintf( fp, L"%6d %12.3lf %12.3lf %12.3lf\n"
	                            , chgnum, center.x/1000, center.y/1000, center.z/1000);
              		 chkflg = 1;
	                 break;
              	  }
              	  if (chkflg == 1){ break;}
              }
			  break;
        }
    }
}


/*-----------------------------------------------------------------------
|  モジュール名：create_circle   * チェック用のサークル作成              |
-----------------------------------------------------------------------*/
void    create_circle( )
{
    if ( layset->check == -1){
        clv = 63;   cco = 0;   clc = 0;   cwt = 2;
        mdlEllipse_create( &cele, NULL, &center, 2000, 2000, NULL, 0);
        element_add( &clv, &cco, &clc, &cwt, NULL, &cele);
    }
}

/*------------------------------------------------------------------------
**	画層名→画層ID
------------------------------------------------------------------------*/
int cnv_level_name2IDa(LevelId *lvID, WChar *wlvName)
{

	StatusInt	status;

	/*----- （6-3562） -----*/
	status = mdlLevel_getIdFromName(lvID, MASTERFILE, LEVEL_NULL_ID, wlvName);

	return(status);
}
/*-----------------------------------------------------------------------
**	要素記述子の構成要素を追加する
-----------------------------------------------------------------------*/
void  appendElemtoDP(MSElementDescr *TelmDP, UInt32 cmpcnt)
{
	while (cmpcnt != 0)
	{
		if (TelmDP->h.isHeader) {
			appendElemtoDP(TelmDP->h.firstElem, cmpcnt);
			return;
		}
		else
		{
			mdlElmdscr_appendElement(NelmDP, &TelmDP->el);
			cmpcnt--;
			appendElemtoDP(TelmDP->h.next, cmpcnt);
			return;
		}
	}
	return;
}