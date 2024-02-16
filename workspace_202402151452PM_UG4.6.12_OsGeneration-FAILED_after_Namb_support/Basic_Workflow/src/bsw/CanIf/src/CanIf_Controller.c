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

/*This hold the current config set details*/
#define CANIF_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
const CanIf_ConfigType * CanIf_Prv_ConfigSet_tpst;
#define CANIF_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"

#if (CANIF_XCORE_CFG_ENABLED != STD_OFF)
#define CANIF_START_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
const CanIf_XCore_ConfigType * CanIf_XCore_Config_pcst;
#define CANIF_STOP_SEC_VAR_FAST_CLEARED_UNSPECIFIED
#include "CanIf_MemMap.h"
#endif

/**********************************************************************************************************************
** Function name     : CanIf_SetControllerMode                                                                       **
** Syntax            : Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, Can_ControllerStateType            **
**                     ControllerMode)                                                                               **
** Service ID[hex]   : 0x03                                                                                          **
** Sync/Async        : Asynchronous                                                                                  **
** Reentrancy        : Reentrant for different ControllerIds. Non reentrant for the same ControllerId.               **
** Parameters (in)   : ControllerId, ControllerMode                                                                  **
** Parameters (inout): None                                                                                          **
** Parameters (out)  : None                                                                                          **
** Return value      : Std_ReturnType                                                                                **
** Description       : This service calls the corresponding CAN Driver service for changing of the CAN controller    **
**                     mode.                                                                                         **
**********************************************************************************************************************/

