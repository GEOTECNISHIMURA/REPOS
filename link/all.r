/*-----------------------------------------------------------------------*/
/*-		LINK_ALL			                        	-*/
/*-									-*/
/*-	        ALL.R						-*/
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

DllMdlApp DLLAPP_LINK_ALL =
{
    L"LINK_ALL", L"Link_All"          // taskid, dllName
}

