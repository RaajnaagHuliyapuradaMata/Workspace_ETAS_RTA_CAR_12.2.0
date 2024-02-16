




#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

/*********************************************************************
  Function name     :   EcuM_Prv_GetResource
  Description       :   This call aquires the EcuM resource for the core which id is passed as parameter.
                        This API is used for Resource mapping with respect to the Core_ID.
  Parameter (in)    :   void
  Parameter (inout) :   None
  Parameter (out)   :   None
  Return value      :   None
  Remarks           :
***********************************************************************/

void EcuM_Prv_GetResource(void);
void EcuM_Prv_GetResource(void)
{
    /* This function is empty because sleep feature is inactive*/
    return;

}

/*********************************************************************
  Function name     :   EcuM_Prv_ReleaseResource
  Description       :   This call releases the EcuM resource for the core which id is passed as parameter.
                        This API is used for Resource mapping with respect to the Core_ID.
  Parameter (in)    :   void
  Parameter (inout) :   None
  Parameter (out)   :   None
  Return value      :   None
  Remarks           :
***********************************************************************/

void EcuM_Prv_ReleaseResource(void);
void EcuM_Prv_ReleaseResource(void)
{
    /* This function is empty because sleep feature is inactive*/
    return;
}

#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"



