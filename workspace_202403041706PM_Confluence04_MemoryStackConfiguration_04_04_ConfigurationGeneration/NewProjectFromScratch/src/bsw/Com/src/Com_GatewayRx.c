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


#ifdef COM_ENABLE_MAINFUNCTION_RX

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
# ifdef COM_SIGNALGATEWAY
LOCAL_INLINE void Com_Prv_RxToGwFifoSignals(Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst);
# endif

# ifdef COM_SIGNALGROUPGATEWAY
LOCAL_INLINE void Com_Prv_RxToGwFifoSignalGrps(Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst);
LOCAL_INLINE void Com_Prv_RxToGwFifoGrpSignals(Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst,
                                                  Com_MainFunc_tuo idRxMainFunc_uo);
# endif


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

# if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY)

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxToGwFifo
 Description      : Push signal, group-signal data into FIFO
 Parameter        : idIpdu_uo -> Rx IPDU ID
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_RxToGwFifo(Com_IpduId_tuo idIpdu_uo)
{
    Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);

#  ifdef COM_SIGNALGATEWAY
    if (rxIpduConstPtr_pcst->numOfGwSrcSignals_u16 > COM_ZERO)
    {
        Com_Prv_RxToGwFifoSignals(rxIpduConstPtr_pcst);
    }
#  endif /* #ifdef COM_SIGNALGATEWAY */

#  if defined COM_SIGNALGROUPGATEWAY
    if (rxIpduConstPtr_pcst->numOfGwSrcSignalGrps_u16 > COM_ZERO)
    {
        Com_Prv_RxToGwFifoSignalGrps(rxIpduConstPtr_pcst);
    }
#  endif /* #if defined COM_SIGNALGROUPGATEWAY */

}

# endif /* #if defined(COM_SIGNALGATEWAY) || defined(COM_SIGNALGROUPGATEWAY) */


# ifdef COM_SIGNALGATEWAY
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxToGwFifoSignals
 Description      : Push rx signal data into FIFO
 Parameter        : rxIpduConstPtr_pcst  -> Address of the RxIpdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_RxToGwFifoSignals(Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst)
{
    const void * msgData_pcv;
    Com_RxSigCfg_tpcst rxSigConstPtr_pcst;
    const Com_Prv_xGwMapSigCfg_tst * gwMapSigCfg_pcst;
    const Com_Prv_xGwMapSigFifoCfg_tst * gwMapSigFifoCfg_pcst;
    uint16_least idxSig_qu16;
    uint16_least idxFifo_qu16;
#  ifdef COM_RXSIG_FLOAT64SUPP
    float64 rxValue_f64;
#  endif
#  ifdef COM_RXSIG_INT64
    uint64 rxValue_u64;
#  endif
    struct rba_BswSrv_FifoMsgInfo msgInfo_st;
    uint32 rxValue_u32;
    uint16 rxValue_u16;
    Com_MainFunc_tuo idRxMainFunc_uo;
    uint8 rxValue_u8;
    uint8 type_u8;
    boolean rxValue_b;

    rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(rxIpduConstPtr_pcst->idRxSig_uo);
    idRxMainFunc_uo = rxIpduConstPtr_pcst->idMainFunc_uo;

    for (idxSig_qu16 = rxIpduConstPtr_pcst->numOfGwSrcSignals_u16; idxSig_qu16 > 0u; --idxSig_qu16)
    {
        if (Com_GetRamValue(RXSIG,_SIGNALFILLBIT,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8)))
        {
            type_u8 = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

            /* Take a local copy of rx-signal-data and push the same data into different destination fifo's */
            switch (type_u8)
            {
                /* MR12 DIR 1.1 VIOLATION: "Implicit conversion from a pointer to object type to a pointer to void."
                *  warning can be suppressed as the SignalDataPtr will be dereferenced at destination side */
                case COM_UINT8:
                case COM_SINT8:
                {
                    rxValue_u8 = COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo);
                    msgData_pcv = (const void *)&rxValue_u8;
                    msgInfo_st.len = 1u;
                }
                break;
                case COM_BOOLEAN:
                {
                    rxValue_b = COM_UNSIGNED_TO_BOOL(
                            COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo));
                    msgData_pcv = (const void *)&rxValue_b;
                    msgInfo_st.len = 1u;
                }
                break;
                case COM_UINT16:
                case COM_SINT16:
                {
                    rxValue_u16 = COM_GET_SIGTYPEUINT16_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo);
                    msgData_pcv = (const void *)&rxValue_u16;
                    msgInfo_st.len = 2u;
                }
                break;
                case COM_UINT32:
                case COM_SINT32:
#  ifdef COM_FLOAT32SUPP
                case COM_FLOAT32:
#  endif
                {
                    rxValue_u32 = COM_GET_SIGTYPEUINT32_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo);
                    msgData_pcv = (const void *)&rxValue_u32;
                    msgInfo_st.len = 4u;
                }
                break;
#  ifdef COM_RXSIG_INT64
                case COM_UINT64:
                case COM_SINT64:
                {
                    SchM_Enter_Com_RxSigBuff(MAINFUNCTIONRX);
                    rxValue_u64 = COM_GET_SIGTYPEUINT64_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo);
                    SchM_Exit_Com_RxSigBuff(MAINFUNCTIONRX);
                    msgData_pcv = (const void *)&rxValue_u64;
                    msgInfo_st.len = 8u;
                }
                break;
#  endif /*end of #ifdef COM_RXSIG_INT64*/
#  ifdef COM_RXSIG_FLOAT64SUPP
                case COM_FLOAT64:
                {
                    SchM_Enter_Com_RxSigBuff(MAINFUNCTIONRX);
                    rxValue_f64 = COM_GET_SIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo);
                    SchM_Exit_Com_RxSigBuff(MAINFUNCTIONRX);
                    msgData_pcv = (const void *)&rxValue_f64;
                    msgInfo_st.len = 8u;
                }
                break;
#  endif /* #ifdef COM_RXSIG_FLOAT64SUPP */
                case COM_UINT8_N:
                {
                    msgData_pcv = (const void *)COM_GET_DEFERREDBUFFER(idRxMainFunc_uo);
                    msgInfo_st.len = (rba_BswSrv_FifoMsgLen_t)rxSigConstPtr_pcst->bitSize_uo;

                    SchM_Enter_Com_RxSigBuff(MAINFUNCTIONRX);
                    COM_MEMCOPY((void *)COM_GET_DEFERREDBUFFER(idRxMainFunc_uo),
                            (const void *)&COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo),
                            msgInfo_st.len);
                    SchM_Exit_Com_RxSigBuff(MAINFUNCTIONRX);
                }
                break;

                default:
                {
                    /* undefined type, avoid pushing data */
                    msgData_pcv = (const void *)NULL_PTR;
                }
                break;
            }

            Com_SetRamValue(RXSIG,_SIGNALFILLBIT,(rxSigConstPtr_pcst->sigRAM_pst->rxSigRAMFields_u8),COM_FALSE);

            if (msgData_pcv != NULL_PTR)
            {
                gwMapSigCfg_pcst = COM_GET_GWMAPSIG_CONSTDATA(rxSigConstPtr_pcst->idxGwMap_uo);
                gwMapSigFifoCfg_pcst = COM_GET_GWMAPSIGFIFO_CONSTDATA(gwMapSigCfg_pcst->idxFifo_u16);

                msgInfo_st.tag = (rba_BswSrv_FifoMsgTag_t)(gwMapSigCfg_pcst->idxFifo_u16);

                for (idxFifo_qu16 = gwMapSigCfg_pcst->numOfFifo_u8; idxFifo_qu16 > 0u; --idxFifo_qu16)
                {
                    (void)rba_BswSrv_FifoPush((rba_BswSrv_FifoId_t)gwMapSigFifoCfg_pcst->idFifo_u16,
                            (const struct rba_BswSrv_FifoMsgInfo *) &msgInfo_st, msgData_pcv);
                    ++msgInfo_st.tag;
                    ++gwMapSigFifoCfg_pcst;
                }
            }
        }
        rxSigConstPtr_pcst++;
    }
}
# endif /* #ifdef COM_SIGNALGATEWAY */


