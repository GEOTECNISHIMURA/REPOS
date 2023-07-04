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

DllMdlApp DLLAPP_DNGLCHK_CHK =
{
    L"DNGLCHK_CHK", L"Dnglchk_Chk"          // taskid, dllName
}

