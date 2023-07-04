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
#include	"ffref.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_FFREF_FF =
{
    L"FF", L"Ffref_FF"          // taskid, dllName
}

