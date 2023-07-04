/*-----------------------------------------------------------------------*/
/*-     参照をＦＦソース                                                -*/
/*-         FF.MC                                                       -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\MdlApi.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\mselems.h>
#include <Mstn\MdlApi\rdbmslib.fdf>
#include <Mstn\MdlApi\deprecated10_0.fdf>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../CommonCE/naka8.cpp"

#include "ffref.h"

FILE            *fp;
RscFileHandle   Rsc;
int             exitStatus, gridct, namect, elect, ii, polnv, masElemCt;
WChar           file[256], filename[256], info[256], orifile[256];
UInt32          CO, WT, LV;
int             LC, TYP;
ULong           gridPos[5000], shapePos;
Dpoint3d        polPoi[10];

FFREF           *ffref;
#ifndef Private
#define Private
#endif

/*- プロトタイプ    -*/
Private  bool   unload(UnloadProgramReason unloadType);
Private void    syori( void);
Private void    scanShape( void);
Private int     get_Number( void);
Private int     ff_copy( void);
Private int     ff_copy_text( void);
Private int     delete1( void);
Private void    no_del( void);
Private int fence_clip_outside(void);
Private int fence_clip_inside(void);
Private void    on_all_dgn(void);
Private void    on_all_ref(void);

/*-----------------------------------------------------------------------
**  メイン
-----------------------------------------------------------------------*/
extern "C" DLLEXPORT void MdlMain
(
	int             argc,
	WCharCP         argv[]
)
{
UInt32           param[8][4];
int             slot;
CExprValue      value;
CExprResult     result;
DgnModelRefP		modelRefP;
ModelRefIteratorP	iterator;
bool				active;
UInt32          oldoptimized;


    for( ii=0; ii<8; ii++){
        param[ii][0] = 0xFFFF;
        param[ii][1] = 0xFFFF;
        param[ii][2] = 0xFFFF;
        param[ii][3] = 0x7FFF;
    }

    /*--- リソースファイルのオープン ---*/
    if ( mdlResource_openFile( &Rsc, NULL, RSC_READONLY) != SUCCESS){
        mdlOutput_error( L"FF のリソースファイルが開きません");
        mdlSystem_exit( exitStatus, 1);
    }

	/*--- アンロード関数のセット ---*/
	SystemCallback::SetUnloadProgramFunction(unload);
	mdlDialog_sendMessagesOnUnload(mdlSystem_getCurrMdlDesc(), TRUE);

    /*--- 広域変数の取得 ---*/
    if ( mdlCExpression_getValue( &value, &result, "&ffref",
         VISIBILITY_DIALOG_BOX | VISIBILITY_CALCULATOR) != SUCCESS){
        mdlDialog_openInfoBox( L"広域変数が取得できません");
        mdlSystem_exit( exitStatus, 1);
    }
    ffref = (FFREF *)value.val.valPointer;
    wcscpy( orifile,  tcb->dgnfilenm);

    /*--- フィットオール・ＤＰ・参照ファイルのLV_ON・設定値の保存 ---*/
    mdlView_fit( 0, 0);
    mdlView_setDisplayDepth( 0, -999999.0000, 3294968.2940);
    mdlView_updateSingle( 0);
	/*--- 最適化された枠クリップを無効にする ---*/
	oldoptimized = userPrefsP->smartGeomFlags.optimizedFenceClip;
	userPrefsP->smartGeomFlags.optimizedFenceClip = 0;


	/*
DgnModelRefP		modelRefP;
ModelRefIteratorP	iterator;
bool				active;
WChar               valuestr[512];
LevelId             lvId;
int                 cnt = 0;
bool                ret;

swprintf(valuestr, L"");
mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
{
	mdlRefFile_getBooleanParameters(&active, REFERENCE_SLOTACTIVE, modelRefP);
	if (mdlModelRef_isReference(modelRefP)) {

		LevelIteratorR lviter = mdlLevelIterator_create(modelRefP);

		while (TRUE)
		{
			if (cnt == 0) {
				lvId = mdlLevelIterator_getFirst(lviter);
				cnt = cnt++;
			}
			else {
				lvId = mdlLevelIterator_getNext(lviter);
			}
			mdlLevel_setDisplay(modelRefP, lvId, FALSE);
			ret = mdlLevel_isValid(modelRefP, lvId);
			if (!ret) {
				break;
			}
		}
		mdlLevelIterator_free(lviter);
	}
}
mdlModelRefIterator_free(&iterator);







	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_PrimaryChildRefs, 0);
	while (NULL != (modelRefP = mdlModelRefIterator_getNext(iterator)))
	{
		mdlRefFile_getBooleanParameters(&active, REFERENCE_SLOTACTIVE, modelRefP);
        if (!mdlModelRef_isReference(modelRefP)) {
            //mdlRefFile_setParameters( param, REFERENCE_LEVELFLAGS, ii);REFERENCE_LEVELCONTROLSDISPLAY
			mdlRefFile_setIntegerParameters(param, REFERENCE_LEVEL, modelRefP);
            mdlRefFile_writeAttachment( modelRefP);
        }
    }
	*/
    mdlSystem_fileDesign( );

    scanShape( );

	userPrefsP->smartGeomFlags.optimizedFenceClip = oldoptimized;

}


