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
#include "CanSM_Prv.h"
/**
***************************************************************************************************
** Function Name        : CanSM_ControllerBusOff
**
** Service ID           : 0x04
**
** Description          : This callback function notifies the CanSM about a bus-off event on a certain CAN controller,
**                        which needs to be considered with the specified bus-off recovery handling for the impacted
**                        CAN network.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant (only for different CanControllers)
**
** Input Parameters     : ControllerId-CAN controller, which detected a bus-off event
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/
#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"
/* HIS METRIC LEVEL VIOLATION IN CanSM_ControllerBusOff: Function contains very simple "if-else" statements.
   HIS metric compliance would result in new sub functions which will affect the performance. */
void CanSM_ControllerBusOff(uint8 ControllerId )
{
    /*To track the ComM mode status*/
    ComM_ModeType CanSM_ComM_ModeType_uo;
    /*Variable to track the status of BSWM*/
    CanSM_BswMCurrentStateType CanSM_BswM_Mode_en;
    /*To keep a track of current BOR state */
    CanSM_BusOffRecoveryStateType_ten CanSM_CurrBORState_en;
    /*Temporary pointer to hold the adress of configured network*/
    const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;
    /*Variable to hold the network index for the given controller*/
    uint8 network_indx_u8;
    /*To track the Pending Bus Off status for the network to which the CAN Controller belongs*/
    boolean BusoffISRPend_Temp_b;
    /*To keep a track of current BOR mode Enable/Disable */
    uint8 CanSM_BORMode_u8;

    /* Report DET : Development Error for Invalid  Controller ID */
    CANSM_REPORT_ERROR((ControllerId >= CANSM_NUM_CAN_CONTROLLERS),
                                                 (uint8)CANSM_CONTROLLERBUSOFF_SID,
                                                 (uint8)CANSM_E_PARAM_CONTROLLER)

    /* Notify DET if CanSM module is not initialized */
    CANSM_REPORT_ERROR((CanSM_Init_ab == (boolean)CANSM_UNINITED),
                           ((uint8)CANSM_CONTROLLERBUSOFF_SID),
                           ((uint8)CANSM_E_UNINIT)
                           )


   /*If the given controller id is valid, get the associated network*/
   network_indx_u8 = (uint8)CanSM_ConfigSet_pcst->CanSM_NetworktoCtrlConf_pcu8[ControllerId];
    /* Report DET : Development Error for Invalid  network ID */
   CANSM_REPORT_ERROR((network_indx_u8 == CANSM_INVALID_ID), (uint8)CANSM_CONTROLLERBUSOFF_SID,
           (uint8)CANSM_E_PARAM_CONTROLLER)


    /*Hold the address of the configured network*/
    CanSM_NetworkConf_ps = &CanSM_Network_pcst[network_indx_u8];
   /*Make a local copy of CanSM_stBusOffISRPend_a[network_indx_u8]*/
   BusoffISRPend_Temp_b = CanSM_BusOffISRPend_ab[network_indx_u8];

   /*Make a local copy of BOR Mode[network_indx_u8]*/
   CanSM_BORMode_u8 = CanSM_BORMode_au8[network_indx_u8];
   /* Get the Current BOR State only after the Mutex is accquired */
   CanSM_CurrBORState_en = CanSM_currBOR_State_en[network_indx_u8];

   if((CanSM_CurrNw_Mode_en[network_indx_u8] == CANSM_BSM_S_SILENTCOM) || (CanSM_ReqComM_Mode_en[network_indx_u8] == COMM_SILENT_COMMUNICATION))
       {
           /*Update the global array for the network as TRUE to indicate that the network has bus off */

          CanSM_BusOff_Indicated_ab[network_indx_u8] = FALSE;
          CanSM_BusOffISRPend_ab[network_indx_u8] = FALSE;
          CanSM_BOR_SilentCom_ab[network_indx_u8] = TRUE;


           /*Change the current BOR state to CANSM_S_RESTART_CC*/
           CanSM_currBOR_State_en[network_indx_u8] = CANSM_S_RESTART_CC;

       /*Report the DEM event status  to DEM*/
       (void)Dem_SetEventStatus(CanSM_Network_pcst[network_indx_u8].BusOffEventId_uo,DEM_EVENT_STATUS_PREFAILED);

       /*Update the intended network variable with latest DEM event status to be used to provide status to DEM module on request*/
       #if(CANSM_DSM_REINIT_ENABLED != STD_OFF)
       CanSM_DSM_ReInit_Status_ast[network_indx_u8].CanSM_DEM_Eventstatus_u8 = DEM_EVENT_STATUS_PREFAILED;
       #endif

       CanSM_StartTimer(network_indx_u8);
       }
   else
       {

   /*Update the global array for the network as TRUE to indicate that the network has bus off */
   CanSM_BusOff_Indicated_ab[network_indx_u8] = TRUE;

    /* Check if the BOR State Machine is updated to CANSM_RECOVERY_ENABLED*/
    if( CanSM_BORMode_u8 == (uint8)CANSM_RECOVERY_ENABLED)
        {
            /* Check if the mutex is available*/
            if(E_OK == CanSM_GetMutex(network_indx_u8))
                {

                    /* Check the current BOR state */
                    if((CanSM_CurrBORState_en == CANSM_S_BUS_OFF_CHECK) || (CanSM_CurrBORState_en == CANSM_S_NO_BUS_OFF)
                            || (CanSM_CurrBORState_en == CANSM_S_RESTART_CC) || (CanSM_CurrBORState_en == CANSM_S_RESTART_CC_WAIT)
                            || (CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L1) || (CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L2))
                        {
                            /*Update the state of Network for BswM*/
                            CanSM_BswM_Mode_en = CANSM_BSWM_BUS_OFF;

                            /*Notify BswM with CANSM_BSWM_NO_COMMUNICATION as part of E_PRE_NOCOM*/
                            BswM_CanSM_CurrentState(CanSM_NetworkConf_ps->ComM_channelId_uo,CanSM_BswM_Mode_en);

                            /*Update CanSM_BusSMMode_au8[] to CANSM_BSM_S_SILENTCOM i.e COMM_SILENT_COMMUNICATION*/
                            CanSM_BusSMMode_au8[network_indx_u8] = (ComM_ModeType)CANSM_BSM_S_SILENTCOM;
                            /*Check if multiple CAN controllers are mapped to the network*/
                            if(CanSM_ConfigSet_pcst->CanSM_NetworkConf_pcst[network_indx_u8].SizeofController_u8 > 1u)
                                {
                                CanSM_BOR_Controller_Stopped_state_en[network_indx_u8] = CANSM_BOR_CONTROLLER_STOPPED_REQUEST;
                                }
                            else
                                {
                                    /*Change the current BOR state to CANSM_S_RESTART_CC*/
                                    CanSM_currBOR_State_en[network_indx_u8] = CANSM_S_RESTART_CC;
                                    /*Start the Wait state Timer if only single controller is mapped to the CanSM network */
                                    CanSM_StartTimer(network_indx_u8);

                                }

                            /*Notify ComM about CanSM state changes*/
                            CanSM_ComM_ModeType_uo = COMM_SILENT_COMMUNICATION;
                            ComM_BusSM_ModeIndication(CanSM_Network_pcst[network_indx_u8].ComM_channelId_uo,CanSM_ComM_ModeType_uo);
                            /*Report the DEM event status  to DEM*/
                            (void)Dem_SetEventStatus(CanSM_Network_pcst[network_indx_u8].BusOffEventId_uo,DEM_EVENT_STATUS_PREFAILED);

                            /*Update the intended network variable with latest DEM event status to be used to provide status to DEM module on request*/
                            #if(CANSM_DSM_REINIT_ENABLED != STD_OFF)
                            CanSM_DSM_ReInit_Status_ast[network_indx_u8].CanSM_DEM_Eventstatus_u8 = DEM_EVENT_STATUS_PREFAILED;
                            #endif
                        }

                    /*Release the mutex*/
                    CanSM_ReleaseMutex(network_indx_u8);
                 }

                /*If the mutex was busy and no pending bus off*/
            else if(BusoffISRPend_Temp_b == FALSE)
                {
                    /*update the pending bus off as TRUE*/
                    CanSM_BusOffISRPend_ab[network_indx_u8]= TRUE;
                    /* Check if current BOR state is BUS_OFF_CHECK OR NO_BUSOFF */
                        if((CanSM_CurrBORState_en == CANSM_S_BUS_OFF_CHECK) || (CanSM_CurrBORState_en == CANSM_S_NO_BUS_OFF)
                                || (CanSM_CurrBORState_en == CANSM_S_RESTART_CC) || (CanSM_CurrBORState_en == CANSM_S_RESTART_CC_WAIT)
                                || (CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L1) || (CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L2))
                            {
                                /*Update the state of Network for BswM*/
                                CanSM_BswM_Mode_en = CANSM_BSWM_BUS_OFF;

                                /*Notify BswM with CANSM_BSWM_NO_COMMUNICATION as part of E_PRE_NOCOM*/
                                BswM_CanSM_CurrentState(CanSM_NetworkConf_ps->ComM_channelId_uo,CanSM_BswM_Mode_en);

                                /*Update CanSM_BusSMMode_au8[] to CANSM_BSM_S_SILENTCOM i.e COMM_SILENT_COMMUNICATION*/
                                CanSM_BusSMMode_au8[network_indx_u8] = (ComM_ModeType)CANSM_BSM_S_SILENTCOM;
                                /*Check if multiple CAN controllers are mapped to the network*/
                                if(CanSM_ConfigSet_pcst->CanSM_NetworkConf_pcst[network_indx_u8].SizeofController_u8 > 1u)
                                    {
                                    CanSM_BOR_Controller_Stopped_state_en[network_indx_u8] = CANSM_BOR_CONTROLLER_STOPPED_REQUEST;
                                    }
                                else
                                    {
                                        /*Change the current BOR state to CANSM_S_RESTART_CC*/
                                        CanSM_currBOR_State_en[network_indx_u8] = CANSM_S_RESTART_CC;
                                        /*Start the Wait state Timer if only single controller is mapped to the CanSM network */
                                         CanSM_StartTimer(network_indx_u8);

                                    }
                                /*Notify ComM about CanSM state changes*/
                                CanSM_ComM_ModeType_uo = COMM_SILENT_COMMUNICATION;
                                ComM_BusSM_ModeIndication(CanSM_Network_pcst[network_indx_u8].ComM_channelId_uo,CanSM_ComM_ModeType_uo);
                                /*Report the DEM event status  to DEM*/
                                (void)Dem_SetEventStatus(CanSM_Network_pcst[network_indx_u8].BusOffEventId_uo,DEM_EVENT_STATUS_PREFAILED);

                                /*Update the intended network variable with latest DEM event status to be used to provide status to DEM module on request*/
                                #if(CANSM_DSM_REINIT_ENABLED != STD_OFF)
                                CanSM_DSM_ReInit_Status_ast[network_indx_u8].CanSM_DEM_Eventstatus_u8 = DEM_EVENT_STATUS_PREFAILED;
                                #endif
                            }

                }
            else
                {
                    /*do nothing*/
                }

        }
       }
}


