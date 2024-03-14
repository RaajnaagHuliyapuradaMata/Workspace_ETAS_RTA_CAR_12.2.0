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
 * Function name    : ComM_DCM_ActiveDiagnostic
 * Syntax           : void ComM_DCM_ActiveDiagnostic( NetworkHandleType Channel )
 * Description      : Indication of active diagnostic by the DCM.
 * Parameters       : Channel -> Channel needed for Diagnostic communication
 * Return value     : void
 ******************************************************************************/
void ComM_DCM_ActiveDiagnostic(NetworkHandleType Channel)

{
    /* Local Variables Declaration */
    ComM_ChannelVarType_tst *  channelRamPtr_pst;
    const ComM_ChannelType_tst *    channelConfigPtr_pcst;

    /* Local variable initialization */
    channelRamPtr_pst = &ComM_ChannelStruct[Channel];
    channelConfigPtr_pcst = &(COMM_GET_CHANNEL_LIST[Channel]);

    /* DET check for communication manager initialization */
    /* Check if ComM is initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        /* Report DET with ComM not initialized*/
        COMM_DET_REPORT_ERROR(COMM_DCM_ACTIVEDIAG, COMM_E_NOT_INITED);
        return;
    }

    /* Active Diagnostic is not possible on a Passive, Slave Active and Slave Passive channel since it is not possible
       to keep the channel awake. Active Diagnostic shall be ignored by ComM for passive channels */
    if ((channelConfigPtr_pcst->ComMNmVariant_en != PASSIVE) &&
        (channelConfigPtr_pcst->ComMNmVariant_en != SLAVE_ACTIVE) &&
        (channelConfigPtr_pcst->ComMNmVariant_en != SLAVE_PASSIVE))
    {
        /* Update the diagnostic request state for channel to Active_Diagnostics */
        channelRamPtr_pst->DiagnosticRequestState_b = COMM_ACTIVE_DIAGNOSTICS;
    }

}
#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

