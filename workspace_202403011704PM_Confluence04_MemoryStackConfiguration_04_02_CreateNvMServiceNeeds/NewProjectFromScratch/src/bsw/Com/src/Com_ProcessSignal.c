/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2018, all rights reserved
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
#ifdef COM_RxSigUpdateBit
LOCAL_INLINE boolean Com_Prv_ProcessSigUpdateBit(
                                        Com_SignalIdType    idSignal_u16,
                                        const PduInfoType * pduInfoPtr_pcst
                                                );
#endif

LOCAL_INLINE boolean Com_Prv_IsCompleteSigReceived(
                                        Com_SignalIdType    idSignal_u16,
                                        PduLengthType       rxSduLength_uo,
                                        Com_Bitsize_tuo   * size_puo
                                                  );

LOCAL_INLINE boolean Com_Prv_IsRxSigDataCopied(
                                        Com_SignalIdType    idSignal_u16,
                                        const PduInfoType * pduInfoPtr_pcst,
                                        Com_Bitsize_tuo     size_uo
                                         );

#ifdef COM_RxSigInvalid
LOCAL_INLINE boolean Com_Prv_IsInvalidSigData(Com_SignalIdType idSignal_u16, void * rxSrcPtr_pv);
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
 Function name    : Com_Prv_ProcessSignal
 Description      : Process rx-signals of the received I-PDU.
 Parameter        : idRxPdu_uo      - ID of the received I-PDU.
                  : pduInfoPtr_pcst - Contains the length (SduLength) of the received I-PDU and
                                 a pointer to a buffer (SduDataPtr) containing the I-PDU.
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_ProcessSignal(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
    uint16_least                idxIdSig_qu16;
    uint16_least                idMaxRxSig_qu16;
#ifdef COM_RxSigUpdateTimeout
    uint16_least                idxSigTout_qu16;
    uint8                       ipduUpdateBitStatus_u8;
    uint8                       sigUpdateBitStatus_u8;
#endif
    boolean                     notifySignal_b;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);

#ifdef COM_RxSigUpdateTimeout
    /* If this IPDU has signals with update-bits , initialise the pointers
    * Separate deadline monitoring should be performed for signals with update-bit
    * Deadline monitoring should not be done if it is disabled
    */
    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
    * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
    * system behaviour. Hence suppressed. */
    if ((rxIpduConstPtr_pcst->signalTimeoutRef_pcst != NULL_PTR) && (COM_CheckRxIPduDMStatus(idRxPdu_uo)))
    {

        /* Update the variable UpdateBitStatus with value COM_UPDATEBIT_APPLICABLE
        * ie at least one of the signal within the IPDU is configured with both update-bit and deadline monitoring.
        */
        ipduUpdateBitStatus_u8 = COM_UPDATEBIT_APPLICABLE;
    }
    else
    {
        ipduUpdateBitStatus_u8 = COM_UPDATEBIT_NOT_APPLICABLE;
    }

    /* Initialize the variable to be used to refer the first signal timeout configuration */
    idxSigTout_qu16 = COM_ZERO;
#endif /* #ifdef COM_RxSigUpdateTimeout */

    idxIdSig_qu16   = rxIpduConstPtr_pcst->idRxSig_uo;
    idMaxRxSig_qu16  = idxIdSig_qu16 + rxIpduConstPtr_pcst->numOfSig_u16;

    rxSigConstPtr_pcst   = COM_GET_RXSIG_CONSTDATA(idxIdSig_qu16);

    for ( ; idxIdSig_qu16 < idMaxRxSig_qu16; idxIdSig_qu16++ )
    {
        notifySignal_b = COM_FALSE;

#ifdef COM_RxSigUpdateTimeout
        sigUpdateBitStatus_u8 = ipduUpdateBitStatus_u8;
#endif

#ifdef COM_RxSigUpdateBit
        /* Returns false if signal UpdateBit is configured and is received with reset state,
        * otherwise true is returned */
        if (Com_Prv_ProcessSigUpdateBit( (Com_SignalIdType)idxIdSig_qu16, pduInfoPtr_pcst ))
#endif /* #ifdef COM_RxSigUpdateBit */
        {
            Com_Bitsize_tuo size_uo;

            size_uo = 0u;

            /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
            * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
            * system behaviour. Hence suppressed. */
            if (Com_Prv_IsCompleteSigReceived((Com_SignalIdType)idxIdSig_qu16, pduInfoPtr_pcst->SduLength, &size_uo) &&
                Com_Prv_IsRxSigDataCopied((Com_SignalIdType)idxIdSig_qu16, pduInfoPtr_pcst, size_uo))
            {
                notifySignal_b = COM_TRUE;

#ifdef COM_RxSigUpdateTimeout
                /* If sigUpdateBitStatus_u8 is
                * COM_UPDATEBIT_NOT_APPLICABLE(0x0), then it remains unchanged.
                * COM_UPDATEBIT_APPLICABLE(0x1), then it is changed to COM_UPDATEBIT_SET(0x2) here. */
                sigUpdateBitStatus_u8 *= COM_UPDATEBIT_SET;
#endif
            }

        } /* End of Com_Prv_ProcessSigUpdateBit() check */

#ifdef COM_RxSigUpdateTimeout
        /* If update-bit and timeout is configured */
        if (sigUpdateBitStatus_u8 != COM_UPDATEBIT_NOT_APPLICABLE)
        {
            Com_Prv_LoadSigTimeoutValue( (Com_SignalIdType)idxIdSig_qu16, sigUpdateBitStatus_u8, &idxSigTout_qu16 );
        }
#endif /* #ifdef COM_RxSigUpdateTimeout */

#ifdef COM_RxSignalNotify
        /* Set the _SIGNOTIF flag to invoke configured signal-ComNotification */
        Com_SetRamValue(RXSIG,_SIGNOTIF,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8),notifySignal_b);
