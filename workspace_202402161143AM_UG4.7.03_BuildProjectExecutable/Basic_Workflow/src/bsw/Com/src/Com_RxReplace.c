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
LOCAL_INLINE uint32 Com_Prv_GetRxSigReplaceValueIndex(Com_RxSigCfg_tpcst rxSigConstPtr_pcst, uint8 replaceAction_u8);

#ifdef COM_RX_SIGNALGROUP
LOCAL_INLINE uint32 Com_Prv_GetRxGrpSigReplaceValueIndex(
                                                Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                uint8                   replaceAction_u8
                                                        );

# ifdef COM_RX_SIGNALGROUP_ARRAY
LOCAL_INLINE Com_SigMax_tuo Com_Prv_RxReplaceSigGrpArrayAccessBuf(
                                                    Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                    uint32                  idxSigValue_u32,
                                                    uint8                   sigType_u8,
                                                    uint8 *                 sigGrpBuffer_pu8,
                                                    Com_Bitposition_tuo     sigGrpBitOffset_uo
                                                                 );
# endif /* end of COM_RX_SIGNALGROUP_ARRAY */

LOCAL_INLINE Com_SigMax_tuo Com_Prv_RxReplaceGrpSigShadowBuf(
                                                    Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                    uint32                  idxSigValue_u32,
                                                    uint8                   sigType_u8,
                                                    Com_MainFunc_tuo        idRxMainFunc_uo
                                                            );
#endif /* end of COM_RX_SIGNALGROUP */


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
 Function name    : Com_Prv_GetRxSigReplaceValueIndex
 Description      : Inline Function is get init_value-index or substitute_value-index
 Parameter        : rxSigConstPtr_pcst - pointer to RxSignal configuration
                  : replaceAction_u8   - COM_REPLACE or COM_SUBSTITUTE
 Return value     : init_value-index or substitute_value-index is returned
 **********************************************************************************************************************
*/
LOCAL_INLINE uint32 Com_Prv_GetRxSigReplaceValueIndex(Com_RxSigCfg_tpcst rxSigConstPtr_pcst, uint8 replaceAction_u8)
{
    (void)replaceAction_u8;

#ifdef COM_PRV_RX_SIG_SUBSTITUTE
    if (replaceAction_u8 == COM_SUBSTITUTE)
    {
        return rxSigConstPtr_pcst->idxSubstValue_uo;
    }
    else
#endif /* end of COM_PRV_RX_SIG_SUBSTITUTE */
    {
        return rxSigConstPtr_pcst->idxInitValue_uo;
    }
}

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxReplaceSignal
 Description      : Service to replace rx signal buffer
 Parameter        : rxSigConstPtr_pcst -> rx signal const structure
                    idRxMainFunc_uo -> rx mainFunction ID
                    replaceAction_u8 -> COM_REPLACE/COM_SUBSTITUTE
 Return value     : return signal init or substitution value for replace/substitute action respectively
 **********************************************************************************************************************
*/
Com_SigMax_tuo Com_Prv_RxReplaceSignal(
                                Com_RxSigCfg_tpcst  rxSigConstPtr_pcst,
                                Com_MainFunc_tuo    idRxMainFunc_uo,
                                uint8               replaceAction_u8
                                      )
{
    Com_SigMax_tuo      rxSigVal_uo;
    uint32              idxSigValue_u32;
    uint8               sigType_u8;

    rxSigVal_uo = 0;

    idxSigValue_u32 = Com_Prv_GetRxSigReplaceValueIndex(rxSigConstPtr_pcst, replaceAction_u8);

    sigType_u8 = Com_GetValue(GEN,_TYPE,rxSigConstPtr_pcst->general_u8);

    switch (sigType_u8)
    {
        case COM_UINT8_N:
        {
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below mem-copy is
            * optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY(
                    &COM_GET_SIGTYPEUINT8_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo),
                    COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigByteArray_pau8,
                    rxSigConstPtr_pcst->bitSize_uo
                       );

        }
        break;
#ifdef COM_RX_DYNAMIC_SIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
            PduLengthType dynLength_uo;
            dynLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigLength_uo;

            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void". As below mem-copy is
            * optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY(
                    &COM_GET_SIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo),
                    COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigByteArray_pau8, dynLength_uo
                        );

            COM_GET_RX_IPDU_CONSTDATA(rxSigConstPtr_pcst->idComIPdu_uo)->ipduRam_pst->dynSigLength_u16 =
                                                                                                 (uint16)dynLength_uo;
        }
        break;
