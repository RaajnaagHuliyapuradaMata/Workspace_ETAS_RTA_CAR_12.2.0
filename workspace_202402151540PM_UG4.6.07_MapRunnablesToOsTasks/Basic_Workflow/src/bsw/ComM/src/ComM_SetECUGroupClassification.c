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

#if (COMM_INHIBITION_ENABLED)

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/*******************************************************************************
 * Function name    : ComM_SetECUGroupClassification
 * Syntax           : Std_ReturnType ComM_SetECUGroupClassification( ComM_InhibitionStatusType Status )
 * Description      : API to change the ECUGroupClassification value
 * Parameters       : Status-> Value of ECU group classification.
 * Return value     : E_OK: Successfully change the ECU Group Classification Status
 *                    E_NOT_OK: Change of the ECU Group Classification Status failed
 *                    COMM_E_UNINIT: ComM not initialized
 ******************************************************************************/

Std_ReturnType ComM_SetECUGroupClassification(ComM_InhibitionStatusType Status)
{
    /* Local Variables Declaration */
    /** Variable declaration **/
    Std_ReturnType retVal_u8;
    /* Local variable initialization */
    retVal_u8 = E_NOT_OK;

    /************************************DET CHECK STARTS HERE*********************************/

    /* Check for ComM Initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_SET_ECUGROUPCLASS_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /************************************DET CHECK ENDS HERE**********************************/

    /* Update requested ECU group classification status if the status is in the range 0 to 3*/
    ComM_GlobalStruct.ComM_EcuGroupClassification_u8 = Status;
    retVal_u8 = E_OK;
    return retVal_u8;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /* #if (COMM_INHIBITION_ENABLED) */

