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
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "Com_Prv.h"
#include "Com_Prv_Inl.h"


#if defined(COM_RX_DYNAMIC_SIGNAL_SUPPORT) || defined(COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT)
/*
 **********************************************************************************************************************
 * Defines/Macros
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Prototypes
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Constants
 **********************************************************************************************************************
*/


/*
 **********************************************************************************************************************
 * Functions
 **********************************************************************************************************************
*/

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_ReceiveDynSignal
 Description      : Service for receiving the signal, This API copies the data to the application buffer
 Parameter        : idSignal_u16            -> Id of the Signal
                  : signalDataPtr_pv        -> Pointer to the address where signal data needs to be copied.
                  : lengthPtr_pu16          -> Pointer to length of the recieved length.
 Return value     : E_OK/E_NOT_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
/* MR12 RULE 8.13 VIOLATION: A pointer parameter(SignalDataPtr) in a function prototype should be declared as pointer
   to const if the pointer is not used to modify the addressed object. But AUTOSAR SWS specification signature is
   as below, hence suppressed */
uint8 Com_ReceiveDynSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint16 * lengthPtr_pu16)
{

    uint8                       status_u8;        /* Return status */

    /* Returns with COM_SERVICE_NOT_AVAILABLE when IPDU Group is Stopped or
    * DET error occurs or when Type of the signal is not configured with UINT8_DYN */
    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveDynSignal, COM_E_UNINIT);
    }
    else if ((signalDataPtr_pv == NULL_PTR) || (lengthPtr_pu16 == NULL_PTR))
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveDynSignal, COM_E_PARAM_POINTER);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
# ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
    /* Check for dynamic signal */
    if (Com_Prv_IsValidRxSignalId(idSignal_u16))
    {
        status_u8 = Com_Prv_ReceiveDynSignal(idSignal_u16, signalDataPtr_pv, lengthPtr_pu16);
    }
    else
# endif
# ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
    /* Check for dynamic group signal */
    if (Com_Prv_IsValidRxGroupSignalId(idSignal_u16))
    {
        status_u8 = Com_Prv_ReceiveDynGroupSignal(idSignal_u16, signalDataPtr_pv, lengthPtr_pu16);
    }
    else
# endif
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReceiveDynSignal, COM_E_PARAM);
    }

    return(status_u8);
}


# ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ReceiveDynSignal
 Description      : Service for receiving the dynamic signal, This API copies the data to the application buffer
 Parameter        : idSignal_u16        -> Id of the Signal
                  : signalDataPtr_pv    -> Pointer to the address where signal data needs to be copied.
                  : lengthPtr_pu16      -> Pointer to length of the received length.
 Return value     : E_OK/E_NOT_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
/* MR12 RULE 8.13 VIOLATION: A pointer parameter(signalDataPtr_pv) in a function prototype should be declared as
* pointer to const if the pointer is not used to modify the addressed object. But AUTOSAR SWS specification signature
* is as below, hence suppressed */
uint8 Com_Prv_ReceiveDynSignal(Com_SignalIdType idSignal_u16, void* signalDataPtr_pv, uint16 * lengthPtr_pu16)
{

    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst; /* Local pointer holds the address of Rx signal
                                                                                               static configuration */
    uint16                      dynLength_u16;     /* Received Dynamic Signal Length  */
    Com_SigBuffIndex_tuo        idxSigBuff_uo;
    Com_IpduId_tuo              idIpdu_uo;
    uint8                       type_u8;          /* Local variable holds the signal type */
    uint8                       status_u8;        /* Return status */

    /* Initialize variable */
    status_u8  = COM_SERVICE_NOT_AVAILABLE;

    /* If PB variant is selected, then PduId which is passed to this function will be changed
    * to internal Id which is generated through configuration
    * If PC variant is selected, then no mapping table will be used. */
    idSignal_u16        = COM_GET_RXSIGNAL_ID(idSignal_u16);

    rxSigConstPtr_pcst  = COM_GET_RXSIG_CONSTDATA(idSignal_u16);
    idxSigBuff_uo       = rxSigConstPtr_pcst->idxSigBuff_uo;
    type_u8             = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

    idIpdu_uo = rxSigConstPtr_pcst->idComIPdu_uo;

    /* Check if Ipdu Group is started */
    if (Com_Prv_CheckRxIPduStatus((PduIdType)idIpdu_uo) && (COM_UINT8_DYN == type_u8))
    {
        /*Copy each byte into the SignalDataPtr from Signal buffer  */
        /* The locks inside the function Com_ByteCopy() are removed.
        * Hence locks are used here
        */
        SchM_Enter_Com_RxSigDynBuff();

        /* Retrieve dynamic signal length */
        dynLength_u16 = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->dynSigLength_u16;

        /* If the requested length is less than the received length, then return E_NOT_OK */
        if (*lengthPtr_pu16 >= dynLength_u16)
        {
            *lengthPtr_pu16 = dynLength_u16;

            if (COM_ZERO != dynLength_u16)
            {
                Com_MainFunc_tuo idRxMainFunc_uo;

                /* Fetch the Rx-MainFunction internal Id */
                idRxMainFunc_uo = (COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo))->idMainFunc_uo;

                /* MR12 RULE 11.5 VIOLATION: For SignalDataPtr is defined as void pointer in the AUTOSAR
                * specification, so suppressing warning "Cast from a pointer to void to a pointer to object" */
                Com_ByteCopy((uint8*)signalDataPtr_pv,
                                &COM_GET_SIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,idxSigBuff_uo),dynLength_u16);

            }

            /* Positive condition hence status_u8 as E_OK */
            status_u8 = E_OK;
        }
        else
        {
            status_u8 = E_NOT_OK;
        }

        SchM_Exit_Com_RxSigDynBuff();
    }
    return(status_u8);
}

