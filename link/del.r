/*-----------------------------------------------------------------------*/
/*-		LINK_DEL			                        	-*/
/*-									-*/
/*-	        DEL.R						-*/
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

DllMdlApp DLLAPP_LINK_DEL =
{
    L"LINK_DEL", L"Link_Del"          // taskid, dllName
}