/*-----------------------------------------------------------------------
**  アンロード関数
-----------------------------------------------------------------------*/
Private  bool   unload(UnloadProgramReason unloadType)
{
    return( SUCCESS);
}


/*----------------------------------------------------------------------+
|  モジュール名：scanShape                                              |
+----------------------------------------------------------------------*/
Private void scanShape( )
{
int    		getSize;
int    		Error;
int    		word;
ULong  		filePos, EndPos, percent;

MSElement   ele;
ExtScanList    scanList;
ULong		elemAddr[1];
//int			i, numAddr;

    /*--- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

   // scanList.scantype |= ( unsigned short)ELEMDATA;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= TMSK0_SHAPE;       /* 対象はｼｪｲﾌﾟ */

    /*--- スキャン実行 ---------------*/
    filePos = 0;   gridct = 0;

    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
        //Error = mdlScan_file( &ele, &getSize, sizeof( ele), &filePos);
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);

        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        get_layer( &LV, &CO, &LC, &WT, NULL, &ele);

        if ( ( ffref->lv == LV) && ( ffref->co == CO) && ( ffref->wt == WT) && ( ffref->lc == LC)){
            //gridPos[gridct] = filePos;
			gridPos[gridct] = elemAddr[0];
            gridct++;
        }
    }

    /*--- シェイプでフェンスを作成 ---*/
    for( ii=0; ii<gridct; ii++){
        elect    = 0;   namect = 0;
        shapePos = gridPos[ii];
        mdlElement_read( &ele, MASTERFILE, shapePos);
        mdlFence_fromShape( &ele);

        /*--- フェンスの設定（内側重なり） ---*/
        set_fence_overlap( );
        tcb->fenvw = 0;

        /*--- フェンス内のテキストを読み込む ---*/
        mdlState_startFenceCommand((StateFunc_FenceContentOp)get_Number, NULL, NULL, NULL, NULL, NULL, NULL);
        mdlFence_process( NULL);

        /*--- アクティブのテキストが１つで、参照要素が1つ以上ある場合 ---*/
        if ( elect != 0 && namect == 1){
            mdlLinear_extract( polPoi, &polnv, &ele, MASTERFILE);
            mdlFile_copy( file, tcb->dgnfilenm);
            if ( mdlSystem_newDesignFile( filename) == SUCCESS){
				masElemCt = 0;
                syori( );
				if (masElemCt == 0) mdlFile_delete(filename);
            }
        }
        mdlFence_clear( TRUE);
    } // FOR II END
}