/**
***************************************************************************************************
** Function Name        : CanSM_BusOffTransitions
**
** Description          : This functon is either called in the Main Function regularly(also if Mutex was BUSY)
**                        This function inititaes and Controls Bus Off transitions in Bus-Off recovery
**                        state Machine.
**
** Re-entrancy          : Reentrant (only for different networks)
**
** Input Parameters     : network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/

/* HIS METRIC v(G) VIOLATION IN CanSM_BusOffTransitions: Function contains very simple "if-else" statements.
   HIS metric compliance would result in new sub functions which will affect the performance. */
void CanSM_BusOffTransitions(NetworkHandleType network)
{
    /*Variable to track the status of BSWM*/
    CanSM_BswMCurrentStateType CanSM_BswM_Mode_en;
    /*To keep a track of current BOR state */
    CanSM_BusOffRecoveryStateType_ten CanSM_CurrBORState_en;
    /*To keep a track of Controller stopped state when bus off is notified and the network has multiple CAN controllers */
    CanSM_BOR_Controller_Stopped_stateType_ten CanSM_BOR_Controller_Stopped_states_en;
    /*Temporary pointer to hold the adress of configured network*/
    const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;
    /*To hold the controller Id*/
    uint8 CanSM_ControllerId_u8;
    /*Used in FOR loop to access CAN controllers of the corresponding network*/
    uint8 CanSM_Ctrl_index_u8;
    /*To track the ComM mode status*/
    ComM_ModeType CanSM_reqComM_Mode_uo;
    #if(CANSM_GETBUSOFFDELAY_SUPPORT == STD_ON)
    /*To track the Delay value received from upper layer*/
    uint8 CanSM_BusOffdelay_u8 = 0;
    uint8 CanSM_delay_u8 = 0;
    #endif

    /*Hold the address of the configured network*/
    CanSM_NetworkConf_ps = &CanSM_Network_pcst[network];
    /* Get the Current BOR State only after the Mutex is accquired */
    CanSM_CurrBORState_en = CanSM_currBOR_State_en[network];
    /* Get the controller state during bus off  transition*/
    CanSM_BOR_Controller_Stopped_states_en =  CanSM_BOR_Controller_Stopped_state_en[network];

    /*Check if multiple CAN controllers are mapped to the network*/
    if(CanSM_ConfigSet_pcst->CanSM_NetworkConf_pcst[network].SizeofController_u8 > 1u)
        {
            if(CanSM_BOR_Controller_Stopped_states_en == CANSM_BOR_CONTROLLER_STOPPED_REQUEST)
            {
                /*Stop all the other CAN controllers of the affected CanSM network which are not stopped*/
                CanSM_BOR_MultipleCtrlsStop(network);
                /* Get the controller state during bus off  transition*/
                CanSM_BOR_Controller_Stopped_states_en =  CanSM_BOR_Controller_Stopped_state_en[network];
                if(CanSM_BOR_Controller_Stopped_states_en == CANSM_BOR_CONTROLLER_STOPPED_REQUEST_COMPLETED)
                {

                        /*Change the current BOR state to CANSM_S_RESTART_CC*/
                        CanSM_currBOR_State_en[network] = CANSM_S_RESTART_CC;
                        CanSM_CurrBORState_en = CanSM_currBOR_State_en[network];
                        /*Start the Wait state Timer for L1 */
                        CanSM_StartTimer(network);

                }
            }
        }

    /*Check if multiple CAN controllers are mapped to the network*/
    if(CanSM_ConfigSet_pcst->CanSM_NetworkConf_pcst[network].SizeofController_u8 > 1u)
        {
            /*If T_REPEAT_MAX counter has not exceeded*/
            if(CanSM_Num_Unsuccessful_ModeReq_au8[network] < CanSM_ModeRepeatReq_Max_u8)
                {
                    if(CanSM_BOR_Controller_Stopped_states_en == CANSM_BOR_CONTROLLER_STOPPED_WAIT)
                        {
                            /*Check if a controller indication was received that all the CAN controllers are stopped*/
                            if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                                {
                                    /*Set the Timer to Elapsed Mode */
                                    CanSM_TimerConfig_ast[network].stTimer = CANSM_TIMER_CANCELLED;

                                    /*Reset the controller mode indication flag for the network*/
                                    CanSM_Ctrl_ModeInd_ab[network] = FALSE;

                                    /*Move to next substate of the CANSM_BOR_CONTROLLER_STOPPED_REQUEST_COMPLETED */
                                    CanSM_BOR_Controller_Stopped_state_en[network] = CANSM_BOR_CONTROLLER_STOPPED_REQUEST_COMPLETED;

                                    /*Reset the T_REPEAT_MAX counter*/
                                    CanSM_Num_Unsuccessful_ModeReq_au8[network] = CANSM_ZERO;

                                    /*Change the current BOR state to CANSM_S_RESTART_CC*/
                                    CanSM_currBOR_State_en[network] = CANSM_S_RESTART_CC;

                                    /*Start the Wait state Timer for L1 */
                                    CanSM_StartTimer(network);
                                }

                            /*Check if timeout has occurred and so proceed accordingly*/
                            else
                                {
                                    if(CanSM_TimerConfig_ast[network].cntTick_u16 >= CanSM_ModeRepeatReq_Time_u16)
                                        {
                                            /*Set the Timer to Elapsed Mode */
                                            CanSM_TimerConfig_ast[network].stTimer = CANSM_TIMER_ELAPSED;

                                            /*Move to previous substate*/
                                            CanSM_BOR_Controller_Stopped_state_en[network]=CANSM_BOR_CONTROLLER_STOPPED_REQUEST;

                                        }
                                }
                        }
                }
             else
                {
                        /*Report to DET that T_REPEAT_MAX counter has exceeded CanSM_ModeReq_Max_u8*/
                        CANSM_REPORT_RUNTIME_ERROR_NO_CONDITON(((uint8)CANSM_MAINFUNCTION_SID),(uint8)(CANSM_E_MODE_REQUEST_TIMEOUT))
                }
        }

    /* Check whether current BOR state is CANSM_S_RESTART_CC */
    if(CanSM_CurrBORState_en == CANSM_S_RESTART_CC)
        {
            /*Start all the CAN controllers*/
            CanSM_BOR_CtrlsStart(network);
        }
    else if(CanSM_CurrBORState_en == CANSM_S_RESTART_CC_WAIT)
        {
            /*Check if a controller indication was received that all the CAN controllers are stopped*/
            if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                {
                    /*Reset the controller mode indication flag to FALSE*/
                    CanSM_Ctrl_ModeInd_ab[network] = FALSE;

                    /*Reset the T_REPEAT_MAX counter*/
                    CanSM_Num_Unsuccessful_ModeReq_au8[network] = CANSM_ZERO;

                    if(CanSM_BusOff_Cntr_au8[network] < CanSM_Network_pcst[network].BorCntL1L2_u8)
                    {
                        /*Change the current BOR state to CANSM_S_BUS_OFF_RECOVERY_L1*/
                        CanSM_currBOR_State_en[network] = CANSM_S_BUS_OFF_RECOVERY_L1;
                    }
                    else
                    {
                        /*Change the current BOR state to CANSM_S_BUS_OFF_RECOVERY_L2*/
                        CanSM_currBOR_State_en[network] = CANSM_S_BUS_OFF_RECOVERY_L2;
                    }

                }
            else
                {
                if(CanSM_TimerConfig_ast[network].cntTick_u16 >= CanSM_ModeRepeatReq_Time_u16)
                   {
                       /*Move to previous substate ie CANSM_S_RESTART_CC*/
                       CanSM_currBOR_State_en[network]=CANSM_S_RESTART_CC;
                   }
                }

            }
           /*Checking for the guarding condition G_TX_ON*/
    else if((CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L1) ||(CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L2))
            {

               #if(CANSM_GETBUSOFFDELAY_SUPPORT == STD_ON)
               if(CanSM_Network_pcst[network].BusOffDelayConfig_b == TRUE)
                    {
                        /*Fetch the delay value from the User_GetBusOffDelay api*/
                        CanSM_ConfigSet_pcst->User_GetBusOffDelay(CanSM_Network_pcst[network].ComM_channelId_uo, &CanSM_BusOffdelay_u8);
                        /*Store in the local*/
                        CanSM_delay_u8 = CanSM_BusOffdelay_u8;
                        /*Store the value for the network*/
                        CanSM_GetBusOffDelay_Value_u8[network]= CanSM_delay_u8;
                    }
               #endif
               #if (CANSM_CALIBRATION != STD_ON)
                if((( CanSM_BusOff_Cntr_au8[network] < CanSM_Network_pcst[network].BorCntL1L2_u8)&&(CanSM_TimerConfig_ast[network].cntTick_u16 >=(CanSM_Network_pcst[network].BorTimeL1_u16 + CanSM_GetBusOffDelay_Value_u8[network]) ))||
                    ((CanSM_BusOff_Cntr_au8[network] >= CanSM_Network_pcst[network].BorCntL1L2_u8)&&(CanSM_TimerConfig_ast[network].cntTick_u16 >= (CanSM_Network_pcst[network].BorTimeL2_u16 + CanSM_GetBusOffDelay_Value_u8[network]))))

               #else
               /* MR12 RULE 13.5 VIOLATION: Warning is "Not Critical". Calibration functions are being called on the right hand side of the logical && operation to obtain the L1 and L2 timer values.
                  Those functions will return a uint16 value, hence it doesnt have any side effects. Redesign is not recommended as the change would decrease maintainability and increase runtime */
                if((( CanSM_BusOff_Cntr_au8[network] < CanSM_Network_pcst[network].BorCntL1L2_u8)&&(CanSM_TimerConfig_ast[network].cntTick_u16 >= (CanSM_Network_pcst[network].pBorTimeL1_pfct() + CanSM_GetBusOffDelay_Value_u8[network])))||
                                            ((CanSM_BusOff_Cntr_au8[network] >= CanSM_Network_pcst[network].BorCntL1L2_u8)&&(CanSM_TimerConfig_ast[network].cntTick_u16 >= (CanSM_Network_pcst[network].pBorTimeL2_pfct() + CanSM_GetBusOffDelay_Value_u8[network]))))
               #endif
                {
                   /* Increment the Bus Off counter only when the current BOR state is CANSM_S_BUS_OFF_RECOVERY_L1 and
                       the bus off counter is less than 255 */
                    if((CanSM_CurrBORState_en == CANSM_S_BUS_OFF_RECOVERY_L1) &&  (CANSM_BUSOFF_COUNT_MAX > CanSM_BusOff_Cntr_au8[network]))
                        {
                            /* Increment Bus Off counter */
                            CanSM_BusOff_Cntr_au8[network]++;
                        }
                    /*Set the Timer to Elapsed Mode */
                    CanSM_TimerConfig_ast[network].stTimer = CANSM_TIMER_CANCELLED;

                    /* Set Pdu Mode to ONLINE */
                    /* Loop for All the Controllers in the Network*/
                    for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_Network_pcst[network].SizeofController_u8; CanSM_Ctrl_index_u8++)
                        {
                             /* break the loop if any bus off indicated in between */
                             if ((CanSM_currBOR_State_en[network] == CANSM_S_RESTART_CC) ||
                                     (CanSM_BOR_Controller_Stopped_state_en[network] == CANSM_BOR_CONTROLLER_STOPPED_REQUEST))
                             {
                                 break;
                             }

                             CanSM_ControllerId_u8 = CanSM_Network_pcst[network].Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];
                            #if(CANSM_TX_OFFLINE_ACTIVE_SUPPORT == STD_ON)
                             if(CanSM_Passive_b == TRUE)
                             {
                             /*Return Value not checked, as this transition is not expected to return E_NOT_OK */
                             (void)(CanIf_SetPduMode(CanSM_ControllerId_u8, CANIF_TX_OFFLINE_ACTIVE));
                             }
                             else
                             {
                                 /*Return Value not checked, as this transition is not expected to return E_NOT_OK */
                                 (void)(CanIf_SetPduMode(CanSM_ControllerId_u8, CANIF_ONLINE));
                             }
                            #else
                             /*Return Value not checked, as this transition is not expected to return E_NOT_OK */
                             (void)(CanIf_SetPduMode(CanSM_ControllerId_u8, CANIF_ONLINE));
                            #endif

                         }

                    /* Critical section Enter */
                    SchM_Enter_CanSM_BOR_Nw_ModesNoNest();
                    /* Check if any bus-off was occured during recovery of previous bus-off */
                    if (!((CanSM_currBOR_State_en[network] == CANSM_S_RESTART_CC) ||
                               (CanSM_BOR_Controller_Stopped_state_en[network] == CANSM_BOR_CONTROLLER_STOPPED_REQUEST)))
                    {

                        /*Update CanSM_BusSMMode_au8[] to CANSM_BSM_S_FULLCOM i.e COMM_FULL_COMMUNICATION*/
                        CanSM_BusSMMode_au8[network] = (ComM_ModeType)CANSM_BSM_S_FULLCOM;

                        /*Update the BOR state*/
                        CanSM_currBOR_State_en[network] = CANSM_S_BUS_OFF_CHECK;

                        /*Start the timer to check for message confirmtion either by Tx time Ensured or confirmation polling*/
                        CanSM_StartTimer(network);
                    }
                    /* Critical Section Exit */
                    SchM_Exit_CanSM_BOR_Nw_ModesNoNest();

                    /* Notify upper layers if CanSM was moved to FULL COM */
                    if (CanSM_BusSMMode_au8[network] == (ComM_ModeType)CANSM_BSM_S_FULLCOM)
                    {
                        /*Notify the BswM*/
                        CanSM_BswM_Mode_en = CANSM_BSWM_FULL_COMMUNICATION;

                        BswM_CanSM_CurrentState(CanSM_Network_pcst[network].ComM_channelId_uo,CanSM_BswM_Mode_en);

                        /*Notify to ComM about CanSM state Transition*/
                        CanSM_reqComM_Mode_uo = COMM_FULL_COMMUNICATION;

                        ComM_BusSM_ModeIndication(CanSM_Network_pcst[network].ComM_channelId_uo,CanSM_reqComM_Mode_uo);

                    }

              }
            }

    /*For busoffs occurring during the recovery, just restart the controller*/
    else
        {
            /* Loop for All the Controllers in the network and Set them to STOPPED Mode*/
            for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_NetworkConf_ps->SizeofController_u8 ;CanSM_Ctrl_index_u8++)
                {
                    /*Store the CAN controller ID belonging to the network*/
                    CanSM_ControllerId_u8 = CanSM_NetworkConf_ps->Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];
                    if(CanSM_ControllerState_en[CanSM_ControllerId_u8] != CANSM_ControllerState_STARTED)
                    {
                       /*Return Value not checked, as this transition is not expected to return E_NOT_OK */
                        (void)CanIf_SetControllerMode(CanSM_ControllerId_u8,CAN_CS_STARTED);

                    }
                }
        }
}

