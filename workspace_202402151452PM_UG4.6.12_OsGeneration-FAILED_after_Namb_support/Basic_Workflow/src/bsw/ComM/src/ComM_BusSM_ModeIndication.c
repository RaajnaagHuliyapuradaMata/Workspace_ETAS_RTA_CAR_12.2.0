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
#include "EcuM.h"
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
/* ComM_BusSM_ModeIndication */
/*******************************************************************************
 * Function name    : ComM_BusSM_ModeIndication
 * Syntax           : void ComM_BusSM_ModeIndication
 *                    (
 *                      NetworkHandleType Channel, ComM_ModeType* ComMode
 *                      )
 * Description      : Indication of the actual bus mode by the corresponding Bus State Manager.
 * Parameters       : Channel
 *                    ComMode
 * Return value     : void
 ******************************************************************************/
/* MR12 RULE 8.13 VIOLATION:Violated since SWS gives function definition in the given way */
void ComM_BusSM_ModeIndication(NetworkHandleType Channel, ComM_ModeType ComMode)
{
    /* Local variable initialization */
    /**********************************************DET CHECKS STARTS HERE********************************/
    /* DET check for communication manager initialization */
    /* Check if ComManager is initialized */
    if (ComM_GlobalStruct.ComM_InitStatus_en != COMM_INIT)
    {
        /* Report DET with ComM not initialized*/
        COMM_DET_REPORT_ERROR(COMM_BUSSM_MODEINDICATION, COMM_E_NOT_INITED);
        return;
    }
    /* Check if the channel is present in the configured channels */
    if (Channel >= COMM_NO_OF_CHANNELS)
    {
        /* Report DET with Invalid channel parameter*/
        COMM_DET_REPORT_ERROR(COMM_BUSSM_MODEINDICATION, COMM_E_WRONG_PARAMETERS);
        return;
    }
    /*****************************************DET ENDS HERE*************************************************/
    (void) Channel;
    (void) ComMode;
}
#define COMM_STOP_SEC_CODE
#include "ComM_MemMap.h"

