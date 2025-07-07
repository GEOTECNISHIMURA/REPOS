#include <Mstn\MicroStation.r.h>
#include <RmgrTools\Tools\datadef.r.h>
#include <RmgrTools\Tools\cexprrsc.r.h>


CexprName 0 = {"PTN_INFO"};
CexprName 1 = {"table"};
CexprName 2 = {"mode"};
CexprName 3 = {"opt"};
CexprName 4 = {"bat"};
CexprName 5 = {"cel"};
CexprName 6 = {"ptlv"};
CexprName 7 = {"fence"};
CexprName 8 = {"bunkatu"};
CexprName 9 = {"lv"};
CexprName 10 = {"nn"};
CexprName 11 = {"ct"};


CexprNamedStructures 1 =
    {{
    {0,  	0,	453005579},	/*  PTN_INFO  */
    }};

/*    struct PTN_INFO    */
CexprType 0 = {18,	800,	0,	0,	0};
CexprType 1 = {4,	4,	0,	65535,	0};
CexprType 2 = {9,	4,	0,	65535,	0};


CexprMemberList 0 =
    {{
    {0xc0008008, 	0,	1, 	0x7e000d0d}, 	/*  table  */
    {0x1, 	256,	2, 	0x3000651}, 	/*  mode  */
    {0x1, 	260,	3, 	0x6b000310}, 	/*  opt  */
    {0xc0008008, 	264,	4, 	0x770002be}, 	/*  bat  */
    {0xc0008008, 	520,	5, 	0x6a0002c2}, 	/*  cel  */
    {0x1, 	776,	6, 	0x1e00069e}, 	/*  ptlv  */
    {0x1, 	780,	7, 	0x6b000c6b}, 	/*  fence  */
    {0x1, 	784,	8, 	0x72003439}, 	/*  bunkatu  */
    {0x2, 	788,	9, 	0x1a00014e}, 	/*  lv  */
    {0x1, 	792,	10, 	0x14a}, 	/*  nn  */
    {0x1, 	796,	11, 	0x1700013a}, 	/*  ct  */
    }};
