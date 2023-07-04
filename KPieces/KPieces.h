/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:d:\mdl\txreso\txreso.h
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
#include <Mstn\MicroStation.r.h>
#include <DgnPlatform\DgnPlatform.r.h>
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
// DLLÇÃID
#define  DLLAPP_KPIECES			1

/*-	É^ÉXÉNÇhÇc							-*/
#define		TD_KPIECES  	            "KPIECES"


#define DIALOGID_MainDialog           	    1
#define DIALOGID_Workspace                   	    2
#define DIALOGID_Levels                   	    3

#define PUSHBTNID_s        	           	    200000001
#define PUSHBTNID_CANCEL        	       	    200000002
#define PUSHBTNID_FILE                    	    200000003
#define PUSHBTNID_Levels              	            200000004
#define PUSHBTNID_Workspace           	            200000005
#define PUSHBTNID_TBL                 	            200000006

#define TOGGLEID_TEXT      	          	    200000001
#define TOGGLEID_LINE                    	    200000002
#define TOGGLEID_CELL                     	    200000003
#define TOGGLEID_DEL				    200000004
#define TOGGLEID_COPY                    	    200000005
#define TOGGLEID_RESO                    	    200000006
#define TOGGLEID_BAT                    	    200000007
#define TOGGLEID_Ftchk                	            200000008
#define TOGGLEID_LevelsChk            	            200000009
#define TOGGLEID_TBLCHK                    	    200000010


#define TEXTID_IDX                       	    200000001
#define TEXTID_TBL                    	            200000002
#define TEXTID_FT                       	    200000003

#define OPTIONBUTTONID_SYORI             	    200000001
#define OPTIONBUTTONID_JIKO            	            200000002
#define OPTIONBUTTONID_JUST             	    200000003
#define OPTIONBUTTONID_TJUST          	            200000004

#define MACROVAL_0                    	    0
#define MACROVAL_1                    	    1
#define MACROVAL_262146               	    262146

/*----------------------------------------------------------------------+
|									|
|   Hook Function ID's							|
|									|
+----------------------------------------------------------------------*/
#define HOOKITEMID_Practice                  	    1
#define HOOKITEMID_FileIdx			    2
#define HOOKITEMID_CkBat			    3
#define HOOKITEMID_Syori			    4
#define HOOKITEMID_Jiko			    5
#define HOOKITEMID_Workspace                        6
#define HOOKITEMID_Levels                        7
#define HOOKITEMID_LevelsOk                        8
#define HOOKITEMID_Levelschk                        9
#define HOOKITEMID_FileTbl                        10
#define HOOKITEMID_Logbottn                        11

/*----------------------------------------------------------------------+
|									|
|   Typedefs								|
|									|
+----------------------------------------------------------------------*/
typedef struct
    {
	int	ley;
	int	lv;
	int	co;
	int	wt;
	int	sy;
	int	ft;
    } ChkLey;
typedef struct kpiecesglobals
    {
    int	  idxggle;
    WChar  idxname[10000];
    int   syori;
    int   jiko;
	int   just;
	int   tjust;
    int   ft;
    int   ftchk;
    int   levels[4];
    int   setlevels[64];
    int   levelschk;
    int   tbli;
    WChar  tblname[10000];
    int   backup;
    int   CompressDesign;
    int   viewIndep;

    ChkLey chkley;

    int   log;
    WChar  logname[10000];

    } KpiecesGlobals;

typedef struct activeparam
    {
	WChar	dgnFileName[1000];
	int	DgnType;
	int	level;
	short	levels[4];
	int	color;
	int	weight;
	int	style;
	int	viewList[8];
	WChar	cellName[10];
	WChar	getcwd[500];
	WChar	TempName[128];
	int	gr;
	double	gu;
    } ActiveParam;


#define LEVELMAPID_Levels             	    200000001
#define TOGGLEID_Backup                   	    200000002
#define TOGGLEID_CompressDesign                    	    200000001
#define PUSHBTNID_LevelsOk            	    200000007
#define TOGGLEID_LOG                  	    200000003
#define TEXTID_Logname                	    200000009
#define PUSHBTNID_LogB                	    200000008
#define MACROVAL_2                    	    2



#define OPTIONBUTTONID_viewIndep      	    200000005