# endif /* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */


# ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ReceiveDynGroupSignal
 Description      : Service for receiving the dynamic group signal, This API copies the data to the application buffer
 Parameter        : idSignal_u16        -> Id of the Signal
                  : signalDataPtr_pv    -> Pointer to the address where group signal data needs to be copied.
                  : lengthPtr_pu16      -> Pointer to length of the received length.
 Return value     : E_OK/E_NOT_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
/* MR12 RULE 8.13 VIOLATION: A pointer parameter(signalDataPtr_pv) in a function prototype should be declared as
* pointer to const if the pointer is not used to modify the addressed object. But AUTOSAR SWS specification signature
* is as below, hence suppressed */
uint8 Com_Prv_ReceiveDynGroupSignal(Com_SignalIdType idSignal_u16, void * signalDataPtr_pv, uint16 * lengthPtr_pu16)
{

    /* Local pointer holds the address of Rx group signal static configuration */
    Com_RxGrpSigCfg_tpcst           rxGrpSigConstPtr_pcst;
    uint16                          dynLength_u16;     /* Received Dynamic Signal Length  */
    Com_IpduId_tuo                  idIpdu_uo;
    Com_RxGrpSigBuffIndex_tuo       idxRxGrpSignalBuff_uo;
    uint8                           type_u8;          /* Local variable holds the signal type */
    uint8                           status_u8;        /* Return status */

    /* Initialize variable */
    status_u8               = COM_SERVICE_NOT_AVAILABLE;

    /* If PB variant is selected, then PduId which is passed to this function will be changed
    * to internal Id which is generated through configuration
    * If PC variant is selected, then no mapping table will be used. */
    idSignal_u16            = COM_GET_RXGRPSIGNAL_ID(idSignal_u16);
    rxGrpSigConstPtr_pcst   = COM_GET_RXGRPSIG_CONSTDATA(idSignal_u16);
    idxRxGrpSignalBuff_uo   = rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo;
    idIpdu_uo               = COM_GET_RXSIGGRP_CONSTDATA(rxGrpSigConstPtr_pcst->idSigGrp_uo)->idComIPdu_uo;
    type_u8                 = Com_GetValue(RXGRPSIG,_TYPE,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

    /* Check if Ipdu Group is started */
    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
    * to store and read here, may read obsolete value. Evaluation(or no evaluation) of 2nd operand
    * shall not impact the system behaviour. Hence suppressed. */
    if (COM_UINT8_DYN == type_u8)
    {
        /*Copy each byte into the SignalDataPtr from Signal buffer  */
        /* The locks inside the function Com_ByteCopy() are removed.
        * Hence locks are used here
        */
        SchM_Enter_Com_RxGrpSigDynBuff();

        /* Retrieve dynamic group signal length */
        dynLength_u16 = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->dynRxGrpSigLength_u16;

        /* If the requested length is less than the received length, then return E_NOT_OK */
        if (*lengthPtr_pu16 >= dynLength_u16)
        {
            *lengthPtr_pu16 = dynLength_u16;

            if (COM_ZERO != dynLength_u16)
            {
                Com_MainFunc_tuo idRxMainFunc_uo;

                /* Fetch the Rx-MainFunction internal Id */
                idRxMainFunc_uo = (COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo))->idMainFunc_uo;

                /* MR12 RULE 11.5 VIOLATION: For signalDataPtr_pv is defined as void pointer in the AUTOSAR
                * specification, so suppressing warning "Cast from a pointer to void to a pointer to object" */
                Com_ByteCopy((uint8*)signalDataPtr_pv,
                              &COM_GET_RXGRPSIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,idxRxGrpSignalBuff_uo),dynLength_u16);

            }

            /* Positive condition hence status as E_OK */
            status_u8 = E_OK;
        }
        else
        {
            status_u8 = E_NOT_OK;
        }

        SchM_Exit_Com_RxGrpSigDynBuff();
    }
    return(status_u8);
}

# endif /* #ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #if defined(COM_RX_DYNAMIC_SIGNAL_SUPPORT) || defined(COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT) */

