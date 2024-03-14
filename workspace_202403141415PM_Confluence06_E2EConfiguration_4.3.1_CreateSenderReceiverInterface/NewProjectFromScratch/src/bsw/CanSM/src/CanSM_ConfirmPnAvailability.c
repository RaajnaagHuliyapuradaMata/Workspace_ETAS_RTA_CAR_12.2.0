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

/***************************************************************************************************
** Function Name        : CanSM_ConfirmPnAvailability
**
** Service ID           : 0x06
**
** Description          : This callback function indicates that the transceiver is running in PN communication mode.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Reentrant for different CAN Transceivers
**
** Input Parameters     : TransceiverId - CAN transceiver, which was checked for PN availability
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : None

***************************************************************************************************/

#if(CANSM_PN_SUPPORT_CONFIGD == STD_ON)
#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"
void CanSM_ConfirmPnAvailability(uint8 TransceiverId)
{

    /*Counter variable*/
	uint8_least network_indx_uo;
    #if (CANSM_CFG_GLOBAL_PN_NM_SUPPORT == STD_ON)
    uint8 CanSM_networkHandle_u8;
    #endif

    #if (CANSM_DEV_ERROR_DETECT != STD_OFF)
    boolean CanSM_TrcvConfigd_b;
	CanSM_NetworkModeStateType_ten CurrNwMode_Temp_uo;
    /*Initialise the flag used to keep track if there is TransceiverId is valid one*/
    CanSM_TrcvConfigd_b = FALSE;
    #endif

	/*Notify DET if CanSM is not initialized.*/
    CANSM_REPORT_ERROR((CanSM_Init_ab == (boolean)CANSM_UNINITED),((uint8)CANSM_CONFIRMPNAVAILABILITY_SID),((uint8)CANSM_E_UNINIT))

   /* Loop through the networks to find the corresponding network for the 'Transceiver' */
	for(network_indx_uo =0; network_indx_uo < CanSM_ConfigSet_pcst->CanSM_SizeOfCanSMNetworks_u8 ; network_indx_uo++)
    {
        #if CANSM_DEV_ERROR_DETECT != STD_OFF
	    CurrNwMode_Temp_uo = CanSM_CurrNw_Mode_en[network_indx_uo];
        #endif
		/*If the input 'TransceiverId' matches the transceiver's id of a network*/
        if(CanSM_Network_pcst[network_indx_uo].Trcv_hndle_u8 == TransceiverId)
        {
            /*Notify DET if CanSM is not initialized.
             * Cannot be checked for CanSM_stInit_a[] since in case of PRE_NOCOM, the network is
             * still considered to be uninitialised but mode indications are required to proceed*/
            CANSM_REPORT_ERROR((CANSM_BSM_S_NOT_INITIALIZED == CurrNwMode_Temp_uo),
                               ((uint8)CANSM_CONFIRMPNAVAILABILITY_SID),
                               ((uint8)CANSM_E_UNINIT)
                               )

            #if CANSM_DEV_ERROR_DETECT != STD_OFF
            CanSM_TrcvConfigd_b = TRUE;
            #endif

            #if (CANSM_CFG_GLOBAL_PN_NM_SUPPORT == STD_ON)
            /*Get the network handle*/
            CanSM_networkHandle_u8 = CanSM_Network_pcst[network_indx_uo].ComM_channelId_uo;
            /*Inform CanNm about PN availability*/
            CanNm_ConfirmPnAvailability((NetworkHandleType)CanSM_networkHandle_u8);
            #endif
            break;
        }

    }

        /*Report to DET for invalid transceiver Id*/
        CANSM_REPORT_ERROR(CanSM_TrcvConfigd_b == FALSE,(uint8)CANSM_CONFIRMPNAVAILABILITY_SID, (uint8)CANSM_E_PARAM_TRANSCEIVER)

}
#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"
#endif
