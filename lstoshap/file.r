/*-----------------------------------------------------------------------*/
/*-		LSTOSHAP_FILE			                        	-*/
/*-									-*/
/*-	        FILE.R						-*/
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

DllMdlApp DLLAPP_LSTOSHAP_FILE =
{
    L"LSTOSHAP_FILE", L"lstoshap_file"          // taskid, dllName
}