#ifdef COM_PRV_ENABLECONFIGINTERFACES
        Com_SetRamValue(RXSIGCFG,_SIGNOTIF,
                        *(Com_GlobalConfig_pcst->rxSigCfg_pcst[idxIdSig_qu16].rxSigRAMFields_pu8),notifySignal_b);
#endif /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */
#endif
        /* return value from CopyRxSignal() is not used, if Signal notifications are not configured */
        /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
         * they are intentionally not placed under compiler switch statement,
         * so that these lines are always available for compilation*/
        (void)(notifySignal_b);

        rxSigConstPtr_pcst++;

    } /* End of for loop */
}


#ifdef COM_RxSigUpdateBit
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ProcessSigUpdateBit
 Description      : Private function to validate signal UpdateBit
 Parameter        : idSignal_u16        - ID of rx-signal.
                  : pduInfoPtr_pcst     - Contains the length (SduLength) of the received I-PDU and
                                  a pointer to a buffer (SduDataPtr) containing the I-PDU.
 Return value     : COM_FALSE   - UpdateBit is configured & received with reset state.
                    COM_TRUE    - updatebit is set(other cases).
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean Com_Prv_ProcessSigUpdateBit(
                                            Com_SignalIdType    idSignal_u16,
                                            const PduInfoType * pduInfoPtr_pcst
                                                )
{
    Com_RxSigCfg_tpcst               rxSigConstPtr_pcst;
    boolean                          isUpdateBitSet_b;

    /* Initialize update bit to true */
    isUpdateBitSet_b = COM_TRUE;

    rxSigConstPtr_pcst  = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    /* Check for the update bit if configured */
    if (Com_GetValue(GEN,_UPDBITCONF,rxSigConstPtr_pcst->general_u8))
    {
        PduLengthType       updateBitByteNo_uo;

        updateBitByteNo_uo     = (PduLengthType)(rxSigConstPtr_pcst->updateBitPos_uo >> 3u);

        /* Start: Check if the Update bit is SET or NOT */
        if ((updateBitByteNo_uo + COM_ONE) <= pduInfoPtr_pcst->SduLength)
        {
            /* TRACE[SWS_Com_00802] If a signal or signal group with an attached update bit is received and
            * the update-bit is not set, the AUTOSAR COM module shall discard this signal.
            * If the signal has not been updated, the signal will not be routed via the signal gateway.
            * It will only be discarded.
            * If the RTE reads a signal with an associated cleared update-bit, the init value or
            * the last received value is returned. */
            isUpdateBitSet_b  = Com_GetUpdateBitValue(pduInfoPtr_pcst->SduDataPtr,
                                                      rxSigConstPtr_pcst->updateBitPos_uo,
                                                      updateBitByteNo_uo);
        }
        else
        {
            /* TRACE[SWS_Com_00794] In case of receiving of a smaller I-PDU than expected results into receiving a
            * signal without its configured update-bit, the AUTOSAR COM module shall treat this signal as if
            * its update-bit was set and interpret such a signal as updated.(SRS_Com_02046) */
        }
    } /* End of UpdateBit check */

    return isUpdateBitSet_b;
}
#endif /* #ifdef COM_RxSigUpdateBit */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsCompleteSigReceived
 Description      : Check if complete rx signal is received
 Parameter        : idSignal_u16      - ID of rx-signal.
                  : rxSduLength_uo - Contains the length (SduLength) of the received I-PDU
                    size_puo - received signal size
 Return value     : true/false, if complete signal is received or not respectively
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean Com_Prv_IsCompleteSigReceived(
                                        Com_SignalIdType  idSignal_u16,
                                        PduLengthType     rxSduLength_uo,
                                        Com_Bitsize_tuo * size_puo
                                                  )
{
    Com_RxSigCfg_tpcst  rxSigConstPtr_pcst;
    uint8               constByteValue_u8;
    uint8               sigType_u8;
    boolean             isCompleteSigReceived_b;

    isCompleteSigReceived_b = COM_TRUE;

    rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    constByteValue_u8 = rxSigConstPtr_pcst->general_u8;

    sigType_u8 = Com_GetValue(GEN,_TYPE,constByteValue_u8);

#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT

    if (sigType_u8 == COM_UINT8_DYN)
    {
        PduLengthType startByteNo_uo;

        /* Calculate Dynamic Signal Start Position */
        startByteNo_uo = (PduLengthType)(rxSigConstPtr_pcst->bitPos_uo >> 3u);

        /* Check if Dynamic Signal is received with 0 byte or more than that */
        if (rxSduLength_uo >= startByteNo_uo)
        {
            /* calculate received dynamic signal length */
            *size_puo = (Com_Bitsize_tuo)(rxSduLength_uo - startByteNo_uo);

            /* check if received dynamic signal length exceeds the configured length */
            if ((*size_puo) > rxSigConstPtr_pcst->bitSize_uo)
            {
                /* if so, consider configured dynamic signal length */
                *size_puo = rxSigConstPtr_pcst->bitSize_uo;
            }
            else
            {
                /* if not, consider partial received dynamic signal length */
            }
        }
        else
        {
            /* [DesignDecision] if (length >= 0) bytes are not received for a dynamic signal in RxIPdu,
            * consider signal as not received and ignore further processing */
            isCompleteSigReceived_b = COM_FALSE;

            *size_puo = 0u;
        }
    }
    else
#endif /* #ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT */
    {
        PduLengthType sigLastByteNo_uo;

        *size_puo = rxSigConstPtr_pcst->bitSize_uo;

        /* Endian behavior: calculate the last byte the signal span in the Rx-IPdu buffer
        * LITTLE/OPAQUE : MSB > LSB; i.e, signal last byte is same as the byte where MSB is configured.
        * BIG           : LSB > MSB; i.e, signal last byte is same as the byte where LSB is configured.
        */
        if (Com_GetValue(GEN,_ENDIANESS,constByteValue_u8) != COM_BIGENDIAN)
        {
            Com_Bitposition_tuo lastBitPosition_uo;

            /* Currently COM_OPAQUE is same as COM_LITTLEENDIAN and
            * its SignalLength in bytes is stored in BitSize */
            lastBitPosition_uo = (rxSigConstPtr_pcst->bitPos_uo +
                              ((sigType_u8 == COM_UINT8_N) ? ((Com_Bitposition_tuo)*size_puo * 8u) : *size_puo));

            sigLastByteNo_uo   = (PduLengthType)((lastBitPosition_uo != COM_ZERO) ?
                                 ((lastBitPosition_uo - COM_ONE) >> 3u) : (COM_ZERO));
        }
        else
        {
            /* COM_BIGENDIAN */
            sigLastByteNo_uo   = (PduLengthType)(rxSigConstPtr_pcst->bitPos_uo >> 3u);
        }

        /* TRACE[SWS_Com_00574] When unpacking an I-PDU, the AUTOSAR COM module shall check the
        * received data length(PduInfoPtr->SduLength) and
        * unpack and notify only completely received signals via ComNotification.
        * Check if the Signal is part of the received rx pdu length */
        if ((sigLastByteNo_uo + COM_ONE) > rxSduLength_uo)
        {
            /* Signal is not received or received partially, Ignore further processing */
            isCompleteSigReceived_b = COM_FALSE;
        }
    }

    return isCompleteSigReceived_b;
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsRxSigDataCopied
 Description      : Private function to unpack and copy rx-signal
 Parameter        : idSignal_u16   - rx-signal to be processed
                    pduInfoPtr_pcst - Contains the length (SduLength) of the received I-PDU and
                                 a pointer to a buffer (SduDataPtr) containing the I-PDU.
                    size_uo       - received signal length
 Return value     : true  - signal is copied into buffer
                    false - signal is discarded
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean Com_Prv_IsRxSigDataCopied(
                                    Com_SignalIdType    idSignal_u16,
                                    const PduInfoType * pduInfoPtr_pcst,
                                    Com_Bitsize_tuo     size_uo
                                              )
{
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
    Com_SigBuffIndex_tuo        idxSigBuff_uo;
    Com_IpduId_tuo              idRxPdu_uo;
    Com_MainFunc_tuo            idRxMainFunc_uo;
    uint8                       constByteValue_u8;
    uint8                       sigType_u8;
    uint8                       sigEndianess_u8;
    boolean                     isRxSigDataCopied_b;

    isRxSigDataCopied_b     = COM_FALSE;

    rxSigConstPtr_pcst      = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    idRxPdu_uo              = rxSigConstPtr_pcst->idComIPdu_uo;
    rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
    idRxMainFunc_uo         = rxIpduConstPtr_pcst->idMainFunc_uo; /* Fetch the Rx-MainFunction internal Id */

    constByteValue_u8       = rxSigConstPtr_pcst->general_u8;

    sigType_u8              = Com_GetValue(GEN,_TYPE,constByteValue_u8);
    sigEndianess_u8         = Com_GetValue(GEN,_ENDIANESS,constByteValue_u8);

    idxSigBuff_uo = rxSigConstPtr_pcst->idxSigBuff_uo;

    /* NOTE on Exclusive area COM_RXINDICATION:
    * If there is a possibility that Com_RxIndication() can be interrupted by
    * Com_ReceiveSignal() or Com_Init() function for any IPdu,
    * then this exclusive area needs to be defined in Com_Cfg_SchM.h file.
    */
    /* Copy the new value of the signal into signal buffer */

    switch (sigType_u8)
    {
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
            Com_ByteArraySig_tst byteArraySig_st;

            byteArraySig_st.sigByteArray_pau8 = (pduInfoPtr_pcst->SduDataPtr + (rxSigConstPtr_pcst->bitPos_uo >> 3u));
            byteArraySig_st.sigLength_uo = size_uo;

# if defined(COM_RxSigInvalid) || defined(COM_RxFilters)
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". rxSrcPtr_pv parameter
            * is always deferenced with correct type, so, there will not be any impact. Hence, it is suppressed. */
            if (Com_Prv_ValidateRxSignal( idSignal_u16, (void *)&byteArraySig_st ))
# endif
            {

                Com_Get_SchM_Enter_Com_RxSigDynBuff

                Com_ByteCopy( &COM_GET_SIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,idxSigBuff_uo),
                        byteArraySig_st.sigByteArray_pau8, byteArraySig_st.sigLength_uo );

                COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo)->ipduRam_pst->dynSigLength_u16 = byteArraySig_st.sigLength_uo;

                Com_Get_SchM_Exit_Com_RxSigDynBuff

                isRxSigDataCopied_b = COM_TRUE;
            }
        }
        break;
