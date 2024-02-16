

/********************************************************************************************************************/
/*                                                                                                                  */
/* TOOL-GENERATED SOURCECODE, DO NOT CHANGE                                                                         */
/*                                                                                                                  */
/********************************************************************************************************************/


#ifndef DET_EXTERNALS_H
#define DET_EXTERNALS_H


/* Configuration status of Error Hooks */
#define DET_CFG_HOOKCOUNT                       0
#define DET_CFG_RUNTIMEERRORCALLOUTCNT          0
#define DET_CFG_TRANSIENTCALLOUTCNT             0



#if (DET_CFG_HOOKCOUNT > 0)
#define DET_START_SEC_CODE
#include "Det_MemMap.h"
#define DET_STOP_SEC_CODE
#include "Det_MemMap.h"

#define DET_CFG_CALLERRORHOOKS(ModuleId, InstanceId, ApiId, ErrorId) \
do{ \
}while (0)
#endif /* #if  (DET_CFG_HOOKCOUNT > 0) */

#if (DET_CFG_RUNTIMEERRORCALLOUTCNT > 0)
#define DET_START_SEC_CODE
#include "Det_MemMap.h"
#define DET_STOP_SEC_CODE
#include "Det_MemMap.h"

#define DET_CFG_CALLRUNTIMECALLOUT(ModuleId, InstanceId, ApiId, ErrorId) \
do{ \
}while (0)
#endif /* #if  (DET_CFG_RUNTIMEERRORCALLOUTCNT > 0) */

#if (DET_CFG_TRANSIENTCALLOUTCNT > 0)
#define DET_START_SEC_CODE
#include "Det_MemMap.h"
#define DET_STOP_SEC_CODE
#include "Det_MemMap.h"

#define DET_CFG_CALLTRANSIENTCALLOUT(ModuleId, InstanceId, ApiId, FaultId, retval) \
do{ \
}while (0)
#endif /* #if  (DET_CFG_TRANSIENTCALLOUTCNT > 0) */


#endif /* Det_Externals_h*/