#define CANIF_START_SEC_CODE
#include "CanIf_MemMap.h"

Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, Can_ControllerStateType ControllerMode)
{

    /* Pointer to Controller State configuration */
    CanIf_ControllerStateType * lControllerState_p;

    /* Local variable to store return value */
    Std_ReturnType lRetVal_en = E_NOT_OK;

    /* Local pointer to the Controller Config structure */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        /* Local Pointer to the Driver Config structure */
        const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
    #endif

    /* Local variable to store controller CustId */
    uint8 lCtrlCustId_u8;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NOT_OK((FALSE == CanIf_Prv_InitStatus_b), CANIF_SET_CONTROLLER_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((ControllerId >= CANIF_TOTAL_CTRLS), CANIF_SET_CONTROLLER_SID, CANIF_E_PARAM_CONTROLLERID)

    /* Obtain the custId for the requested controller in the selected variant */
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((lCtrlCustId_u8 == 0xFFu), CANIF_SET_CONTROLLER_SID, CANIF_E_PARAM_CONTROLLERID)

    /* If Controller mode is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((CAN_CS_STARTED != ControllerMode) && (CAN_CS_STOPPED != ControllerMode) && \
                (CAN_CS_SLEEP != ControllerMode), CANIF_SET_CONTROLLER_SID, CANIF_E_PARAM_CTRLMODE)

    /*Get current status of the requested controller*/
    lControllerState_p = CanIf_Prv_ControllerState_ast + ControllerId;

    /*Get configured data of requested controller*/
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        /* Select the Can Driver for which this request needs to be forwared */
        lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
    #endif

    /* Request the selected Can Driver to set the Controller mode to requested mode */

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        lRetVal_en = lCtrlDrvConfig_pst->SetCrtlMode(lCtrlConfig_pst->CtrlCanCtrlRef, ControllerMode);
    #else
        lRetVal_en = Can_SetControllerMode((lCtrlConfig_pst->CtrlCanCtrlRef), ControllerMode);
    #endif

    /*Update the Pdu Mode if Can Driver retruns E_OK */
    if(lRetVal_en != E_NOT_OK)
    {
        /* Requested mode change */
        switch(ControllerMode)
        {
            case CAN_CS_SLEEP:
            {
                /*[SWS_CANIF_00865] Set PduMode to CANIF_OFFLINE*/
                lControllerState_p->Ctrl_Pdu_mode = ((uint8)CANIF_OFFLINE)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_PDU_MODE);
                #if (CANIF_PUBLIC_PN_SUPPORT == STD_ON)
                    #if (CANIF_RB_NODE_CALIBRATION == STD_OFF)
                        /*Check for the flag at least one pdu linked to this controller is a pnfilter pdu*/
                        if((lCtrlConfig_pst->PnCtrlEn == TRUE) && (lControllerState_p->BlPnTxFilter != TRUE))
                    #else
                        if((lControllerState_p->BlPnCtrl_Calib == TRUE) && (lControllerState_p->BlPnTxFilter != TRUE))
                    #endif
                        {
                            /*As per requirement [SWS_CANIF_00749] PnTxFilter shall be enabled */
                            lControllerState_p->BlPnTxFilter = TRUE;
                        }
                #endif
                 /* [SWS_CANIF_00756] Old Wakeup event cleared */
                #if(CANIF_PUBLIC_WAKEUP_CHECK_VALIDATION_SUPPORT == STD_ON)
                    lControllerState_p->ValidationSts_b = CANIF_RESET;
                #endif
                #if((CANIF_PUBLIC_TXBUFFERING == STD_ON)&&(CANIF_CFG_TX_FEATURE_ENABLED==STD_ON))
                    /* Enter Critical Section */
                    SchM_Enter_CanIf_TxBufAccessNoNest();

                    /*If buffering is enabled, clear all the buffers assigned to requested controller*/
                    CanIf_Prv_ClearTxChannelBuffer(lCtrlCustId_u8);

                    /* Leave Critical Section */
                    SchM_Exit_CanIf_TxBufAccessNoNest();
                #endif
                #if (CANIF_XCORE_CFG_ENABLED != STD_OFF)

                    /* Flag set to True to clear all Pipes */
                    CanIf_XCore_RxClrPipeFlag_b = TRUE;

                    /* If XCore is enabled, Clear the pipe packets assigned to requested controller in Tx and Tx confirmation pipes */
                    CanIf_XCore_ClearPipes(lCtrlCustId_u8);
                #endif
            }/*End of case CAN_CS_SLEEP:*/
            break;
            case CAN_CS_STARTED:
            {
                #if (CANIF_LITE_CONFIGURATION == STD_ON)
                    /*Set PduMode to CANIF_ONLINE*/
                    lControllerState_p->Ctrl_Pdu_mode = (ControllerMode<<CANIF_Ctrl_BIT_SHIFT)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_CTRL_MODE);
               #endif
            }/*End of case CAN_CS_STARTED:*/
            break;
            case CAN_CS_STOPPED:
            {
                /*Set PduMode to CANIF_TX_OFFLINE*/
                lControllerState_p->Ctrl_Pdu_mode = ((uint8)CANIF_TX_OFFLINE)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_PDU_MODE);
                #if((CANIF_PUBLIC_TXBUFFERING == STD_ON)&&(CANIF_CFG_TX_FEATURE_ENABLED==STD_ON))
                    /* Enter Critical Section */
                    SchM_Enter_CanIf_TxBufAccessNoNest();

                    /*If buffering is enabled, clear all the buffers assigned to requested controller*/
                    CanIf_Prv_ClearTxChannelBuffer(lCtrlCustId_u8);

                    /* Enter Critical Section */
                    SchM_Exit_CanIf_TxBufAccessNoNest();
                #endif
                #if (CANIF_XCORE_CFG_ENABLED != STD_OFF)
                    /* If XCore is enabled, Clear the pipe packets assigned to requested controller in Tx and Tx confirmation pipes */
                    CanIf_XCore_ClearPipes(lCtrlCustId_u8);
                #endif
            }/*End of case CAN_CS_STOPPED:*/
            break;
            default:
                /* Intentionally Empty */
            break;
        } /* End of switch(ControllerMode) */
    }/*End of if(lRetVal_en != E_NOT_OK)*/
    else
    {
        /*Do nothing */
    }
    return(lRetVal_en);
}


/**********************************************************************************************************************
 * Function name    : CanIf_GetControllerMode
 * Syntax           : Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId,
 * Can_ControllerStateType * ControllerModePtr)
 *
 * Description      : Service reports about the current status of the requested CAN Controller.
 * Parameter        : Controller, ControllerModePtr
 * Return value     : E_OK / E_NOT_OK
 **********************************************************************************************************************/
