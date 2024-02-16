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


/* This file contains signal groups processing from the received Pdu.
 * Hence complete file contents have to be placed inside this macro */
#ifdef COM_RX_SIGNALGROUP

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
#ifdef COM_RxSigGrpUpdateBit
LOCAL_INLINE boolean Com_Prv_ProcessSigGrpUpdateBit(
                                    Com_SignalGroupIdType   idSigGrp_u16,
                                    const PduInfoType *     pduInfoPtr_pcst
                                                   );
#endif

#if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
LOCAL_INLINE uint8 Com_Prv_ValidateRxSignalGroup(Com_SignalGroupIdType idSigGrp_u16,
                                                 const PduInfoType * pduInfoPtr_pcst);

LOCAL_INLINE uint8 Com_Prv_ValidateRxGroupSignal(uint16_least idGrpSig_qu16, const PduInfoType * pduInfoPtr_pcst);

# ifdef COM_RxSigGrpInvalid
LOCAL_INLINE boolean Com_Prv_IsInvalidGrpSigData(Com_SignalIdType idGrpSig_u16, const void * rxSrcPtr_pcv);
# endif

#endif

# ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
LOCAL_INLINE uint16 Com_Prv_GetRxDynSigLength(
                                    PduLengthType           sduLength_uo,
                                    Com_Bitposition_tuo     bitPos_uo,
                                    Com_Bitsize_tuo         bitSize_uo
                                             );
# endif

LOCAL_INLINE void Com_Prv_IpduBuffToSecondBuff(
                                    Com_SignalGroupIdType   idSigGrp_u16,
                                    const PduInfoType *     pduInfoPtr_pcst,
                                    uint8                   validateSigGrp_u8
                                              );

LOCAL_INLINE void Com_Prv_CopySecondaryBuffer(
                                    Com_RxSigGrpCfg_tpcst   rxSigGrpConstPtr_pcst,
                                    const PduInfoType *     pduInfoPtr_pcst
                                             );


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
 Function name    : Com_Prv_ProcessSignalGroup
 Description      : Process rx-signal groups of the received I-PDU.
 Parameter        : idRxPdu_uo      - ID of the received I-PDU.
                  : pduInfoPtr_pcst - Contains the length (SduLength) of the received I-PDU and
                                 a pointer to a buffer (SduDataPtr) containing the I-PDU.
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_ProcessSignalGroup(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst             rxIpduConstPtr_pcst;
    Com_RxSigGrpCfg_tpcst           rxSigGrpConstPtr_pcst;
    uint16_least                    idxSigGrpId_qu16;
    uint16_least                    idMaxSigGrp_qu16;
    uint8                           validateSigGrp_u8;
#ifdef COM_RxSigGrpUpdateTimeout
    uint16_least                    idxSigTout_qu16;
    uint8                           ipduUpdateBitStatus_u8;
    uint8                           sigGrpUpdateBitStatus_u8;
#endif

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);

#ifdef COM_RxSigGrpUpdateTimeout
    /* If this IPDU has signal groups with update-bits , initialise the pointers
    * Separate deadline monitoring should be performed for signal groups with update-bit
    * Deadline monitoring should not be done if it is disabled
    */
    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
    * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
    * system behaviour. Hence suppressed. */
    if ((rxIpduConstPtr_pcst->signalGrpTimeoutRef_pcst != NULL_PTR) && (COM_CheckRxIPduDMStatus(idRxPdu_uo)))
    {

        /* Update the variable UpdateBitStatus with value COM_UPDATEBIT_APPLICABLE. ie at least one of the signal group
        * within the IPDU is configured with both update-bit and deadline monitoring. */
        ipduUpdateBitStatus_u8 = COM_UPDATEBIT_APPLICABLE;
    }
    else
    {
        /* Reset the variable to default value ie COM_UPDATEBIT_NOT_APPLICABLE */
        ipduUpdateBitStatus_u8 = COM_UPDATEBIT_NOT_APPLICABLE;
    }

    /* Initialize the variable to be used to refer the first signal timeout configuration */
    idxSigTout_qu16 = COM_ZERO;
