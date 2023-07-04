/*-----------------------------------------------------------------------*/
/*-		LINK_FILE			                        	-*/
/*-									-*/
/*-	        FILE.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"link.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_LINK_FILE =
{
    L"LINK_FILE", L"Link_File"          // taskid, dllName
}

