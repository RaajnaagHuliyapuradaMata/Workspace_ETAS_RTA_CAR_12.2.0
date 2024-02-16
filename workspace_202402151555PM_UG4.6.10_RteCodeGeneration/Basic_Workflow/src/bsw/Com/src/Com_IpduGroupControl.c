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

#if defined(COM_TxSigUpdateBit) || defined(COM_TxSigGrpUpdateBit) || (defined(COM_TxFilters) && \
defined(COM_F_ONEEVERYN))
LOCAL_INLINE void Com_Prv_UpdateBitAndFilter(Com_IpduId_tuo ipduId_uo);
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
 Function name    : Com_Prv_RxIPduStart
 Description      : Service to start the Rx-IPDU
 Parameter        : idIpdu_uo       - Id of the Rx-IPDU
                  : initialize_b    - flag to request initialization of the I-PDUs which are newly started
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_RxIPduStart(Com_IpduId_tuo idIpdu_uo, boolean initialize_b)
{
#if (defined(COM_RxFilters) && defined(COM_F_ONEEVERYN))
    Com_RxIpduCfg_tpcst             rxIpduConstPtr_pcst;
    Com_RxSigCfg_tpcst              rxSigConstPtr_pcst;
    uint16_least                    index_qu16;  /* Index for both Rx-SignalGroup and Rx-Signal */
# ifdef COM_RX_SIGNALGROUP
    Com_RxSigGrpCfg_tpcst           rxSigGrpConstPtr_pcst;
    Com_RxGrpSigCfg_tpcst           rxGrpSigConstPtr_pcst;
    uint16_least                    idxGrpSig_qu16;   /* Index for Rx-GroupSignal */
# endif

    rxIpduConstPtr_pcst   = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

    /* Rx-SignalGroup */
# ifdef COM_RX_SIGNALGROUP
    rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);

    for (index_qu16 = rxIpduConstPtr_pcst->numOfSigGrp_u16; index_qu16 != COM_ZERO; index_qu16--)
    {
        if (Com_GetValue(RXSIGGRP,_FILTEREVALREQ,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8))
        {
            rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA(rxSigGrpConstPtr_pcst->idFirstGrpSig_uo);

            for (idxGrpSig_qu16 = rxSigGrpConstPtr_pcst->numOfGrpSig_uo; idxGrpSig_qu16 != COM_ZERO; idxGrpSig_qu16--)
            {
                if (rxGrpSigConstPtr_pcst->filterAlgo_u8 == COM_ONE_EVERY_N)
                {
                    /* TRACE[SWS_Com_00787] reset OCCURRENCE of filter with ComFilterAlgorithm ONE_EVERY_N */
                    (*(COM_GET_ONEEVERYN_CONST(rxGrpSigConstPtr_pcst->idxFilter_uo).occurrence_pu32)) = COM_ZERO;
                }
                rxGrpSigConstPtr_pcst++;
            }
        }
        rxSigGrpConstPtr_pcst++;
    }
# endif /* #ifdef COM_RX_SIGNALGROUP */

    /* Rx-Signal */
    rxSigConstPtr_pcst  = COM_GET_RXSIG_CONSTDATA(rxIpduConstPtr_pcst->idRxSig_uo);

    for (index_qu16 = rxIpduConstPtr_pcst->numOfSig_u16; index_qu16 != COM_ZERO; index_qu16--)
    {
        if (Com_GetValue(RXSIG,_FILTRALG,rxSigConstPtr_pcst->rxSignalFields_u16) == COM_ONE_EVERY_N)
        {
            /* TRACE[SWS_Com_00787] reset OCCURRENCE of filter with ComFilterAlgorithm ONE_EVERY_N */
            (*(COM_GET_ONEEVERYN_CONST(rxSigConstPtr_pcst->idxFilter_uo).occurrence_pu32)) = COM_ZERO;
        }
        rxSigConstPtr_pcst++;
    }
