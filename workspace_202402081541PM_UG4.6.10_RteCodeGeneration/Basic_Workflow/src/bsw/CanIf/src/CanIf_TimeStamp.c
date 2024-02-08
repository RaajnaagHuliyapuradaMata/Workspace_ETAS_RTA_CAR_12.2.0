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

#if (CANIF_GLOBAL_TIME_SUPPORT == STD_ON)
#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"
/**********************************************************************************************************************
** Function name     : CanIf_GetCurrentTime                                                                          **
** Syntax            : Std_ReturnType CanIf_GetCurrentTime(uint8 ControllerId,                                       **
**                     Can_TimeStampType* timeStampPtr)                                                              **
** Service ID[hex]   : 0x51                                                                                          **
** Sync/Async        : Synchronous                                                                                   **
** Reentrancy        : Non Reentrant                                                                                 **
** Parameters (in)   : ControllerId                                                                                  **
** Parameters (inout): None                                                                                          **
** Parameters (out)  : timeStampPtr                                                                                  **
** Return value      : Std_ReturnType                                                                                **
** Description       : This function shall call Can_GetCurrentTime.                                                  **
**********************************************************************************************************************/
Std_ReturnType CanIf_GetCurrentTime(uint8 ControllerId, Can_TimeStampType* timeStampPtr)
{
    /*Return value*/
    Std_ReturnType Return_Status;

    /* Pointer to Controller State configuration */
    CanIf_ControllerStateType * lControllerState_p;

    /* Local pointer to the Controller Config structure */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

	/* Local variable to store controller CustId */
    uint8 lCtrlCustId_u8;

#if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /* Local Pointer to the Driver Config structure */
    const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
#endif

	/* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NOT_OK((FALSE == CanIf_Prv_InitStatus_b), CANIF_GET_CURRENT_TIME_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((ControllerId >= CANIF_TOTAL_CTRLS), CANIF_GET_CURRENT_TIME_SID, CANIF_E_PARAM_CONTROLLERID)

    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((lCtrlCustId_u8 == 0xFFu), CANIF_GET_CURRENT_TIME_SID, CANIF_E_PARAM_CONTROLLERID)

	/* If timeStampPtr is null, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((NULL_PTR == timeStampPtr), CANIF_GET_CURRENT_TIME_SID, CANIF_E_PARAM_POINTER)

    /*Get configured data of requested controller*/
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        /* Select the Can Driver for which this request needs to be forwared */
        lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
    #endif

    /* Call Can_GetCurrentTime() for respective CanDrv */
    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        Return_Status = lCtrlDrvConfig_pst->GetCurrentTime((lCtrlConfig_pst->CtrlCanCtrlRef), timeStampPtr);
    #else
	    Return_Status = Can_GetCurrentTime((lCtrlConfig_pst->CtrlCanCtrlRef), timeStampPtr);
    #endif

    /*return value*/
    return (Return_Status);

} /*End of CanIf_GetCurrentTime()*/

/**********************************************************************************************************************
** Function name     : CanIf_EnableEgressTimeStamp                                                                   **
** Syntax            : void CanIf_EnableEgressTimeStamp(PduIdType TxPduId)                                           **
** Service ID[hex]   : 0x52                                                                                          **
** Sync/Async        : Synchronous                                                                                   **
** Reentrancy        : Non Reentrant                                                                                 **
** Parameters (in)   : TxPduId                                                                                       **
** Parameters (inout): None                                                                                          **
** Parameters (out)  : None                                                                                          **
** Return value      : Std_ReturnType                                                                                **
** Description       : This function shall call Can_GetCurrentTime.                                                  **
**********************************************************************************************************************/
void CanIf_EnableEgressTimeStamp(PduIdType TxPduId)
{
    /* Local variable to store CustId for given PduId for the selected variant */
    uint16 ltxPduCustId_t;

    /* Pointer to Tx Pdu configuration */
    const CanIf_Cfg_TxPduConfig_tst * lTxPduConfig_pst;

    /*Local variable to hold the ControllerId for controller */
    uint8 lControllerId_u8 = 0;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /*local pointer to ROM structure of controller */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

    /*Local variable to hold the custId for controller */
    uint8 lCtrlCustId_u8;

    /*Local pointer to controller Driver structure */
    const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
    #endif

    /* local variable to store Hth Value */
    Can_HwHandleType lHth_Id = 0;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_VOID_RET((FALSE == CanIf_Prv_InitStatus_b), CANIF_ENABLE_EGRESS_TIME_STAMP_SID, CANIF_E_UNINIT)

    /* If TxPduId is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_VOID_RET(TxPduId >= CANIF_TOTAL_TXPDUS, CANIF_ENABLE_EGRESS_TIME_STAMP_SID,CANIF_E_PARAM_LPDU)

    ltxPduCustId_t = CanIf_Prv_ConfigSet_tpst->TxPduIdTable_Ptr[TxPduId];

    /* If TxPduId is invalid, report to DET and exit the function */
    CANIF_DET_REPORT_ERROR_VOID_RET((ltxPduCustId_t == 0xFFFFu), CANIF_ENABLE_EGRESS_TIME_STAMP_SID, CANIF_E_PARAM_LPDU)

    /*Get the address of config structure of requested PDU*/
    lTxPduConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_TxPduConfigPtr) + ltxPduCustId_t;

    /*Get the configured Hth Id for this PDU*/
    lHth_Id = lTxPduConfig_pst->CanIf_TxBufferConfigPtr->CanIf_HthConfigPtr->CanObjectId;

    /*Get the configured controllerId for this PDU*/
    lControllerId_u8  = lTxPduConfig_pst->CanIf_TxBufferConfigPtr->CanIf_HthConfigPtr->CanIf_CtrlConfigPtr->CtrlId;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /* Store custId for the requested controller for the selected variant */
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[lControllerId_u8];

    /* Get configured data of requested controller */
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
    #endif

    /* Call Can_EnableEgressTimeStamp() for respective CanDrv */
    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        lCtrlDrvConfig_pst->EnableEgressTimeStamp(lHth_Id);
    #else
        Can_EnableEgressTimeStamp(lHth_Id);
    #endif

}/*End of CanIf_EnableEgressTimeStamp()*/