#endif /* end of COM_RX_DYNAMIC_SIGNAL_SUPPORT */
#ifdef COM_RXSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            /* For float64 signals, idxSigValue_u32 holds index to float array holding init values. */
            COM_GET_SIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo) =
                                                     Com_getfloat64SigValue(idxSigValue_u32);
        }
        break;
#endif /* end of COM_RXSIG_FLOAT64SUPP */
#ifdef COM_RXSIG_INT64
        case COM_UINT64:
        case COM_SINT64:
        {
            rxSigVal_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(idxSigValue_u32));
            COM_GET_SIGTYPEUINT64_BUFF(idRxMainFunc_uo,rxSigConstPtr_pcst->idxSigBuff_uo) = rxSigVal_uo;
        }
        break;
#endif /* end of COM_RXSIG_INT64 */
        default:
        {
            rxSigVal_uo = COM_GET_INT32_SIG_VALUE(idxSigValue_u32);

            Com_Prv_UpdateRxSignalBuffer(rxSigConstPtr_pcst, rxSigVal_uo, idRxMainFunc_uo);
        }
        break;
    }

    return rxSigVal_uo;
}

#ifdef COM_RX_SIGNALGROUP

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_GetRxGrpSigReplaceValueIndex
 Description      : Inline Function is get init_value-index or substitute_value-index
 Parameter        : rxGrpSigConstPtr_pcst - pointer to RxGroupSignal configuration
                  : replaceAction_u8   - COM_REPLACE or COM_SUBSTITUTE
 Return value     : init_value-index or substitute_value-index is returned
 **********************************************************************************************************************
*/
LOCAL_INLINE uint32 Com_Prv_GetRxGrpSigReplaceValueIndex(
                                                Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                uint8                   replaceAction_u8
                                                        )
{
    (void)replaceAction_u8;

# ifdef COM_PRV_RX_SIGGRP_SUBSTITUTE
    if (replaceAction_u8 == COM_SUBSTITUTE)
    {
        return rxGrpSigConstPtr_pcst->idxSubstValue_uo;
    }
    else
# endif /* end of COM_PRV_RX_SIGGRP_SUBSTITUTE */
    {
        return rxGrpSigConstPtr_pcst->idxInitValue_uo;
    }
}


# ifdef COM_RX_SIGNALGROUP_ARRAY
/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxReplaceSigGrpArrayAccessBuf
 Description      : replace rx-signal-group array-access buffer
 Parameter        : rxGrpSigConstPtr_pcst - pointer to RxGroupSignal configuration
                  : idxSigValue_u32 - index to group-signal initial or substitution value
                  : sigType_u8 - signal type
                  : sigGrpBuffer_pu8 - pointer to signal-group array-access buffer
                  : sigGrpBitOffset_uo - signal-group bit-position
 Return value     : group-signal initial or substitution value
 **********************************************************************************************************************