#endif/* #if (defined(COM_RxFilters) && defined(COM_F_ONEEVERYN)) */

    /* conditionally used variable(s) is type-cast to 'void' to avoid warnings,
     * they are intentionally not placed under compiler switch statement,
     * so that these lines are always available for compilation*/
    (void)idIpdu_uo;

    (void)initialize_b; /* This variable is retained for future use */

#ifdef COM_RX_IPDUCOUNTER
    /* Set the flag until first successful reception of an Rx IPDU with counter */
    Com_SetRamValue(RXIPDU,_IS_FIRST_RECEPTION,
                    COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->rxFlags_u16,COM_TRUE);
#endif

}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_UpdateBitAndFilter
 Description      : Service to clear update bit if configured for all the configured signal and signalgroup
 Parameter        : ipduId_uo      -> Id of the ipdu
 Return value     : None
 **********************************************************************************************************************
*/
#if defined(COM_TxSigUpdateBit) || defined(COM_TxSigGrpUpdateBit) || (defined(COM_TxFilters) && \
defined(COM_F_ONEEVERYN))

LOCAL_INLINE void Com_Prv_UpdateBitAndFilter(Com_IpduId_tuo ipduId_uo)
{

    Com_TxIpduCfg_tpcst             txIpduConstPtr_pcst;
#if defined(COM_TxSigUpdateBit) || (defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))
    Com_TxSigCfg_tpcst              txSigConstPtr_pcst;
#endif

#if defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) && defined(COM_F_ONEEVERYN)
    Com_TxGrpSigCfg_tpcst           txGrpSigConstPtr_pcst;
#endif

#if defined(COM_TxSigGrpUpdateBit) || \
    (defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
    uint16_least                    idxSigGrp_qu16;
#endif

# if defined(COM_TxSigUpdateBit) || (defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))
    uint16_least                    idxSig_qu16;
# endif

#if defined (COM_TxSigUpdateBit) || defined(COM_TxSigGrpUpdateBit)
    PduLengthType                   byteNo_uo;
#endif

    txIpduConstPtr_pcst   = COM_GET_TX_IPDU_CONSTDATA(ipduId_uo);

#if defined(COM_TxSigUpdateBit) || (defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))

    txSigConstPtr_pcst    = COM_GET_TXSIG_CONSTDATA(txIpduConstPtr_pcst->idTxSig_uo);

    for (idxSig_qu16 = txIpduConstPtr_pcst->numOfSig_u16; idxSig_qu16 > COM_ZERO; idxSig_qu16--)
    {

# ifdef COM_TxSigUpdateBit

        /* TRACE[SWS_Com_00787] all included update-bits shall be cleared */
        if (Com_GetValue(GEN,_UPDBITCONF,txSigConstPtr_pcst->general_u8))
        {
            /* Find the Byte No, where the Update bit lies */
            byteNo_uo = (PduLengthType)((txSigConstPtr_pcst->updateBitPos_uo) >> 3u);
            /* This macro clears the update bit value at the Update bit position */
            Com_ClearUpdateBitValue(txIpduConstPtr_pcst,txSigConstPtr_pcst->updateBitPos_uo,byteNo_uo)
        }
# endif /* # ifdef COM_TxSigUpdateBit */

# if defined(COM_TxFilters) && defined(COM_F_ONEEVERYN)

        /* OCCURRENCE shall be set to zero by Com_IpduGroupStart*/
        if (Com_GetValue(TXSIG,_FILTRALG,txSigConstPtr_pcst->txSignalFields_u16) == COM_ONE_EVERY_N)
        {
            (*(COM_GET_ONEEVERYN_CONST(txSigConstPtr_pcst->idxFilter_uo).occurrence_pu32)) = COM_ZERO;
        }
# endif /* # if defined(COM_TxFilters) && defined(COM_F_ONEEVERYN) */

        txSigConstPtr_pcst++;
    }

#endif /* #if defined(COM_TxSigUpdateBit) || (defined(COM_TxFilters) && defined(COM_F_ONEEVERYN)) */


#if defined(COM_TxSigGrpUpdateBit) || \
    (defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))

    txSigGrpConstPtr_pcst   = COM_GET_TXSIGGRP_CONSTDATA(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);

    for (idxSigGrp_qu16 = txIpduConstPtr_pcst->numOfSigGroups_u16; idxSigGrp_qu16 > COM_ZERO; idxSigGrp_qu16--)
    {

# if (defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) && defined(COM_F_ONEEVERYN))

        txGrpSigConstPtr_pcst = COM_GET_TXGRPSIG_CONSTDATA(txSigGrpConstPtr_pcst->idFirstGrpSig_uo);

        for (idxSig_qu16 = txSigGrpConstPtr_pcst->numOfGrpSig_uo; idxSig_qu16 > COM_ZERO; idxSig_qu16--)
        {
            /* OCCURRENCE shall be set to zero by Com_IpduGroupStart*/
            if (txGrpSigConstPtr_pcst->filterAlgo_u8 == COM_ONE_EVERY_N)
            {
                (*(COM_GET_ONEEVERYN_CONST(txGrpSigConstPtr_pcst->idxFilter_uo).occurrence_pu32)) = COM_ZERO;
            }
            txGrpSigConstPtr_pcst++;
        }
# endif /* # if (defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) && defined(COM_F_ONEEVERYN)) */

# ifdef COM_TxSigGrpUpdateBit

        /* TRACE[SWS_Com_00787] all included update-bits shall be cleared */
        if (Com_GetValue(TXSIGGRP,_UPDBITCONF,txSigGrpConstPtr_pcst->txSignalGrpFields_u8))
        {
            /*Find the Byte No, where the Update bit lies */
            byteNo_uo = (PduLengthType)((txSigGrpConstPtr_pcst->updateBitPos_uo) >> 3u);
            /* This macro clears the update bit value at the Update bit position */
            Com_ClearUpdateBitValue(txIpduConstPtr_pcst,txSigGrpConstPtr_pcst->updateBitPos_uo,byteNo_uo)
        }
# endif  /* # ifdef COM_TxSigGrpUpdateBit */

        txSigGrpConstPtr_pcst++;
    }
#endif /* # if defined(COM_TxSigGrpUpdateBit) || (defined(COM_TX_SIGNALGROUP) && defined(COM_TxFilters) &&
               defined(COM_F_ONEEVERYN)) */
}

