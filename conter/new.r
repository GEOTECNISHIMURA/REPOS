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

DllMdlApp DLLAPP_CONTER_NEW =
{
    TD_NEW, L"conter_new"          // taskid, dllName
}

