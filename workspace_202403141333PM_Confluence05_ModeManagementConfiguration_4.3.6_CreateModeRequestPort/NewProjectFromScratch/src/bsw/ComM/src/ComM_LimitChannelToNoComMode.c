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

#if (COMM_LIMIT_TO_NOCOM_ENABLED != STD_OFF)

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/*******************************************************************************
 * Function name    : ComM_LimitChannelToNoComMode
 * Syntax           : Std_ReturnType ComM_LimitChannelToNoComMode( NetworkHandleType Channel, ComM_StateType* State )
 * Description      : API to request limit to no com for a channel.
 * Parameters       : Channel-> The Network Channel for the requested state of ComM state machine.
 *                    Status-> Status of limit to no com
 * Return value     : E_OK: Successfully changed inhibition status for the channel
 *                    E_NOT_OK: Changed of inhibition status for the channel failed
 *                    COMM_E_UNINIT: ComM not initialized
 ******************************************************************************/
/* MR12 RULE 8.3 VIOLATION:Two declarations exists for this api. One is in ComM.h & another generated by RTE.
 * Hence,the difference in identifiers.*/
Std_ReturnType ComM_LimitChannelToNoComMode(NetworkHandleType Channel, boolean Status)
{
    /* Local variable declaration */
    ComM_GlobalVarType_tst *    globalRamPtr_pst;
    Std_ReturnType              retVal_u8;

    /* Local variable initialisation*/
    globalRamPtr_pst = &ComM_GlobalStruct;

    /************************************DET CHECK STARTS HERE*********************************/

    /* Check for ComM Initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_LIMIT_CHNLTO_NOCOMMODE_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }
    /* Check if the channel passed is valid */
    if (Channel >= COMM_NO_OF_CHANNELS)
    {
        COMM_DET_REPORT_ERROR(COMM_LIMIT_CHNLTO_NOCOMMODE_APIID, COMM_E_WRONG_PARAMETERS);
        return E_NOT_OK;
    }

    /************************************DET CHECK ENDS HERE**********************************/

    /* When ComMEcuGroupClassification is not set, the LimitToNoCom request for the channel is not processed and
    returns E_NOT_OK to indicate to the user that ComMEcuGroupClassification is not set
    in the configuration */
    if((COMM_GET_LIMIT_TO_NO_COM(globalRamPtr_pst->ComM_EcuGroupClassification_u8) == FALSE))
    {
        retVal_u8 = E_NOT_OK;
    }
    else
    {
        /* Call Internal funtion to process Limit to No Com request*/
        ComM_Prv_ProcessLimitToNoCom(Channel,Status);
        retVal_u8 = E_OK;
    }

    return retVal_u8;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif  /*  (COMM_LIMIT_TO_NOCOM_ENABLED != STD_OFF)  */

