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
 * Function name    : ComM_ReadInhibitCounter
 * Syntax           : Std_ReturnType ComM_ReadInhibitCounter( uint16 * CounterValue )
 * Description      : API to get the inhibition counter value
 * Parameters       : CounterValue-> pointer to update inhibition counter value.
 * Return value     : E_OK: Successfully returned Inhibition Counter
 *                    E_NOT_OK: Return of Inhibition Counter failed
 *                    COMM_E_UNINIT: ComM not initialized
 ******************************************************************************/

Std_ReturnType ComM_ReadInhibitCounter(uint16 * CounterValue)
{
    /* Local Variables Declaration */
    /* Local variable initialization */
    /************************************DET CHECK STARTS HERE*********************************/
    /* Check for ComM Initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_READ_INHIBIT_COUNTER_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /* Check if the pointer passed is null pointer */
    if (CounterValue == NULL_PTR)
    {
        COMM_DET_REPORT_ERROR(COMM_READ_INHIBIT_COUNTER_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }
    /************************************DET CHECK ENDS HERE**********************************/


    /* Update the inhibition counter value */
    *CounterValue = ComM_GlobalStruct.ComM_InhibitCounter_u16;
    return E_OK;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif  /* #if (COMM_INHIBITION_ENABLED) */

