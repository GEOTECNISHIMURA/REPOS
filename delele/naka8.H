
#ifndef __nakalibH__
#define __nakalibH__


#include <Mstn\MdlApi\MdlApi.h>	// すげえ集めたやつ

USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;


/*--- デバッグの時のみメッセージ出力 ---*/
#ifdef DEBUG
#define debugPrint0( message)                               fprintf( stderr, message)
#define debugPrint1( format, var1)                          fprintf( stderr, format, var1)
#define debugPrint2( format, var1, var2)                    fprintf( stderr, format, var1, var2)
#define debugPrint3( format, var1, var2, var3)              fprintf( stderr, format, var1, var2, var3)
#define debugPrint4( format, var1, var2, var3, var4)        fprintf( stderr, format, var1, var2, var3, var4)
#define debugPrint5( format, var1, var2, var3, var4, var5)  fprintf( stderr, format, var1, var2, var3, var4, var5)
#else
#define debugPrint0( message) 
#define debugPrint1( format, var1) 
#define debugPrint2( format, var1, var2) 
#define debugPrint3( format, var1, var2, var3) 
#define debugPrint4( format, var1, var2, var3, var4) 
#define debugPrint5( format, var1, var2, var3, var4, var5) 
#endif



typedef struct {
	UInt32	lv, co, wt;
	Int32	st;
} ELMSYM;

/*--- 半角→全角（１文字） ---*/
int hantozen(char *zen, char *han);
int hantozen(WCharP zen, WCharP han);

/*--- 全角→半角（１文字） ---*/
int zentohan(char *han, char *zen);
int zentohan(WCharP han, WCharP zen);

/*--- ラインの起点から、ラインに沿って設定距離の位置の座標（設定距離よりラインが短い時は ERROR） ---*/
int	get_point_along_line(
	double 		leng, 		/*  => 起点からの距離 				*/
	int 		lnv, 		/*  => 基準ラインの点数 			*/
	Dpoint3d 	*lPoi, 		/*  => 基準ラインの座標				*/
	int 		*iti, 		/* <=  得られた座標のライン上の位置	*/
	Dpoint3d 	*Poi		/* <=  得られた座標					*/
);

/*--- ライン上の２点間のラインに沿った設定距離を取得 ---*/
void get_leng_along_line(
	double 		*leng, 		/* <=  距離 			*/
	int 		lnv, 		/*  => 基準ラインの点数	*/
	Dpoint3d 	*lPoi, 		/*  => 基準ラインの座標	*/
	Dpoint3d 	*Poi1, 		/*  => 点１の座標		*/
	Dpoint3d 	*Poi2		/*  => 点２の座標		*/
);

/*--- 変数の交換 ---*/
void exchangei(int *ii, int *jj);
void exchangel(long *ii, long *jj);
void exchangeul(ULong *ii, ULong *jj);
void exchanged(double *ii, double *jj);
void exchanged3(Dpoint3d *iPoi, Dpoint3d *jPoi);

/*--- レイヤーを与えて表示 ---*/
void element_display(UInt32 *co, Int32 *lc, UInt32 *wt, MSElement *ele, DgnDrawMode drawMode);

/*--- 四捨五入 ---*/
int sya4_nyu5(int keta, double num1, double *num2);

/*--- ポリゴンが右回りか左回りか判定する（右回り:１、左回り:２） ---*/
int get_round_dir(
	int 		lnv, 	/*  => ポリゴンの点数 	*/
	Dpoint3d 	*lPoi 	/*  => ポリゴンの座標 	*/
);

/*--- ある点がポリゴン内にあるかどうかの判定 0:外、1:内、2:ライン上 ---*/
int judge_inout(int *polnv, Dpoint3d *polPoi, Dpoint3d *Poi);

/*--- あるラインと別のラインとの交点（判定するラインの1番起点側） ---*/
int judge_kouten(
	int *, 		/*  => 判定するラインの点数 		*/
	Dpoint3d *,  /*  => 判定するラインの各点の座標 	*/
	int *,  		/*  => 検索するラインの点数 		*/
	Dpoint3d *,  /*  => 検索するラインの各点の座標 	*/
	Dpoint3d *   /* <= 交点 				*/
);

/*--- あるラインと別のラインとの交点、交点数を返す ---*/
int judge_kouten_all(
	int *, 		/*  => 判定するラインの点数 		*/
	Dpoint3d *,  /*  => 判定するラインの各点の座標 	*/
	int *,  		/*  => 検索するラインの点数 		*/
	Dpoint3d *,  /*  => 検索するラインの各点の座標 	*/
	Dpoint3d *   /* <=  交点の各点の座標 		*/
);