#endif /* #ifdef COM_RxSigGrpUpdateTimeout */

    idxSigGrpId_qu16  = rxIpduConstPtr_pcst->idFirstRxSigGrp_uo;
    idMaxSigGrp_qu16   = idxSigGrpId_qu16 + rxIpduConstPtr_pcst->numOfSigGrp_u16;

    rxSigGrpConstPtr_pcst  = COM_GET_RXSIGGRP_CONSTDATA(idxSigGrpId_qu16);

    /* Start processing for signal groups. */
    for ( ; idxSigGrpId_qu16 < idMaxSigGrp_qu16; idxSigGrpId_qu16++)
    {
        validateSigGrp_u8 = COM_SIGGRP_ACCEPT;

#ifdef COM_RxSigGrpUpdateTimeout
        sigGrpUpdateBitStatus_u8 = ipduUpdateBitStatus_u8;
#endif

        /* SignalGroup is processed only if below conditions are satisfied:
        * 1. Update bit validation for signal groups with Update bit configured.
        * 2. Completely received SignalGroup TRACE[SWS_Com_00575].
        */
        if (
#ifdef COM_RxSigGrpUpdateBit
             (Com_Prv_ProcessSigGrpUpdateBit( (Com_SignalGroupIdType)idxSigGrpId_qu16, pduInfoPtr_pcst)) &&
#endif
             (rxSigGrpConstPtr_pcst->lastByte_uo <= pduInfoPtr_pcst->SduLength)
           )
        {
#if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters) || defined(COM_SIGNALGROUPGATEWAY)
            uint8   constByteValue_u8;
# ifdef COM_SIGNALGROUPGATEWAY
            boolean isGwSigGrp_b;
# endif
# if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
            boolean isValidationReq_b;
# endif
            constByteValue_u8 = rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8;

# ifdef COM_SIGNALGROUPGATEWAY
            isGwSigGrp_b      = Com_GetValue(RXSIGGRP,_ISGWSIGGRP,constByteValue_u8);
# endif

# if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
            isValidationReq_b =
#  ifdef COM_SIGNALGROUPGATEWAY
                /* TRACE[SWS_Com_00872] skip validation & filtering for gateway signal-group */
                (isGwSigGrp_b == COM_FALSE) &&
#  endif
                /* MR12 RULE 13.5 VIOLATION: Below conditional statement reads flag status. Evaluation
                * (or no evaluation) of the 2nd or 3rd operands does not impact the system behaviour even though of
                * volatile-qualified, moreover HIS-LEVEL parameter will be violated if more conditional statements are
                * added. Hence the warning is suppressed. */
                ((Com_GetValue(RXSIGGRP,_INVACTION,constByteValue_u8) != COM_NONE)
#  ifdef COM_RxFilters
                 || (Com_GetValue(RXSIGGRP,_FILTEREVALREQ,constByteValue_u8))
#  endif
                );

            if (isValidationReq_b)
            {
                validateSigGrp_u8 = Com_Prv_ValidateRxSignalGroup((Com_SignalGroupIdType)idxSigGrpId_qu16,
                                                                               pduInfoPtr_pcst);
            }

            if (validateSigGrp_u8 != COM_SIGGRP_DISCARD)
# endif /* #if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters) */
#endif /* #if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters) || defined(COM_SIGNALGROUPGATEWAY) */
            {
                /* The processed data is copied into second buffer, because, if the API Com_Recivesignalgroup() is
                * not interrupt safe there can be data corruption on the group signal values. */
                Com_Prv_IpduBuffToSecondBuff((Com_SignalGroupIdType)idxSigGrpId_qu16,
                                                                                pduInfoPtr_pcst, validateSigGrp_u8);

#ifdef COM_SIGNALGROUPGATEWAY

                if (isGwSigGrp_b)
                {
                    Com_SetRamValue(RXSIGGRP,_SIGNALFILLBIT,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8,
                                                                                                            COM_TRUE);

                    /* This flag will tell if any GW signal groups in the Ipdu are updated */
                    Com_SetRamValue(RXIPDU,_GWSIG_UPDATED,(rxIpduConstPtr_pcst->ipduRam_pst->rxFlags_u16),COM_TRUE);
                }
#endif /* #ifdef COM_SIGNALGROUPGATEWAY */

                /* If sigGrpUpdateBitStatus_u8 is
                * COM_UPDATEBIT_NOT_APPLICABLE(0x0), then it remains unchanged.
                * COM_UPDATEBIT_APPLICABLE(0x1), then it is changed to COM_UPDATEBIT_SET(0x2) here. */
#ifdef COM_RxSigGrpUpdateTimeout
                sigGrpUpdateBitStatus_u8 *= COM_UPDATEBIT_SET;
#endif
            }
        }
        else
        {
            /* Either Update bit is received with reset state or signalgroup is partially received */
            validateSigGrp_u8 = COM_SIGGRP_DISCARD;
        }

#ifdef COM_RxSigGrpUpdateTimeout
         /* If update bit and timeout is configured */
        if (sigGrpUpdateBitStatus_u8 != COM_UPDATEBIT_NOT_APPLICABLE)
        {
            Com_Prv_LoadSigGrpTimeoutValue((Com_SignalGroupIdType)idxSigGrpId_qu16,
                                                                        sigGrpUpdateBitStatus_u8, &idxSigTout_qu16);
        }
#endif

#ifdef COM_RxSignalGrpNotify
        /* Set _SIGNOTIF flag to invoke configured SignalGroup-ComNotification */
        Com_SetRamValue(RXSIGGRP,_SIGNOTIF,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8,
                        (validateSigGrp_u8 != COM_SIGGRP_DISCARD));
#ifdef COM_PRV_ENABLECONFIGINTERFACES
        Com_SetRamValue(RXSIGGRPCFG,_SIGNOTIF,
                        *(Com_GlobalConfig_pcst->rxSigGrpCfg_pcst[idxSigGrpId_qu16].rxSigGrpRAMFields_pu8),
                         (validateSigGrp_u8 != COM_SIGGRP_DISCARD));
#endif /* end of #ifdef COM_PRV_ENABLECONFIGINTERFACES */
#endif

        rxSigGrpConstPtr_pcst++;

    }/* End of for loop */
}