#if (CANIF_LITE_CONFIGURATION == STD_OFF)


Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, Can_ControllerStateType * ControllerModePtr)
{

    /* Pointer to Controller State configuration */
    CanIf_ControllerStateType * lControllerState_p;

    /* Local pointer to the Controller Config structure */
    const CanIf_Cfg_CtrlConfig_tst * lCtrlConfig_pst;

    /* Local pointer to store the return value of Can_GetControllerMode() API */
    Std_ReturnType lRetVal_en;

    Can_ControllerStateType CtrlState_st;

#if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
       /* Local Pointer to the Driver Config structure */
       const CanIf_CtrlDrvCfgStruct * lCtrlDrvConfig_pst;
   #endif


    /* Local variable to store controller CustId */
    uint8 lCtrlCustId_u8;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_NOT_OK((FALSE == CanIf_Prv_InitStatus_b), CANIF_GET_CONTROLLER_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((ControllerId >= CANIF_TOTAL_CTRLS), CANIF_GET_CONTROLLER_SID, \
                                                                            CANIF_E_PARAM_CONTROLLERID)
    #if CANIF_DEV_ERROR_DETECT == STD_ON
        lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];
    #endif

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_NOT_OK((lCtrlCustId_u8 == 0xFFu), CANIF_GET_CONTROLLER_SID, \
                                                                            CANIF_E_PARAM_CONTROLLERID)
    /* If ControllerModePtr is null, report to DET and return E_NOT_OK*/
    CANIF_DET_REPORT_ERROR_NOT_OK((NULL_PTR == ControllerModePtr), CANIF_GET_CONTROLLER_SID,CANIF_E_PARAM_POINTER)

    /* Obtain the custId for the requested controller in the selected variant */
    lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];

    /*Get configured data of requested controller*/
    lCtrlConfig_pst  = (CanIf_Prv_ConfigSet_tpst->CanIf_CtrlConfigPtr) + lCtrlCustId_u8;

    /*Get current status of the requested controller*/
    lControllerState_p = CanIf_Prv_ControllerState_ast + ControllerId;

#if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
       /* Select the Can Driver for which this request needs to be forwared */
       lCtrlDrvConfig_pst = CanIf_Prv_ConfigSet_tpst->CanCtrlDrvCont_Ptr +(lCtrlConfig_pst->CanDrvIndx);
   #endif

    /* Call Can_GetControllerMode() for respective CanDrv */

    #if CANIF_CFG_PUBLIC_MULTIPLE_DRIVER_SUPPORT == STD_ON || THIRD_PARTY_DRIVER_INTEGRATION == STD_ON
        lRetVal_en = lCtrlDrvConfig_pst->GetControllerMode(lCtrlConfig_pst->CtrlCanCtrlRef, ControllerModePtr);
    #else
        lRetVal_en = Can_GetControllerMode((lCtrlConfig_pst->CtrlCanCtrlRef), ControllerModePtr);
    #endif
        CtrlState_st = *ControllerModePtr;
     /* If Can_GetControllerMode request is accepted by CanDrv store the mode locally */
    if(lRetVal_en !=E_NOT_OK)
    {
        /*Update the controller mode*/
        lControllerState_p->Ctrl_Pdu_mode = ((((uint8)CtrlState_st)<<CANIF_Ctrl_BIT_SHIFT)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_CTRL_MODE));

    }

    return lRetVal_en;
}

#endif


/***********************************************************************************************************************
 * Function name    : CanIf_ControllerBusOff
 * Syntax           : void CanIf_ControllerBusOff(uint8 ControllerId)
 * Description      : Service indicates a CAN controller BusOff event referring to the corresponding
                      CAN controller. This service is called by the CAN driver.
 * Parameter        : Controller
 * Return value     : None
 **********************************************************************************************************************/


void CANIF_CONTROLLERBUSOFF_FNAME( uint8 ControllerId )
{

    /* Pointer to Controller State configuration */
    CanIf_ControllerStateType * lControllerState_p;
    #if ((CANIF_DEV_ERROR_DETECT == STD_ON)||(CANIF_PUBLIC_TXBUFFERING == STD_ON) || (CANIF_XCORE_CFG_ENABLED != STD_OFF))
        uint8 lCtrlCustId_u8;
    #endif

    /*Pointer to the structure CanIf_CallbackFuncType */
    const CanIf_CallbackFuncType * lCallBackPtr_p;

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_VOID_RET((FALSE == CanIf_Prv_InitStatus_b), CANIF_CONTROLLER_BUSOFF_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and exit the function */
    CANIF_DET_REPORT_ERROR_VOID_RET((ControllerId >= CANIF_TOTAL_CTRLS), \
                                                                    CANIF_CONTROLLER_BUSOFF_SID,CANIF_E_PARAM_CONTROLLER)
    #if ((CANIF_DEV_ERROR_DETECT == STD_ON)||(CANIF_PUBLIC_TXBUFFERING == STD_ON)||(CANIF_XCORE_CFG_ENABLED != STD_OFF))
        lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];
    #endif

    /* If Controller is invalid, report to DET and exit the function */
    CANIF_DET_REPORT_ERROR_VOID_RET((lCtrlCustId_u8 == 0xFFu), CANIF_CONTROLLER_BUSOFF_SID,CANIF_E_PARAM_CONTROLLER)

    /*Get current status of the requested controller*/
    lControllerState_p = CanIf_Prv_ControllerState_ast + ControllerId;

    /*Get the address of structure having dispatch function pointers*/
    lCallBackPtr_p = &CanIf_Callback;

    /* Enter Critical Section */
    SchM_Enter_CanIf_ControllerState();

    #if CANIF_PUBLIC_TXCONFIRM_POLLING_SUPPORT!= STD_OFF

        /* Reset Tx Confirmation state for the controller*/
        lControllerState_p->CanIf_TxCnfmStatus = CANIF_NO_NOTIFICATION;
    #endif

    /*Set PduMode to CANIF_TX_OFFLINE*/
    lControllerState_p->Ctrl_Pdu_mode = ((uint8)CANIF_TX_OFFLINE)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_PDU_MODE);

    /* Leave Critical Section */
    SchM_Exit_CanIf_ControllerState();

    #if((CANIF_PUBLIC_TXBUFFERING == STD_ON)&&(CANIF_CFG_TX_FEATURE_ENABLED==STD_ON))

        /* Enter Critical Section */
        SchM_Enter_CanIf_TxBufAccessNoNest();

        /*If buffering is enabled, clear all the buffers assigned to requested controller*/
        CanIf_Prv_ClearTxChannelBuffer(lCtrlCustId_u8);

        /* Leave Critical Section */
        SchM_Exit_CanIf_TxBufAccessNoNest();

    #endif

    #if (CANIF_XCORE_CFG_ENABLED != STD_OFF)
        /* If XCore is enabled, Clear the pipe packets assigned to requested controller in Tx and Tx confirmation pipes */
        CanIf_XCore_ClearPipes(lCtrlCustId_u8);
    #endif

    if(lCallBackPtr_p->User_ControllerBusOff != NULL_PTR)
    {
        /*Invoke User_ControllerBusOff for configured user(CANSM or CDD)*/
        lCallBackPtr_p->User_ControllerBusOff(ControllerId);
    }

} /* End of CanIf_ControllerBusOff() */