/*--- ある点から1番近いライン上の点とその点のライン上での位置 ---*/
void get_closest_point(
	int *, 		/*  => ラインの点数 					*/
	Dpoint3d *,  /*  => ラインの各点の座標 				*/
	Dpoint3d *,  /*  => 元の点の座標 					*/
	Dpoint3d *,  /* <= １番近い点の座標 					*/
	int *, 		/* <= １番近い点のライン上の位置（どの線分上か） 		*/
	double *		/* <= １番近い点の線分上の位置（線分の起点からの距離） 	*/
);

/*--- 重なっている点を削除 ---*/
void del_doubled3(int *num, Dpoint3d *Poi, double limit);

/*--- 重なっている点を削除（ファイル位置も削除） ---*/
void del_doubled3_ul(int *num, Dpoint3d *Poi, double limit, ULong *Pos);

/*--- ２点間の平面距離 ---*/
double get_leng(Dpoint3d *iPoi, Dpoint3d *jPoi);

/*--- 複合図形の配置 ---*/
void compShape_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);

/*--- 複数ラインストリングの表示 ---*/
void lineString_display(UInt32 *co, Int32 *lc, UInt32 *wt, Dpoint3d *lPoi, int *nv, DgnDrawMode drawMode);

/*--- 複数ラインストリングの配置 ---*/
void lineString_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int *nv);