#endif /* #if defined(COM_TxSigUpdateBit) || defined(COM_TxSigGrpUpdateBit) || (defined(COM_TxFilters) &&
              defined(COM_F_ONEEVERYN)) */

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIPduStart
 Description      : Service to start the Tx ipdu
 Parameter        : idIpdu_uo     -> Id of the ipdu
                  : initialize_b  -> flag to request initialization of the I-PDUs which are newly started
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_TxIPduStart(Com_IpduId_tuo idIpdu_uo, boolean initialize_b)
{
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    uint8                       latestTransMode_u8;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    txIpduRamPtr_pst    = txIpduConstPtr_pcst->ipduRam_pst;

    /* The below Initialize check should be only after Com_LatestTransMode is calculated */
    if (initialize_b)
    {
#if defined(COM_TxSigUpdateBit) || defined(COM_TxSigGrpUpdateBit) || (defined(COM_TxFilters) && \
defined(COM_F_ONEEVERYN))
        /* Call function to update bit   */
        Com_Prv_UpdateBitAndFilter(idIpdu_uo);
#endif

        /* Call Com_Prv_TxChangeMode( ) to calculate the _LATESTMODE */
        Com_Prv_TxChangeMode(idIpdu_uo);
        /* Note: Even though cntrTimePeriod_u16 is updated in Com_Prv_TxChangeMode(),
        it is once again updated in Ipdu Group Start i.e below */
        latestTransMode_u8 = Com_GetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8);

        /* Reset the global TMS status flag on the start of IPduGroup */
        Com_SetRamValue(TXIPDU,_GWIPDUTMSCHANGED,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);

        /* time period and offset attributes of IPDUs in Periodic or Mixed Transmission Mode */
        /* The CurrentTxModePtr points to the structure information of the current TxModeState */
        if ( COM_TX_MODE_IS_CYCLIC(latestTransMode_u8) )
        {
            /* I.e Load the TxTimeOffset for cyclic type Pdu's(PERIODIC or MIXED) */
            txIpduRamPtr_pst->cntrTimePeriod_u16 =
                                    COM_GET_TXIPDU_TIMEOFFSET(idIpdu_uo, txIpduRamPtr_pst->currentTxModePtr_pcst);
        }

        /* the minimum delay time attribute of IPDUs in Direct/N-Times or Mixed Transmission Mode */
        /*Reset it to Zero i.e No Min delay check as soon as IpduGroup is started */
        txIpduRamPtr_pst->cntrMinDelayTime_u16 = COM_ZERO;