/**
***************************************************************************************************
** Function Name        : CanSM_BOR_MultipleCtrlsStop
**
** Description          : This api will stop all the CAN controllers of the network during bus off.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant (only for different network)
**
** Input Parameters     : network-The passed CanSM network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/


void CanSM_BOR_MultipleCtrlsStop(NetworkHandleType network)
{
        /*Temporary pointer to hold the adress of configured network*/
        const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;
        /*To hold the controller Id*/
        uint8 CanSM_ControllerId_u8;
        /*Used in FOR loop to access CAN controllers of the corresponding network*/
        uint8 CanSM_Ctrl_index_u8;
        /*Hold the counter number to know that all the controllers are stopped*/
        uint8 CanSM_Controller_Counter_u8 = 0;

        /*Hold the address of the configured network*/
        CanSM_NetworkConf_ps = &CanSM_Network_pcst[network];

        /* Loop for All the Controllers in the network and Set them to STOPPED Mode*/
        for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_NetworkConf_ps->SizeofController_u8 ;CanSM_Ctrl_index_u8++)
        {
            /*Store the CAN controller ID belonging to the network*/
            CanSM_ControllerId_u8 = CanSM_NetworkConf_ps->Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];

               /*If E_NOT_OK is returned for any of the controller, increment the T_REPEAT_MAX counter and break*/
                if(E_NOT_OK == CanIf_SetControllerMode(CanSM_ControllerId_u8,CAN_CS_STOPPED))
                    {
                        /*If atleast one unsuccessful attempt to stop the controller occured for the corresponding network then increment the unsuccessful attempt*/
                        CANSM_INCREMENT_UINT8_COUNTER(CanSM_Num_Unsuccessful_ModeReq_au8[network])
                        break;
                    }
                else
                    {
                        /* increment the counter to indicate if all controller has returned E_OK */
                        CanSM_Controller_Counter_u8++;
                    }
        }

        if(CanSM_Controller_Counter_u8 == CanSM_NetworkConf_ps->SizeofController_u8 )
            {
                /*Check if in the meantime, controller mode indication was received and update the substate if so*/
                if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                    {
                        /*Reset the controller mode indication flag to FALSE*/
                        CanSM_Ctrl_ModeInd_ab[network] = FALSE;

                        /*Reset the T_REPEAT_MAX counter*/
                        CanSM_Num_Unsuccessful_ModeReq_au8[network] = CANSM_ZERO;

                        /*Move to next substate of the CANSM_BOR_CONTROLLER_STOPPED_REQUEST_COMPLETED */
                        CanSM_BOR_Controller_Stopped_state_en[network] = CANSM_BOR_CONTROLLER_STOPPED_REQUEST_COMPLETED;

                    }
                else
                    {
                        /*Move to next substate of the CANSM_BOR_CONTROLLER_STOPPED_WAIT */
                    CanSM_BOR_Controller_Stopped_state_en[network] = CANSM_BOR_CONTROLLER_STOPPED_WAIT;
                        /*Start the Wait state Timer */
                        CanSM_StartTimer(network);


                    }
            }
        else
            {

            }

}