/*--- レイヤーとグループ番号の取得 ---*/
void get_layer(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- レイヤーとグループ番号の設定 ---*/
void set_layer(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- 連続するラインストリング（含ライン）の座標を返す ---*/
void get_renzoku(
	int, 		/*  => 検索対象の要素数 					*/
	ULong *, 	/*  => 検索対象要素のファイル位置 			*/
	int *, 		/* <=> １つ目の要素の点数を渡して、連続要素の点数 		*/
	Dpoint3d *, 	/* <=> １つ目の要素の座標を渡して、連続要素の座標を返す */
	int *, 		/* <=> １を渡して、連続要素の数							*/
	ULong *		/* <=> １つ目のファイル位置を渡して、連続要素のファイル位置を返す	*/
);

/*--- 連続する同一レイヤーのラインストリング（含ライン）の座標を返す ---*/
void get_renzoku_doulayer(
	int, 		/*  => 検索対象の要素数 					*/
	ULong *, 	/*  => 検索対象要素のファイル位置 			*/
	int *, 		/* <=> １つ目の要素の点数を渡して、連続要素の点数 		*/
	Dpoint3d *, 	/* <=> １つ目の要素の座標を渡して、連続要素の座標を返す */
	int *, 		/* <=> １を渡して、連続要素の数							*/
	ULong *		/* <=> １つ目のファイル位置を渡して、連続要素のファイル位置を返す	*/
);

/*--- 点がライン（2点）の左右どちらにあるか　左：１、右：２ ---*/
int judge_point_lr(Dpoint3d *Poi, Dpoint3d *lPoi1, Dpoint3d *lPoi2);

/*--- 点がライン（2点以上）の左右どちらにあるか　左：１、右：２ ---*/
int judge_point_lr_lstring(Dpoint3d *Poi, int lnv, Dpoint3d *lPoi);

/*--- 枠タイプ設定 ---*/
void set_fence_overlap();
void set_fence_inside();
void set_fence_clip();
void set_fence_void_overlap();
void set_fence_void_outside();
void set_fence_void_clip();

/*--- 全画層表示 ---*/
void on_all();

/*--- 全画層非表示 ---*/
void off_all();

/*--- 画層名→画層ID ---*/
int cnv_level_name2ID(ULong *lvID, WChar *lvName);

/*--- 画層ID→画層名 ---*/
int cnv_level_ID2name(WChar *lvName, LevelId lvID);

/*--- 画層名変更 ---*/
int chg_levelName(WChar *lvName, LevelId lvID);

/*--- オプションボタンに画層名一覧を追加 -------------*/
void add_option_lvList(
	MSDialogP	db_p,		/*	=>	オプションボタンの存在するダイアログボックス	*/
	long 		optionID 	/*	=>	オプションボタンID		*/
);

/*--- オプションボタンの画層名一覧を削除 -------------*/
void del_option_lvList(
	MSDialogP	db_p,		/*	=>	オプションボタンの存在するダイアログボックス	*/
	long 		optionID 	/*	=>	オプションボタンID		*/
);

/*--- コンボボックスに画層名一覧を追加 -------------*/
int add_combo_lvList(MSDialogP db_p, long comboID);

/*--- スキャンの範囲を設定 -------------*/
void get_scan_range(ExtScanList *scanlist, Dpoint3d *min, Dpoint3d *max);
void get_scan_range(ScanCriteria *scanP, Dpoint3d *min, Dpoint3d *max);

/*--- レイヤーとグループ番号を与えて配置 ---*/
ULong element_add(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, MSElement *ele);

/*--- 円の作成 -------------*/
void create_errCircle(UInt32 lv, UInt32 co, Int32 lc, UInt32 wt, double hankei, Dpoint3d *center);

/*--- ある点がラインから許容値以内の距離にあるかどうかの判定 1:許容値内、0:許容値外 ---*/
int	judge_point_nearline(
	int *,		/*  => ラインの点数 		*/
	Dpoint3d *,	/*  => ラインの各点の座標 	*/
	Dpoint3d *,	/*  => 判定する点の座標		*/
	double 		/*  => 許容値 				*/
);

/*--- 図形を囲む長方形の左下と右上の座標 ---*/
void get_min_max(Dpoint3d *min, Dpoint3d *max, int polNv, Dpoint3d *polPoi);

/*--- レイヤー取得 ---*/
int elmSymbChk(MSElement *el, ELMSYM *tmpElm, ULong offset, MSElementDescr	*eld);

/*--- 既存要素に MSLINK をアタッチする。SUCCESS：成功、ERROR：失敗 ---*/
ULong attach_mslink_filePos(
	WChar 	*table,  /*  => データベーステーブル名		*/
	ULong 	mslink,  /*  => MSLINK						*/
	ULong 	*filePos /*  => 要素のファイルポジション	*/
);

/*--- 新規要素に MSLINK をアタッチする。SUCCESS：成功、ERROR：失敗 ---*/
ULong attach_mslink_dscr(
	WChar 			*table, /*  => データベーステーブル名	*/
	ULong 			mslink, /*  => MSLINK					*/
	MSElementDescr 	*elmDP, /* <=> 要素						*/
	int		 		flag  	/*  => 0:add する, 1:add しない	*/
);

/*--- 新規要素に MSLINK をアタッチする。SUCCESS：成功、ERROR：失敗 ---*/
ULong attach_mslink_ele(
	WChar 		*table, /*  => データベーステーブル名	*/
	ULong 		mslink, /*  => MSLINK					*/
	MSElement 	*ele,  	/* <=> 要素						*/
	int		 	flag  	/*  => 0:add する, 1:add しない	*/
);

/*-----------------------------------------------------------------------
**	画層の表示状況を取得する  BoolInt disp[1000];
-----------------------------------------------------------------------*/
void get_display_level_orig(bool *disp);

/*-----------------------------------------------------------------------
**	画層の表示状況をセットする　BoolInt disp[1000];
-----------------------------------------------------------------------*/
void set_display_level_orig(bool *disp);


/*------------------------------------------------------------------------------------------------*/
/*--- レベルマスクの状態に表示する （旧方式）-----------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int	set_view_lvmask(short *viewList, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- mdlView_setLevelsの代わり （旧方式）--------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int mdlView_setLevels(bool onOffFlag, short *lvmask, short *viewList, int doUpdate);

/*------------------------------------------------------------------------------------------------*/
/*--- レベルマスクから各レベルの ON/OFFをint配列で取得 （旧方式）---------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_lv_from_lvmask(int *lvOn, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- レベルマスクから各レベルの ON/OFFをbool配列で取得 （旧方式）---------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_lv_from_lvmask_bool(bool *lvOn, short *lvmask);

/*------------------------------------------------------------------------------------------------*/
/*--- レベルマスクへ設定 （旧方式）-----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void set_lvmask(UInt32 *lv, short *lvMask);

/*------------------------------------------------------------------------------------------------*/
/*--- ２変数の交換（char） -----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void exchangechar(WChar *str1, WChar *str2);

/*-----------------------------------------------------------------------
**	ファイルポジションから座標を取得
-----------------------------------------------------------------------*/
void get_point_from_Pos(int *nv, Dpoint3d *Poi, ULong Pos);

/*------------------------------------------------------------------------------------------------*/
/*--- ２本のライン、ラインストリングの方向を合わせる ---------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void set_dir(int nv1, Dpoint3d *Poi1, int nv2, Dpoint3d *Poi2);

/*-----------------------------------------------------------------------
**	ラインを逆向きにする
-----------------------------------------------------------------------*/
void reverse(int nv, Dpoint3d *Poi);

/*-----------------------------------------------------------------------
**	mdlElement_display の代わり
-----------------------------------------------------------------------*/
void mdlElement_displayCE(MSElementP elm, DgnDrawMode drawMode);

/*-----------------------------------------------------------------------
**	mdlElmdscr_display の代わり
-----------------------------------------------------------------------*/
void mdlElmdscr_displayCE(MSElementDescrP elmDesc, DgnModelRefP modelRefP, DgnDrawMode drawMode);

/*-----------------------------------------------------------------------
**	一時描画要素の削除
-----------------------------------------------------------------------*/
void mdlTransient_free2(bool eraseDisplay);







// 以下、naka.hより移植 ----------------------------------------------------------------------------------------------

/*------------------------------------------------------------------------------------------------*/
/*--- 配列に変数を追加 (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int judge_point_within_range(int iti1, double leng1, int iti, double leng, int iti2, double leng2);

/*------------------------------------------------------------------------------------------------*/
/*--- 配列に変数を追加 (Dpoint3d)(増加) ----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_d3_increase(Dpoint3d *addPoi, int start, int end, Dpoint3d *oriPoi, int *nv);

/*------------------------------------------------------------------------------------------------*/
/*--- 配列に変数を追加 (Dpoint3d)(減少) ----------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_d3_decrease(Dpoint3d *addPoi, int start, int end, Dpoint3d *oriPoi, int *nv);

/*------------------------------------------------------------------------------------------------*/
/*--- 配列に変数を追加 (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_variable_to_arrayd3(Dpoint3d *addVar, Dpoint3d *chgArray, int *arrayNum);

/*------------------------------------------------------------------------------------------------*/
/*--- 配列に配列を追加 (Dpoint3d) ----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void add_array_to_arrayd3(Dpoint3d *addArray, int start, int end, Dpoint3d *chgArray, int *arrayNum);

/*------------------------------------------------------------------------------------------------*/
/*--- ポリゴンの作成 -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void polygon_create(UInt32 *lv, UInt32 *co, Int32 *lc, UInt32 *wt, UInt32 *ggnum, Dpoint3d *lPoi, int nv, MSElementDescr **elmDP);

/*------------------------------------------------------------------------------------------------*/
/*--- 交点とその位置 -----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
void get_kouten_iti(int nv1, Dpoint3d *Poi1, int nv2, Dpoint3d *Poi2, Dpoint3d *nearPoi, Dpoint3d *kouPoi, int *iti, double *leng);

/*-----------------------------------------------------------------------
/*	ラインの中点を取得
/*---------------------------------------------------------------------*/
void get_line_tyuuten(Dpoint3d *mPoi, int nv, Dpoint3d *lPoi);

/*-----------------------------------------------------------------------
/*	２点間の中点を取得
/*---------------------------------------------------------------------*/
void get_tyuuten(Dpoint3d *mPoi, Dpoint3d *lPoi1, Dpoint3d *lPoi2);

/*---------------------------------------------------------------------------------------------------------*/
/*--- 文字列を指定した桁数の文字列に変換（桁数が余る時には頭にスペースを入れる） --------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int ttostxt(int keta, WChar *txt1, WChar *txt2);

/*---------------------------------------------------------------------------------------------------------*/
/*--- 文字列を指定した桁数の文字列に変換（桁数が余る時には頭に0を入れる） ---------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
int tto0txt(int keta, WChar *txt1, WChar *txt2);

/*------------------------------------------------------------------------------------------------*/
/*--- 数値を指定した桁数の文字列に変換（桁数が余る時には頭にスペースを入れる） -------------------*/
/*------------------------------------------------------------------------------------------------*/
int itostxt(int keta, int num, WChar *txt, int flag);

/*------------------------------------------------------------------------------------------------*/
/*--- 数値を指定した桁数の文字列に変換（桁数が余る時には頭に0を入れる） --------------------------*/
/*------------------------------------------------------------------------------------------------*/
int ito0txt(int keta, int num, WChar *txt);





// 以下、nakadb.hより移植 ----------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------
** データベーステーブルの上書きコピー
-----------------------------------------------------------------------*/
void drop_table(WChar *copy, WChar *orig);

/*-----------------------------------------------------------------------
**	SQL文の実行
-----------------------------------------------------------------------*/
int process_SQL(WChar *ins);

/*-----------------------------------------------------------------------
**	要素が既にリンク済みかどうか検査する
-----------------------------------------------------------------------*/
int check_mslink(MSElement *ele, ULong *mslink);


// ODBCで接続しているか
bool IsOdbcConnection();



#endif