#ifdef COM_RxSigGrpUpdateBit
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ProcessSigGrpUpdateBit
 Description      : Private function to validate signalgroup UpdateBit
 Parameter        : idSigGrp_u16    - ID of rx-signal group.
                  : pduInfoPtr_pcst  - Contains the length (SduLength) of the received I-PDU and
                                  a pointer to a buffer (SduDataPtr) containing the I-PDU.
 Return value     : COM_FALSE   - UpdateBit is configured & received with reset state.
                    COM_TRUE    - updatebit is set(other cases).
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean Com_Prv_ProcessSigGrpUpdateBit(
                                                 Com_SignalGroupIdType       idSigGrp_u16,
                                                 const PduInfoType *         pduInfoPtr_pcst
                                                   )
{
    Com_RxSigGrpCfg_tpcst            rxSigGrpConstPtr_pcst;
    boolean                          isUpdateBitSet_b;

    /* Initialize update bit to true */
    isUpdateBitSet_b = COM_TRUE;

    rxSigGrpConstPtr_pcst  = COM_GET_RXSIGGRP_CONSTDATA(idSigGrp_u16);

    /* Check if the signal group contained within this IPDU has update-bit configured */
    if (Com_GetValue(RXSIGGRP,_UPDBITCONF,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8))
    {
        PduLengthType   updateBitByteNo_uo;

        /* Find the byte No of this Update bit */
        updateBitByteNo_uo     = (PduLengthType)(rxSigGrpConstPtr_pcst->updateBitPos_uo >> 3u);

        /* Start: Check if the Update bit is SET or NOT */
        if ((updateBitByteNo_uo + COM_ONE) <= pduInfoPtr_pcst->SduLength)
        {
            /* TRACE[SWS_Com_00802] If a signal or signal group with an attached update bit is received and
            * the update-bit is not set, the AUTOSAR COM module shall discard this signal.
            * If the signal has not been updated, the signal will not be routed via the signal gateway.
            * It will only be discarded.
            */
            isUpdateBitSet_b  = Com_GetUpdateBitValue( pduInfoPtr_pcst->SduDataPtr,
                                                       rxSigGrpConstPtr_pcst->updateBitPos_uo,
                                                       updateBitByteNo_uo );
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

#endif /* #ifdef COM_RxSigGrpUpdateBit */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IpduBuffToSecondBuff
 Description      : Internal function used for copying the signal group contents from IPdu buffer to second buffer
 Parameter        : idSigGrp_u16      -> Signal Group Id
                  : pduInfoPtr_pcst    -> Pointer to Rx SignalGroup static configuration
                  : validateSigGrp_u8 -> Flag to tell if the group signal value should be copied
                                         with Initial values/received value
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_IpduBuffToSecondBuff(
                                            Com_SignalGroupIdType  idSigGrp_u16,
                                            const PduInfoType *    pduInfoPtr_pcst,
                                            uint8                  validateSigGrp_u8
                                              )
{
    Com_RxSigGrpCfg_tpcst        rxSigGrpConstPtr_pcst;
# if defined(COM_RX_SIGNALGROUP_ARRAY) || defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
    Com_MainFunc_tuo             idRxMainFunc_uo;
# endif

    rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(idSigGrp_u16);

    /* Fetch the Rx-MainFunction internal Id */
# if defined(COM_RX_SIGNALGROUP_ARRAY) || defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
    idRxMainFunc_uo     = (COM_GET_RX_IPDU_CONSTDATA(rxSigGrpConstPtr_pcst->idComIPdu_uo))->idMainFunc_uo;
# endif

    /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
     * they are intentionally not placed under compiler switch statement,
     * so that these lines are always available for compilation*/
    (void)validateSigGrp_u8;

# if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
    if (validateSigGrp_u8 != COM_SIGGRP_REPLACE_TO_INIT)
# endif
    {

# ifdef COM_RX_SIGNALGROUP_ARRAY
        if (Com_GetValue(RXSIGGRP,_ARRAYACCESS,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8))
        {
            Com_RxSigGrpArrayCfg_tpcst    rxSigGrpArrayConstPtr_pcst;
            uint8                       * rxSigGrpArrayBufRamPtr_pu8;

            rxSigGrpArrayConstPtr_pcst  = COM_GET_RXSIGGRP_ARRAY_CONSTDATA( rxSigGrpConstPtr_pcst->idxSigGrpArray_uo );

            rxSigGrpArrayBufRamPtr_pu8 =
                        &COM_GET_RXSIGGRP_ARRAY_BUFF( idRxMainFunc_uo, rxSigGrpArrayConstPtr_pcst->idxRxSigGrpBuf_uo );

            Com_Get_SchM_Enter_Com_RxSigGrpArrayBuff

            /* Copy GroupSignal's received data into RxSignalGroupArray Buffer from IPdu Buffer.
            * As it is UINT8 array copy, so RxSignalGroupArray Buffer can also hold other
            * interleaved Signal's received data of the same IPdu */
            Com_ByteCopy((uint8 *)(rxSigGrpArrayBufRamPtr_pu8),
                   (const uint8 *)(pduInfoPtr_pcst->SduDataPtr + rxSigGrpArrayConstPtr_pcst->firstByteNo_uo),
                                  (rxSigGrpConstPtr_pcst->lastByte_uo - rxSigGrpArrayConstPtr_pcst->firstByteNo_uo));

            Com_Get_SchM_Exit_Com_RxSigGrpArrayBuff
        }
        else
# endif /* #ifdef COM_RX_SIGNALGROUP_ARRAY */
        {
            Com_Prv_CopySecondaryBuffer(rxSigGrpConstPtr_pcst, pduInfoPtr_pcst);
        }
    }
# if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
    else
    {
        /* Discard GroupSignal's received data and place its SignalInitValues in RxSignalGroupArray Buffer */
        Com_Prv_RxReplaceSigGrp(rxSigGrpConstPtr_pcst, idRxMainFunc_uo, COM_REPLACE);
    }
# endif /* # if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters) */
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_CopySecondaryBuffer
 Description      : Internal function used for copying the signal group contents from IPdu buffer to second buffer
 Parameter        : rxSigGrpConstPtr_pcst  -> Pointer to Rx SignalGroup static configuration
                  : pduInfoPtr_pcst        -> Address where the data is available
 Return value     : None
 **********************************************************************************************************************
*/

LOCAL_INLINE void Com_Prv_CopySecondaryBuffer(Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst,
                                              const PduInfoType * pduInfoPtr_pcst)
{

    /* Local pointer to hold the address of Rx GroupSignal static configuration */
    Com_RxGrpSigCfg_tpcst           rxGrpSigConstPtr_pcst;
    Com_RxIpduCfg_tpcst             rxIpduConstPtr_pcst;
    uint16_least                    idxGrpSig_qu16;
    Com_MainFunc_tuo                idRxMainFunc_uo;
    uint8                           sigType_u8;
    uint8                           constByteValue_u8;

    rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA(rxSigGrpConstPtr_pcst->idFirstGrpSig_uo);

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(rxSigGrpConstPtr_pcst->idComIPdu_uo);

    /* Fetch the Rx-MainFunction internal Id */
    idRxMainFunc_uo     = rxIpduConstPtr_pcst->idMainFunc_uo;

    Com_Get_SchM_Enter_Com_RxGrpSigSecBuff

    for (idxGrpSig_qu16 = rxSigGrpConstPtr_pcst->numOfGrpSig_uo; idxGrpSig_qu16 > COM_ZERO; idxGrpSig_qu16--)
    {
        constByteValue_u8 = rxGrpSigConstPtr_pcst->rxGrpSigFields_u8;

        sigType_u8 = Com_GetValue(RXGRPSIG,_TYPE,constByteValue_u8);

        if (sigType_u8 == COM_UINT8_N)
        {
            Com_ByteCopy( &COM_GET_SECRXGRPSIGTYPEU8_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo),
                          (pduInfoPtr_pcst->SduDataPtr +(rxGrpSigConstPtr_pcst->bitPos_uo >> 3u)),
                           rxGrpSigConstPtr_pcst->bitSize_uo );
        }
        else
#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
        if (sigType_u8 == COM_UINT8_DYN)
        {
            PduLengthType   startByteNo_uo;
            uint16          dynLength_u16;

            startByteNo_uo = (PduLengthType)(rxGrpSigConstPtr_pcst->bitPos_uo >> 3u);

            dynLength_u16 = Com_Prv_GetRxDynSigLength(pduInfoPtr_pcst->SduLength,
                                                      rxGrpSigConstPtr_pcst->bitPos_uo,
                                                      rxGrpSigConstPtr_pcst->bitSize_uo);

            Com_ByteCopy( &COM_GET_SECRXGRPSIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo),
                          (pduInfoPtr_pcst->SduDataPtr + startByteNo_uo),
                          dynLength_u16 );

            rxIpduConstPtr_pcst->ipduRam_pst->dynSigLength_u16 = dynLength_u16;
        }
        else
#endif
        {
            uint8   sigEndianess_u8;

            sigEndianess_u8 = Com_GetValue(RXGRPSIG,_ENDIANESS,constByteValue_u8);

# ifdef COM_RXGRPSIG_FLOAT64SUPP
            if (sigType_u8 == COM_FLOAT64)
            {
                float64 rxNewVal_f64;

                rxNewVal_f64 = Com_Prv_UnPackFloatSignal( sigEndianess_u8, rxGrpSigConstPtr_pcst->bitPos_uo,
                                                                                        pduInfoPtr_pcst->SduDataPtr );

                COM_GET_SECRXGRPSIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo) =
                                                                                                        rxNewVal_f64;
            }
            else
# endif
            {
                Com_SigMax_tuo  rxGrpSigNewVal_uo;

                rxGrpSigNewVal_uo = Com_Prv_UnpackSignal( sigEndianess_u8,
                                                           rxGrpSigConstPtr_pcst->bitPos_uo,
                                                           rxGrpSigConstPtr_pcst->bitSize_uo,
                                                           pduInfoPtr_pcst->SduDataPtr,
                                                           sigType_u8);

                /* Now copy corresponding Group signal buffer with the received value */
                Com_Prv_UpdateRxGroupSignalSecondBuffer(rxGrpSigConstPtr_pcst, rxGrpSigNewVal_uo, idRxMainFunc_uo);
            }
        }

        rxGrpSigConstPtr_pcst++;
    }
    Com_Get_SchM_Exit_Com_RxGrpSigSecBuff
}


#if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters)
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ValidateRxSignalGroup
 Description      : Private function to validate and process the configured filters for the given signal group
 Parameter        : idSigGrp_u16  - Signal Group Id to be validated
                    pduInfoPtr_pcst - IPdu buffer pointer
 Return value     : COM_SIGGRP_DISCARD            - discard signalgroup
                    COM_SET              - valid signalgroup is received, copy into local buffer
                    COM_SIGGRP_REPLACE_TO_INIT  - replace groupsignals with SignalInit values
 **********************************************************************************************************************
*/
LOCAL_INLINE uint8 Com_Prv_ValidateRxSignalGroup(Com_SignalGroupIdType idSigGrp_u16,
                                                 const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst;
    Com_RxSigGrpCfg_tpcst   rxSigGrpConstPtr_pcst;
    uint16_least            idxIdGrpSig_qu16;
    uint16_least            idMaxGrpSig_qu16;
    uint8                   validateSigGrp_u8;

    validateSigGrp_u8      = COM_SIGGRP_ACCEPT;

    rxSigGrpConstPtr_pcst  = COM_GET_RXSIGGRP_CONSTDATA(idSigGrp_u16);

    idxIdGrpSig_qu16       = rxSigGrpConstPtr_pcst->idFirstGrpSig_uo;
    idMaxGrpSig_qu16       = idxIdGrpSig_qu16 + rxSigGrpConstPtr_pcst->numOfGrpSig_uo;

    rxGrpSigConstPtr_pcst  = COM_GET_RXGRPSIG_CONSTDATA(idxIdGrpSig_qu16);

    for ( ; ((validateSigGrp_u8 == COM_SIGGRP_ACCEPT) && (idxIdGrpSig_qu16 < idMaxGrpSig_qu16)); idxIdGrpSig_qu16++)
    {
        if ((Com_GetValue(RXGRPSIG,_INVACTION,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8))
#ifdef COM_RxFilters
           || (rxGrpSigConstPtr_pcst->filterAlgo_u8 != COM_NOTCONFIGURED)
#endif
          )
        {
            validateSigGrp_u8 = Com_Prv_ValidateRxGroupSignal(idxIdGrpSig_qu16, pduInfoPtr_pcst);
        }

        rxGrpSigConstPtr_pcst++;
    }

#if (defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) && \
                                                    (defined(COM_RxIPduTimeout) || defined(COM_RxSigGrpUpdateTimeout))
    /* Reset the DM filtering flag only if it was set during the previous timeout session */
    if (validateSigGrp_u8 != COM_SIGGRP_DISCARD)
    {
        /* Resetting this flag which is maintained at SignalGroup level,
        * for further reception of the signal, if Rx deadline monitoring timer elapses,
        * again this flag shall be set in Com_MainFunctionRx()
        * This flag is resetted after processing all the ComGroupSignal inside this SignalGroup */
        Com_SetRamValue(RXSIGGRP,_DMFILTERING,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8,COM_FALSE);
    }
#endif /* #if (defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD)) &&
                                                (defined(COM_RxIPduTimeout) || defined(COM_RxSigGrpUpdateTimeout)) */

    return validateSigGrp_u8;
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ValidateRxGroupSignal
 Description      : check if RxGroupSignal is received with valid value
 Parameter        : idGrpSig_qu16            - Signal Group Id to be validated
                    rxNewValGrpSig_uo      - Rx GroupSignal value
 Return value     : COM_SIGGRP_DISCARD            - invalid GroupSignal value is received and InvalidAction is notify
                    COM_SIGGRP_REPLACE_TO_INIT    - invalid GroupSignal value is received and InvalidAction is replace
                    COM_SET                 - valid GroupSignal is received
 **********************************************************************************************************************
*/
LOCAL_INLINE uint8 Com_Prv_ValidateRxGroupSignal(uint16_least idGrpSig_qu16, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxGrpSigCfg_tpcst           rxGrpSigConstPtr_pcst;
    Com_RxSigGrpCfg_tpcst           rxSigGrpConstPtr_pcst;
    const void *                    rxSrcPtr_pcv;
    Com_RxIntSignalGroupId_tuo      idSignalGrp_uo;
    Com_SigMax_tuo                  rxNewValGrpSig_uo;
    uint8                           constByteValue_u8;
    uint8                           sigType_u8;
    uint8                           validateSigGrp_u8;

    validateSigGrp_u8 = COM_SIGGRP_ACCEPT;

    rxNewValGrpSig_uo = COM_ZERO;

    rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA(idGrpSig_qu16);
    idSignalGrp_uo        = rxGrpSigConstPtr_pcst->idSigGrp_uo;
    rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(idSignalGrp_uo);

    constByteValue_u8 = rxGrpSigConstPtr_pcst->rxGrpSigFields_u8;

    sigType_u8 = Com_GetValue(RXGRPSIG,_TYPE,constByteValue_u8);

    /* Unpack the group signal value */
    switch (sigType_u8)
    {
        case COM_UINT8_N:
#ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
        case COM_UINT8_DYN:
#endif
#ifdef COM_RXGRPSIG_FLOAT64SUPP
        case COM_FLOAT64:
#endif
        {
            /* MR12 DIR 1.1 VIOLATION: uses generic function independent of the type, and also dereferenced with
            * same type in the target function, hence suppressed. */
            rxSrcPtr_pcv = (const void *)pduInfoPtr_pcst;

            /* for reception invalidation, signal data is unpacked & checked directly in Com_Prv_IsInvalidGrpSigData().
            * for filter processing, signal data is not needed for these types,
            * so, already initialized "rxNewValGrpSig_uo" to zero is used */
        }
        break;
        default:
        {
            /* as FLOAT32 is considered as UINT32 internally, so its data is unpacked here itself. */

            rxNewValGrpSig_uo = Com_Prv_UnpackSignal(Com_GetValue(RXGRPSIG,_ENDIANESS,constByteValue_u8),
                                                     rxGrpSigConstPtr_pcst->bitPos_uo,
                                                     rxGrpSigConstPtr_pcst->bitSize_uo,
                                                     pduInfoPtr_pcst->SduDataPtr,
                                                     sigType_u8);

            /* MR12 DIR 1.1 VIOLATION: uses generic function independent of the type, and also dereferenced with
            * same type in the target function, hence suppressed. */
            rxSrcPtr_pcv = (const void *)&rxNewValGrpSig_uo;

        }
        break;
    }

    /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
     * they are intentionally not placed under compiler switch statement,
     * so that these lines are always available for compilation*/
    (void)idSignalGrp_uo;
    (void)rxSrcPtr_pcv;

#ifdef COM_RxSigGrpInvalid

    /* MR12 RULE 13.5 VIOLATION: Code is designed such that Com_Prv_IsInvalidGrpSigData() needs to be conditionally
    * executed, so, there is no impact independent whether Com_Prv_IsInvalidGrpSigData() is executed/not,
    * hence suppressed. */
    if (Com_GetValue(RXGRPSIG,_INVACTION,constByteValue_u8) &&
        Com_Prv_IsInvalidGrpSigData((Com_SignalIdType)idGrpSig_qu16, rxSrcPtr_pcv))
    {

        if (Com_GetValue(RXSIGGRP,_INVACTION,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8) == COM_REPLACE)
        {
            /**
            * TRACE[SWS_Com_00683] If the configured ComSignalDataInvalidValue is received
            * for at least one group signal of a signal group and
            * the ComDataInvalidAction, is configured to REPLACE for this signal group,
            * the AUTOSAR COM module shall replace all group signals of this signal group
            * by their configured ComSignalInitValue values. After the replacement,
            * the normal signal group/ group signal processing like reception notification shall take place
            * as if the ComSignalInitValue would have been received for all group signals.
            */
            /*  set below variable to Replace all the Group Signals with Initial value */
            validateSigGrp_u8 = COM_SIGGRP_REPLACE_TO_INIT;
        }
#ifdef COM_RxSigGrpInvalidNotify
        else
        {
            /* InvalidAction is NOTIFY */
            /**
            * TRACE[SWS_Com_00682] If the configured ComSignalDataInvalidValue is received
            * for at least one group signal of a signal group and
            * the ComDataInvalidAction is configured to NOTIFY for this signal group,
            * the AUTOSAR COM module shall notify the RTE via the configured ComInvalidNotification function.
            * In this case, no other signal group/ group signal processing like the normal indication shall take
            * place.
            * TRACE[SWS_Com_00718] If the configured ComSignalDataInvalidValue is received
            * for at least one group signal of a signal group and
            * its ComDataInvalidAction is configured to NOTIFY,
            * the AUTOSAR COM module shall not store any of the received group signals into the signal objects.
            * The next call to Com_ReceiveSignalGroup will copy the last valid received group
            * signals or the ComSignalInitValues in case the signal group was not received yet
            * respectively into the shadow buffer.
            */
            /* set below variable to ignore received signal group data */
            validateSigGrp_u8 = COM_SIGGRP_DISCARD;

            /* Call Invalid notification for the whole Signal Group as soon as the
            * first group signal with invalid value is encountered
            */
            if (rxSigGrpConstPtr_pcst->invalidNotification_pfct != NULL_PTR)
            {
                rxSigGrpConstPtr_pcst->invalidNotification_pfct();
            }
#  ifdef COM_CALLBACK_RX
            else
            {
                const Com_Prv_xRxSigGrpVarCfg_tst * rxSigGrpVarCfg_pcst;

                rxSigGrpVarCfg_pcst = COM_GET_RXSIGGRP_VAR_CFG(idSignalGrp_uo);
                if (rxSigGrpVarCfg_pcst->invCbk_pcauo != NULL_PTR)
                {
                    Com_Prv_InvokeRxUserCbks(rxSigGrpVarCfg_pcst->invCbk_pcauo, rxSigGrpConstPtr_pcst->idHandle_u16);
                }
            }
#  endif /* end of COM_CALLBACK_RX */
        }
#endif /* #ifdef COM_RxSigGrpInvalidNotify */

        /* SignalGroup is invalid, so no filtering process has to be done for this signal group
        * Break out of the for loop, as no more processing of group signals is required  */
    }
    else
#endif /* #ifdef COM_RxSigGrpInvalid */
    {
#ifdef COM_RxFilters
        uint8   filterAlgo_u8;

        /* The Groupsignal is valid, hence perform filtering */
        filterAlgo_u8 = rxGrpSigConstPtr_pcst->filterAlgo_u8;

        /* Proceed only if the GroupSignal is configured with a filter */
        if (filterAlgo_u8 != COM_NOTCONFIGURED)
        {
#if defined(COM_F_MASKEDNEWDIFFERSOLD) && (defined(COM_RxIPduTimeout) || defined(COM_RxSigGrpUpdateTimeout))

            /* Get return value of the filter which need not be returned if signal is invalid */
            if ( COM_FALSE == ((Com_GetRamValue(RXSIGGRP,_DMFILTERING,
                               rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8)) &&
                               (COM_MASKED_NEW_DIFFERS_MASKED_OLD == filterAlgo_u8)))
#endif /* end of COM_F_MASKEDNEWDIFFERSOLD handling */
            {
                /* Call the Com_Prv_Filter API only if the conditions meet:
                * 1. If the GroupSignal is not invalidated
                * 2. This SignalGroup was not timed out */

                if (COM_FALSE == Com_Prv_Filter((uint32)rxNewValGrpSig_uo, filterAlgo_u8,
                                                rxGrpSigConstPtr_pcst->idxFilter_uo, COM_PRV_NOT_FROM_IPDU_GRP_START))
                {
                    /**
                    * TRACE[SWS_Com_00836] If the AUTOSAR COM module filters out a group signal on receiver side,
                    * i.e. filter condition evaluates to false,
                    * the AUTOSAR COM module shall discard the whole signal group and shall not process it.
                    */
                    validateSigGrp_u8 = COM_SIGGRP_DISCARD;
                }
            }
        }
#endif /* #ifdef COM_RxFilters */
    }

    return validateSigGrp_u8;
}
#endif /* #if defined(COM_RxSigGrpInvalid) || defined(COM_RxFilters) */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsInvalidGrpSigData
 Description      : Checks if received group signal value is equivalent to DataInvalidValue
 Parameter        : idGrpSignal_u16 -> Id of the GroupSignal
                  : rxSrcPtr_pcv    -> Pointer to New Signal value
 Return value     : true  - is invalid value
                    false - is not invalid value
 **********************************************************************************************************************
*/
# ifdef COM_RxSigGrpInvalid

LOCAL_INLINE boolean Com_Prv_IsInvalidGrpSigData(Com_SignalIdType idGrpSig_u16, const void * rxSrcPtr_pcv)
{
    Com_RxGrpSigCfg_tpcst rxGrpSigConstPtr_pcst;
    uint32 invalidVal_u32;
    uint8 sigType_u8;
    boolean isInvalidValue_b;

    isInvalidValue_b = COM_FALSE;

    rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA(idGrpSig_u16);

    invalidVal_u32 = rxGrpSigConstPtr_pcst->dataInvalidVal_u32;

    sigType_u8 = Com_GetValue(GEN,_TYPE,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

    switch (sigType_u8)
    {
#  ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
            const PduInfoType * pduInfoPtr_pcst;
            PduLengthType startByteNo_uo;
            uint16 dynLength_u16;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            pduInfoPtr_pcst = (const PduInfoType *)rxSrcPtr_pcv;

            startByteNo_uo = (rxGrpSigConstPtr_pcst->bitPos_uo >> 3u);

            /* calculate received dynamic signal length */
            dynLength_u16 = Com_Prv_GetRxDynSigLength(pduInfoPtr_pcst->SduLength, rxGrpSigConstPtr_pcst->bitPos_uo,
                                                                                  rxGrpSigConstPtr_pcst->bitSize_uo);

            /* MR12 DIR 1.1, MR12 RULE 13.5 VIOLATION: Evaluation(or no evaluation) of the second part of logical-OR
            * shall not impact the system behaviour. Hence suppressed.
            * Explicit cast is provided in line with COM_MEMCOMPARE definition.
            * But the void pointer is always deferenced to a type, based on the pointers
            * alignment.Hence the alignment warning can safely be ignored. */
            if ((dynLength_u16 == COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigLength_uo) &&
                (COM_MEMCOMPARE((pduInfoPtr_pcst->SduDataPtr + startByteNo_uo),
                                COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigByteArray_pau8,
                                dynLength_u16) == 0x00))
            {

                isInvalidValue_b = COM_TRUE;
            }
        }
        break;
#  endif /* end of COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */
        case COM_UINT8_N:
        {
            const PduInfoType * pduInfoPtr_pcst;
            PduLengthType startByteNo_uo;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            pduInfoPtr_pcst = (const PduInfoType *)rxSrcPtr_pcv;

            startByteNo_uo = (rxGrpSigConstPtr_pcst->bitPos_uo >> 3u);

            /* MR12 DIR 1.1 VIOLATION: usage of generic function to compare memories. But there is no impact
            due to this, hence suppressed. */
            if (COM_MEMCOMPARE((pduInfoPtr_pcst->SduDataPtr + startByteNo_uo),
                               COM_GET_BYTE_ARRAY_SIG_VAL(invalidVal_u32).sigByteArray_pau8,
                               (uint32)(rxGrpSigConstPtr_pcst->bitSize_uo)) == 0x00)
            {

                isInvalidValue_b = COM_TRUE;
            }
        }
        break;
#  ifdef COM_RXGRPSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            const PduInfoType * pduInfoPtr_pcst;
            float64 rxSigNewVal_f64;
            uint8 sigEndianess_u8;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            pduInfoPtr_pcst = (const PduInfoType *)rxSrcPtr_pcv;

            sigEndianess_u8 = Com_GetValue(RXGRPSIG,_ENDIANESS,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

            rxSigNewVal_f64 = Com_Prv_UnPackFloatSignal( sigEndianess_u8, rxGrpSigConstPtr_pcst->bitPos_uo,
                                                         pduInfoPtr_pcst->SduDataPtr );

            /* MR12 DIR 1.1 VIOLATION: usage of generic function to compare memories. But there is no impact
            due to this, hence suppressed. */
            if (COM_MEMCOMPARE(&rxSigNewVal_f64, &Com_getfloat64SigValue(invalidVal_u32),
                               (uint32)(COM_F64_SIZE_IN_BYTES)) == 0x00)
            {

                isInvalidValue_b = COM_TRUE;
            }
        }
        break;
#  endif /* end of COM_RXGRPSIG_FLOAT64SUPP */
#  ifdef COM_RXGRPSIG_INT64
        case COM_UINT64:
        case COM_SINT64:
        {
            const Com_SigMax_tuo * rxSigNewVal_uo;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            rxSigNewVal_uo = (const Com_SigMax_tuo *)rxSrcPtr_pcv;

            if (*rxSigNewVal_uo == (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(invalidVal_u32)))
            {
                isInvalidValue_b = COM_TRUE;
            }
        }
        break;
#  endif /* end of COM_RXGRPSIG_INT64 */
        default:
        {
            const Com_SigMax_tuo * rxSigNewVal_uo;

            /* MR12 RULE 11.5 VIOLATION: pointer is referenced with correct type, hence suppressed. */
            rxSigNewVal_uo = (const Com_SigMax_tuo *)rxSrcPtr_pcv;

            if (*rxSigNewVal_uo == invalidVal_u32)
            {
                isInvalidValue_b = COM_TRUE;
            }
        }
        break;
    }   /* end of switch() */

    return (isInvalidValue_b);
}
# endif /* end of COM_RxSigGrpInvalid */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_GetRxDynSigLength
 Description      : returns received dynamic signal length
 Parameter        : sduLength_uo-> received IPDU length
                  : bitPos_uo   -> bit position
                  : bitSize_uo  -> bit size
 Return value     : dynamic signal length
 **********************************************************************************************************************
*/
# ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT

LOCAL_INLINE uint16 Com_Prv_GetRxDynSigLength(PduLengthType sduLength_uo,
                                               Com_Bitposition_tuo bitPos_uo, Com_Bitsize_tuo bitSize_uo)
{
    PduLengthType startByteNo_uo;
    uint16 dynLength_u16;

    startByteNo_uo = (bitPos_uo >> 3u);

    /* Dynamic Signal is received with 0 byte or more than that */
    /* calculate received dynamic signal length */
    dynLength_u16 = (uint16)(sduLength_uo - startByteNo_uo);

    /* check if received dynamic signal length exceeds the configured length */
    if (dynLength_u16 > (uint16)bitSize_uo)
    {
        /* if so, consider configured dynamic signal length */
        dynLength_u16 = (uint16)bitSize_uo;
    }
    else
    {
        /* if not, consider partial received dynamic signal length */
    }

    return dynLength_u16;
}
# endif /* end of COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* end of COM_RX_SIGNALGROUP */