*/
LOCAL_INLINE Com_SigMax_tuo Com_Prv_RxReplaceSigGrpArrayAccessBuf(
                                                    Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                    uint32                  idxSigValue_u32,
                                                    uint8                   sigType_u8,
                                                    uint8 *                 sigGrpBuffer_pu8,
                                                    Com_Bitposition_tuo     sigGrpBitOffset_uo
                                                                 )
{
    Com_SigMax_tuo sigValue_uo;
    Com_Bitposition_tuo relativeSigBitPos_uo; /* Relative BitPosition in RxSignalGroup Array */
    uint8 sigEndianess_u8;

    sigValue_uo = 0u; /* default value */

    /* The Buffer into which Signal Values needs to be packed will be available with starting address
    * of the Rx-SignalGroup Array Buffer.
    * But we have GroupSignal: Bit_Pos, i.e., Signal's actual LSB in the IPdu Buffer.
    * So, Rx-SignalGroup BitOffset(sigGrpBitOffset_uo) is removed from each of the GroupSignal and
    * is used as BitPosition to pack its value into the Rx-SignalGroup Array Buffer */
    relativeSigBitPos_uo = rxGrpSigConstPtr_pcst->bitPos_uo - ( Com_Bitposition_tuo )sigGrpBitOffset_uo;

    sigEndianess_u8 = Com_GetValue(RXGRPSIG,_ENDIANESS,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

    switch (sigType_u8)
    {
        /* COM_UINT8_DYN is not handled here, as it cannot be configured in Rx-ComSignalGroup-with-ArrayAccess */

        case COM_UINT8_N:
        {
            PduLengthType byteOffset_uo;

            /* Find the starting byte position of group-signal in signal-group buffer */
            byteOffset_uo = ( PduLengthType )( relativeSigBitPos_uo >> 3 );

            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void".
            * As below mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY(
                    (sigGrpBuffer_pu8 + byteOffset_uo),
                    COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigByteArray_pau8,
                    rxGrpSigConstPtr_pcst->bitSize_uo
                        );

        }
        break;
#  ifdef COM_RXGRPSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            Com_Prv_PackFloatSignal( sigEndianess_u8, relativeSigBitPos_uo,
                                        Com_getfloat64SigValue(idxSigValue_u32), sigGrpBuffer_pu8 );
        }
        break;
#  endif /* end of COM_RXGRPSIG_FLOAT64SUPP */
        default:
        {
#  ifdef COM_RXGRPSIG_INT64
            if (COM_SIG_IS_INT64_TYPE(sigType_u8))
            {
                sigValue_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(idxSigValue_u32));
            }
            else
#  endif /* end of COM_RXSIG_INT64 */
            {
                sigValue_uo = COM_GET_INT32_SIG_VALUE(idxSigValue_u32);
            }

            Com_Prv_PackSignal( sigEndianess_u8, relativeSigBitPos_uo, rxGrpSigConstPtr_pcst->bitSize_uo,
                                sigValue_uo, sigGrpBuffer_pu8 );
        }
        break;
    } /* end of switch() */

    return sigValue_uo;
}
# endif /* end of COM_RX_SIGNALGROUP_ARRAY */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxReplaceGrpSigShadowBuf
 Description      : replace rx-group-signal shadow buffer
 Parameter        : rxGrpSigConstPtr_pcst - pointer to RxGroupSignal configuration
                  : idxSigValue_u32 - index to group-signal initial or substitution value
                  : sigType_u8 - signal type
                  : idRxMainFunc_uo - associated rx-main-function ID
 Return value     : group-signal initial or substitution value
 **********************************************************************************************************************