/**********************************************************************************************************************
** Function name     : CanIf_GetEgressTimeStamp                                                                      **
** Syntax            : Std_ReturnType CanIf_GetEgressTimeStamp(PduIdType TxPduId,                                    **
**                     Can_TimeStampType* timeStampPtr                                                               **
** Service ID[hex]   : 0x53                                                                                          **
** Sync/Async        : Synchronous                                                                                   **
** Reentrancy        : Non Reentrant for the same TxPduId, Reentrant for different TxPduId                           **
** Parameters (in)   : TxPduId                                                                                       **
** Parameters (inout): None                                                                                          **
** Parameters (out)  : timeStampPtr                                                                                  **
** Return value      : Std_ReturnType                                                                                **
** Description       : This function shall call Can_GetEgressTimeStamp.                                              **
**********************************************************************************************************************/
Std_ReturnType CanIf_GetEgressTimeStamp(PduIdType TxPduId, Can_TimeStampType* timeStampPtr)
{
	/*Return value*/
    Std_ReturnType Return_Status;

	/* Local variable to store CustId for given PduId for the selected variant */
    uint16 ltxPduCustId_t;

    /* Pointer to Tx Pdu configuration */
    const CanIf_Cfg_TxPduConfig_tst * lTxPduConfig_pst;

    /*Local variable to hold the ControllerId for controller */
    uint8 lControllerId_u8 = 0;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /*local pointer to ROM structure of controller */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

    /*Local variable to hold the custId for controller */
    uint8 lCtrlCustId_u8;

    /*Local pointer to controller Driver structure */
    const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
    #endif

    /* local variable to store Hth Value */
    Can_HwHandleType lHth_Id = 0;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NOT_OK((FALSE == CanIf_Prv_InitStatus_b), CANIF_GET_EGRESS_TIME_STAMP_SID, CANIF_E_UNINIT)

    /* If TxPduId is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK(TxPduId >= CANIF_TOTAL_TXPDUS, CANIF_GET_EGRESS_TIME_STAMP_SID,CANIF_E_PARAM_LPDU)

    ltxPduCustId_t = CanIf_Prv_ConfigSet_tpst->TxPduIdTable_Ptr[TxPduId];

    /* If TxPduId is invalid, report to DET and exit the function */
    CANIF_DET_REPORT_ERROR_NOT_OK((ltxPduCustId_t == 0xFFFFu), CANIF_GET_EGRESS_TIME_STAMP_SID, CANIF_E_PARAM_LPDU)

	/* If timeStampPtr is null, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((NULL_PTR == timeStampPtr), CANIF_GET_EGRESS_TIME_STAMP_SID, CANIF_E_PARAM_POINTER)

    /*Get the address of config structure of requested PDU*/
    lTxPduConfig_pst = (CanIf_Prv_ConfigSet_tpst->CanIf_TxPduConfigPtr) + ltxPduCustId_t;

    /*Get the configured Hth Id for this PDU*/
    lHth_Id = lTxPduConfig_pst->CanIf_TxBufferConfigPtr->CanIf_HthConfigPtr->CanObjectId;

    /*Get the configured controllerId for this PDU*/
    lControllerId_u8  = lTxPduConfig_pst->CanIf_TxBufferConfigPtr->CanIf_HthConfigPtr->CanIf_CtrlConfigPtr->CtrlId;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /* Store custId for the requested controller for the selected variant */
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[lControllerId_u8];

    /* Get configured data of requested controller */
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
    #endif

    /* Call Can_GetEgressTimeStamp() for respective CanDrv */
    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        Return_Status = lCtrlDrvConfig_pst->GetEgressTimeStamp(TxPduId, lHth_Id, timeStampPtr);
    #else
        Return_Status = Can_GetEgressTimeStamp(TxPduId, lHth_Id, timeStampPtr);
    #endif

    /*return value*/
    return (Return_Status);
}/*End of CanIf_GetEgressTimeStamp()*/

