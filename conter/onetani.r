/*-----------------------------------------------------------------------*/
/*-		 			                        	-*/
/*-									-*/
/*-	        file.R						-*/
/*-----------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include	"CONTER.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_CONTER_ONETANI =
{
    TD_ONETANI, L"conter_onetani"          // taskid, dllName
}

