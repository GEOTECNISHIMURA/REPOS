/*-----------------------------------------------------------------------*/
/*-		TXCELL_TXCE			                        	-*/
/*-									-*/
/*-	        TXCE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"txcell.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_TXCELL_TXCE =
{
    L"TXCE", L"txcell_txce"          // taskid, dllName
}

