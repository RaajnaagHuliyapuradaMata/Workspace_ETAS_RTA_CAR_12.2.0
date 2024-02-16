/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2022, all rights reserved
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

#ifdef COM_ENABLE_MAINFUNCTION_TX
# if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)
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
LOCAL_INLINE void Com_Prv_GwSendSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv);


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
 Function name    : Com_Prv_GwFifoToTx
 Description      : Pop rx-signal-data from FIFO and copy to tx-signal location in its associated tx-IPDU-buffer
                  : Pop rx-group-signal-data from FIFO and copy to tx-group-signal buffer
 Parameter        : idTxMainFunc_uo -> MainFunctionTx internal ID
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_GwFifoToTx(Com_MainFunc_tuo idTxMainFunc_uo)
{
    const Com_MainFuncTxCfg_tst * mainFuncTxCfg_pcst;
    const Com_Prv_xGwMapSigFifoCfg_tst * gwMapSigFifoCfg_pcst;
    const Com_Prv_xGwMapSigIdCfg_tst * gwMapSigIdCfg_pcst;
    void * msgData_pv;
    uint16_least idxFifo_qu16;
    uint16_least idxSig_qu16;
    struct rba_BswSrv_FifoMsgInfo msgInfo_st;
#  if defined(COM_TXSIG_FLOAT64SUPP) || defined(COM_TXGRPSIG_FLOAT64SUPP)
    float64 txValue_f64;
#  endif
#  if defined(COM_TXSIG_INT64) || defined(COM_TXGRPSIG_INT64)
    uint64 txValue_u64;
#  endif
    uint32 txValue_u32;
    rba_BswSrv_FifoId_t idFifo_uo;
    uint16 txValue_u16;
    uint8 txValue_u8;
    boolean txValue_b;

    mainFuncTxCfg_pcst = COM_GET_MAINFUNCTX_CFG(idTxMainFunc_uo);

    for (idxFifo_qu16 = 0u; idxFifo_qu16 < mainFuncTxCfg_pcst->numOfFifo_u16; ++idxFifo_qu16)
    {
        idFifo_uo = (rba_BswSrv_FifoId_t)mainFuncTxCfg_pcst->idFifo_pcau16[idxFifo_qu16];
        msgInfo_st.len = 0u;
        msgInfo_st.tag = 0u;

        while (((rba_BswSrv_FifoError_t)RBA_BSWSRV_FIFO_ERROR_NONE) == rba_BswSrv_FifoPeek(idFifo_uo, &msgInfo_st))
        {
            gwMapSigFifoCfg_pcst = COM_GET_GWMAPSIGFIFO_CONSTDATA(msgInfo_st.tag);

            /* Take a local copy of rx-signal-data from fifo and update the same data into destination signal's */
            switch (gwMapSigFifoCfg_pcst->sigType_u8)
            {
                /* MR12 DIR 1.1 VIOLATION: "Implicit conversion from a pointer to object type to a pointer to void."
                *  warning can be suppressed as the SignalDataPtr will be dereferenced with the
                * same type at destination side */
                case COM_UINT8:
                case COM_SINT8:
                {
                    msgData_pv = (void *)&txValue_u8;
                }
                break;
                case COM_BOOLEAN:
                {
                    msgData_pv = (void *)&txValue_b;
                }
                break;
                case COM_UINT16:
                case COM_SINT16:
                {
                    msgData_pv = (void *)&txValue_u16;
                }
                break;
                case COM_UINT32:
                case COM_SINT32:
#  if defined(COM_FLOAT32SUPP) || defined(COM_GRPSIGFLOAT32SUPP)
                case COM_FLOAT32:
#  endif
                {
                    msgData_pv = (void *)&txValue_u32;
                }
                break;
#  if defined(COM_TXSIG_INT64) || defined(COM_TXGRPSIG_INT64)
                case COM_UINT64:
                case COM_SINT64:
                {
                    msgData_pv = (void *)&txValue_u64;
                }
                break;
#  endif /* end of INT64*/
#  if defined(COM_TXSIG_FLOAT64SUPP) || defined(COM_TXGRPSIG_FLOAT64SUPP)
                case COM_FLOAT64:
                {
                    msgData_pv = (void *)&txValue_f64;
                }
                break;
#  endif /* end of FLOAT64 */
                case COM_UINT8_N:
                {
                    msgData_pv = (void *)COM_GET_TXRAM_BUF(idTxMainFunc_uo).gwSigBuf_pu8;
                }
                break;

                default:
                {
                    /* undefined type, avoid pushing data */
                    msgData_pv = (void *)NULL_PTR;
                }
                break;
            }

            if (msgData_pv != NULL_PTR)
            {
                /* ignore return value, as it is already checked in last call to fifoPeek() */
                (void)rba_BswSrv_FifoPop(idFifo_uo, &msgInfo_st, msgData_pv, msgInfo_st.len);

                gwMapSigIdCfg_pcst = COM_GET_GWMAPSIGID_CONSTDATA(gwMapSigFifoCfg_pcst->idxDest_u16);

                for (idxSig_qu16 = gwMapSigFifoCfg_pcst->numOfDest_u8; idxSig_qu16 > 0u; --idxSig_qu16)
                {
                    Com_Prv_GwSendSignal(gwMapSigIdCfg_pcst->idDest_uo, (const void *)msgData_pv);

                    ++gwMapSigIdCfg_pcst;
                }
            }
        }
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_GwSendSignal
 Description      : checks whether destination is signal/group-signal, and acc call required interal function
 Parameter        : idSignal_u16 -> Signal or GroupSignal external ID
                    signalDataPtr_pcv -> pointer to signal data
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_GwSendSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv)
{
#  ifdef COM_SIGNALGATEWAY
    if (Com_Prv_IsValidTxSignalId(idSignal_u16))
    {
        idSignal_u16 = COM_GET_TXSIGNAL_ID(idSignal_u16);

        (void)Com_Prv_InternalSendSignal(idSignal_u16, signalDataPtr_pcv);
    }
    else
#  endif /* end of COM_SIGNALGATEWAY */
#  ifdef COM_SIGNALGROUPGATEWAY
    if (Com_Prv_IsValidTxGroupSignalId(idSignal_u16))
    {
        Com_TxGrpSigCfg_tpcst txGrpSigConstPtr_pcst;
        Com_TxSigGrpCfg_tpcst txSigGrpConstPtr_pcst;

        idSignal_u16 = COM_GET_TXGRPSIGNAL_ID(idSignal_u16);

        txGrpSigConstPtr_pcst = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);
        txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo);

        Com_Prv_UpdateShadowSignal(idSignal_u16, signalDataPtr_pcv);

        /* This is to flag the signal group, that a group signal within it was updated */
        Com_SetRamValue(TXSIGGRP,_ISGWUPDTD,
                            txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_TRUE);
    }
    else
