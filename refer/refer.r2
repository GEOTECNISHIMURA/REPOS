/*----------------------------------------------------------------------+
|                                                                       |
|   Current Revision:                                                   |
|    Workfile:c:\mdl\include\main.r
|    Revision:
|                                                                       |
+----------------------------------------------------------------------*/
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
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include "refer.h"

DllMdlApp DLLAPP_REFER =

{
    L"REFER", L"refer"          // taskid, dllName
}
/*----------------------------------------------------------------------+
|                                                                       |
|   Main Dialog Box                                        |
|                                                                       |
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_MainDialog =
    {
	DIALOGATTR_GROWABLE | DIALOGATTR_AUTOUNLOADAPP,
    49.50*XC, 2.50*YC,
    NOHELP, MHELP, HOOKID_REFER_DIALOG, NOPARENTID,
    "MFC検索",
    {
{{14.83*XC, 0.33*YC,33.50*XC,       1.83*YC}, PushButton, PUSHBTNID_BatName, ON, 0, "", ""},
{{1.17*XC, 0.50*YC,11.67*XC,       1.58*YC}, PushButton, PUSHBTNID_Jikou, ON, 0, "", ""},
    }
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   実 行 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_Jikou =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_Jikou, ACTIONBUTTON_APPLY, NOCMD, LCMD,
    "",
    "Get"
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   バッチ処理 PushButton Item Resource 			    	|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_BatName =
    {
    PBUTATTR_ALIGNCENTER, NOHELP, LHELP,
    HOOKITEMID_BatName, ACTIONBUTTON_APPLY, NOCMD, LCMD,
    "",
    "MfcName"
    };