#ifdef COM_TxIPduTimeOut
        if (Com_GetValue(TXIPDU,_NONETOSTATUS,txIpduConstPtr_pcst->txIPduFields_u16))
        {
            /* TRACE[SWS_Com_00696] In case transmission deadline monitoring is configured for an I-PDU with
            * transmission mode NONE only, the AUTOSAR COM module shall start the transmission deadline monitoring
            * timer for this I-PDU upon the start of the I-PDU group to which the I-PDU belongs to. */
            Com_SetRamValue(TXIPDU,_TICKTXTO,txIpduRamPtr_pst->txFlags_u16,COM_START);
            txIpduRamPtr_pst->cntrTxTimeout_u16 = txIpduConstPtr_pcst->timeout_u16;
        }
        else
#endif/* #ifdef COM_TxIPduTimeOut */
        {
#ifdef COM_TxIPduTimeOut

            /* Reset it to Stop.
            * Note: This Flag has to be set to START only in SendIpdu() i,e just before Sending the IPDU */
            /* TRACE[SWS_Com_00835] In case transmission deadline monitoring is configured for an I-PDU with
            * transmission mode NONE and another transmission mode, the transmission deadline monitoring
            * shall be disabled whenever the transmission mode NONE is active */
            Com_SetRamValue(TXIPDU,_TICKTXTO,txIpduRamPtr_pst->txFlags_u16,COM_STOP);

            txIpduRamPtr_pst->cntrTxTimeout_u16 = COM_ZERO; /* Reset the timer to zero */
#endif/* #ifdef COM_TxIPduTimeOut */
            Com_SetRamValue(TXIPDU,_CONFIR,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
        }
    } /* End of Initialize */
    else
    {
        /* Check if IPDU group is TX group
        * Cyclic IPDU's will be sent out cyclically after the call of this API
        */

        /* Call Com_Prv_TxChangeMode() to calculate the _LATESTMODE */
        Com_Prv_TxChangeMode(idIpdu_uo);
        /* Note: Eventhough if the Com_Tick_Tx is updated in Com_Prv_TxChangeMode(),
        * it is once again updated in Ipdu Group Start i.e below */

        /* The AUTOSAR COM module shall start the transmission deadline monitoring timer
        * for this I-PDU upon the start of the I-PDU group to which the I-PDU belongs to*/
#ifdef COM_TxIPduTimeOut
        if (Com_GetValue(TXIPDU,_NONETOSTATUS,txIpduConstPtr_pcst->txIPduFields_u16))
        {
            Com_SetRamValue(TXIPDU,_TICKTXTO,txIpduRamPtr_pst->txFlags_u16,COM_START);
            txIpduRamPtr_pst->cntrTxTimeout_u16 = txIpduConstPtr_pcst->timeout_u16;
        }
#endif
    }

