#include <Mstn\MicroStation.r.h>
#include <RmgrTools\Tools\datadef.r.h>
#include <RmgrTools\Tools\cexprrsc.r.h>


CexprName 0 = {"DELSET_INFO"};
CexprName 1 = {"table"};
CexprName 2 = {"mode"};
CexprName 3 = {"bat"};
CexprName 4 = {"nn"};


CexprNamedStructures 1 =
    {{
    {0,  	0,	1577203083},	/*  DELSET_INFO  */
    }};

/*    struct DELSET_INFO    */
CexprType 0 = {18,	520,	0,	0,	0};
CexprType 1 = {4,	4,	0,	65535,	0};


CexprMemberList 0 =
    {{
    {0xc0008008, 	0,	1, 	0x7e000d0d}, 	/*  table  */
    {0x1, 	256,	2, 	0x3000651}, 	/*  mode  */
    {0xc0008008, 	260,	3, 	0x770002be}, 	/*  bat  */
    {0x1, 	516,	4, 	0x14a}, 	/*  nn  */
    }};
