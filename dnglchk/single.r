/*-----------------------------------------------------------------------*/
/*-		çÌèú			                        	-*/
/*-									-*/
/*-	        CHK.R						-*/
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

DllMdlApp DLLAPP_DNGLCHK_SINGLE =
{
    L"DNGLCHK_SINGLE", L"Dnglchk_Single"          // taskid, dllName
}

