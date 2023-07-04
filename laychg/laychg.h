/*-----------------------------------------------------------------------*/
/*-	        LAYCHG.H		ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

#define FOUND       1
#define NOTFOUND    0

// DLLのID
#define  DLLAPP_LAYCHG			    1
#define  DLLAPP_LAYCHG_FILE	        2
#define  DLLAPP_LAYCHG_LAY			3
#define  DLLAPP_LAYCHG_SINGCHG		4

/*-	タスクＩＤ			-*/
#define		TD_LAYCHG		L"LAYCHG"
#define		TD_LAY			L"LAYCHG_LAY"
#define		TD_SINGCHG		L"LAYCHG_SINGCHG"
#define		TD_FILE			L"LAYCHG_FILE"

/*-	ダイアログボックスフック関数ＩＤ　　-*/
#define		HOOKID_LAYCHG_DIALOG	200

/*-	ダイアログアイテムフック関数ＩＤ　　-*/
#define		HOOKID_PUSH_TBL			302
#define		HOOKID_PUSH_OK			300
#define		HOOKID_PUSH_END			301
#define		HOOKID_PUSH_BAT			303
#define		HOOKID_PUSH_CEL			304

#define		HOOKID_OPTION_TYPE		310
#define		HOOKID_OPTION_CELLTYPE	311

#define		HOOKID_TOGGLE_TBL		320
#define		HOOKID_TOGGLE_SCALE		321
#define		HOOKID_TOGGLE_ANGLE		322
#define		HOOKID_TOGGLE_TH		323
#define		HOOKID_TOGGLE_TW		324
#define		HOOKID_TOGGLE_FONT		325
#define		HOOKID_TOGGLE_BAT		326
#define		HOOKID_TOGGLE_CELLTYPE	327

/*-	ダイアログボックスＩＤ		　　-*/
#define		DIALOGID_LAYCHG		400

/*-	テキストＩＤ		　　-*/
#define		TEXTID_TBL			600
#define		TEXTID_OCELL		601
#define		TEXTID_OLV			602
#define		TEXTID_OCO			603
#define		TEXTID_OLC			604
#define		TEXTID_OWT			605
#define		TEXTID_OSTR			618
#define		TEXTID_NCELL		606
#define		TEXTID_NLV			607
#define		TEXTID_NCO			608
#define		TEXTID_NLC			609
#define		TEXTID_NWT			610
#define		TEXTID_NSTR			619
#define		TEXTID_SCALE		611
#define		TEXTID_ANGLE		612
#define		TEXTID_TH			613
#define		TEXTID_TW			614
#define		TEXTID_FONT			615
#define		TEXTID_BAT			616
#define		TEXTID_CEL			617

/*-	プッシュボタンＩＤ		　　-*/
#define		PUSHID_TBL			702
#define		PUSHID_OK			700
#define		PUSHID_END			701
#define		PUSHID_BAT			703
#define		PUSHID_CEL			704

/*-	オプションボタンＩＤ		　　-*/
#define		OPTIONID_TYPE		800
#define		OPTIONID_CELLTYPE	801
#define		OPTIONID_OLV		802

/*-	トグルボタンＩＤ		　　-*/
#define		TOGGLEID_TBL		849
#define		TOGGLEID_SCALE		850
#define		TOGGLEID_ANGLE		851
#define		TOGGLEID_TH			852
#define		TOGGLEID_TW			853
#define		TOGGLEID_FONT		854
#define		TOGGLEID_CHECK		855
#define		TOGGLEID_BAT		856
#define		TOGGLEID_CELLTYPE	857


/*-	ラベルＩＤ		　　-*/
#define		LABELID_OLD			901
#define		LABELID_NEW			902


/*-	コンボボックスＩＤ		　　-*/
#define		COMBOID_OLV			951
#define		COMBOID_NLV			952

typedef struct	LAYSET_INFO{
    int		ttbl;
    int		type;
    WChar	ocell[10], ostr[100];
    UInt32	olv, oco,  owt;
	int     olc;
    WChar	ncell[10], nstr[100];
	WChar	olvstr[256], nlvstr[256];
	UInt32	nlv, nco, nwt;
	int		nlc;
    int	    tscale;
    int		tangle;
    int		tth;
    int		ttw;
    int		tfont;
    double	scale;
    double	angle;
    double	th;
    double	tw;
    int		font;
    int		check;
    WChar	file[128];
    int		tbat;
	WChar	bfile[128], cfile[128];
    int		nn;
    int		tcelltype, ocelltype;
}	LAYSET;

/*
#define  RSCID_DataDef_Link  1

typedef struct LINK_INFO{
    long     hhh;
} LINKSET;

typedef struct linkdata{
    LinkageHeader  linkHdr;
    LINKSET 	   linkageInfo;
} LinkData;
*/

//Public void appendElemtoDPa(MSElementDescr *TelmDP, UInt32 cmpcnt);
