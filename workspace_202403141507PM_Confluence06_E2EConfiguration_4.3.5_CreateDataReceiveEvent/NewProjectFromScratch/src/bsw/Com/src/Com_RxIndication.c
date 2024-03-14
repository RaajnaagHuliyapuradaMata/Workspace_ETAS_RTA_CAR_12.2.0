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
#ifdef COM_RX_IPDUCOUNTER
LOCAL_INLINE boolean Com_Prv_ProcessRxIPduCounter(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst);
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
 Function name    : Com_RxIndication
 Description      : Service called by the lower layer after an I-PDU has been received.
 Parameter        : idRxPdu_uo,pduInfoPtr_pcst
 Return value     : None
 **********************************************************************************************************************
*/
void Com_RxIndication(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;

#if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_RxIndication, COM_E_UNINIT);
    }
    else if (pduInfoPtr_pcst == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_RxIndication, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidRxIpduId(idRxPdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_RxIndication, COM_E_PARAM);
    }
    else
#endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idRxPdu_uo              = COM_GET_RX_IPDU_ID(idRxPdu_uo);

        rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
        rxIpduRamPtr_pst        = rxIpduConstPtr_pcst->ipduRam_pst;

        /* Check if the Ipdu group containing this IPDU is started */
        if (Com_Prv_CheckRxIPduStatus(idRxPdu_uo))
        {
            PduLengthType rxIPduLengthCfg_uo;
            boolean sigProcessing_b;

            rxIPduLengthCfg_uo = COM_GET_RXIPDU_SIZE(idRxPdu_uo);

            sigProcessing_b = Com_GetValue(RXIPDU,_SIGPROC,rxIpduConstPtr_pcst->rxIPduFields_u8);

            /* Long lock: To avoid mulitple short locks. If ComEnablePduBasedLocks parameter is enabled,
            * then long lock will be enabled, otherwise short lock will be enabled */
            Com_Get_SchM_Enter_Com_RxIndication
            Com_Get_SchM_Enter_Com_RxPduBuffer

            /* The AUTOSAR COM module does not copy or handle additional received data for not configured signals in
            * case the received data length is greater than expected. */
            rxIpduRamPtr_pst->rxIPduLength_uo = ((pduInfoPtr_pcst->SduLength > rxIPduLengthCfg_uo) ?
                                                 (rxIPduLengthCfg_uo) : (pduInfoPtr_pcst->SduLength));

#if defined (COM_RxIPduDeferredProcessing) || defined (COM_COPY_IMMEDIATE_RXIPDU)
#ifndef COM_COPY_IMMEDIATE_RXIPDU
            if (COM_DEFERRED == sigProcessing_b)
#endif
            {
                /* Now Copy the received Data into the RxIPDU buffer */
                Com_ByteCopy(rxIpduConstPtr_pcst->buffPtr_pau8, (uint8*)(pduInfoPtr_pcst->SduDataPtr),
                              rxIpduRamPtr_pst->rxIPduLength_uo);
            }
#endif /* #if defined (COM_RxIPduDeferredProcessing) || defined (COM_COPY_IMMEDIATE_RXIPDU) */

            /* This operation of setting the bit-field is moved inside the lock, to avoid corrupting the bit-fields */
            Com_SetRamValue(RXIPDU,_INDICATION, rxIpduRamPtr_pst->rxFlags_u16, COM_TRUE);

#if defined(COM_METADATA_SUPPORT)
            /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
            * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
            * system behaviour. Hence suppressed. */
            if ((COM_DEFERRED == sigProcessing_b)
                && (pduInfoPtr_pcst->MetaDataPtr != NULL_PTR)
                && Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
            {

                SchM_Enter_Com_RxIpduProtArea(RECEIVE_METADATA);

                /* Copy the given MetaData into RxMetaData internal buffer */
                Com_ByteCopy(&rxIpduRamPtr_pst->rxMetadataBuf_au8[0],
                            (uint8*)(pduInfoPtr_pcst->MetaDataPtr),
                             COM_GET_RXMETADATA(idRxPdu_uo)->rxMetaDataLength_u8);

                SchM_Exit_Com_RxIpduProtArea(RECEIVE_METADATA);
            }
#endif

            Com_Get_SchM_Exit_Com_RxPduBuffer
            Com_Get_SchM_Exit_Com_RxIndication

            if (COM_IMMEDIATE == sigProcessing_b)
            {
#if (defined(COM_RxIPduCallOuts) || defined(COM_RX_IPDUCOUNTER))

                /* In case callout/counter mismatch occurs, IPDU can be discarded */
                if (Com_Prv_IsValidRxIpdu(idRxPdu_uo, pduInfoPtr_pcst))
#endif
                {
                    Com_Prv_ProcessRxIPdu(idRxPdu_uo, pduInfoPtr_pcst);
                }
            }
        }
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ProcessRxIPdu
 Description      : Service called by the lower layer after an I-PDU has been received.
 Parameter        : idRxPdu_uo    - Id of the Rx ipdu
                  : pduInfoPtr_pcst - Pointer to the buffer + sdu length where the received data is stored
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_ProcessRxIPdu(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst         rxIpduConstPtr_pcst;
#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
    Com_RxIpduRam_tpst          rxIpduRamPtr_pst;
    boolean                     isGwRxIpdu_b;
#endif

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
#if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
    rxIpduRamPtr_pst   = rxIpduConstPtr_pcst->ipduRam_pst;

    isGwRxIpdu_b = Com_GetValue(RXIPDU,_ISGWPDU,rxIpduConstPtr_pcst->rxIPduFields_u8);

#endif /* #if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY)*/

    /* This lock is required only if ComIPduProcessing is IMMEDIATE, as it could interefere with
    * Com_MainFunctionRx() for time-out related part */
    Com_Get_SchM_Enter_Com_RxIndication

    /* Process all the Signals in the IPdu
    * 1. Update bit validation for signals with Update bit configured
    * 2. Timeout monitoring for Signals with Update bit
    * 3. Data Invalid value validation
    * 4. Validation of filters for Signals with filter configured */
    if (rxIpduConstPtr_pcst->numOfSig_u16 > COM_ZERO)
    {
        Com_Prv_ProcessSignal(idRxPdu_uo, pduInfoPtr_pcst);
    }

#ifdef COM_RX_SIGNALGROUP

    /* Process all the Signal groups in the IPdu
    * 1. Update bit validation for signal groups with Update bit configured
    * 2. Timeout monitoring for signal groups with Update bit
    * 3. Data Invalid value validation for group signals with Invalid value configured */
    if (rxIpduConstPtr_pcst->numOfSigGrp_u16 > COM_ZERO)
    {
        Com_Prv_ProcessSignalGroup(idRxPdu_uo, pduInfoPtr_pcst);
    }
#endif /* #ifdef COM_RX_SIGNALGROUP */

#if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY)

    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of
    * intermediate variables to store and read here, may read obsolete value.
    * Evaluation(or no evaluation) shall not impact the system behaviour.
    * Hence suppressed. */
    if ((isGwRxIpdu_b) &&
        (Com_GetRamValue(RXIPDU,_GWSIG_UPDATED,rxIpduRamPtr_pst->rxFlags_u16)))
    {

        /* here lock is required only to protect RxFlags.
        * As it is critical to the queue addition */
        Com_Get_SchM_Enter_Com_RxPduBuffer
        Com_SetRamValue(RXIPDU,_GWSIG_UPDATED,rxIpduRamPtr_pst->rxFlags_u16,COM_FALSE);
        Com_SetRamValue(RXIPDU,_GWPDU_TOBEPROCESSED,rxIpduRamPtr_pst->rxFlags_u16,COM_TRUE);
        Com_Get_SchM_Exit_Com_RxPduBuffer
    }
#endif /* #if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY) */

#if defined(COM_METADATA_SUPPORT)

    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
    * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
    * system behaviour. Hence suppressed. */
    if ((pduInfoPtr_pcst->MetaDataPtr != NULL_PTR) &&
        Com_GetValue(RXIPDU,_IS_METADATAPDU,rxIpduConstPtr_pcst->rxIPduFields_u8))
    {

        Com_Get_SchM_Enter_Com_RxIPduMetaData

        /* Copy the given MetaData into RxMetaData internal buffer */
        Com_ByteCopy( COM_GET_RXMETADATA(idRxPdu_uo)->rxMetaDataValue_pau8,
                      (uint8*)(pduInfoPtr_pcst->MetaDataPtr),
                      COM_GET_RXMETADATA(idRxPdu_uo)->rxMetaDataLength_u8);

        Com_Get_SchM_Exit_Com_RxIPduMetaData
    }

#endif /* #if defined(COM_METADATA_SUPPORT) */

    Com_Get_SchM_Exit_Com_RxIndication

#if defined(COM_RxIPduNotification) || defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify)

    Com_Prv_InvokeRxNotifications( idRxPdu_uo );

#endif
}


#if defined(COM_RxIPduCallOuts) || defined(COM_RX_IPDUCOUNTER)
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_IsValidRxIpdu
 Description      : Check for configured IPDU counter or IPDU callout
 Parameter        : idPdu_uo    - Id of the Rx I-PDU
                  : pduInfoPtr_pcst - Pointer to the PduInfo(buffer + length) where the received data is stored
 Return value     : true  - Proceed further to process RxIPdu
                  : false - discard received RxIPdu
 **********************************************************************************************************************
*/
boolean Com_Prv_IsValidRxIpdu(PduIdType idPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst     rxIpduConstPtr_pcst;
    boolean                 isValidRxIpdu_b;

    isValidRxIpdu_b   = COM_TRUE;

    rxIpduConstPtr_pcst  = COM_GET_RX_IPDU_CONSTDATA(idPdu_uo);

#ifdef COM_RX_IPDUCOUNTER

    /* If IPduCounter is configured */
    if (rxIpduConstPtr_pcst->idxRxIpduCntr_uo != COM_RXIPDU_CNTR_INV_IDX)
    {
        /* If received counter value is not within the expected counter range, then no further processing is done. */
        isValidRxIpdu_b = Com_Prv_ProcessRxIPduCounter(idPdu_uo, pduInfoPtr_pcst);
    }
    else
    {
        /* do nothing: If Ipdu counter is not configured, then proceed further to process Ipdu */
    }

#endif /* End of #ifdef COM_RX_IPDUCOUNTER */

#ifdef COM_RxIPduCallOuts

    /* If IPdu callout is configured for the Pdu, Callout is called,
    * if it returns false, no further processing is done */
    if (
# ifdef COM_RX_IPDUCOUNTER
            (isValidRxIpdu_b) &&
# endif
            (rxIpduConstPtr_pcst->callOut_pfct != NULL_PTR)
       )
    {
        isValidRxIpdu_b =
                rxIpduConstPtr_pcst->callOut_pfct((PduIdType)COM_GET_RXIPDU_HANDLEID(idPdu_uo), pduInfoPtr_pcst);
    }
    else
    {
        /* If callout is not configured, proceed further to process Ipdu */
    }

#endif /* End of #ifdef COM_RxIPduCallOuts */

    return isValidRxIpdu_b;
}

#endif /* End of #if defined(COM_RX_IPDUCOUNTER) && defined(COM_RxIPduCallOuts) */


#ifdef COM_RX_IPDUCOUNTER
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_ProcessRxIPduCounter
 Description      : Process RxIPdu counter and check if the frame can be processed further
 Parameter        : idRxPdu_uo    - Id of the Rx I-PDU
                  : pduInfoPtr_pcst - Pointer to the PduInfo(buffer + length) where the received data is stored
 Return value     : COM_ACCEPT - Proceed further to process RxIPdu
                  : COM_REJECT - Ignore received RxIPdu
 **********************************************************************************************************************
*/
LOCAL_INLINE boolean Com_Prv_ProcessRxIPduCounter(PduIdType idRxPdu_uo, const PduInfoType * pduInfoPtr_pcst)
{
    Com_RxIpduCfg_tpcst             rxIpduConstPtr_pcst;
    Com_RxIpduRam_tpst              rxIpduRamPtr_pst;
    Com_RxIpduCntrCfg_tpcst         rxIpduCntrConstPtr_pcst;
    uint8 *                         counterBytePtr_pu8;
    PduLengthType                   counterByteNo_uo;
    uint16                          counterMaxValue_u16;
    uint8                           rxCounterValue_u8;
    uint8                           counterMinExpValue_u8;
    uint8                           counterMaxExpValue_u8;
    boolean                         counterStatus_b;

    counterStatus_b         = COM_ACCEPT;

    rxIpduConstPtr_pcst     = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
    rxIpduCntrConstPtr_pcst = COM_GET_RX_IPDU_CNTR_CONSTDATA(rxIpduConstPtr_pcst->idxRxIpduCntr_uo);

    counterByteNo_uo   = (PduLengthType)(rxIpduCntrConstPtr_pcst->bitPos_uo / 8u);

    /* Check if counter byte is received */
    if (counterByteNo_uo < pduInfoPtr_pcst->SduLength)
    {
        rxIpduRamPtr_pst      = rxIpduConstPtr_pcst->ipduRam_pst;

        counterBytePtr_pu8    = (uint8 *)(pduInfoPtr_pcst->SduDataPtr + counterByteNo_uo);

        rxCounterValue_u8 = COM_UNPACK_CNTR_FROM_IPDU_BUFF(
                                                    (*counterBytePtr_pu8),
                                             (uint8)(rxIpduCntrConstPtr_pcst->bitPos_uo % 8u),
                                                     rxIpduCntrConstPtr_pcst->bitSize_u8
                                                          );

        /* Check if this is not the first reception after Ipdu is started */
        if (!(Com_GetRamValue(RXIPDU,_IS_FIRST_RECEPTION,rxIpduRamPtr_pst->rxFlags_u16)))
        {
            /* Calculate 2^BitSize */
            counterMaxValue_u16   = (uint16)COM_GET_BASE_2_POWER_X(rxIpduCntrConstPtr_pcst->bitSize_u8);

            /* Calculate expected counter range based upon last received counter value */
            counterMinExpValue_u8 = COM_GET_CNTR_WRAP_AROUND_VALUE(
            ((uint16)rxIpduRamPtr_pst->rxIpduLastCntrValue_u8 + COM_ONE),
            counterMaxValue_u16);
            counterMaxExpValue_u8 = COM_GET_CNTR_WRAP_AROUND_VALUE(
                 ((uint16)counterMinExpValue_u8 + (uint16)rxIpduCntrConstPtr_pcst->threshold_u8), counterMaxValue_u16
                                                                  );

            /* TRACE[SWS_Com_00590] At reception of an I-PDU containing an I-PDU counter, the AUTOSAR COM module
            * shall discard the I-PDU, after setting the next expected value as defined in SWS_Com_00588,
            * if no I-PDU replication for this I-PDU is configured and:
            * received I-PDU counter < expected I-PDU counter OR
            * received I-PDU counter > expected I-PDU counter + ComIPduCounterThreshold
            * with respect to counter wrap-around.
            */
            if (counterMinExpValue_u8 <= counterMaxExpValue_u8)
            {
                /* Check if received counter is within the expected range */
                if (!((rxCounterValue_u8 < counterMinExpValue_u8) || (rxCounterValue_u8 > counterMaxExpValue_u8)))
                {
                    /* Accept RxIPdu */
                }
                else
                {
                    /* Reject RxIPdu */
                    counterStatus_b = COM_REJECT;
                }
            }
            /* (counterMinExpValue_u8 > counterMaxExpValue_u8) */
            /* Check if received counter is within the expected range */
            else if (!((rxCounterValue_u8 < counterMinExpValue_u8) && (rxCounterValue_u8 > counterMaxExpValue_u8)))
            {
                /* Accept RxIPdu */
            }
            else
            {
                /* Reject RxIPdu */
                counterStatus_b = COM_REJECT;
            }

            /* TRACE[SWS_Com_00727] In case a mismatch of the expected and the received I-PDU counter is detected,
            * the AUTOSAR COM module shall notify this mismatch independently of the configured Threshold. */
            if ((rxCounterValue_u8 != counterMinExpValue_u8) &&
                                    (rxIpduCntrConstPtr_pcst->counterErrNotification_pfct != NULL_PTR))
            {
                rxIpduCntrConstPtr_pcst->counterErrNotification_pfct((PduIdType)COM_GET_RXIPDU_HANDLEID(idRxPdu_uo),
                                                                            counterMinExpValue_u8, rxCounterValue_u8);
            }
            else
            {
                /* do nothing: error notification is not configured */
            }
        }
        else
        {
            /* TRACE[SWS_Com_00587] For all I-PDUs with ComIPduDirection configured to RECEIVE that have a configured
            * ComIPduCounter, the AUTOSAR COM module shall accept any incoming I-PDU, regardless of the value of
            * the I-PDU counter, after the I-PDU was initialized by Com_Init or reinitialized by Com_IpduGroupStart.
            */
            Com_SetRamValue(RXIPDU,_IS_FIRST_RECEPTION,rxIpduRamPtr_pst->rxFlags_u16,COM_FALSE);
        }

        /* TRACE[SWS_Com_00588] At reception of an I-PDU containing an I-PDU counter, the AUTOSAR COM module shall
        * set the next expected value to the value following the received value with respect
        * to counter wrap-around. */
        /* DesignDecision: In our implementation only last received counter is stored, next expected value is
        * calculated in the next reception. */
        rxIpduRamPtr_pst->rxIpduLastCntrValue_u8 = rxCounterValue_u8;
    }
    else
    {
        /* Accept RxIPdu.
        * DesignDecision: But expected counter value is unchanged, in case counter byte is not received */
    }

    return counterStatus_b;

} /* End of Com_Prv_ProcessRxIPduCounter */

#endif /* #ifdef COM_RX_IPDUCOUNTER */


#if defined(COM_RxIPduNotification) || defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify)

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_InvokeRxNotifications
 Description      : Invoke notifications configured at RxIPdu, Signal, SignalGroup
 Parameter        : idRxPdu_uo - Rx-ComIPdu ID
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_InvokeRxNotifications(PduIdType idRxPdu_uo)
{
    Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst;
# if defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify)
    const Com_Prv_xRxIpduVarCfg_tst * rxIpduVarCfg_pcst;
    uint16_least index_qu16;
# endif

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo);
# if defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify)
    rxIpduVarCfg_pcst = COM_GET_RX_IPDU_VAR_CFG(idRxPdu_uo);
# endif

# ifdef COM_RxIPduNotification

    /* Ipdu notification is called for the Ipdu */
    if (rxIpduConstPtr_pcst->notification_pfct != NULL_PTR)
    {
        rxIpduConstPtr_pcst->notification_pfct();
    }
# endif

# ifdef COM_RxSignalNotify
    /* Check if any signal notification callbacks are configured for the IPdu */
    if (Com_GetValue(RXIPDUVAR,_IS_SIG_ACKCBK,rxIpduVarCfg_pcst->fields_u8))
    {
#  ifdef COM_PRV_ENABLECONFIGINTERFACES
        const Com_RxSigConfigType_tst * rxSigCfgInterface_pcst;
#  endif
        Com_RxSigCfg_tpcst rxSigConstPtr_pcst;
#  ifdef COM_CALLBACK_RX
        const Com_Prv_xRxSigVarCfg_tst * rxSigVarCfg_pcst;
#  endif

#  ifdef COM_PRV_ENABLECONFIGINTERFACES
        rxSigCfgInterface_pcst = &(Com_GlobalConfig_pcst->rxSigCfg_pcst[rxIpduConstPtr_pcst->idRxSig_uo]);
#  endif

        rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(rxIpduConstPtr_pcst->idRxSig_uo);
#  ifdef COM_CALLBACK_RX
        rxSigVarCfg_pcst = COM_GET_RXSIG_VAR_CFG(rxIpduConstPtr_pcst->idRxSig_uo);
#  endif

        /* Signal notifications are called for all the signals with notifications configured */
        for (index_qu16 = rxIpduConstPtr_pcst->numOfSig_u16; index_qu16 != COM_ZERO; index_qu16--)
        {
            if (Com_GetRamValue(RXSIG,_SIGNOTIF,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8)))
            {
                Com_SetRamValue(RXSIG,_SIGNOTIF,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8),COM_FALSE);

#  ifdef COM_PRV_ENABLECONFIGINTERFACES
                Com_SetRamValue(RXSIGCFG,_SIGNOTIF,*(rxSigCfgInterface_pcst->rxSigRAMFields_pu8),COM_FALSE);
#  endif
                /* either ComNotification or UserCallback(s) will be configured for Rx-Signal */
                if (rxSigConstPtr_pcst->notification_pfct != NULL_PTR)
                {
                    rxSigConstPtr_pcst->notification_pfct();
                }
#  ifdef COM_CALLBACK_RX
                else if (rxSigVarCfg_pcst->ackCbk_pcauo != NULL_PTR)
                {
                    Com_Prv_InvokeRxUserCbks(rxSigVarCfg_pcst->ackCbk_pcauo, rxSigConstPtr_pcst->idHandle_u16);
                }
#  endif /* end of COM_CALLBACK_RX */
            }
            rxSigConstPtr_pcst++;
#  ifdef COM_CALLBACK_RX
            rxSigVarCfg_pcst++;
#  endif
#  ifdef COM_PRV_ENABLECONFIGINTERFACES
            rxSigCfgInterface_pcst++;
#  endif
        }
    }
