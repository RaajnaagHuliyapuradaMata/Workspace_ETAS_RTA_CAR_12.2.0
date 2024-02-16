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
#ifdef COM_TxInvalid
LOCAL_INLINE uint8 Com_Prv_InternalInvalidateSignal(Com_SignalIdType idSignal_u16);
#endif

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
 Function name    : Com_InvalidateSignal
 Description      : Service for Invalidating the signal
 Parameter        : idSignal_u16 -> Id of the signal
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
uint8 Com_InvalidateSignal(Com_SignalIdType idSignal_u16)
{
#if defined(COM_TxInvalid) || defined(COM_TxGrpSigInvalid)
    uint8                       status_u8;

    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_InvalidateSignal, COM_E_UNINIT);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
# ifdef COM_TxInvalid
    if (Com_Prv_IsValidTxSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16    = COM_GET_TXSIGNAL_ID(idSignal_u16);

        status_u8       = Com_Prv_InternalInvalidateSignal(idSignal_u16);
    }
    else
# endif
# ifdef COM_TxGrpSigInvalid
    if (Com_Prv_IsValidTxGroupSignalId(idSignal_u16))
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idSignal_u16    = COM_GET_TXGRPSIGNAL_ID(idSignal_u16);

        status_u8       = Com_Prv_InternalInvalidateShadowSignal(idSignal_u16);
    }
    else
# endif
    {
        COM_DET_REPORT_ERROR(COMServiceId_InvalidateSignal, COM_E_PARAM);
    }

    return (status_u8);
#else
    (void)idSignal_u16;
    return( COM_SERVICE_NOT_AVAILABLE );
#endif /* #if defined(COM_TxInvalid) || defined(COM_TxGrpSigInvalid) */
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_InternalInvalidateSignal
 Description      : Service for Invalidating the signal
 Parameter        : idSignal_u16 -> Id of the signal
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
#ifdef COM_TxInvalid