/**
***************************************************************************************************
** Function Name        : CanSM_BOR_CtrlsStart
**
** Service ID           :
**
** Description          : This api will start all the CAN controllers of the network during bus off recovery.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant (only for different network)
**
** Input Parameters     : network-The passed CanSM network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/


void CanSM_BOR_CtrlsStart(NetworkHandleType network)
{
        /*Temporary pointer to hold the adress of configured network*/
        const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;
        /*To hold the controller Id*/
        uint8 CanSM_ControllerId_u8;
        /*Used in FOR loop to access CAN controllers of the corresponding network*/
        uint8 CanSM_Ctrl_index_u8;
        /*Hold the counter number to know that all the controllers are stopped*/
        uint8 CanSM_Controller_Counter_u8 = 0;


        /*Hold the address of the configured network*/
        CanSM_NetworkConf_ps = &CanSM_Network_pcst[network];

        /* Loop for All the Controllers in the network and Set them to STOPPED Mode*/
        for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_NetworkConf_ps->SizeofController_u8 ;CanSM_Ctrl_index_u8++)
        {
            /*Store the CAN controller ID belonging to the network*/
            CanSM_ControllerId_u8 = CanSM_NetworkConf_ps->Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];

               /*If E_NOT_OK is returned for any of the controller, increment the T_REPEAT_MAX counter and break*/
                if(E_NOT_OK == CanIf_SetControllerMode(CanSM_ControllerId_u8,CAN_CS_STARTED))
                    {
                        /*If atleast one unsuccessful attempt to stop the controller occured for the corresponding network then increment the unsuccessful attempt*/
                        CANSM_INCREMENT_UINT8_COUNTER(CanSM_Num_Unsuccessful_ModeReq_au8[network])
                        break;
                    }
                else
                    {
                        /* increment the counter to indicate if all controller has returned E_OK */
                        CanSM_Controller_Counter_u8++;
                    }
        }

        if(CanSM_Controller_Counter_u8 == CanSM_NetworkConf_ps->SizeofController_u8 )
            {
                /*Check if in the meantime, controller mode indication was received and update the substate if so*/
                if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                    {
                        /*Reset the controller mode indication flag to FALSE*/
                        CanSM_Ctrl_ModeInd_ab[network] = FALSE;

                        /*Reset the T_REPEAT_MAX counter*/
                        CanSM_Num_Unsuccessful_ModeReq_au8[network] = CANSM_ZERO;

                        if(CanSM_BusOff_Cntr_au8[network] < CanSM_Network_pcst[network].BorCntL1L2_u8)
                        {
                            /*Change the current BOR state to CANSM_S_BUS_OFF_RECOVERY_L1*/
                            CanSM_currBOR_State_en[network] = CANSM_S_BUS_OFF_RECOVERY_L1;
                        }
                        else
                        {
                            /*Change the current BOR state to CANSM_S_BUS_OFF_RECOVERY_L2*/
                            CanSM_currBOR_State_en[network] = CANSM_S_BUS_OFF_RECOVERY_L2;
                        }

                    }
                else
                    {
                        /*Move to next substate of the CANSM_S_RESTART_CC_WAIT */
                        CanSM_currBOR_State_en[network] = CANSM_S_RESTART_CC_WAIT;

                    }
            }
        else
            {
                /* No state transition */
            }

}