#endif /* end of COM_RX_DYNAMIC_SIGNAL_SUPPORT */
        case COM_UINT8_N:
        {
            Com_ByteArraySig_tst byteArraySig_st;

            byteArraySig_st.sigByteArray_pau8 = (pduInfoPtr_pcst->SduDataPtr + (rxSigConstPtr_pcst->bitPos_uo >> 3u));
            byteArraySig_st.sigLength_uo = size_uo;

#if defined(COM_RxSigInvalid) || defined(COM_RxFilters)
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". rxSrcPtr_pv parameter
            * is always deferenced with correct type, so, there will not be any impact. Hence, it is suppressed. */
            if (Com_Prv_ValidateRxSignal( idSignal_u16, (void *)&byteArraySig_st ))
#endif
            {

                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                Com_ByteCopy( &COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,idxSigBuff_uo),
                        byteArraySig_st.sigByteArray_pau8, rxSigConstPtr_pcst->bitSize_uo );

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                isRxSigDataCopied_b = COM_TRUE;
            }
        }
        break;
#ifdef COM_RXSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            float64 rxSigNewVal_f64;

            rxSigNewVal_f64 = Com_Prv_UnPackFloatSignal( sigEndianess_u8, rxSigConstPtr_pcst->bitPos_uo,
                                                         pduInfoPtr_pcst->SduDataPtr );