#  endif /* end of COM_SIGNALGROUPGATEWAY */
    {
        /* skip unknown signal */
    }
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_PrepareGwTxIpdu
 Description      : Update GroupSignal shadow buffers into Ipdu buffer and check for IPDU transmission properties
 Parameter        : idIpdu_uo -> TX IPDU ID
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_PrepareGwTxIpdu(Com_IpduId_tuo idIpdu_uo)
{
    Com_TxIpduCfg_tpcst         txIpduConstPtr_pcst;
    Com_TxIpduRam_tpst          txIpduRamPtr_pst;
#  if defined (COM_SIGNALGROUPGATEWAY) && defined (COM_TX_SIGNALGROUP)
    Com_TxSigGrpCfg_tpcst       txSigGrpConstPtr_pcst;
    uint16_least                idxIdTxSigGrp_qu16;
    uint16_least                idMaxTxSigGrp_qu16;
#  endif
    boolean                     isTriggered_b;
#  ifdef COM_TxFilters
    boolean                     isTMSChanged_b;
#  endif
#  ifdef COM_TxIPduTimeOut
    boolean                     isTimeoutReq_b;
#  endif

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    txIpduRamPtr_pst    = txIpduConstPtr_pcst->ipduRam_pst;

    /* Process Tx-IPdu transmission */
#  if defined (COM_SIGNALGROUPGATEWAY) && defined (COM_TX_SIGNALGROUP)

    idxIdTxSigGrp_qu16  = txIpduConstPtr_pcst->idFirstTxSigGrp_uo;
    idMaxTxSigGrp_qu16  = idxIdTxSigGrp_qu16 + txIpduConstPtr_pcst->numOfSigGroups_u16;

    txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(idxIdTxSigGrp_qu16);

    for ( ; idxIdTxSigGrp_qu16 < idMaxTxSigGrp_qu16; idxIdTxSigGrp_qu16++ )
    {
        if (Com_GetRamValue(TXSIGGRP,_ISGWUPDTD, txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8))
        {
            Com_SetRamValue(TXSIGGRP,_ISGWUPDTD,
                                        txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_FALSE);

            /* 1. Set the UpdateBit
            * 2. Update TxIPdu buffer with the latest Tx-GroupSignal data
            * 3. Evaluate TMC of GroupSignals
            * 4. Evaluate TMS of TxIPdu
            * Return value is based on IPduGroup status that can be ignored here,
            * as it is already checked beforet this function call from Com_MainFunctionTx()
            */
            (void)Com_Prv_InternalSendSignalGroup( (Com_SignalGroupIdType)idxIdTxSigGrp_qu16 );
        }
        txSigGrpConstPtr_pcst++;
    }
#  endif /* #if defined (COM_SIGNALGROUPGATEWAY) && defined (COM_TX_SIGNALGROUP) */

    /* locks are required to protect Com_TxFlags */
    SchM_Enter_Com_TxIpduProtArea(MAINFUNCTIONTX);

    /* read and clear the global flags */
    isTriggered_b = Com_GetRamValue(TXIPDU,_ISGWTRIGGRD,txIpduRamPtr_pst->txFlags_u16);
    Com_SetRamValue(TXIPDU,_ISGWTRIGGRD,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);

#  ifdef COM_TxFilters
    isTMSChanged_b = Com_GetRamValue(TXIPDU,_GWIPDUTMSCHANGED,txIpduRamPtr_pst->txFlags_u16);
    Com_SetRamValue(TXIPDU,_GWIPDUTMSCHANGED,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
#  endif

#  ifdef COM_TxIPduTimeOut
    isTimeoutReq_b = Com_GetRamValue(TXIPDU,_ISTOENABLED,txIpduRamPtr_pst->txFlags_u16);
    Com_SetRamValue(TXIPDU,_ISTOENABLED,txIpduRamPtr_pst->txFlags_u16,COM_FALSE);
#  endif
    SchM_Exit_Com_TxIpduProtArea(MAINFUNCTIONTX);

    /* MR12 RULE 13.5 VIOLATION: Below conditional statement reads flag status.
    * Evaluation(or no evaluation) of the 2nd or 3rd operands does not impact the system behaviour,
    * moreover HIS-LEVEL parameter will be violated if more conditional statements are added.
    * Hence the warning is suppressed. */
    if (((isTriggered_b) &&
        (COM_TX_MODE_IS_EVENT(Com_GetRamValue(TXIPDU,_LATESTMODE,txIpduRamPtr_pst->transMode_u8))))
#  ifdef COM_TxFilters
        || (isTMSChanged_b)
#  endif
        )
    {

        /* All unused flags are reset here */
        Com_SendIpduInfo_tst sendIpduFlag_st = COM_CLEAR_SENDIPDU_FLAGS;

        /* this flag will be set, as call happens as a event trigger */
        sendIpduFlag_st.isEventTrig_u1      = COM_SET;
        /* Event update from MainFunctionTx */
        sendIpduFlag_st.prepareGwIpduTx_u1  = COM_SET;

        /* For the direct part of the Mixed TM, Signal Transfer property is set directly from
        * _ISGWTRIGGRD flag. In all the other cases, it is ignored. */
#  ifdef COM_TxFilters
        sendIpduFlag_st.isSigTriggered_u1   = COM_BOOL_TO_UNSIGNED(isTriggered_b);
#  else
        sendIpduFlag_st.isSigTriggered_u1   = COM_SET;
#  endif

#  ifdef COM_TxFilters
        sendIpduFlag_st.isModeChangd_u1     = COM_BOOL_TO_UNSIGNED(isTMSChanged_b);
#  endif
        /* Set the timeout flag if any updated signal contributes to timeout */
#  ifdef COM_TxIPduTimeOut
        sendIpduFlag_st.isTimeoutReq_u1     = COM_BOOL_TO_UNSIGNED(isTimeoutReq_b);
#  endif
        /* Call function for Transmitting Tx Ipdu's */
        Com_Prv_SendIpdu((PduIdType)idIpdu_uo, sendIpduFlag_st);
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

# endif /* #if defined (COM_SIGNALGATEWAY) || defined (COM_SIGNALGROUPGATEWAY) */
#endif /* end of COM_ENABLE_MAINFUNCTION_TX */