#ifdef COM_TX_IPDUCOUNTER
    /* Check if IPduCounter is configured */
    if (txIpduConstPtr_pcst->idxTxIpduCntr_uo != COM_TXIPDU_CNTR_INV_IDX)
    {
        /* Locks are required here, to protect Tx Ipdu buffer due to the signal updates as per below requirement.
        * TRACE[SWS_Com_00334] By a call to the functions: Com_SendSignal, Com_SendSignalGroup, or
        * Com_InvalidateSignal, the AUTOSAR COM module shall update the values of its internal buffers even
        * for stopped I-PDUs. */
        SchM_Enter_Com_TxIpduProtArea(IPDUGROUPSTART);

        /* TRACE[SWS_Com_00687] The AUTOSAR COM module shall set the I-PDU counter to 0, after the I-PDU was
        * initialized by Com_Init or reinitialized by Com_IpduGroupStart. */
        Com_Prv_TxIpduInitializeCounter( (PduIdType)idIpdu_uo );

        SchM_Exit_Com_TxIpduProtArea(IPDUGROUPSTART);
    }
    else
    {
        /* do nothing: counter is not configured */
    }
#endif /* #ifdef COM_TX_IPDUCOUNTER */

#ifdef COM_TX_TP_IPDUTYPE
    /* By default release the large Ipdu buffer lock, which is acquired in Com_Prv_SendIpdu() */
    Com_SetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
#endif
}

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIPduStop
 Description      : Service to stop the ipdu
 Parameter        : idIpdu_uo - Id of the ipdu
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_TxIPduStop(Com_IpduId_tuo idIpdu_uo)
{
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);

    txIpduRamPtr_pst    = txIpduConstPtr_pcst->ipduRam_pst;

#ifdef COM_ERRORNOTIFICATION
    /* TRACE[SWS_Com_00479] If an I-PDU is stopped by Com_IpduGroupStop, the AUTOSAR COM module
    * shall immediately invoke the configured ComErrorNotification, for outstanding not confirmed
    * transmitted signals/ signal groups of the stopped I-PDU.
    * Outstanding not confirmed I-PDU's can be one of the below possibilities:
    * => Tx-Timeout timer is started, i.e., already transmitted to the lower layer, waiting for the confirmation.
    * => N-times repetitions is not completed yet, its confirmation is pending
    * => Large data transmission request is initiated */
    /* MR12 RULE 13.5 VIOLATION: Below conditional statement access RAM, use of intermediate variables
    * to store and read here, may read obsolete value. Evaluation(or no evaluation) shall not impact the
    * system behaviour. Hence suppressed. */
    if (   (Com_GetRamValue(TXIPDU,_TICKTXTO,txIpduRamPtr_pst->txFlags_u16) == COM_START)
        || (txIpduRamPtr_pst->cntrRepetitions_u8 != COM_ZERO)
#ifdef COM_TX_TP_IPDUTYPE
        || (Com_GetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16))
#endif
       )

    {

        if (Com_GetRamValue(TXIPDU,_CONFIR,txIpduRamPtr_pst->txFlags_u16) == COM_FALSE)
        {
# if (COM_CONFIGURATION_VARIANT == COM_VARIANT_PRE_COMPILE)
            if (txIpduConstPtr_pcst->errorNotification_pfct != NULL_PTR)
            {
                txIpduConstPtr_pcst->errorNotification_pfct();
            }
# else
            Com_Prv_TxIPduErrCallbacks(idIpdu_uo);
# endif /* end of COM_CONFIGURATION_VARIANT */
        }
    }
#endif /* #ifdef COM_ERRORNOTIFICATION */

#ifdef COM_TxIPduTimeOut
    /* TRACE[SWS_Com_00115] If an I-PDU is stopped by Com_IpduGroupStop,
    * the AUTOSAR COM module shall cancel the deadline monitoring for all pending confirmations. */
    Com_SetRamValue(TXIPDU,_TICKTXTO,txIpduRamPtr_pst->txFlags_u16,COM_STOP);
#endif

