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


/*******************************************************************************
 * Function name    : ComM_GetStatus
 * Syntax           : Std_ReturnType ComM_GetStatus( ComM_InitStatusType* Status )
 * Description      : Returns the initialization status of the AUTOSAR Communication Manager.
 * Parameters       : Status -> COMM_UNINIT: The ComM is not initialized or not usable/
 *                              COMM_INIT: The ComM is initialized and usable.
 * Return value     : E_OK: Successfully return of initialization status
 *                    E_NOT_OK: Return of initialization status failed
 ******************************************************************************/

Std_ReturnType ComM_GetStatus(ComM_InitStatusType * Status)
{
    /* Local Variables Declaration */
    /* Local variable initialization */
    /****************************************DET STARTS HERE*****************************************/
    /* Check for Null pointer */
    if (Status == NULL_PTR)
    {
        /* Report DET with NULL parameter*/
        COMM_DET_REPORT_ERROR(COMM_GET_STATUS_APIID,COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }
    /****************************************DET ENDS HERE*****************************************/

    /* Update ComM initialization status */
    *Status = ComM_GlobalStruct.ComM_InitStatus_en;
    return E_OK;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