/***********************************************************************************************************************
 * Function name    : CanIf_ControllerModeIndication
 * Syntax           : void CanIf_ControllerModeIndication(uint8 ControllerId, Can_ControllerStateType ControllerMode)
 * Description      : Service indicates a mode change is successful for the particular
                      CAN controller. This service is called by the CAN driver.
 * Parameter        : Controller
 * Return value     : None
 **********************************************************************************************************************/


void CANIF_CONTROLLERMODEINDICATION_FNAME(uint8 ControllerId, Can_ControllerStateType ControllerMode)
{

    /*Pointer to the structure CanIf_CallbackFuncType */
    const CanIf_CallbackFuncType * lCallBackPtr_p;

    /* Pointer to Controller State configuration */
    CanIf_ControllerStateType * lControllerState_p;

    #if (CANIF_DEV_ERROR_DETECT == STD_ON)
        uint8 lCtrlCustId_u8;
    #endif

    /* If CAN Interface is uninitialized, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_INIT_VOID_RET((FALSE == CanIf_Prv_InitStatus_b), CANIF_CONTROLLER_MODE_IND_SID, CANIF_E_UNINIT)

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_VOID_RET((ControllerId >= CANIF_TOTAL_CTRLS), \
                                                                CANIF_CONTROLLER_MODE_IND_SID,CANIF_E_PARAM_CONTROLLER)
    #if (CANIF_DEV_ERROR_DETECT == STD_ON)
        lCtrlCustId_u8 = CanIf_Prv_ConfigSet_tpst->CtrlIdTable_Ptr[ControllerId];
    #endif

    /* If Controller is invalid, report to DET and return E_NOT_OK */
    CANIF_DET_REPORT_ERROR_VOID_RET((lCtrlCustId_u8 == 0xFFu), CANIF_CONTROLLER_MODE_IND_SID,CANIF_E_PARAM_CONTROLLER)

    /*Get the address of structure having dispatch function pointers*/
    lCallBackPtr_p =  &CanIf_Callback;

    /*Get current status of the requested controller*/
    lControllerState_p = CanIf_Prv_ControllerState_ast + ControllerId;

    /*Update the controller mode*/
    lControllerState_p->Ctrl_Pdu_mode = ((((uint8)ControllerMode)<<CANIF_Ctrl_BIT_SHIFT)|((lControllerState_p->Ctrl_Pdu_mode)&CANIF_BIT_MASK_CTRL_MODE));

    /*Invoke User_ControllerBusOff for configured user(CANSM or CDD)*/
    if(lCallBackPtr_p->User_ControllerModeIndication != NULL_PTR)
    {
        /*Invoke User_ControllerBusOff for configured user(CANSM or CDD)*/
        lCallBackPtr_p->User_ControllerModeIndication(ControllerId, ControllerMode);
    }

}
#define CANIF_STOP_SEC_CODE
#include "CanIf_MemMap.h"
