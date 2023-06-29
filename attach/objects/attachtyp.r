#include <Mstn\MicroStation.r.h>
#include <RmgrTools\Tools\datadef.r.h>
#include <RmgrTools\Tools\cexprrsc.r.h>


CexprName 0 = {"ATTACH8_INFO"};
CexprName 1 = {"mode"};
CexprName 2 = {"tog"};
CexprName 3 = {"tog_kotei"};
CexprName 4 = {"tog_detach"};
CexprName 5 = {"idxName"};
CexprName 6 = {"refName"};


CexprNamedStructures 1 =
    {{
    {0,  	0,	1644463563},	/*  ATTACH8_INFO  */
    }};

/*    struct ATTACH8_INFO    */
CexprType 0 = {18,	5720,	0,	0,	0};
CexprType 1 = {4,	4,	0,	65535,	0};
CexprType 2 = {15,	5120,	3,	65535,	10};
CexprType 3 = {15,	512,	4,	65535,	256};
CexprType 4 = {8,	2,	0,	65535,	0};


CexprMemberList 0 =
    {{
    {0x1, 	0,	1, 	0x3000651}, 	/*  mode  */
    {0xc0000a04, 	4,	2, 	0x7c000315}, 	/*  tog  */
    {0xc0000a04, 	44,	3, 	0x5f00de4b}, 	/*  tog_kotei  */
    {0x1, 	84,	4, 	0x3c01bb62}, 	/*  tog_detach  */
    {0xc0010008, 	88,	5, 	0x52003373}, 	/*  idxName  */
    {0x2, 	600,	6, 	0x560034b3}, 	/*  refName  */
    }};