/**
***************************************************************************************************
** Function Name        : CanSm_Rb_DisableBusOffRecovery
**
** Description          : This api shall be called by Upper layer and it will update a global array .The api does the
**                        following operations in Main_Function()
**                         1. The api shall stop the busoff recovery
**                         2. As busoff is not recovered within the intended time, stop the CAN controllers: This is
**                         to make sure that after the API is called there is no further communication
**                         (Tx or Rx by this ECU on the affected network).
**                         3. Put all the TX and Rx pdu to OFFLINE mode for the intended CAN controller mapped to the
**                         CanSM network
**
** Re-entrancy          : Reentrant for different networks
**
** Input Parameters     : network - index of the Network Configuration structure
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : Void

**********************************************************************************************************************/

void CanSM_Rb_DisableBusOffRecovery(NetworkHandleType network)
{
    /* Get the CanSM Network Index value corresponding to the ComM channel Id received */
    network = CanSM_GetHandle(network);
    /* Update the array for the network to TRUE for disabling the BOR operation for the intended network*/
    CanSM_Rb_DisableBusOffRecovery_ab[network] = TRUE;
}


/**
***************************************************************************************************
** Function Name        : CanSM_GetMutex
**
** Service ID           : None
**
** Description          : This function attempts to to aqquire the Mutex for function execution in pre-emptive environment.
**                        If Mutex is available, this Function returns E_OK else it returns E_NOT_OK.
**
** Sync/Async           : Asynchronous
**
** Re-entrancy          : Reentrant for different network
**
** Input Parameters     : network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : E_OK(Mutex FREE)/E_NOT_OK(Mutex BUSY)

***************************************************************************************************/

