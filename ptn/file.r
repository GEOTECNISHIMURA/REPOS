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
#include	"ptn.h"


/*----------------------------------------------------------------------+
|
|  Register Application and DLL
|
+----------------------------------------------------------------------*/

DllMdlApp DLLAPP_PTN_FILE =
{
    TD_FILE, L"ptn_file"          // taskid, dllName
}

