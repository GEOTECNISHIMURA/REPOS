/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:c:\mdl\include\main.h
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>

#define SETUPDATE1	"setrefer"
#define SETUPDATE2	"setRefer"
//#define ACTIVE1		"active"
//#define ACTIVE2		"acTive"

/*----------------------------------------------------------------------+
|                                                                       |
|                                                                       |
|                                                                       |
|									|
|									|
|									|
|									|
|									|
|                                                                       |
+----------------------------------------------------------------------*/

#define  DLLAPP_REFER			1



#define DIALOGID_MainDialog                 	    10


#define PUSHBTNID_Jikou              	    1001



#define PUSHBTNID_BatName              	    1002


#define TOGGLEID_Backup               	    200000001

#define HOOKITEMID_Jikou		    101

#define HOOKITEMID_BatName		    102

#define HOOKID_REFER_DIALOG		    103

#define MACROVAL_262146               	    262146




typedef struct
{
	UInt32	level;
	UInt32	color;
	UInt32	weight;
	long	style;
} Symbol;


typedef struct setrefer
    {
	WChar  idxname[1000];		//バッチファイル名
	int	sagyou;			//作業方法 0:バッチ処理 1:一括処理 2:一要素
	Symbol	symb;
	int	type;
	WChar	cellName[10];
	WChar	kisai[100];
    } SetRefer;
/*
typedef struct active
    {
	WChar	dgnFileName[1000];
	int	DgnType;
	WChar	getcwd[500];
	WChar	TempName[128];
	Symbol	symb;

	short	levels[4];
	int	viewList[8];
    } AcTive;
*/




