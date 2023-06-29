#include <Mstn\MicroStation.r.h>
#include <RmgrTools\Tools\datadef.r.h>
#include <RmgrTools\Tools\cexprrsc.r.h>


CexprName 0 = {"CONTER_INFO"};
CexprName 1 = {"mode"};
CexprName 2 = {"zz"};
CexprName 3 = {"pitch"};
CexprName 4 = {"bspln"};
CexprName 5 = {"genkou"};
CexprName 6 = {"klv"};
CexprName 7 = {"slv"};
CexprName 8 = {"otlv"};
CexprName 9 = {"hani"};


CexprNamedStructures 1 =
    {{
    {0,  	0,	1342326731},	/*  CONTER_INFO  */
    }};

/*    struct CONTER_INFO    */
CexprType 0 = {18,	56,	0,	0,	0};
CexprType 1 = {4,	4,	0,	65535,	0};
CexprType 2 = {6,	8,	0,	65535,	0};
CexprType 3 = {9,	4,	0,	65535,	0};


CexprMemberList 0 =
    {{
    {0x1, 	0,	1, 	0x3000651}, 	/*  mode  */
    {0x2, 	8,	2, 	0x16e}, 	/*  zz  */
    {0x2, 	16,	3, 	0x66000d46}, 	/*  pitch  */
    {0x1, 	24,	4, 	0x63000cbe}, 	/*  bspln  */
    {0x2, 	32,	5, 	0x1d00199f}, 	/*  genkou  */
    {0x3, 	40,	6, 	0x710002fa}, 	/*  klv  */
    {0x3, 	44,	7, 	0x6900031a}, 	/*  slv  */
    {0x3, 	48,	8, 	0x1000696}, 	/*  otlv  */
    {0x1, 	52,	9, 	0xe000609}, 	/*  hani  */
    }};