*/
LOCAL_INLINE Com_SigMax_tuo Com_Prv_RxReplaceGrpSigShadowBuf(
                                                    Com_RxGrpSigCfg_tpcst   rxGrpSigConstPtr_pcst,
                                                    uint32                  idxSigValue_u32,
                                                    uint8                   sigType_u8,
                                                    Com_MainFunc_tuo        idRxMainFunc_uo
                                                            )
{
    Com_SigMax_tuo sigValue_uo;

    sigValue_uo = 0u; /* default value */

    switch (sigType_u8)
    {
        case COM_UINT8_N:
        {
            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void".
            * As below mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY(
                &COM_GET_SECRXGRPSIGTYPEU8_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo),
                COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigByteArray_pau8,
                rxGrpSigConstPtr_pcst->bitSize_uo
                        );

        }
        break;
# ifdef COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT
        case COM_UINT8_DYN:
        {
            Com_RxSigGrpCfg_tpcst   rxSigGrpConstPtr_pcst;
            PduLengthType           dynLength_uo;

            rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(rxGrpSigConstPtr_pcst->idSigGrp_uo);
            dynLength_uo = COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigLength_uo;

            /* MR12 DIR 1.1 VIOLATION: "Cast from a pointer to object type to a pointer to void".
            * As below mem-copy is optimized operation to copy data, so, type cast warning is ignored here */
            COM_MEMCOPY(
                &COM_GET_SECRXGRPSIGTYPEUINT8DYN_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo),
                COM_GET_BYTE_ARRAY_SIG_VAL(idxSigValue_u32).sigByteArray_pau8, dynLength_uo
                        );

            COM_GET_RX_IPDU_CONSTDATA(rxSigGrpConstPtr_pcst->idComIPdu_uo)->ipduRam_pst->dynSigLength_u16 =
                                                                                            (uint16)dynLength_uo;
        }
        break;
# endif /* end of COM_RX_DYNAMIC_GROUPSIGNAL_SUPPORT */
# ifdef COM_RXGRPSIG_FLOAT64SUPP
        case COM_FLOAT64:
        {
            COM_GET_SECRXGRPSIGTYPEFLOAT64_BUFF(idRxMainFunc_uo,rxGrpSigConstPtr_pcst->idxRxGrpSigBuff_uo) =
                                                                    Com_getfloat64SigValue(idxSigValue_u32);
        }
        break;
# endif /* end of COM_RXGRPSIG_FLOAT64SUPP */
        default:
        {
# ifdef COM_RXGRPSIG_INT64
            if (COM_SIG_IS_INT64_TYPE(sigType_u8))
            {
                sigValue_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(idxSigValue_u32));
            }
            else