# ifdef COM_SIGNALGROUPGATEWAY
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxToGwFifoSignalGrps
 Description      : Push rx signal data into FIFO
 Parameter        : rxIpduConstPtr_pcst  -> Address of the RxIpdu
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_RxToGwFifoSignalGrps(Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst)
{
    Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst;
    uint16_least idxSigGrp_qu16;

    rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);

    for (idxSigGrp_qu16 = rxIpduConstPtr_pcst->numOfGwSrcSignalGrps_u16; idxSigGrp_qu16 > 0u; --idxSigGrp_qu16)
    {
        if (Com_GetRamValue(RXSIGGRP,_SIGNALFILLBIT,rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8))
        {
            Com_Prv_RxToGwFifoGrpSignals(rxSigGrpConstPtr_pcst, rxIpduConstPtr_pcst->idMainFunc_uo);

            /* clear signal-group data update flag, this is to capture any further updates during the processing */
            Com_SetRamValue(RXSIGGRP,_SIGNALFILLBIT,
                                                rxSigGrpConstPtr_pcst->sigGrpRAM_pst->rxSigGrpRAMFields_u8,COM_FALSE);
        }
        rxSigGrpConstPtr_pcst++;
    }
}
# endif /* #ifdef COM_SIGNALGROUPGATEWAY */


# ifdef COM_SIGNALGROUPGATEWAY
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxToGwFifoGrpSignals
 Description      : Push rx group-signal data into FIFO
 Parameter        : rxSigGrpConstPtr_pcst -> rx signal-grp cfg
                    idRxMainFunc_uo -> rx main-function id
 Return value     : None
 **********************************************************************************************************************
*/
LOCAL_INLINE void Com_Prv_RxToGwFifoGrpSignals(
        Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst,
        Com_MainFunc_tuo idRxMainFunc_uo
)
{
    const void * msgData_pcv;
    Com_RxGrpSigCfg_tpcst rxGrpSigConstPtr_pcst;
    const Com_Prv_xGwMapSigCfg_tst * gwMapSigCfg_pcst;
    const Com_Prv_xGwMapSigFifoCfg_tst * gwMapSigFifoCfg_pcst;
    uint16_least idxGrpSig_qu16;
    uint16_least idxFifo_qu16;
#  ifdef COM_RXGRPSIG_FLOAT64SUPP
    float64 rxValue_f64;
#  endif
#  ifdef COM_RXGRPSIG_INT64
    uint64 rxValue_u64;
#  endif
    struct rba_BswSrv_FifoMsgInfo msgInfo_st;
    uint32 rxValue_u32;
    uint16 rxValue_u16;
    uint8 rxValue_u8;
    uint8 type_u8;
    boolean rxValue_b;

    rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA(rxSigGrpConstPtr_pcst->idFirstGrpSig_uo);

    for (idxGrpSig_qu16 = rxSigGrpConstPtr_pcst->numOfGwSrcGrpSignals_u16; idxGrpSig_qu16 > 0u; --idxGrpSig_qu16)
    {
        type_u8 = Com_GetValue(RXGRPSIG,_TYPE,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

        /* Take a local copy of rx--grp-signal-data and push the same data into different destination fifo's */
        switch (type_u8)
        {
            /* MR12 DIR 1.1 VIOLATION: "Implicit conversion from a pointer to object type to a pointer to void."
            *  warning can be suppressed as the SignalDataPtr will be dereferenced at destination side */
            case COM_UINT8:
            case COM_SINT8:
            {
                rxValue_u8 = COM_GET_SECRXGRPSIGTYPEU8_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo);
                msgData_pcv = (const void *)&rxValue_u8;
                msgInfo_st.len = 1u;
            }
            break;
            case COM_BOOLEAN:
            {
                rxValue_b = COM_UNSIGNED_TO_BOOL(
                        COM_GET_SECRXGRPSIGTYPEU8_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo));
                msgData_pcv = (const void *)&rxValue_b;
                msgInfo_st.len = 1u;
            }
            break;
            case COM_UINT16:
            case COM_SINT16:
            {
                rxValue_u16 =
                        COM_GET_SECRXGRPSIGTYPEU16_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo);
                msgData_pcv = (const void *)&rxValue_u16;
                msgInfo_st.len = 2u;
            }
            break;
            case COM_UINT32:
            case COM_SINT32:
#  ifdef COM_GRPSIGFLOAT32SUPP
            case COM_FLOAT32:
#  endif
            {
                rxValue_u32 =
                        COM_GET_SECRXGRPSIGTYPEU32_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo);
                msgData_pcv = (const void *)&rxValue_u32;
                msgInfo_st.len = 4u;
            }
            break;
