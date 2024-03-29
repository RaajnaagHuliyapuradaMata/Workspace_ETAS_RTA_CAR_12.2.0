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


#if (COMM_LIMIT_TO_NOCOM_ENABLED != STD_OFF)

/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
 */

#define COMM_START_SEC_CODE
#include "ComM_MemMap.h"


/*******************************************************************************
 * Function name    : ComM_LimitECUToNoComMode
 * Syntax           : Std_ReturnType ComM_LimitChannelToNoComMode( NetworkHandleType Channel, ComM_StateType* State )
 * Description      : API to enable or disable limit to no com for the ECU.
 * Parameters       : Channel-> The Network Channel for the requested state of ComM state machine.
 *                    Status-> Status of limit to no com
 * Return value     : E_OK: Successfully changed inhibition status for the channel
 *                    E_NOT_OK: Changed of inhibition status for the channel failed
 *                    COMM_E_UNINIT: ComM not initialized
 ******************************************************************************/

Std_ReturnType ComM_LimitECUToNoComMode(boolean Status)
{
    /* Local variable declaration */

    uint8                       channelId_u8;
    Std_ReturnType              retVal_u8;
    ComM_ChannelVarType_tst *   channelRamPtr_pst;
    ComM_GlobalVarType_tst *    globalRamPtr_pst;

    /* Local variable initialization */

    retVal_u8 = E_OK;

    /************************************DET CHECK STARTS HERE*********************************/
    /* Check for ComM Initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        COMM_DET_REPORT_ERROR(COMM_LIMIT_CHNLTO_NOCOMMODE_APIID, COMM_E_NOT_INITED);
        return COMM_E_NOT_INITIALIZED;
    }

    /************************************DET CHECK ENDS HERE**********************************/

    globalRamPtr_pst = &ComM_GlobalStruct;

    /* When ComMEcuGroupClassification is not set, the LimitToNoCom request for the channel is not processed and
    returns E_NOT_OK to indicate to the user that ComMEcuGroupClassification is not set
    in the configuration */
    if((COMM_GET_LIMIT_TO_NO_COM(globalRamPtr_pst->ComM_EcuGroupClassification_u8) == FALSE))
    {
        return E_NOT_OK;
    }

    /* Proceed only if the request has changed from previous request */
    if((globalRamPtr_pst->ComM_LimitECUToNoCom_b) != Status)
    {
        /* Update the global limit ecu to no com status */
        globalRamPtr_pst->ComM_LimitECUToNoCom_b = Status;

        /* If limit ECU to no com is requested */
        if(Status != FALSE)
        {
            /* Initialise Channel Ram Ptr variable */
            channelRamPtr_pst = &ComM_ChannelStruct[C_ZERO];

            /* Set limit to no com for all channels */
            for(channelId_u8 = C_ZERO; channelId_u8 < COMM_NO_OF_CHANNELS; ++channelId_u8)
            {
                /*Internal funtion to set Limit to No Com for Channel based on the Current state is invoked*/
                ComM_Prv_ProcessLimitToNoCom (channelId_u8, TRUE);
                ++channelRamPtr_pst;
            }
        }
        /* If limit ECU to no com is released */
        else
        {
            /* Initialise Channel Ram Ptr variable */
            channelRamPtr_pst = &ComM_ChannelStruct[C_ZERO];

            /* Clear limit to no com for all channels */
            for(channelId_u8 = C_ZERO; channelId_u8 < COMM_NO_OF_CHANNELS; channelId_u8++)
            {
                /* Internal function to Clear limit to no com flag for all configured channels*/
                ComM_Prv_ProcessLimitToNoCom (channelId_u8, FALSE);
                channelRamPtr_pst++;
            }
        }
    }

    return retVal_u8;
}

#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

#endif  /*  (COMM_LIMIT_TO_NOCOM_ENABLED != STD_OFF)  */