LOCAL_INLINE uint8 Com_Prv_InternalInvalidateSignal(Com_SignalIdType idSignal_u16)
{
    Com_TxSigCfg_tpcst          txSigConstPtr_pcst;
#ifdef COM_TX_TP_IPDUTYPE
    Com_IpduId_tuo              idIpdu_uo;
#endif
    uint8                       status_u8;

    /* TRACE[SWS_Com_00643] Com_InvalidateSignal shall return COM_SERVICE_NOT_AVAILABLE
    * in case no ComSignalDataInvalidValue is configured for the signal with the given SignalId. */
    status_u8           = COM_SERVICE_NOT_AVAILABLE;

    txSigConstPtr_pcst  = COM_GET_TXSIG_CONSTDATA(idSignal_u16);

#ifdef COM_TX_TP_IPDUTYPE
    idIpdu_uo           = txSigConstPtr_pcst->idComIPdu_uo;
#endif

    /* The AUTOSAR COM module shall only process Com_InvalidateSignal only if a ComSignalDataInvalidValue is
    * configured for the signal with the given SignalId.
    * Copy the Invalid value to all the Bytes of this signal in Ipdu buffer */
    if (Com_GetValue(TXSIG,_INVACTION,txSigConstPtr_pcst->txSignalFields_u16))
    {
#ifdef COM_TX_TP_IPDUTYPE
        /* Proceed only if Large Data tranmission is not in Progress */
        if (Com_GetRamValue(TXIPDU,_LARGEDATAINPROG,COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->txFlags_u16))
        {
            /* The TxIPdu transmission of large Data IPdu is in progress, hence no signal update is allowed until
            * the transmission is completed. */
            status_u8 = COM_BUSY;
        }
        else
#endif /*#ifdef COM_TX_TP_IPDUTYPE*/
        {
            uint32         txSigInvValue_u32;
            uint8          type_u8;

            type_u8 = Com_GetValue(GEN,_TYPE,txSigConstPtr_pcst->general_u8);

            txSigInvValue_u32 = txSigConstPtr_pcst->dataInvalidVal_u32;

#ifdef COM_TX_DYNAMIC_SIGNAL_SUPPORT
            if (type_u8 == COM_UINT8_DYN)
            {
                /* MR12 DIR 1.1 VIOLATION: Explicit cast is provided in line with Com_Prv_SendDynSignal()
                * definition. */
                status_u8 = Com_Prv_SendDynSignal( idSignal_u16,
                                        (const void *)COM_GET_BYTE_ARRAY_SIG_VAL(txSigInvValue_u32).sigByteArray_pau8,
                                        COM_GET_BYTE_ARRAY_SIG_VAL(txSigInvValue_u32).sigLength_uo );

            }
            else
#endif /* end of COM_TX_DYNAMIC_SIGNAL_SUPPORT */
            {
                const void * signalDataPtr_pcv;
                uint16 txSigInvValue_u16;
                uint8 txSigInvValue_u8;
                boolean txSigInvValue_b;

                switch (type_u8)
                {
                    /* MR12 DIR 1.1 VIOLATION: "Implicit conversion from a pointer to object type to a pointer to void."
                    *  warning can be suppressed as the SignalDataPtr will be dereferenced with the same datatype as
                    *  used in Com_Prv_InternalSendSignal  */
                    case COM_UINT8:
                    case COM_SINT8:
                    {
                        txSigInvValue_u8 = (uint8)txSigInvValue_u32;
                        signalDataPtr_pcv = (const void *)&txSigInvValue_u8;
                    }
                    break;
                    case COM_UINT16:
                    case COM_SINT16:
                    {
                        txSigInvValue_u16 = (uint16)txSigInvValue_u32;
                        signalDataPtr_pcv = (const void *)&txSigInvValue_u16;
                    }
                    break;
                    case COM_UINT32:
                    case COM_SINT32:
#ifdef COM_FLOAT32SUPP
                    case COM_FLOAT32:
#endif /* #ifdef COM_FLOAT32SUPP */
                    {
                        signalDataPtr_pcv = (const void *)&txSigInvValue_u32;
                    }
                    break;
                    case COM_BOOLEAN:
                    {
                        txSigInvValue_b = (boolean)COM_UNSIGNED_TO_BOOL(txSigInvValue_u32);
                        signalDataPtr_pcv = (const void *)&txSigInvValue_b;
                    }
                    break;
#ifdef COM_TXSIG_INT64
                    case COM_UINT64:
                    case COM_SINT64:
                    {
                        signalDataPtr_pcv = &COM_GET_INT64_SIG_VALUE(txSigInvValue_u32);
                    }
                    break;
#endif /*end of #ifdef COM_TXSIG_INT64*/
#ifdef COM_TXSIG_FLOAT64SUPP
                    case COM_FLOAT64:
                    {
                        signalDataPtr_pcv = &Com_getfloat64SigValue(txSigInvValue_u32);
                    }
                    break;
#endif /* #ifdef COM_TXSIG_FLOAT64SUPP */
                    case COM_UINT8_N:
                    {
                        signalDataPtr_pcv = COM_GET_BYTE_ARRAY_SIG_VAL(txSigInvValue_u32).sigByteArray_pau8;
                    }
                    break;

                    default:
                    {
                        /* undefined type */
                        signalDataPtr_pcv = (const void *)NULL_PTR;
                    }
                    break;
                }

                if (signalDataPtr_pcv != NULL_PTR)
                {
                    /* After invaliding the signal data, this function shall perform a call of Com_SendSignal
                    * internally. But as per design, Com_Prv_InternalSendSignal() is called with the signal Id and pass
                    * Address of Data. Checking if the IpduGroup is started is checked in Com_Prv_InternalSendSignal()
                    * so no need of checking it here. */
                    status_u8 = Com_Prv_InternalSendSignal( idSignal_u16, signalDataPtr_pcv );
                }
            }
        }
    }
    else
    {
        /* Return COM_SERVICE_NOT_AVAILABLE if ComSignalDataInvalidValue is not configured with the given SignalId*/
    }

    /* Note: if IPdu Group is stopped, then the Com_InternalSendSignal( ) will return COM_SERVICE_NOT_AVAILABLE */
    return (status_u8);
}
#endif /* end of COM_TxInvalid */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

