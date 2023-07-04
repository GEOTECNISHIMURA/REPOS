/*-----------------------------------------------------------------------*/
/*-		LINK_NAME			                        	-*/
/*-									-*/
/*-	        NAME.R						-*/
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

DllMdlApp DLLAPP_LINK_NAME =
{
    L"LINK_NAME", L"Link_Name"          // taskid, dllName
}