Std_ReturnType CanSM_GetMutex(NetworkHandleType network)
{
    /*To keep a track of Return value*/
    Std_ReturnType CanSM_FuncVal_uo;

    /* Critical section Enter*/
    SchM_Enter_CanSM_GetMutexNoNest();

    /* Check if Mutex is available */
    if(CanSM_MutexMode_au8[network] == CANSM_MUTEX_FREE)
    {
        /* Set Mutex to BUSY */
        CanSM_MutexMode_au8[network] = CANSM_MUTEX_BUSY;

        CanSM_FuncVal_uo = E_OK;
    }
    /* Check if Mutex is BUSY */
    else
    {
        CanSM_FuncVal_uo = E_NOT_OK;
    }

    /* Critical section Exit*/
    SchM_Exit_CanSM_GetMutexNoNest();

    return(CanSM_FuncVal_uo);
}


/**
***************************************************************************************************
** Function Name        : CanSM_GetBusoff_Substate
**
** Service ID           : 0x81u
**
** Description          : This API provides the current BusOff-state of a given CanController
**
** Sync/Async           : Asynchronous
**
** Re-entrancy          : Reentrant(only for other different Can Controllers)
**
** Input Parameters     :  ControllerId - Id of the controller
**                         BORStatePtr - Data pointer to store the BusoffStatus
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : E_OK/E_NOT_OK

***************************************************************************************************/

