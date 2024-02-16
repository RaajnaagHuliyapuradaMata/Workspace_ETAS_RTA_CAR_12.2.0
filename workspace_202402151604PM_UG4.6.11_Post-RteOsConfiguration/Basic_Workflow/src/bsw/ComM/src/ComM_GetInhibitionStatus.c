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
 * Function name    : ComM_GetInhibitionStatus
 * Syntax           : Std_ReturnType ComM_GetInhibitionStatus( NetworkHandleType Channel,
 *                    ComM_InhibitionStatusType* Status )
 * Description      : Read the current state of ComM Channel.
 * Parameters       : Channel-> The Network Channel for the requested state of ComM state machine.
 *                    Status-> pointer to update inhibition status of channel
 * Return value     : E_OK: Successfully returned Inhibition Status
 *                    E_NOT_OK: Return of Inhibition Status failed
 *                    COMM_E_UNINIT: ComM not initialized
 ******************************************************************************/
/* MR12 RULE 8.3 VIOLATION:Two declarations exists for this api. One is in ComM.h & another generated by RTE.
 * Hence,the difference in identifiers.*/
Std_ReturnType ComM_GetInhibitionStatus(NetworkHandleType Channel, ComM_InhibitionStatusType * Status)
{
    /* Local Variables Declaration */
    ComM_ChannelVarType_tst *  channelRamPtr_pst;
    /* Local variable initialization */
    channelRamPtr_pst = &ComM_ChannelStruct[Channel];

    /************************************DET CHECK STARTS HERE*********************************/

    /* Check for ComM Initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_INHIBITION_STATUS_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /* Check if the channel passed is valid */
    if (Channel >= COMM_NO_OF_CHANNELS)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_INHIBITION_STATUS_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /* Check if the pointer passed is null pointer */
    if (Status == NULL_PTR)
    {
        COMM_DET_REPORT_ERROR(COMM_GET_INHIBITION_STATUS_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }
    /************************************DET CHECK ENDS HERE**********************************/

    /* Update the channel's inhibition status */
    *Status = channelRamPtr_pst->InhibitionReqStatus_u8;
    return E_OK;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif /* #if (COMM_INHIBITION_ENABLED) */

