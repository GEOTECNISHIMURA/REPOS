/*-----------------------------------------------------------------------*/
/*-		LSTOSHAP_SINGLE			                        	-*/
/*-									-*/
/*-	        SINGLE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"lstoshap.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LSTOSHAP_SINGLE =
{
    L"LSTOSHAP_SINGLE", L"lstoshap_single"          // taskid, dllName
}