# if defined(COM_RxSigInvalid) || defined(COM_RxFilters)
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". rxSrcPtr_pv parameter
            * is always deferenced with correct type, so, there will not be any impact. Hence, it is suppressed. */
            if (Com_Prv_ValidateRxSignal( idSignal_u16, (void *)&rxSigNewVal_f64 ))
# endif
            {

                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                COM_GET_SIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,idxSigBuff_uo) = rxSigNewVal_f64;

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                isRxSigDataCopied_b = COM_TRUE;
            }
        }
        break;
#endif /* end of COM_RXSIG_FLOAT64SUPP */
        default:
        {
            Com_SigMax_tuo  rxSigNewVal_uo;

            rxSigNewVal_uo = Com_Prv_UnpackSignal( sigEndianess_u8, rxSigConstPtr_pcst->bitPos_uo, size_uo,
                                                   pduInfoPtr_pcst->SduDataPtr, sigType_u8);

#if defined(COM_RxSigInvalid) || defined(COM_RxFilters)
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". rxSrcPtr_pv parameter
            * is always deferenced with correct type, so, there will not be any impact. Hence, it is suppressed. */
            if (Com_Prv_ValidateRxSignal( idSignal_u16, (void *)&rxSigNewVal_uo ))
#endif
            {

                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                Com_Prv_UpdateRxSignalBuffer(rxSigConstPtr_pcst, rxSigNewVal_uo, idRxMainFunc_uo);

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                isRxSigDataCopied_b = COM_TRUE;
            }
        }
        break;
    }   /* end of switch() */

