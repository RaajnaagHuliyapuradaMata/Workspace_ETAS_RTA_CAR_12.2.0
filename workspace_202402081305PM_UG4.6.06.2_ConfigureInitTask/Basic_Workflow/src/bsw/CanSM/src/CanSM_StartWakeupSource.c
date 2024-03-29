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
** Function Name        : CanSM_StartWakeupSource
**
** Service ID           : 0x11
**
** Description          : This function shall be called by EcuM when a wakeup source shall be started.
**
** Sync/Async           : Synchronous
**
** Re-entrancy          : Non Reentrant
**
** Input Parameters     : network
**
** InOut parameter      : None
**
** Output Parameters    : None
**
** Return parameter     : Std_ReturnType

***************************************************************************************************
**/

#define CANSM_START_SEC_CODE
#include "CanSM_MemMap.h"
Std_ReturnType CanSM_StartWakeupSource(NetworkHandleType network)
{
	/*To keep a track of return value*/
	Std_ReturnType stFuncVal;
	/*To keep a track of current network mode*/
    CanSM_NetworkModeStateType_ten CanSM_CurrNwMode_en;
	/*To keep a track of current BOR state*/
    CanSM_BusOffRecoveryStateType_ten CanSM_CurrBORMode_en;

    stFuncVal = E_NOT_OK;

	/*Notify DET if the CanSM module is not initialized and return E_NOT_OK*/
    CANSM_REPORT_ERROR_NOK((CanSM_Init_ab == (boolean)CANSM_UNINITED),
                           ((uint8)CANSM_STARTWAKEUPSOURCE_SID),
                           ((uint8)CANSM_E_UNINIT))

	/* Get the CanSM network Index value corresponding to the ComM channel Id received */
    network = CanSM_GetHandle(network);

	/* Notify DET for Invalid network handle.*/
    CANSM_REPORT_ERROR_NOK(((uint8)network >= (CanSM_ConfigSet_pcst->CanSM_SizeOfCanSMNetworks_u8)),
                           (uint8)CANSM_STARTWAKEUPSOURCE_SID,
                           (uint8)CANSM_E_INVALID_NETWORK_HANDLE)

    /*Make a local copy of current network mode and current BOR state for the network*/
    CanSM_CurrNwMode_en = CanSM_CurrNw_Mode_en[network];
    CanSM_CurrBORMode_en = CanSM_currBOR_State_en[network];
	/*Check if the network is initialised and current network is CANSM_BSM_S_PRE_NOCOM or CANSM_BSM_S_NOCOM and current BOR state is CANSM_BOR_IDLE or CANSM_S_NO_BUS_OFF */
	if((CanSM_Network_Init_ab[network] == TRUE) && ((CanSM_CurrNwMode_en == CANSM_BSM_S_PRE_NOCOM) || (CanSM_CurrNwMode_en == CANSM_BSM_S_NOCOM)) && ((CanSM_CurrBORMode_en == CANSM_BOR_IDLE) || (CanSM_CurrBORMode_en == CANSM_S_NO_BUS_OFF)))
		{
			/*The trigger for WUValidation start*/
			CanSM_Wuvalidation_Start_ab[network] = TRUE;

			/*Update the current network mode to CANSM_BSM_WUVALIDATION*/
			CanSM_CurrNw_Mode_en[network]= CANSM_BSM_WUVALIDATION;

			/*Update the array state to CANSM_WAKEUP_VALIDATION_S_TRCV_NORMAL */
			CanSM_WakeUpValidation_Substates_en[network] = CANSM_WAKEUP_VALIDATION_S_TRCV_NORMAL;

			stFuncVal = E_OK;
		}

    return stFuncVal;
}
#define CANSM_STOP_SEC_CODE
#include "CanSM_MemMap.h"
