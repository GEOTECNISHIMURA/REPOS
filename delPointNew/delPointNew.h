/*-----------------------------------------------------------------------*/
/*-	        DELPOINTNEW.H			ﾍｯﾀﾞｰﾌｧｲﾙ			-*/
/*-----------------------------------------------------------------------*/

#include <Mstn\MicroStation.r.h>


// DLLのID
#define  DLLAPP_DELPOINTNEW			1


/*-	タスクＩＤ							-*/
#define		TD_DELPOINTNEW  	    "DELPOINTNEW"


/*-	プラグマＩＤ（リソースＩＤ）		-*/
#define		DELPOINTNEW_ID  	    100


/*-	ダイアログボックスＩＤ				-*/
#define		DIALOGID_MAIN			200


/*-	ダイアログボックスフック関数ＩＤ	-*/
#define		HOOKID_MAIN_DIALOG		200


/*-	プッシュボタンＩＤ					-*/
#define		PUSHID_TABLE			310
#define		PUSHID_TABLE_KUKAN		311
#define		PUSHID_OK				300
#define		PUSHID_KUKAN			302
#define		PUSHID_END			    301


/*-	ダイアログアイテムフック関数ＩＤ	-*/
#define		HOOKID_PUSH_TABLE		310
#define		HOOKID_PUSH_TABLE_KUKAN	311
#define		HOOKID_PUSH_OK	        300
#define		HOOKID_PUSH_KUKAN       302
#define		HOOKID_PUSH_END	        301


/*-	テキストＩＤ					-*/
#define		TEXTID_TABLE			401
#define		TEXTID_TABLE_KUKAN		403
#define		TEXTID_RR			    402

typedef struct	DELPOINTNEW_INFO {
	WChar	tblName[512];
	WChar	kukanName[512];
    double  rr;
}	DELPOINTNEW;

#define  RSCID_DataDef_Link  	1

typedef struct LINK_INFO{
    char	rosen[6];	/* 路線番号	*/
    int		kukan;		/* 区間番号	*/
    int		lleng;		/* 左側延長 */
    int		rleng;		/* 右側延長	*/
    long	huku;		/* 幅員		*/
    long	hoka;		/* その他幅員	*/
    long	inout;		/* 側溝が歩道に付いているか、車道に付いているか	*/
} LINKSET;

struct LinkageHeader_a
{
	UInt16      wdMantissa : 8;           // mantissa words: wtf if wdExponent=0 
	UInt16      wdExponent : 4;           // exponent for words multiplier       
	UInt16      user : 1;                 // boolean: user data linkage          
	UInt16      modified : 1;             // boolean: user linkage modified      
	UInt16      remote : 1;               // boolean: remote linkage             
	UInt16      info : 1;                 // boolean: informational linkage      
	UInt16      primaryID;              // User ID number                      
};


typedef struct linkdata{
    LinkageHeader_a  linkHdr;
    LINKSET 	   linkage;
} LinkData;
