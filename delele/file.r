/*-----------------------------------------------------------------------*/
/*-		�폜			                        	-*/
/*-									-*/
/*-	        FILE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"delele.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_DELELE_FILE =
{
    L"FILEA", L"Delele_File"          // taskid, dllName
}