#ifdef COM_SIGNALGATEWAY
    if (isRxSigDataCopied_b)
    {
        /* This part of code updates the Rx Gateway queue If the signal is updated in the signal buffer by one
        * of the below means
        * 1. Reception of Ipdu and thereby signal update after update bit processing
        */
        if (Com_GetValue(RXSIG,_ISGWSIG,rxSigConstPtr_pcst->rxSignalFields_u16))
        {
            Com_SetRamValue(RXSIG,_SIGNALFILLBIT,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8),COM_TRUE);

            /* This flag will tell if any GW signals in the Ipdu are updated */
            Com_SetRamValue(RXIPDU,_GWSIG_UPDATED,(rxIpduConstPtr_pcst->ipduRam_pst->rxFlags_u16),COM_TRUE);
        }
    }
#endif /* #ifdef COM_SIGNALGATEWAY */

    return isRxSigDataCopied_b;
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ValidateRxSignal
 Description      : Internal function used for validating Rx signal
 Parameter        : idSignal_u16    -> Id of the Signal
                  : rxSrcPtr_pv     -> Pointer to New Signal value
 Return value     : true  - Copy new signal value into signal buffer
                    false - Do not copy new signal value into signal buffer
 **********************************************************************************************************************
*/
#if defined(COM_RxSigInvalid) || defined(COM_RxFilters)

boolean Com_Prv_ValidateRxSignal(Com_SignalIdType idSignal_u16, void * rxSrcPtr_pv)
{
#if defined(COM_SIGNALGATEWAY) || defined(COM_RxSigInvalid)
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
    uint16                      rxSignalFields_u16;
#endif
    boolean                     isValidRxSig_b;

    isValidRxSig_b = COM_TRUE;

#if defined(COM_SIGNALGATEWAY) || defined(COM_RxSigInvalid)
    rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    rxSignalFields_u16 = rxSigConstPtr_pcst->rxSignalFields_u16;
# endif

# ifdef COM_SIGNALGATEWAY
    /* TRACE[SWS_Com_00872] skip validation & filtering for gateway signal */
    if (Com_GetValue(RXSIG,_ISGWSIG,rxSignalFields_u16) == COM_FALSE)
# endif
    {
# ifdef COM_RxSigInvalid
        {
            uint8 invAction_u8;

            invAction_u8 = (uint8)Com_GetValue(RXSIG,_INVACTION,rxSignalFields_u16);

            /* Check if the signal has invalid value configured and the newly received value is invalid value */
            /* MR12 RULE 13.5 VIOLATION: Code is designed such that Com_Prv_IsInvalidSigData() needs to be
            * conditionally executed, so, there is no impact independent whether Com_Prv_IsInvalidSigData() is
            * executed/not, hence suppressed. */
            if ((invAction_u8 != COM_NONE) && Com_Prv_IsInvalidSigData(idSignal_u16, rxSrcPtr_pv))
            {

                if ( invAction_u8 == COM_REPLACE )
                {
                    /* TRACE[SWS_Com_00681] If the configured ComSignalDataInvalidValue is received for a signal and
                    * the ComDataInvalidAction is configured to REPLACE for this signal,
                    * the AUTOSAR COM module shall replace the signal's value by its configured ComSignalInitValue.
                    * After the replacement, the normal signal processing like filtering and
                    * notification shall take place as if the ComSignalInitValue would have been received
                    * instead of the ComSignalDataInvalidValue. */

                    /* rxSrcPtr_pv already points-to/contains INIT data */
                }
#  ifdef COM_RxSigInvalidNotify
                else
                {
                    /* invAction_u8 is COM_NOTIFY */

                    /* TRACE[SWS_Com_00680] If the configured ComSignalDataInvalidValue is received for a signal and
                    * the ComDataInvalidAction is configured to NOTIFY for this signal,
                    * the AUTOSAR COM module shall notify the RTE via the configured ComInvalidNotification function.
                    * In this case, no other signal processing like filtering or
                    * the normal signal indication shall take place.
                    * TRACE[SWS_Com_00717] If the configured ComSignalDataInvalidValue is received for a signal and
                    * its ComDataInvalidAction is configured to NOTIFY,
                    * the AUTOSAR COM module shall not store the received ComSignalDataInvalidValue
                    * into the signal object.
                    * The next call to Com_ReceiveSignal will return the last valid received signal or the
                    * ComSignalInitValue in case no signal was received yet respectively. */
                    isValidRxSig_b = COM_FALSE;

                    if (rxSigConstPtr_pcst->invalidNotification_pfct != NULL_PTR)
                    {
                        rxSigConstPtr_pcst->invalidNotification_pfct();
                    }
#  ifdef COM_CALLBACK_RX
                    else
                    {
                        const Com_Prv_xRxSigVarCfg_tst * rxSigVarCfg_pcst;

                        rxSigVarCfg_pcst = COM_GET_RXSIG_VAR_CFG(idSignal_u16);
                        if (rxSigVarCfg_pcst->invCbk_pcauo != NULL_PTR)
                        {
                            Com_Prv_InvokeRxUserCbks(rxSigVarCfg_pcst->invCbk_pcauo, rxSigConstPtr_pcst->idHandle_u16);
                        }
                    }
#  endif /* end of COM_CALLBACK_RX */
                }
#  endif /* #ifdef COM_RxSigInvalidNotify */

            }
        }
# endif /* #ifdef COM_RxSigInvalid */

# ifdef COM_RxFilters

        /* if signal is valid then perforn filtering */
        if ( isValidRxSig_b )
        {
            /* MR12 RULE 11.5 VIOLATION: data read by type casting to "Com_SigMax_tuo" is used in only in case
            * reference data is of that type, otherwise its data is not used. Hence, suppressed. */
            isValidRxSig_b = Com_Prv_EvaluateRxSigFilter(idSignal_u16, *(Com_SigMax_tuo *)rxSrcPtr_pv);

        }

# endif /* #ifdef COM_RxFilters */
    }

    return (isValidRxSig_b);
}

