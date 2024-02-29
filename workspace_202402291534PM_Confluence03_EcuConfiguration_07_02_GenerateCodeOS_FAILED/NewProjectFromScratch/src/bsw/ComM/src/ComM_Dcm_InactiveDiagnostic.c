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
 * Function name    : ComM_DCM_InactiveDiagnostic
 * Syntax           : void ComM_DCM_InactiveDiagnostic( NetworkHandleType Channel )
 * Description      : Indication of inactive diagnostic by the DCM.
 * Parameters       : Channel -> Channel no longer needed for Diagnostic communication
 * Return value     : void
 ******************************************************************************/
void ComM_DCM_InactiveDiagnostic(NetworkHandleType Channel)
{
    /* Local Variables Declaration */
    ComM_ChannelVarType_tst *   channelRamPtr_pst;

    /* Local variable initialization */
    channelRamPtr_pst = &ComM_ChannelStruct[Channel];

    /* DET check for communication manager initialization */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        /* Report DET with ComM not initialized*/
        COMM_DET_REPORT_ERROR(COMM_DCM_INACTIVEDIAG, COMM_E_NOT_INITED);
        return;
    }

    /* Update the diagnostic request state for channel to Active_Diagnostics */
    channelRamPtr_pst->DiagnosticRequestState_b = COMM_INACTIVE_DIAGNOSTICS;

}
#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