Std_ReturnType CanSM_GetBusoff_Substate(uint8 ControllerId, CanSM_BusOffRecoveryStateType_ten* BORStatePtr)
{
    /*Variable to hold the network index for the given controller*/
    uint8 network_indx_u8;

    /*To keep a track of Return value*/
    Std_ReturnType CanSM_FuncVal_uo;

    /* Report DET : Development Error Handling for CANSM is Uninitialized */
    CANSM_REPORT_ERROR_NOK((CanSM_Init_ab == (boolean)CANSM_UNINITED),
                                (uint8)CANSM_GETBUSOFFSUBSTATE_SID,
                                (uint8)CANSM_E_UNINIT)

    /* Report DET : Development Error for Invalid  Controller ID */
    /* Controllers are assumed to have 0 based ID*/
    CANSM_REPORT_ERROR_NOK((ControllerId >= CANSM_NUM_CAN_CONTROLLERS),
                       (uint8)CANSM_GETBUSOFFSUBSTATE_SID,
                       (uint8)CANSM_E_PARAM_CONTROLLER)

    /* Report DET : Development Error for Invalid data pointer */
    CANSM_REPORT_ERROR_NOK((BORStatePtr == NULL_PTR),
                       (uint8)CANSM_GETBUSOFFSUBSTATE_SID,
                       (uint8)CANSM_E_PARAM_POINTER)


    /*If the given controller id is valid, get the associated network*/
    network_indx_u8 = (uint8)CanSM_ConfigSet_pcst->CanSM_NetworktoCtrlConf_pcu8[ControllerId];
    /* Report DET : Development Error for Invalid networkID */
    CANSM_REPORT_ERROR_NOK((network_indx_u8 == CANSM_INVALID_ID), (uint8)CANSM_GETBUSOFFSUBSTATE_SID,
            (uint8)CANSM_E_INVALID_NETWORK_HANDLE)

    /* Get the Current BOR State  only after the Mutex is acquired */
    *BORStatePtr = CanSM_currBOR_State_en[network_indx_u8];
    CanSM_FuncVal_uo = E_OK;

    return(CanSM_FuncVal_uo);
}


/**
 ***************************************************************************************************
 * \moduledescription
 * This functon is an internal non-Autosar function to check if a TxConfirmation is received for any of the controllers
 * in the network.This function calls CanIf_GetTxConfirmationState(ControllerId) to check if a TxConfirmation was
 * received for the Controller.
 *
 * \param   uint8
 * \arg     Indx_Network - index of the Network Configuration structure
 * \return  Std_ReturnType
 * \retval  -
 *
 *
 ***************************************************************************************************
 */
#if (CANSM_BOR_TX_CONFIRMATION_POLLING != STD_OFF)