#endif /* #if defined(COM_RxSigInvalid) || defined(COM_RxFilters) */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsInvalidSigData
 Description      : Checks if received signal value is equivalent to DataInvalidValue
 Parameter        : idSignal_u16    -> Id of the Signal
                  : rxSrcPtr_pv     -> Pointer to New Signal value
 Return value     : true  - is invalid value
                    false - is not invalid value
 **********************************************************************************************************************
*/
#ifdef COM_RxSigInvalid

LOCAL_INLINE boolean Com_Prv_IsInvalidSigData(Com_SignalIdType idSignal_u16, void * rxSrcPtr_pv)
{
    Com_RxSigCfg_tpcst rxSigConstPtr_pcst;
    uint32 idxInitVal_u32;
    uint32 invalidVal_u32;
    uint8 sigType_u8;
    boolean isInvalidValue_b;

    isInvalidValue_b = COM_FALSE;

    rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    idxInitVal_u32 = rxSigConstPtr_pcst->idxInitValue_uo;

    invalidVal_u32 = rxSigConstPtr_pcst->dataInvalidVal_u32;

    sigType_u8 = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

    switch (sigType_u8)
    {
# ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
            Com_ByteArraySig_tst * byteArraySig_pst;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            byteArraySig_pst = (Com_ByteArraySig_tst *)rxSrcPtr_pv;

            /* MR12 DIR 1.1, MR12 RULE 13.5 VIOLATION: Evaluation(or no evaluation) of the second part of logical-OR
            * shall not impact the system behaviour. Hence suppressed.
            * Explicit cast is provided in line with COM_MEMCOMPARE definition.
            * But the void pointer is always deferenced to a type, based on the pointers
            * alignment.Hence the alignment warning can safely be ignored. */
            if ((byteArraySig_pst->sigLength_uo == COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigLength_uo) &&
                (COM_MEMCOMPARE(byteArraySig_pst->sigByteArray_pau8,
                                COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigByteArray_pau8,
                                (uint32)(byteArraySig_pst->sigLength_uo)) == 0x00))
            {

                isInvalidValue_b = COM_TRUE;
                byteArraySig_pst->sigByteArray_pau8 = COM_GET_BYTE_ARRAY_SIG_VAL(idxInitVal_u32).sigByteArray_pau8;
                byteArraySig_pst->sigLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(idxInitVal_u32).sigLength_uo;
            }
        }
        break;
# endif /* end of COM_RX_DYNAMIC_SIGNAL_SUPPORT */
        case COM_UINT8_N:
        {
            Com_ByteArraySig_tst * byteArraySig_pst;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            byteArraySig_pst = (Com_ByteArraySig_tst *)rxSrcPtr_pv;

            /* MR12 DIR 1.1 VIOLATION: usage of generic function to compare memories. But there is no impact
            * due to this, hence suppressed. */
            if (COM_MEMCOMPARE(byteArraySig_pst->sigByteArray_pau8,
                               COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigByteArray_pau8,
                               (uint32)(byteArraySig_pst->sigLength_uo)) == 0x00)
            {

                isInvalidValue_b = COM_TRUE;
                byteArraySig_pst->sigByteArray_pau8 = COM_GET_BYTE_ARRAY_SIG_VAL(idxInitVal_u32).sigByteArray_pau8;
            }
        }
        break;
# ifdef COM_RXSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            /* MR12 DIR 1.1 VIOLATION: usage of generic function to compare memories. But there is no impact
            * due to this, hence suppressed. */
            if (COM_MEMCOMPARE(rxSrcPtr_pv, &Com_getfloat64SigValue(invalidVal_u32),
                               (uint32)(COM_F64_SIZE_IN_BYTES)) == 0x00)
            {

                isInvalidValue_b = COM_TRUE;
                /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
                *((float64 *)rxSrcPtr_pv) = Com_getfloat64SigValue(idxInitVal_u32);

            }
        }
        break;
