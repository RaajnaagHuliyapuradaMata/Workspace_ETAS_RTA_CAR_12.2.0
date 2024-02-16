/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2014, all rights reserved
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
*/


/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "ComM_Priv.h"

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Type definitions
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
 */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/********************************************************************************************
 * Function name    : ComM_GetCurrentComMode
 * Syntax           : Std_ReturnType ComM_GetCurrentComMode
 *                   (
 *                     ComM_UserHandleType User, ComM_ModeType* ComMode
 *                     )
 * Description      : Function to query the current Communication Mode.
 * Parameters       : User -> Handle of the user who requests a mode
 *                    ComMode -> ComM_ModeType
 * Return value     : E_OK:Succesfully returned Communication Mode from Bus State Manager
 *                    E_NOT_OK: Return of Communication Mode from Bus State Manager failed
 *                    COMM_E_UNINIT: ComM not initialized
 **********************************************************************************************/

/* MR12 RULE 8.3 VIOLATION:Two declarations exists for this api. One is in ComM.h & another generated by RTE.
 * Hence,the difference in identifiers.*/
Std_ReturnType ComM_GetCurrentComMode(ComM_UserHandleType User, ComM_ModeType * ComMode)

{

    /* Local variable initialization */
    /***********************************************DET STARTS HERE****************************************/
    /* DET check for ComM not initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_COMMODE_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /* DET check for pointer passed to be a NULL pointer */
    if (ComMode == NULL_PTR)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_COMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /* DET  for Invalid user */
    if ( ComM_Prv_ValidateUserId(User) == FALSE)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_CUR_COMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /*************************************DET ENDS HERE*************************************************/

    *ComMode = ComM_Prv_getLeastBusSmMode(User);
    return E_OK;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