/**********************************************************************************************************************
** Function name     : CanIf_GetIngressTimeStamp                                                                     **
** Syntax            : Std_ReturnType CanIf_GetIngressTimeStamp (PduIdType RxPduId,                                  **
**                     Can_TimeStampType* timeStampPtr)                                                              **
** Service ID[hex]   : 0x54                                                                                          **
** Sync/Async        : Synchronous                                                                                   **
** Reentrancy        : Non Reentrant for the same RxPduId, Reentrant for different RxPduIds                          **
** Parameters (in)   : RxPduId                                                                                       **
** Parameters (inout): None                                                                                          **
** Parameters (out)  : None                                                                                          **
** Return value      : Std_ReturnType                                                                                **
** Description       : This function shall call Can_GetIngressTimeStamp.                                             **
**********************************************************************************************************************/
Std_ReturnType CanIf_GetIngressTimeStamp (PduIdType RxPduId, Can_TimeStampType* timeStampPtr)
{
	/*Return value*/
    Std_ReturnType Return_Status;

    /* Pointer to RxPdu configuration */
    const CanIf_Cfg_RxPduType_tst * RxPduCfg_pcst;

    /* Pointer to HRH configuration */
    const CanIf_Cfg_Hrhtype_tst * HrhCfg_pcst;

    /*Local variable to hold the ControllerId for controller */
    uint8 lControllerId_u8 = 0;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /*local pointer to ROM structure of controller */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

    /*Local variable to hold the custId for controller */
    uint8 lCtrlCustId_u8;

    /*Local pointer to controller Driver structure */
    const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
    #endif

    /*Local variable to hold Hrh Id */
    uint16 Hrh_u16;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NOT_OK((FALSE == CanIf_Prv_InitStatus_b), CANIF_GET_INGRESS_TIME_STAMP_SID, CANIF_E_UNINIT)

  	/* [SWS_CANIF_00325] If CanRxPduId is invalid, report to DET and return E_NOT_OK
     * first check of the pduid obtained is correct then check if the pdu can be buffered */
    CANIF_DET_REPORT_ERROR_NOT_OK((RxPduId>= CANIF_CFG_NUM_CANRXPDUIDS), \
                                                                CANIF_GET_INGRESS_TIME_STAMP_SID, CANIF_E_PARAM_LPDU)
    CANIF_DET_REPORT_ERROR_NOT_OK((CanIf_Prv_ConfigSet_tpst->RxPduIdTable_Ptr[RxPduId]== CANIF_INVALID_ID), \
                                                                   CANIF_GET_INGRESS_TIME_STAMP_SID, CANIF_E_PARAM_LPDU)

    /* If timeStampPtr is null, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((NULL_PTR == timeStampPtr), CANIF_GET_INGRESS_TIME_STAMP_SID, CANIF_E_PARAM_POINTER)

    /* Get the pointer to desired Rx pdu */
    RxPduCfg_pcst = CanIf_Prv_ConfigSet_tpst->RxPduConfig_pcst + CanIf_Prv_ConfigSet_tpst->RxPduIdTable_Ptr[RxPduId];

    /* Initialize pointer to HRH configuration structure */
    HrhCfg_pcst = (CanIf_Prv_ConfigSet_tpst->HrhConfig_pcst) + RxPduCfg_pcst->Hrhref_t;

    /*Get the configured controllerId for this PDU*/
    lControllerId_u8 = HrhCfg_pcst->ControllerId_u8;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
    /* Store custId for the requested controller for the selected variant */
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[lControllerId_u8];

    /* Get configured data of requested controller */
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
    #endif

    /* Get the Hrh Id from the configuration structure */
    Hrh_u16 = HrhCfg_pcst->CanObjectId;

    /* Call Can_GetIngressTimeStamp() for respective CanDrv */
    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        Return_Status = lCtrlDrvConfig_pst->GetIngressTimeStamp(Hrh_u16, timeStampPtr);
    #else
        Return_Status = Can_GetIngressTimeStamp(Hrh_u16, timeStampPtr);
    #endif

    /*return value*/
    return (Return_Status);
}/*End of CanIf_GetIngressTimeStamp()*/
#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"
#endif