# endif /* end of COM_RXSIG_FLOAT64SUPP */
# ifdef COM_RXSIG_INT64
        case COM_UINT64:
        case COM_SINT64:
        {
            Com_SigMax_tuo * rxSigNewVal_uo;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            rxSigNewVal_uo = (Com_SigMax_tuo *)rxSrcPtr_pv;

            if (*(rxSigNewVal_uo) == (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(invalidVal_u32)))
            {
                isInvalidValue_b = COM_TRUE;

                *rxSigNewVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(idxInitVal_u32));
            }
        }
        break;
# endif /* end of COM_RXSIG_INT64 */
        default:
        {
            Com_SigMax_tuo * rxSigNewVal_uo;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            rxSigNewVal_uo = (Com_SigMax_tuo *)rxSrcPtr_pv;

            if (*rxSigNewVal_uo == invalidVal_u32)
            {
                isInvalidValue_b = COM_TRUE;

                *rxSigNewVal_uo = COM_GET_INT32_SIG_VALUE(idxInitVal_u32);
            }
        }
        break;
    }   /* end of switch() */

    return (isInvalidValue_b);
}
#endif /* end of COM_RxSigInvalid */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_EvaluateRxSigFilter
 Description      : Evaluates rx signal filter
 Parameter        : idSignal_u16    -> Id of the Signal
                  : rxSigVal_uo     -> New Signal value
 Return value     : filter result - true/false
 **********************************************************************************************************************
*/
#ifdef COM_RxFilters

boolean Com_Prv_EvaluateRxSigFilter(Com_SignalIdType idSignal_u16, Com_SigMax_tuo rxSigVal_uo)
{
    Com_RxSigCfg_tpcst rxSigConstPtr_pcst;
    uint8 filterAlgo_u8;
    boolean filterEvaluation_b;

    filterEvaluation_b = COM_TRUE;

    rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    filterAlgo_u8 = (uint8)Com_GetValue(RXSIG,_FILTRALG,rxSigConstPtr_pcst->rxSignalFields_u16);

    /* if signal is valid and filter is configured, then perforn filtering */
    if (COM_NOTCONFIGURED != filterAlgo_u8)
    {
#  if defined (COM_F_MASKEDNEWDIFFERSOLD) && (defined (COM_RxIPduTimeout) || defined(COM_RxSigUpdateTimeout))

        /* TRACE[SWS_Com_00793] For a signal with a configured reception filter
        * MASKED_NEW_DIFFERS_MASKED_OLD, the AUTOSAR COM module shall treat
        * the first value received for this signal after a reception deadline monitoring timeout
        * occurred for this signal the same way as if the value has passed the filter criteria.(SRS_Com_02037)
        * Hence, the AUTOSAR COM module will let pass any value for the filter
        * MASKED_NEW_DIFFERS_MASKED_OLD after an RX deadline timeout for the
        * associated I-PDU. */
        if ((Com_GetRamValue(RXSIG,_DMFILTERING,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8))) &&
            (COM_MASKED_NEW_DIFFERS_MASKED_OLD == filterAlgo_u8))
        {
            /* Resetting this flag, for further reception of the signal,
            * if Rx deadline monitoring timer elapses again this flag shall be set in Com_MainFunctionRx() */
            Com_SetRamValue(RXSIG,_DMFILTERING,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8),COM_FALSE);
        }
        else
#  endif /* end of COM_F_MASKEDNEWDIFFERSOLD special handling */
        {
            filterEvaluation_b = Com_Prv_Filter((uint32)(rxSigVal_uo), filterAlgo_u8,
                                            rxSigConstPtr_pcst->idxFilter_uo, COM_PRV_NOT_FROM_IPDU_GRP_START);
        }
    }

    return filterEvaluation_b;
}

#endif /* end of COM_RxFilters */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsRxSigReplacedToInit
 Description      : If signal INIT is not filtered out, then it is stored in their buffers
 Parameter        : idSignal_u16    -> Id of the Signal
 Return value     : returns true if signal buffer is replaced with INIT
 **********************************************************************************************************************