# endif /* end of COM_RXGRPSIG_INT64 */
            {
                sigValue_uo = COM_GET_INT32_SIG_VALUE(idxSigValue_u32);
            }

            Com_Prv_UpdateRxGroupSignalSecondBuffer(rxGrpSigConstPtr_pcst, sigValue_uo, idRxMainFunc_uo);
        }
        break;
    }
    return sigValue_uo;
}


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxReplaceSigGrp
 Description      : If the SigGrp is enabled with ArrayAccess, RxSigGrpBuffer will be packed with
                    GroupSignal Init Values OR else
                    GroupSignals Shadow buffers will be replaced with its Init Values
 Parameter        : rxSigGrpConstPtr_pcst   - pointer to RxSignalGroup configuration
                  : idRxMainFunc_uo         - Internally generated RxMainFunction-ID
                    replaceAction_u8        - COM_REPLACE/COM_SUBSTITUTE
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_RxReplaceSigGrp(
                    Com_RxSigGrpCfg_tpcst   rxSigGrpConstPtr_pcst,
                    Com_MainFunc_tuo        idRxMainFunc_uo,
                    uint8                   replaceAction_u8
                            )
{
    Com_RxGrpSigCfg_tpcst               rxGrpSigConstPtr_pcst;
# ifdef COM_RX_SIGNALGROUP_ARRAY
    Com_RxSigGrpArrayCfg_tpcst          rxSigGrpArrayConstPtr_pcst;
    uint8 *                             sigGrpBuffer_pu8;
    Com_Bitposition_tuo                 sigGrpBitOffset_uo;
# endif
    uint16_least                        idxRxGrpSig_qu16;
    Com_SigMax_tuo                      sigValue_uo;
    uint32                              idxSigValue_u32;
    uint8                               sigType_u8;
# ifdef COM_RX_SIGNALGROUP_ARRAY
    boolean                             isSigGrpArrayAccess_b;

    isSigGrpArrayAccess_b = Com_GetValue(RXSIGGRP,_ARRAYACCESS,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8);

    if ( isSigGrpArrayAccess_b )
    {
        rxSigGrpArrayConstPtr_pcst  = COM_GET_RXSIGGRP_ARRAY_CONSTDATA( rxSigGrpConstPtr_pcst->idxSigGrpArray_uo );

        sigGrpBuffer_pu8 =
                &COM_GET_RXSIGGRP_ARRAY_BUFF(idRxMainFunc_uo, rxSigGrpArrayConstPtr_pcst->idxRxSigGrpBuf_uo);

        sigGrpBitOffset_uo        = (((Com_Bitposition_tuo)rxSigGrpArrayConstPtr_pcst->firstByteNo_uo) * 8u );

        Com_Get_SchM_Enter_Com_RxSigGrpArrayBuff
    }
    else
# endif /* end of COM_RX_SIGNALGROUP_ARRAY */
    {
        Com_Get_SchM_Enter_Com_RxGrpSigSecBuff
    }

    rxGrpSigConstPtr_pcst = COM_GET_RXGRPSIG_CONSTDATA( rxSigGrpConstPtr_pcst->idFirstGrpSig_uo );

    for ( idxRxGrpSig_qu16 = rxSigGrpConstPtr_pcst->numOfGrpSig_uo; idxRxGrpSig_qu16 > COM_ZERO; idxRxGrpSig_qu16-- )
    {
        sigType_u8 = Com_GetValue(RXGRPSIG,_TYPE,rxGrpSigConstPtr_pcst->rxGrpSigFields_u8);

        idxSigValue_u32 = Com_Prv_GetRxGrpSigReplaceValueIndex(rxGrpSigConstPtr_pcst, replaceAction_u8);

# ifdef COM_RX_SIGNALGROUP_ARRAY
        if ( isSigGrpArrayAccess_b )
        {
            sigValue_uo = Com_Prv_RxReplaceSigGrpArrayAccessBuf(
                                                            rxGrpSigConstPtr_pcst,
                                                            idxSigValue_u32,
                                                            sigType_u8,
                                                            sigGrpBuffer_pu8,
                                                            sigGrpBitOffset_uo
                                                               );
        }
        else
# endif /* end of COM_RX_SIGNALGROUP_ARRAY */
        {
            sigValue_uo = Com_Prv_RxReplaceGrpSigShadowBuf(
                                                    rxGrpSigConstPtr_pcst,
                                                    idxSigValue_u32,
                                                    sigType_u8,
                                                    idRxMainFunc_uo
                                                           );
        }

# if (defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD))
        /* TRACE[SWS_Com_00603] The AUTOSAR COM module shall set the old_value of the filtering mechanisms
        * for each signal to the ComSignalInitValue during start-up */
        if ( ( COM_INIT != Com_InitStatus_en ) &&
             ( COM_MASKED_NEW_DIFFERS_MASKED_OLD == rxGrpSigConstPtr_pcst->filterAlgo_u8 ) )
        {
            COM_GET_F_OLDVALUE(rxGrpSigConstPtr_pcst->idxFilter_uo) = (uint32)sigValue_uo;
        }
# endif /* # if defined(COM_RxFilters) && defined(COM_F_MASKEDNEWDIFFERSOLD) */
        (void)sigValue_uo; /* avoid misra, when it is set and unused */

        rxGrpSigConstPtr_pcst++;
    }

# ifdef COM_RX_SIGNALGROUP_ARRAY

    if ( isSigGrpArrayAccess_b )
    {
        Com_Get_SchM_Exit_Com_RxSigGrpArrayBuff
    }
    else
# endif /* end of COM_RX_SIGNALGROUP_ARRAY */
    {
        Com_Get_SchM_Exit_Com_RxGrpSigSecBuff
    }
}

#endif /* end of COM_RX_SIGNALGROUP */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