# endif /* COM_RxSignalNotify */

# ifdef COM_RxSignalGrpNotify
    /* Check if any signal-group notification callbacks are configured for the IPdu */
    if (Com_GetValue(RXIPDUVAR,_IS_SIGGRP_ACKCBK,rxIpduVarCfg_pcst->fields_u8))
    {
#  ifdef COM_PRV_ENABLECONFIGINTERFACES
        const Com_RxSigGrpConfigType_tst * rxSigGrpCfgInterface_pcst;
#  endif
        Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst;
#  ifdef COM_CALLBACK_RX
        const Com_Prv_xRxSigGrpVarCfg_tst * rxSigGrpVarCfg_pcst;
#  endif

#  ifdef COM_PRV_ENABLECONFIGINTERFACES
        rxSigGrpCfgInterface_pcst =
                &(Com_GlobalConfig_pcst->rxSigGrpCfg_pcst[rxIpduConstPtr_pcst->idFirstRxSigGrp_uo]);
#  endif

        rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);
#  ifdef COM_CALLBACK_RX
        rxSigGrpVarCfg_pcst = COM_GET_RXSIGGRP_VAR_CFG(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);
#  endif

        /* Signal group notifications are called for all the signal groups with notifications configured */
        for (index_qu16 = rxIpduConstPtr_pcst->numOfSigGrp_u16; index_qu16 != COM_ZERO; index_qu16--)
        {
            if (Com_GetRamValue(RXSIGGRP,_SIGNOTIF,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8))
            {
                Com_SetRamValue(RXSIGGRP,_SIGNOTIF,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8,
                                                                                                       COM_FALSE);
#  ifdef COM_PRV_ENABLECONFIGINTERFACES
                Com_SetRamValue(RXSIGGRPCFG,_SIGNOTIF,*(rxSigGrpCfgInterface_pcst->rxSigGrpRAMFields_pu8),
                                                                                                        COM_FALSE);
#  endif
                /* either ComNotification or UserCallback(s) will be configured for Rx-SignalGroup */
                if (rxSigGrpConstPtr_pcst->notification_pfct != NULL_PTR)
                {
                    rxSigGrpConstPtr_pcst->notification_pfct();
                }
#  ifdef COM_CALLBACK_RX
                else if (rxSigGrpVarCfg_pcst->ackCbk_pcauo != NULL_PTR)
                {
                    Com_Prv_InvokeRxUserCbks(rxSigGrpVarCfg_pcst->ackCbk_pcauo, rxSigGrpConstPtr_pcst->idHandle_u16);
                }
#  endif /* end of COM_CALLBACK_RX */
            }
            rxSigGrpConstPtr_pcst++;
#  ifdef COM_CALLBACK_RX
            rxSigGrpVarCfg_pcst++;
#  endif
#  ifdef COM_PRV_ENABLECONFIGINTERFACES
            rxSigGrpCfgInterface_pcst++;
#  endif
        }
    }