#if (COM_RETRY_FAILED_TX_REQUESTS == STD_ON)
    /* TRACE[SWS_Com_00777] - If an I-PDU is stopped by Com_IpduGroupStop,
    * cancel any potential retries with respect to ComRetryFailedTransmitRequests. */
    Com_SetRamValue(TXIPDU,_RETRYFAILEDTXREQ,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
#endif

    /* TRACE[SWS_Com_00800] The AUTOSAR COM module shall ignore any transmit confirmations for a stopped I-PDU. */
    /* Reset all other Tx Global variables including Min Delays */
    Com_SetRamValue(TXIPDU,_CONFIR,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);

#ifdef COM_TxIPduTimeOut
    txIpduRamPtr_pst->cntrTxTimeout_u16 = COM_ZERO;
#endif

    /* TRACE[SWS_Com_00777] - If an I-PDU is stopped by Com_IpduGroupStop,
    * cancel any outstanding transmission requests for this I-PDU. */
    txIpduRamPtr_pst->cntrRepetitions_u8 = COM_ZERO;
    Com_SetRamValue(TXIPDU,_MDT,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
    txIpduRamPtr_pst->cntrMinDelayTime_u16 = COM_ZERO;

#ifdef COM_TX_TP_IPDUTYPE
    /* TRACE[SWS_Com_00714] If a large I-PDU is stopped while its transmission is already in progress,
    * the AUTOSAR COM module shall stop the transmission process immediately.
    * Reset the large data Tx Pdu flag, if the Ipdu group is stopped. */
    Com_SetRamValue(TXIPDU,_LARGEDATAINPROG,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
#endif

}


#ifdef COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
/*
 **********************************************************************************************************************
 Function name    : Com_SetTxIPduControlViaRbaNdsEcuVariant
 Description      : Service called by rba_ComScl to set/reset the status of Tx Ipdu
 Parameter        : idIpdu_uo    -> ID of the Tx IPDU
                  : ipduStatus_b -> TxIpdu status maintained by rba_ComScl
 Return value     : none
 **********************************************************************************************************************
*/
void Com_SetTxIPduControlViaRbaNdsEcuVariant(PduIdType idIpdu_uo, boolean ipduStatus_b)
{
# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetTxIPduControlViaRbaNdsEcuVariant, COM_E_UNINIT);
    }
    else if (!Com_Prv_IsValidTxIpduId(idIpdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetTxIPduControlViaRbaNdsEcuVariant, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idIpdu_uo = COM_GET_TX_IPDU_ID(idIpdu_uo);

        COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->isIpduStartedViaNds_b = ipduStatus_b;
    }
}

#endif /* end of COM_TXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */


#ifdef COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT
/*
 **********************************************************************************************************************
 Function name    : Com_SetRxIPduControlViaRbaNdsEcuVariant
 Description      : Service called by rba_ComScl to set/reset the status of Rx Ipdu
 Parameter        : idIpdu_uo    -> ID of the Rx IPDU
                  : ipduStatus_b -> RxIpdu status maintained by rba_ComScl
 Return value     : none
 **********************************************************************************************************************
*/
void Com_SetRxIPduControlViaRbaNdsEcuVariant(PduIdType idIpdu_uo, boolean ipduStatus_b)
{
# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetRxIPduControlViaRbaNdsEcuVariant, COM_E_UNINIT);
    }
    else if (!Com_Prv_IsValidRxIpduId(idIpdu_uo))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SetRxIPduControlViaRbaNdsEcuVariant, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {

        /* If PB variant is selected, then PduId which is passed to this function will be changed
        * to internal Id which is generated through configuration
        * If PC variant is selected, then no mapping table will be used. */
        idIpdu_uo = COM_GET_RX_IPDU_ID(idIpdu_uo);

        COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo)->ipduRam_pst->isIpduStartedViaNds_b = ipduStatus_b;
    }
}

#endif /* end of COM_RXIPDUCONTROL_VIA_RBA_NDS_ECUVARIANT */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

