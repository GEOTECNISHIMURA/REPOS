/*-----------------------------------------------------------------------*/
/*-		çÌèú			                        	-*/
/*-									-*/
/*-	        FILE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"dnglchk.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DNGLCHK_FILE =
{
    L"DNGLCHK_FILE", L"Dnglchk_File"          // taskid, dllName
}