# endif /* #ifdef COM_RxSignalGrpNotify */

}

#endif /* #if defined(COM_RxIPduNotification) || defined(COM_RxSignalNotify) || defined(COM_RxSignalGrpNotify) */


/* FC_VariationPoint_START */
/*
 **********************************************************************************************************************

 Function name    : Com_ReadRxIPduLength
 Description      : Service for Reading the length of the received IPdu
                    The API returns COM_SERVICE_NOT_AVAILABLE in case the RxPduId is not in range or
                    the corresponding IPdu Group is not started and updates the RxIPdu length with Zero.
                    In normal case it returns E_OK by updating the RxIPdu length.
 Parameter        : idRxPdu_uo         - ID of the reception IPDU
                    rxIPduLengthPtr_puo - memory location to update the PduLength for the requested RxIPDU
 Return value     : E_OK/COM_SERVICE_NOT_AVAILABLE
 **********************************************************************************************************************
*/
#ifdef COM_ENABLE_READRXIPDULENGTH

uint8 Com_ReadRxIPduLength(PduIdType idRxPdu_uo, PduLengthType * rxIPduLengthPtr_puo)
{
    uint8       status_u8;

    status_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReadRxIPduLength, COM_E_UNINIT);
    }
    else if (rxIPduLengthPtr_puo == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReadRxIPduLength, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidRxIpduId(idRxPdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_ReadRxIPduLength, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idRxPdu_uo  = COM_GET_RX_IPDU_ID(idRxPdu_uo);

        /* Check if Ipdu Group is started */
        if (Com_Prv_CheckRxIPduStatus(idRxPdu_uo))
        {
            *rxIPduLengthPtr_puo = COM_GET_RX_IPDU_CONSTDATA(idRxPdu_uo)->ipduRam_pst->rxIPduLength_uo;
            status_u8 = E_OK;
        }
        else
        {
            /* This part will be executed when IPDU Group is Stopped (Zero is returned as IPdu length) */
            *rxIPduLengthPtr_puo = COM_ZERO;
        }
    }

    return(status_u8);
}

#endif /* #ifdef COM_ENABLE_READRXIPDULENGTH */
/* FC_VariationPoint_END */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