*/
#ifdef COM_RxSigInvalid

boolean Com_Prv_IsRxSigReplacedToInit(Com_SignalIdType  idSignal_u16)
{
    Com_RxSigCfg_tpcst          rxSigConstPtr_pcst;
    Com_SigBuffIndex_tuo        idxSigBuff_uo;
    Com_SigMax_tuo              rxSigVal_uo;
    Com_IpduId_tuo              idRxPdu_uo;
    Com_MainFunc_tuo            idRxMainFunc_uo;
    uint8                       sigType_u8;
    boolean                     replacedToInit_b;

    replacedToInit_b = COM_FALSE;

    rxSigConstPtr_pcst      = COM_GET_RXSIG_CONSTDATA(idSignal_u16);

    idRxPdu_uo              = rxSigConstPtr_pcst->idComIPdu_uo;
    idRxMainFunc_uo         = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo)->idMainFunc_uo;

    sigType_u8              = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

    idxSigBuff_uo = rxSigConstPtr_pcst->idxSigBuff_uo;

    rxSigVal_uo = rxSigConstPtr_pcst->idxInitValue_uo;

    /* NOTE on Exclusive area COM_RXINDICATION:
    * If there is a possibility that Com_RxIndication() can be interrupted by
    * Com_ReceiveSignal() or Com_Init() function for any IPdu,
    * then this exclusive area needs to be defined in Com_Cfg_SchM.h file.
    */
    /* Copy the new value of the signal into signal buffer */

    switch (sigType_u8)
    {
        case COM_UINT8_N:
        {
# ifdef COM_RxFilters
            if (Com_Prv_EvaluateRxSigFilter(idSignal_u16, rxSigVal_uo))
# endif
            {
                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below
                * mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
                COM_MEMCOPY(
                        &COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,idxSigBuff_uo),
                        COM_GET_BYTE_ARRAY_SIG_VAL(rxSigVal_uo).sigByteArray_pau8,
                        rxSigConstPtr_pcst->bitSize_uo
                           );

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                replacedToInit_b = COM_TRUE;
            }
        }
        break;
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
# ifdef COM_RxFilters
            if (Com_Prv_EvaluateRxSigFilter(idSignal_u16, rxSigVal_uo))
# endif
            {
                PduLengthType dynLength_uo;

                Com_Get_SchM_Enter_Com_RxSigDynBuff

                dynLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(rxSigVal_uo).sigLength_uo;

                /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below
                * mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
                COM_MEMCOPY(
                        &COM_GET_SIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,idxSigBuff_uo),
                        COM_GET_BYTE_ARRAY_SIG_VAL(rxSigVal_uo).sigByteArray_pau8, dynLength_uo
                            );

                COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo)->ipduRam_pst->dynSigLength_u16 = (uint16)dynLength_uo;

                Com_Get_SchM_Exit_Com_RxSigDynBuff

                replacedToInit_b = COM_TRUE;
            }
        }
        break;
#endif /* end of COM_RX_DYNAMIC_SIGNAL_SUPPORT */
#ifdef COM_RXSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
# ifdef COM_RxFilters
            if (Com_Prv_EvaluateRxSigFilter(idSignal_u16, rxSigVal_uo))
# endif
            {
                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                /* For float64 signals, rxSigVal_uo holds index to float array holding init values. */
                COM_GET_SIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,idxSigBuff_uo) = Com_getfloat64SigValue(rxSigVal_uo);

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                replacedToInit_b = COM_TRUE;
            }
        }
        break;
#endif /* end of COM_RXSIG_FLOAT64SUPP */
        default:
        {
#ifdef COM_RXSIG_INT64
            if (COM_SIG_IS_INT64_TYPE(sigType_u8))
            {
                rxSigVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(rxSigVal_uo));
            }
            else
#endif /* end of COM_RXSIG_INT64 */
            {
                rxSigVal_uo = COM_GET_INT32_SIG_VALUE(rxSigVal_uo);
            }

# ifdef COM_RxFilters
            if (Com_Prv_EvaluateRxSigFilter(idSignal_u16, rxSigVal_uo))
# endif
            {
                Com_Get_SchM_Enter_Com_RxSigBuff(RXINDICATION)

                Com_Prv_UpdateRxSignalBuffer(rxSigConstPtr_pcst, rxSigVal_uo, idRxMainFunc_uo);

                Com_Get_SchM_Exit_Com_RxSigBuff(RXINDICATION)

                replacedToInit_b = COM_TRUE;
            }
        }
        break;
    }   /* end of switch() */

    return replacedToInit_b;
}

#endif /* end of COM_RxSigInvalid */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