Std_ReturnType CanSM_CheckTxRxNotification(NetworkHandleType network)
{
    /*To keep a track of Return value*/
    Std_ReturnType CanSM_FuncVal_uo;

    /*Temporary pointer to hold the adress of configured network*/
    const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;
    /*To hold the controller Id*/
    uint8 CanSM_ControllerId_u8 = 0;
    /*Used in FOR loop to access CAN controllers of the corresponding network*/
    uint8 CanSM_Ctrl_index_u8;

    /*Hold the address of the configured network*/
    CanSM_NetworkConf_ps = &CanSM_Network_pcst[network];

    /*Initialise the local variable as E_NOT_OK*/
    CanSM_FuncVal_uo = E_NOT_OK;

    /* Loop for All the Controllers in the network*/
    for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_NetworkConf_ps->SizeofController_u8 ;CanSM_Ctrl_index_u8++)
    {
        /*Store the CAN controller ID belonging to the network*/
        CanSM_ControllerId_u8 = CanSM_NetworkConf_ps->Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];
        /*Check if a TxConfirmation was received for the Controller*/
        if(CANIF_TX_RX_NOTIFICATION == CanIf_GetTxConfirmationState(CanSM_ControllerId_u8))
        {
            /*Update the return value as E_OK*/
            CanSM_FuncVal_uo = E_OK;

            break;
        }
    }
    return(CanSM_FuncVal_uo);

}

#endif

/**
***************************************************************************************************
** Function Name        : CanSM_BOR_CtrlsStart_SilentCom
**
** Service ID           :
**
** Description          : This api will start all the CAN controllers of the network during bus off recovery.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant (only for different network)
**
** Input Parameters     : network-The passed CanSM network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/


void CanSM_BOR_CtrlsStart_SilentCom(NetworkHandleType network)
{
        /*Temporary pointer to hold the adress of configured network*/
        const CanSM_NetworkConf_tst * CanSM_NetworkConf_ps;

        /*To hold the controller Id*/
        uint8 CanSM_ControllerId_u8;
        /*Used in FOR loop to access CAN controllers of the corresponding network*/
        uint8 CanSM_Ctrl_index_u8;
        /*Hold the counter number to know that all the controllers are stopped*/
        uint8 CanSM_Controller_Counter_u8 = 0;
        /*To keep a track of current BOR state */
        CanSM_BusOffRecoveryStateType_ten CanSM_CurrBORState_en;

        /*Hold the address of the configured network*/
        CanSM_NetworkConf_ps = &CanSM_Network_pcst[network];
        /* Get the Current BOR State only after the Mutex is accquired */
        CanSM_CurrBORState_en = CanSM_currBOR_State_en[network];
        /*If T_REPEAT_MAX counter has not exceeded*/
        if(CanSM_Num_Unsuccessful_ModeReq_au8[network] < CanSM_ModeRepeatReq_Max_u8)
        {
            switch(CanSM_CurrBORState_en)
            {
                case CANSM_S_RESTART_CC_WAIT:
                {
                    /*Check if a controller indication was received that all the CAN controllers are stopped*/
                    if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                    {
                        CanSM_BOR_SilentCom_Exit(network);
                    }
                    /*Check if timeout has occurred and so proceed accordingly*/
                    else
                    {
                        if(CanSM_TimerConfig_ast[network].cntTick_u16 >= CanSM_ModeRepeatReq_Time_u16)
                        {
                            /*Set the Timer to Elapsed Mode */
                            CanSM_TimerConfig_ast[network].stTimer = CANSM_TIMER_ELAPSED;

                            /*Move to previous substate ie CANSM_S_RESTART_CC*/
                            CanSM_currBOR_State_en[network]= CANSM_S_RESTART_CC;
                        }
                    }
                }
                break;
                case CANSM_S_RESTART_CC:
                {
                    /* Loop for All the Controllers in the network and Set them to STOPPED Mode*/
                    for(CanSM_Ctrl_index_u8 = 0; CanSM_Ctrl_index_u8 < CanSM_NetworkConf_ps->SizeofController_u8 ;CanSM_Ctrl_index_u8++)
                    {
                        /*Store the CAN controller ID belonging to the network*/
                        CanSM_ControllerId_u8 = CanSM_NetworkConf_ps->Cntrl_startidx_pu8[CanSM_Ctrl_index_u8];

                           /*If E_NOT_OK is returned for any of the controller, increment the T_REPEAT_MAX counter and break*/
                            if(E_NOT_OK == CanIf_SetControllerMode(CanSM_ControllerId_u8,CAN_CS_STARTED))
                                {
                                    /*If atleast one unsuccessful attempt to stop the controller occured for the corresponding network then increment the unsuccessful attempt*/
                                    CANSM_INCREMENT_UINT8_COUNTER(CanSM_Num_Unsuccessful_ModeReq_au8[network])
                                    break;
                                }
                            else
                                {
                                    /* increment the counter to indicate if all controller has returned E_OK */
                                    CanSM_Controller_Counter_u8++;
                                }
                    }

                    if(CanSM_Controller_Counter_u8 == CanSM_NetworkConf_ps->SizeofController_u8 )
                        {
                            /*Check if in the meantime, controller mode indication was received and update the substate if so*/
                            if(CanSM_Ctrl_ModeInd_ab[network] == TRUE)
                                {
                                    CanSM_BOR_SilentCom_Exit(network);
                                }
                            else
                                {   /*Move to next substate of the CANSM_S_RESTART_CC_WAIT */
                                    CanSM_currBOR_State_en[network] = CANSM_S_RESTART_CC_WAIT;

                                    /*Start the Wait state Timer */
                                    CanSM_StartTimer(network);
                                }
                        }
                }
                break;
                default:
                    /*do nothing*/
                break;
            }
        }

        else
            {
                    /*Report to DET that T_REPEAT_MAX counter has exceeded CanSM_ModeRepeatReq_Max_u8*/
                    CANSM_REPORT_RUNTIME_ERROR_NO_CONDITON(((uint8)CANSM_MAINFUNCTION_SID),(uint8)(CANSM_E_MODE_REQUEST_TIMEOUT))

            }

}
#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"
