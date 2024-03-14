/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2017, all rights reserved
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

#include "CanIf_Prv.h"


/***********************************************************************************************************************
 * Function name    : CanIf_GetTxConfirmationState
 * Syntax           : CanIf_NotifStatusType CanIf_GetTxConfirmationState(
                                                                        uint8 ControllerId
                                                                                    )
 * Description      : This service reports, if any TX confirmation has been done for the whole CAN controller
                      since the last CAN controller start.
 * Parameter        : ControllerId
 * Return value     : CANIF_NO_NOTIFICATION / CANIF_TX_RX_NOTIFICATION
 **********************************************************************************************************************/

#if (CANIF_PUBLIC_TXCONFIRM_POLLING_SUPPORT != STD_OFF)
#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

CanIf_NotifStatusType CanIf_GetTxConfirmationState(
                                       uint8 ControllerId
                                               )
{
    CanIf_NotifStatusType lTxConfmStatus;
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    uint8 lCtrlCustId_u8;
#endif
    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NO_NOTIF((FALSE == CanIf_Prv_InitStatus_b), CANIF_GET_TX_CONFIRMATION_STATE_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NO_NOTIF((ControllerId >= CANIF_TOTAL_CTRLS), CANIF_GET_TX_CONFIRMATION_STATE_SID, \
                                                                                        CANIF_E_PARAM_CONTROLLERID)
#if (CANIF_DEV_ERROR_DETECT == STD_ON)
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];
#endif
    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NO_NOTIF((lCtrlCustId_u8 == 0xFFu), CANIF_GET_TX_CONFIRMATION_STATE_SID, \
                                                                                        CANIF_E_PARAM_CONTROLLERID)
    /*Read TxConfirmationState of the requested controller*/
    lTxConfmStatus = (CanIf_Prv_ControllerState_ast + ControllerId)->CanIf_TxCnfmStatus;


    return(lTxConfmStatus);

}
#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"
#endif


