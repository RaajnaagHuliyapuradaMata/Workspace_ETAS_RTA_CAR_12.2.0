/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/

/**************************************************************************************************/
/* Preprocessor includes                                                                          */
/**************************************************************************************************/

#include "EcuM.h"

#if (ECUM_CFG_MODE_HANDLING == STD_ON)

#if(ECUM_DEV_ERROR_DETECT == STD_ON)/*Det included when Det is enabled in EcuM*/
#include "Det.h"
// #if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != 4))
// #error "AUTOSAR major version undefined or mismatched"
// #endif
// #if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != 2))
// #error "AUTOSAR minor version undefined or mismatched"
// #endif
#endif/*ECUM_DEV_ERROR_DETECT*/

#include "EcuM_Prv.h"

/*************************************************************************************************
                            API's
*************************************************************************************************/
#define ECUM_START_SEC_CODE
#include "EcuM_MemMap.h"

/***************************************************************************************************
 Function name    : EcuM_RequestRUN
 Syntax           : Std_ReturnType EcuM_RequestRUN( EcuM_UserType user )
 Description      : Places a request for the RUN state. Requests can be placed by every user made
                     known to the state manager at configuration time.
 Parameter        : EcuM_UserType user
 Return value     : Std_ReturnType
 ***************************************************************************************************/
Std_ReturnType EcuM_RequestRUN( EcuM_UserType user)

{
    Std_ReturnType Return_Value = E_NOT_OK;
    EcuM_UserType User_u16 = user;

    if(FALSE != EcuM_Prv_IsUserHandlingPossible_b(&User_u16, ECUM_REQUESTRUN_APIID))
    {
        if(FALSE == EcuM_Prv_userRUNReqStatus_ab[User_u16])
        {
            /*The request from the user is registered */
            EcuM_Prv_userRUNReqStatus_ab[User_u16] = TRUE;

            Return_Value = E_OK;
        }
        else
        {
            /* ECUM_E_MULTIPLE_RUN_REQUESTS */
            #if (ECUM_DEV_ERROR_DETECT == STD_ON)
                (void)Det_ReportError(ECUM_MODULE_ID, ECUM_INSTANCE_ID,ECUM_REQUESTRUN_APIID, ECUM_E_MULTIPLE_RUN_REQUESTS);
            #endif//ECUM_DEV_ERROR_DETECT
        }
    }



    return Return_Value;



}

#define ECUM_STOP_SEC_CODE
#include "EcuM_MemMap.h"

#else /*ECUM_CFG_MODE_HANDLING*/
   /*This fuction is not compiled because mode handling parameter is not configured or feature is disabled */
#endif /*ECUM_CFG_MODE_HANDLING*/





