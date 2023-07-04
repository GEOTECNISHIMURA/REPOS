/*-----------------------------------------------------------------------*/
/*-     REFER.R                                                         -*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include    "refer.h"

DllMdlApp DLLAPP_REFER =

{
    L"REFER", L"refer"          // taskid, dllName
}
/*------------------------------------------------------------------------
**  DIALOG BOX 
------------------------------------------------------------------------*/
DialogBoxRsc    DIALOGID_MainDialog = {
    DIALOGATTR_GROWABLE | DIALOGATTR_AUTOUNLOADAPP,
    49.50*XC,
    2.50*YC,
    NOHELP,
    MHELP,
    HOOKID_REFER_DIALOG,
    NOPARENTID,
    "MFC検索",
    {
        { { 14.83*XC, 0.33*YC,33.50*XC,       1.83*YC}, PushButton,  PUSHBTNID_BatName,     ON, 0, "",     ""},
        { { 1.17*XC, 0.50*YC,11.67*XC,       1.58*YC}, PushButton,  PUSHBTNID_Jikou,    ON, 0, "",     ""},
    }
};


/*-----------------------------------------------------------------------
**  プッシュボタン（実行）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_Jikou = {
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELPCMD,
    HOOKITEMID_Jikou,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "Get"
};


/*-----------------------------------------------------------------------
**  プッシュボタン（終了）
-----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBTNID_BatName = {
    PBUTATTR_ALIGNCENTER,
    NOHELP,
    LHELPCMD,
    HOOKITEMID_BatName,
    NOARG,
    NOCMD,
    LCMD,
    "",
    "MfcName"
};