/*-----------------------------------------------------------------------
**  get_Number : シェイプ内の図面番号を記憶する
-----------------------------------------------------------------------*/
Private int get_Number( void)
{
ULong  		   Position;
WChar   		str[128];
WChar   		devP[64], dirP[64], nameP[64];
int    		   fileNum, typ;
MSElement      ele;
DgnModelRefP   ModelRefP;
WString        wstr;

    Position = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

	if (!mdlModelRef_isValid(ModelRefP))
		return ERROR;


    /*--- 参照ファイルの場合 ---*/
    if (mdlModelRef_isReference(ModelRefP)){
       elect++;
    }

    /*--- アクティブファイルの場合 ---*/
    if (!mdlModelRef_isReference(ModelRefP)){
        mdlElement_read( &ele, MASTERFILE, Position);
        typ = mdlElement_getType( &ele);

        /*--- テキストの場合 ---*/
        if ( typ == 17){
            mdlElement_read( &ele, MASTERFILE, Position);
            mdlText_extract( &wstr, NULL, NULL, NULL, NULL, NULL, NULL, &ele);
			swprintf(str, L"%ls", wstr.GetWCharCP());
            get_layer( &LV, &CO, &LC, &WT, NULL, &ele);
            wcscpy( file, L"");
            if ( ffref->nolv == LV && ffref->noco == CO && ffref->nowt == WT && ffref->nolc == LC){
                mdlFile_parseName( tcb->dgnfilenm, devP, dirP, nameP, NULL);
                if ( ffref->rdl != -1){
                    swprintf( file, L"%s:%s%s.dgn", devP, dirP, str);
                }
                else{
                    swprintf( file, L"%s:%s%s.rdl", devP, dirP, str);
                }
                wcscpy( filename, file);
                namect++;
            }
        }
    }
    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  syori
-----------------------------------------------------------------------*/
Private void  syori(void)
{
	MSElement       buf, pele, oele;
	MSElementDescr    *elmDP;
	static short    viewList[8] = { 1, 0, 0, 0, 0, 0, 0, 0 };
	static short    levelMask[4] = { 0xFFFF, 0xFFFF, 0xFFFF, 0x7FFF };
	int             snv, polNo;
	Dpoint3d        sPoi[5];  // 図郭の4隅の座標
	Dpoint3d        fPoi[5];  // 図郭から1mm内側の4隅の座標
	Dpoint3d        oPoi[5];  // 図郭から1mm外側の4隅の座標
	double          AB_len, BC_len;

	tcb->fenvw = 0;
	/*--- 図郭の情報を調べる ---*/
	mdlElement_read(&buf, MASTERFILE, shapePos);
	mdlLinear_extract(sPoi, &snv, &buf, MASTERFILE);


	/*--- 図郭から1mm内側に平行移動した枠の座標を求める ---*/
	AB_len = get_leng(&sPoi[0], &sPoi[1]);
	BC_len = get_leng(&sPoi[1], &sPoi[2]);

	/*--- 辺ABが辺BCより長い場合 ---*/
	if (AB_len >= BC_len) {
		/*--- 頂点AのX座標が頂点BのX座標よりも小さい場合 ---*/
		if (sPoi[0].x < sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;   oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x - 1;               oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x - 1;               oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x + 1;               oPoi[3].x = sPoi[3].x - 1;

			/*--- 頂点BのY座標が頂点CのY座標よりも大きい場合 ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 1;
			}

			/*--- 頂点BのY座標が頂点CのY座標よりも小さい場合 ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 2;
			}
		}

		/*--- 頂点AのX座標が頂点BのX座標よりも大きい場合 ---*/
		else if (sPoi[0].x > sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- 頂点BのY座標が頂点CのY座標よりも大きい場合 ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 3;
			}

			/*--- 頂点BのY座標が頂点CのY座標よりも小さい場合 ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 4;
			}
		}

		/*--- 頂点BのX座標が頂点CのX座標よりも小さい場合 ---*/
		else if (sPoi[1].x < sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;   oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x + 1;               oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x - 1;               oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x - 1;               oPoi[3].x = sPoi[3].x + 1;

			/*--- 頂点AのY座標が頂点BのY座標よりも大きい場合 ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 9;
			}

			/*--- 頂点AのY座標が頂点BのY座標よりも小さい場合 ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 10;
			}
		}

		/*--- 頂点BのX座標が頂点CのX座標よりも大きい場合 ---*/
		else if (sPoi[1].x > sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- 頂点AのY座標が頂点BのY座標よりも大きい場合 ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 11;
			}

			/*--- 頂点AのY座標が頂点BのY座標よりも小さい場合 ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 12;
			}
		}
	} // IF(AB > BC) END

	/*--- 辺ABが辺BCより短い場合 ---*/
	else if (AB_len < BC_len) {
		/*--- 頂点BのX座標が頂点CのX座標よりも小さい場合 ---*/
		if (sPoi[1].x < sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x - 1;                oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- 頂点AのY座標が頂点BのY座標よりも大きい場合 ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 5;
			}

			/*--- 頂点AのY座標が頂点BのY座標よりも小さい場合 ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 6;
			}
		}

		/*--- 頂点BのX座標が頂点CのX座標よりも大きい場合 ---*/
		else if (sPoi[1].x > sPoi[2].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- 頂点AのY座標が頂点BのY座標よりも大きい場合 ---*/
			if (sPoi[0].y > sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 7;
			}

			/*--- 頂点AのY座標が頂点BのY座標よりも小さい場合 ---*/
			else if (sPoi[0].y < sPoi[1].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 8;
			}
		}

		/*--- 頂点AのX座標が頂点BのX座標よりも小さい場合 ---*/
		if (sPoi[0].x < sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x + 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x - 1;
			fPoi[1].x = sPoi[1].x - 1;                oPoi[1].x = sPoi[1].x + 1;
			fPoi[2].x = sPoi[2].x - 1;                oPoi[2].x = sPoi[2].x + 1;
			fPoi[3].x = sPoi[3].x + 1;                oPoi[3].x = sPoi[3].x - 1;

			/*--- 頂点BのY座標が頂点CのY座標よりも大きい場合 ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y + 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y - 1;
				polNo = 13;
			}

			/*--- 頂点BのY座標が頂点CのY座標よりも小さい場合 ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 14;
			}
		}

		/*--- 頂点AのX座標が頂点BのX座標よりも大きい場合 ---*/
		else if (sPoi[0].x > sPoi[1].x) {
			fPoi[0].x = fPoi[4].x = sPoi[0].x - 1;    oPoi[0].x = oPoi[4].x = sPoi[0].x + 1;
			fPoi[1].x = sPoi[1].x + 1;                oPoi[1].x = sPoi[1].x - 1;
			fPoi[2].x = sPoi[2].x + 1;                oPoi[2].x = sPoi[2].x - 1;
			fPoi[3].x = sPoi[3].x - 1;                oPoi[3].x = sPoi[3].x + 1;

			/*--- 頂点BのY座標が頂点CのY座標よりも大きい場合 ---*/
			if (sPoi[1].y > sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y - 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y + 1;
				fPoi[1].y = sPoi[1].y - 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y + 1;                oPoi[2].y = sPoi[2].y - 1;
				fPoi[3].y = sPoi[3].y + 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 15;
			}

			/*--- 頂点BのY座標が頂点CのY座標よりも小さい場合 ---*/
			else if (sPoi[1].y < sPoi[2].y) {
				fPoi[0].y = fPoi[4].y = sPoi[0].y + 1;    oPoi[0].y = oPoi[4].y = sPoi[0].y - 1;
				fPoi[1].y = sPoi[1].y + 1;                oPoi[1].y = sPoi[1].y - 1;
				fPoi[2].y = sPoi[2].y - 1;                oPoi[2].y = sPoi[2].y + 1;
				fPoi[3].y = sPoi[3].y - 1;                oPoi[3].y = sPoi[3].y + 1;
				polNo = 16;
			}
		}
	} // ELSE IF(AB < BC) END

		/*--- dgn 全画層表示 ---*/
	on_all_dgn();

	/*--- ref 全画層表示 ---*/
	on_all_ref();

	/*--- フェンスの設定（外側重なり） ---*/
	set_fence_void_overlap();

	/*--- 図郭から1mm外側にシェイプを作成 ---*/
	mdlShape_create(&oele, &buf, oPoi, snv, -1);
	mdlFence_fromShape(&oele);

	/*--- 枠外（隣接図郭と隣接図番）を削除する ---*/
	mdlState_startFenceCommand((StateFunc_FenceContentOp)delete1, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
	mdlFence_process(NULL);

	tcb->fenvw = 0;
	/*--- 処理が全要素の場合 ---*/
	if (ffref->mode == 0) {
		/*--- フェンスの設定（内側切取） ---*/
		set_fence_clip();

		/*--- 図郭で枠作成 ---*/
		mdlFence_fromShape(&buf);

		//--- フェンス内側重なり ---
		mdlParams_setLock(0, ACTIVELOCK_FENCECLIP);
		mdlParams_setLock(1, ACTIVELOCK_FENCEOVERLAP);
		mdlParams_setLock(0, ACTIVELOCK_FENCEVOID);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_NO_CLIP);
		mdlFence_process(NULL);

		//--- フェンス外側切り取り ---
		mdlParams_setLock(1, ACTIVELOCK_FENCECLIP);
		mdlParams_setLock(0, ACTIVELOCK_FENCEOVERLAP);
		mdlParams_setLock(1, ACTIVELOCK_FENCEVOID);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_outside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
		mdlFence_process(NULL);

		//--- フェンスの設定（外側重なり） ---
		set_fence_void_overlap();
		mdlFence_fromShape(&oele);
		mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_outside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
		mdlFence_process(NULL);

	}

	/*--- 処理がセルのみの場合 ---*/
	if (ffref->mode == 1) {
		/*--- フェンスの設定（内側重なり） ---*/
		set_fence_overlap();

		/*--- 図郭で枠作成 ---*/
		mdlFence_fromShape(&buf);

		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);

	}

	/*--- ポリゴンの場合 ---*/
	if (ffref->mode == 2) {
		/*--- フェンスの設定（内側重なり） ---*/
		set_fence_overlap();

		/*--- 図郭から内側に1mm転移で枠作成 ---*/
		mdlShape_create(&pele, &buf, fPoi, snv, -1);
		mdlFence_fromShape(&pele);

		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);
	}
	/*

	//--- 参照ファイルから枠内をコピー ---
	mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
	mdlFence_process( NULL);
	*/
	//--- 処理が全要素・ポリゴンの場合 ---
	if (ffref->mode != 1) {
		//--- フェンスの設定（内側重なり） ---
		set_fence_overlap();

		//--- 参照ファイルから枠内をコピー（テキスト要素） ---
		mdlState_startFenceCommand((StateFunc_FenceContentOp)ff_copy_text, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_COPY);
		mdlFence_process(NULL);
	}

	mdlRefFile_detach(REFERENCE_All_References);                                   // 参照ファイルを全デタッチ
	mdlView_setLevels(TRUE, levelMask, viewList, TRUE);      // LvをすべてON

	/*--- 図面番号を削除 ---*/
	no_del();
	if (ffref->mode != 0) {
		mdlElmdscr_read(&elmDP, shapePos, MASTERFILE, 0, NULL);
		mdlElmdscr_undoableDelete(elmDP, shapePos, TRUE);
		mdlElmdscr_freeAll(&elmDP);
	}
	/*--- 要素の存在検索 ---*/
	if (ffref->mode == 0) {
		set_fence_clip();
	}
	else {
		set_fence_overlap();
	}
	mdlFence_fromShape(&buf);
	mdlState_startFenceCommand((StateFunc_FenceContentOp)fence_clip_inside, NULL, NULL, NULL, NULL, NULL, FENCE_CLIP_ORIG);
	mdlFence_process(NULL);
	
	mdlElement_add(&buf);
	mdlElement_display(&buf, DRAW_MODE_Normal);
	
    mdlView_fit( 0, 0);                                      // フィットオール
    mdlView_setDisplayDepth( 0, -999999.0000, 3294968.2940); // DPの再設定
    mdlView_updateSingle( 0);                                // ビューの再表示
    mdlSystem_fileDesign( );                                 // 設定値を保存

    mdlSystem_newDesignFile( orifile);
}


/*-----------------------------------------------------------------------
**  ff_copy
-----------------------------------------------------------------------*/
Private int  ff_copy( void)
{
MSElement         ele;
MSElementDescr    *elmDP;
ULong             elePos;
int               fileNum, celct;
Dpoint3d          celPoi;
DgnModelRefP      ModelRefP;
WChar  fileName[512];
int  fileNameBufSize;

    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

	//fileNameBufSize = 512;
	//mdlModelRef_getFileName( ModelRefP,	fileName, fileNameBufSize);
	
	
    /*--- 参照ファイルから要素をコピー ---*/
	if (mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, ModelRefP, elePos);
        TYP = mdlElement_getType( &ele);

        /*--- セル ---*/
        if ( TYP == 2){
            celct = 0;
            /*--- セルのみの場合 ---*/
            if ( ffref->mode == 1){
                mdlCell_extract( &celPoi, NULL, NULL, NULL, NULL, NULL, &ele);

                /*--- セルの基準がポリゴン外の場合 ---*/
                if ( 0 == judge_inout( &polnv, polPoi, &celPoi)){
                    celct++;
                }
            }

            if ( celct == 0){
                mdlElmdscr_read( &elmDP, elePos, ModelRefP, TRUE, NULL);
                mdlElmdscr_add( elmDP);
                mdlElmdscr_display( elmDP, ModelRefP, DRAW_MODE_Normal);
                mdlElmdscr_freeAll( &elmDP);
            }
        }

        /*--- 複合要素 ---*/
        else if ( TYP == 12 || TYP == 14){
            /*--- セルのみでない場合 ---*/
            if ( ffref->mode != 1){
               mdlElmdscr_read( &elmDP, elePos, ModelRefP, TRUE, NULL);
               mdlElmdscr_add( elmDP);
               mdlElmdscr_display( elmDP, ModelRefP, DRAW_MODE_Normal);
               mdlElmdscr_freeAll( &elmDP);
           }
        }

        /*--- テキスト ---*/
        else if ( TYP == 17){
            /*--- セルのみでない場合 ---*/
            //if ( ffref->mode != 1){
                //mdlElement_add( &ele);
                //mdlElement_display( &ele, NORMALDRAW);
            //}
        }
        /*--- その他 ---*/
        else{
            /*--- セルのみでない場合 ---*/
            if ( ffref->mode != 1){
                mdlElement_add( &ele);
                mdlElement_display( &ele, DRAW_MODE_Normal);
            }
        }
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  ff_copy_text
-----------------------------------------------------------------------*/
Private int  ff_copy_text( void)
{
MSElement         ele;
ULong             elePos;
int               fileNum;
Dpoint3d          orig;
DgnModelRefP      ModelRefP;

    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

    /*--- 参照ファイルから要素をコピー ---*/
	if (mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, ModelRefP, elePos);
        TYP = mdlElement_getType( &ele);

        /*--- テキストの場合 ---*/
        if ( TYP == 17){
            mdlText_extract( NULL, &orig, NULL, NULL, NULL, NULL,   NULL, &ele);

            /*--- テキストの基準点が図郭内の場合 ---*/
            if ( 1 == judge_inout( &polnv, polPoi, &orig)){
               mdlElement_add( &ele);
               mdlElement_display( &ele, DRAW_MODE_Normal);
            }
        }
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  delete1
-----------------------------------------------------------------------*/
Private int  delete1( void)
{
MSElement    ele;
ULong        elePos;
int          fileNum;
int          lv, co, lc, wt;
MSElementDescr  *elmDP;
DgnModelRefP      ModelRefP;


    elePos = mdlElement_getFilePos( FILEPOS_CURRENT, &ModelRefP);

    /*--- アクティブなデザインファイルの要素のみ削除 ---*/
    if ( !mdlModelRef_isReference(ModelRefP)) {
        mdlElement_read( &ele, MASTERFILE, elePos);
        mdlElmdscr_read( &elmDP, elePos, MASTERFILE, 0, NULL);
        mdlElmdscr_undoableDelete( elmDP, elePos, TRUE);
        mdlElmdscr_freeAll( &elmDP);
    }

    return( SUCCESS);
}


/*-----------------------------------------------------------------------
**  図面番号削除
-----------------------------------------------------------------------*/
Private void  no_del( void)
{
int    getSize;
int    Error;
int    word;
UInt32 tlv, tco, twt;
int    tlc;
ULong  filePos, delPos;

MSElementDescr  *elmDP;
MSElement       ele;
ExtScanList        scanList;
ULong		elemAddr[1];
//int			i, numAddr;

    /*--- スキャンリストの初期化 ---------------*/
    mdlScan_initScanlist( &scanList);

    /*--- 検索範囲の設定 ---------------*/
    mdlScan_noRangeCheck( &scanList);

    scanList.scantype |= ( unsigned short)FILEPOS;
    scanList.scantype |= ( unsigned short)ONEELEM;
    scanList.scantype |= ( unsigned short)ELEMTYPE;

    scanList.typmask[0] |= TMSK0_LINE;        /* 対象はﾗｲﾝ */
    scanList.typmask[0] |= TMSK0_LINE_STRING; /* 対象はﾗｲﾝｽﾄﾘﾝｸﾞ */
    scanList.typmask[1] |= TMSK1_TEXT;        /* 対象はﾃｷｽﾄ */

    /*--- スキャン実行 ---------------*/
    filePos = 0;

    if ( mdlScan_initialize( MASTERFILE, &scanList) != SUCCESS){
        mdlOutput_prompt( L"スキャンリストが受け入れられませんでした。");
        mdlOutput_message( L"アンロードしました。");
        mdlSystem_exit( exitStatus, 1);
    }

    while( TRUE){
		Error = mdlScan_file(elemAddr, &getSize, sizeof(elemAddr), &filePos);

        if ( Error == 10){ break;}
        if ( Error == 65){ mdlDialog_openInfoBox( L"無効なファイル番号");}
        if ( Error == 66){ mdlDialog_openInfoBox( L"EOFエラー");}
        if ( Error == 67){ mdlDialog_openInfoBox( L"スキャンリストに誤り");}
        if ( Error == 68){ mdlDialog_openInfoBox( L"無効な要素");}
        if ( Error >= 65){ mdlSystem_exit( exitStatus, 1);}

		mdlElement_read(&ele, MASTERFILE, elemAddr[0]);
        get_layer( &tlv, &tco, &tlc, &twt, NULL, &ele);

        /*--- 指定した図面番号と同じレイヤーのテキストを削除する ---*/
        if ( ( ffref->nolv == tlv) && ( ffref->noco == tco) && ( ffref->nowt == twt) && ( ffref->nolc == tlc)){
			delPos = elemAddr[0];
            mdlElmdscr_read( &elmDP, delPos, MASTERFILE, 0, NULL);
            mdlElmdscr_undoableDelete( elmDP, delPos, TRUE);
            mdlElmdscr_freeAll( &elmDP);
        }

        /*--- 指定した図枠と同じレイヤーのラインを削除する ---*/
        else if ( ( ffref->lv == tlv) && ( ffref->co == tco) && ( ffref->wt == twt) && ( ffref->lc == tlc)){
            delPos = elemAddr[0];
            mdlElmdscr_read( &elmDP, delPos, MASTERFILE, 0, NULL);
            mdlElmdscr_undoableDelete( elmDP, delPos, TRUE);
            mdlElmdscr_freeAll( &elmDP);
        }
    } // WHILE END
}

/*------------------------------------------------------------------------
**  フェンス（外側切り取り）
------------------------------------------------------------------------*/
int fence_clip_outside(void)
{
	DgnModelRefP    model;
	ULong           filePos;
	MSElementDescr  *dscr;
	int             type;

	filePos = mdlElement_getFilePos(FILEPOS_CURRENT, &model);
	if (ffref->mode == 0) {
		if (model == MASTERFILE) {
			mdlElmdscr_read(&dscr, filePos, model, FALSE, NULL);

			mdlElmdscr_undoableDelete(dscr, filePos, TRUE);
			mdlElmdscr_freeAll(&dscr);
			return(SUCCESS);

		}
	}
	return(SUCCESS);
}

/*------------------------------------------------------------------------
**  フェンス（内側切り取り）
------------------------------------------------------------------------*/
int fence_clip_inside(void)
{
	DgnModelRefP    model;
	ULong           filePos;
	MSElementDescr  *dscr;
	int             type;

	filePos = mdlElement_getFilePos(FILEPOS_CURRENT, &model);
	
	if (model == MASTERFILE) {

		masElemCt++;
		return(SUCCESS);

	}

	return(SUCCESS);
}

/*------------------------------------------------------------------------
**  dgn 全画層表示
------------------------------------------------------------------------*/
Private void on_all_dgn(void)
{
	int        ii;
	short      viewList[8];
	short      levMask[4];

	for (ii = 0; ii < 8; ii++) { viewList[ii] = 1; }
	for (ii = 0; ii < 4; ii++) { levMask[ii] = 0xFF; }
	mdlView_setLevels(TRUE, levMask, viewList, FALSE);

}

/*------------------------------------------------------------------------
**  ref 全画層表示
------------------------------------------------------------------------*/
Private void on_all_ref(void)
{
	ModelRefIteratorP   iterator = NULL;
	DgnModelRefP        model;
	UInt32              ii;
	LevelId             levelID;
	bool                disp = TRUE;

	mdlModelRefIterator_create(&iterator, MASTERFILE, MRITERATE_Root | MRITERATE_PrimaryChildRefs, 0);

	/*--- 参照ファイル ---*/
	while ((model = mdlModelRefIterator_getNext(iterator)) != NULL) {
		/*--- 参照ファイルを表示 ---*/
		mdlRefFile_setBooleanParameters(disp, REFERENCE_DISPLAY, model);

		/*--- 最大画層ID ---*/
		mdlLevelTable_getHighestLevelId(&levelID, model);

		/*--- 画層表示 ---*/
		for (ii = 1; ii <= levelID; ii++) {
			mdlView_setLevelDisplay(model, tcb->lstvw, ii, TRUE);
		}

		/*--- 参照情報を保存 ---*/
		mdlRefFile_writeAttachmentConditionally(model);
	}

	mdlModelRefIterator_free(&iterator);
}