#  ifdef COM_RXGRPSIG_INT64
            case COM_UINT64:
            case COM_SINT64:
            {
                SchM_Enter_Com_RxGrpSigSecBuff();
                rxValue_u64 =
                        COM_GET_SECRXGRPSIGTYPEU64_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo);
                SchM_Exit_Com_RxGrpSigSecBuff();
                msgData_pcv = (const void *)&rxValue_u64;
                msgInfo_st.len = 8u;
            }
            break;
#  endif /*end of #ifdef COM_RXGRPSIG_INT64*/
#  ifdef COM_RXGRPSIG_FLOAT64SUPP
            case COM_FLOAT64:
            {
                SchM_Enter_Com_RxGrpSigSecBuff();
                rxValue_f64 =
                        COM_GET_SECRXGRPSIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo);
                SchM_Exit_Com_RxGrpSigSecBuff();
                msgData_pcv = (const void *)&rxValue_f64;
                msgInfo_st.len = 8u;
            }
            break;
#  endif /* #ifdef COM_RXGRPSIG_FLOAT64SUPP */
            case COM_UINT8_N:
            {
                msgData_pcv = (const void *)COM_GET_DEFERREDBUFFER(idRxMainFunc_uo);
                msgInfo_st.len = (rba_BswSrv_FifoMsgLen_t)rxGrpSigConstPtr_pcst->bitSize_uo;

                SchM_Enter_Com_RxGrpSigSecBuff();
                COM_MEMCOPY((void *)COM_GET_DEFERREDBUFFER(idRxMainFunc_uo),
                (const void*)&COM_GET_SECRXGRPSIGTYPEU8_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo),
                msgInfo_st.len);
                SchM_Exit_Com_RxGrpSigSecBuff();
            }
            break;

            default:
            {
                /* undefined type, avoid pushing data */
                msgData_pcv = (const void *)NULL_PTR;
            }
            break;
        }

        if (msgData_pcv != NULL_PTR)
        {
            gwMapSigCfg_pcst = COM_GET_GWMAPSIG_CONSTDATA(rxGrpSigConstPtr_pcst->idxGwMap_uo);
            gwMapSigFifoCfg_pcst = COM_GET_GWMAPSIGFIFO_CONSTDATA(gwMapSigCfg_pcst->idxFifo_u16);

            msgInfo_st.tag = (rba_BswSrv_FifoMsgTag_t)(gwMapSigCfg_pcst->idxFifo_u16);

            for (idxFifo_qu16 = gwMapSigCfg_pcst->numOfFifo_u8; idxFifo_qu16 > 0u; --idxFifo_qu16)
            {
                (void)rba_BswSrv_FifoPush((rba_BswSrv_FifoId_t)gwMapSigFifoCfg_pcst->idFifo_u16,
                        (const struct rba_BswSrv_FifoMsgInfo *) &msgInfo_st, msgData_pcv);
                ++msgInfo_st.tag;
                ++gwMapSigFifoCfg_pcst;
            }
        }
        rxGrpSigConstPtr_pcst++;
    }
}
# endif /* #ifdef COM_SIGNALGROUPGATEWAY */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* end of COM_ENABLE_MAINFUNCTION_RX */

