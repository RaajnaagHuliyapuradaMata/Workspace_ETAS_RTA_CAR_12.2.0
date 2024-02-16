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


#ifdef COM_TX_SIGNALGROUP
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
 Function name    : Com_Prv_UpdateShadowSignal
 Description      : Service updates a group signal with the data, referenced by SignalDataPtr
                    The update of the group signal data is done in the shadow buffer
 Parameter        : idSignal_u16        -> Id of the GroupSignal
                  : signalDataPtr_pcv   -> Pointer to the address where application data is available
 Return value     : None
 **********************************************************************************************************************
*/
void Com_Prv_UpdateShadowSignal(Com_SignalIdType idSignal_u16, const void * signalDataPtr_pcv)
{
#ifdef COM_EffectiveSigGrpTOC
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
#endif
    Com_TxGrpSigCfg_tpcst           txGrpSigConstPtr_pcst;
    Com_SigMax_tuo                  txNewVal_uo;             /* Local variable to store the new value */
    uint16                          constByteValue_u16;
    uint16                          type_u16;                 /* Type of the GroupSignal */
    Com_MainFunc_tuo                idTxMainFunc_uo;

    txGrpSigConstPtr_pcst   = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);

    idTxMainFunc_uo = (COM_GET_TX_IPDU_CONSTDATA((COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo))->
                                                                                        idComIPdu_uo))->idMainFunc_uo;

    constByteValue_u16  = txGrpSigConstPtr_pcst->txGrpSigFields_u8;
    type_u16             = Com_GetValue(TXGRPSIG,_TYPE,constByteValue_u16);
    txNewVal_uo         = COM_ZERO;
#ifdef COM_EffectiveSigGrpTOC
    txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo);
#endif

    /* Even if the IPDU Group is stopped this API will update the data in the Internal Buffer */
    if (type_u16 != COM_UINT8_N)
    {
        switch (type_u16)
        {
            /* MR12 RULE 11.5 VIOLATION: signalDataPtr_pcv is defined as void pointer in AUTOSAR specification,
            * so suppressing warning "Cast from a pointer to void to a pointer to object". */
            case COM_UINT8:
            {
                /* Update the Src_Buf with the data i.e typecast the data to uint8 */
                txNewVal_uo = *(const uint8 * )signalDataPtr_pcv;
            }
            break;
            case COM_SINT8:
            {
                /* Update the Src_Buf with the data i.e typecast the data to sint8 */
                txNewVal_uo = (uint32)(*(const sint8 * )signalDataPtr_pcv);
            }
            break;
            case COM_UINT16:
            {
                /* Update the Src_Buf with the data i.e typecast the data to uint16 */
                txNewVal_uo = *(const uint16 * )signalDataPtr_pcv;
            }
            break;
            case COM_SINT16:
            {
                /* Update the Src_Buf with the data i.e typecast the data to sint16 */
                txNewVal_uo = (uint32)(*(const sint16 * )signalDataPtr_pcv);
            }
            break;
            case COM_BOOLEAN:
            {
                /* Update the Src_Buf with the data i.e typecast the data to the boolean variable */
                txNewVal_uo = COM_BOOL_TO_UNSIGNED(*(const boolean * )signalDataPtr_pcv);
            }
            break;
            case COM_UINT32:
            {
                /* Update the Src_Buf with the data i.e typecast the data to the uint32 variable */
                txNewVal_uo = *(const uint32 * )signalDataPtr_pcv;
            }
            break;
            case COM_SINT32:
            {
                /* Update the Src_Buf with the data i.e typecast the data to the sint32 variable */
                txNewVal_uo = (uint32)(*(const sint32 * )signalDataPtr_pcv);
            }
            break;
#ifdef COM_TXGRPSIG_INT64
            case COM_UINT64:
            {
                /* Update the Src_Buf with the data i.e typecast the data to the uint64 variable */
                txNewVal_uo = (Com_SigMax_tuo)(*(const uint64 * )signalDataPtr_pcv);
            }
            break;
            case COM_SINT64:
            {
                /* Update the Src_Buf with the data i.e typecast the data to the sint64 variable */
                txNewVal_uo = (Com_SigMax_tuo)(*(const sint64 * )signalDataPtr_pcv);
            }
            break;
#endif /* #ifdef COM_TXGRPSIG_INT64 */

            /* FC_VariationPoint_START */
#ifdef COM_GRPSIGFLOAT32SUPP
            case COM_FLOAT32:
            {
                uint32 txNewVal_u32;

                txNewVal_u32 = COM_ZERO;

                /* Convert the float value into 32 bit stream and then pack the value into shadow buffer */
                /* MR12 DIR 1.1 VIOLATION: Explicit cast is provided in line with rba_BswSrv_MemCopy() definition.
                * But the void pointer is always deferenced to a type, based on the pointers alignment.
                * Hence the alignment warning can safely be ignored. */
                (void)rba_BswSrv_MemCopy( (void *)(&txNewVal_u32),(const void *)signalDataPtr_pcv,
                                                                                            COM_F32_SIZE_IN_BYTES );

                txNewVal_uo = txNewVal_u32;
            }
            break;
#endif /* #ifdef COM_GRPSIGFLOAT32SUPP */

#ifdef COM_TXGRPSIG_FLOAT64SUPP
            case COM_FLOAT64:
            {
                /* Pack the float value into float64 shadow buffer */
                /* MR12 RULE 11.5 VIOLATION: signalDataPtr_pcv is defined as void pointer in AUTOSAR specification,
                * so suppressing warning "Cast from a pointer to void to a pointer to object". */
                SchM_Enter_Com_TxGrpSigBuffer();
                COM_GET_TXGRPSIGTYPEFLOAT64_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo) =
                        *(const float64 * )signalDataPtr_pcv;
                SchM_Exit_Com_TxGrpSigBuffer();
            }
            break;
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */
            /* FC_VariationPoint_END */

            default:
            {
                /**
                * Default case is mandatory in switch syntax. Moreover only AUTOSAR Com defined signal types are
                * allowed to be configured, hence default case is intentionally left empty.
                */
            }
            break;
        }

        /* Call a generic internal api which copies the TxNewVal data into corresponding GroupSignal buffer */
        Com_Prv_CopyTxGroupSignalData(idSignal_u16, txNewVal_uo, idTxMainFunc_uo);

#ifdef COM_EffectiveSigGrpTOC
        /* Check if the group signal contributes to evaluation of transfer property of signal group */
        if ((Com_GetValue(TXGRPSIG,_IS_TOC,constByteValue_u16))  &&
           (COM_GET_OLDVALTOC(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxOldValue_uo) !=
                                                                   (Com_OldValTOC_tuo)txNewVal_uo))
        {
            /* Update the old value */
            COM_GET_OLDVALTOC(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxOldValue_uo) = (Com_OldValTOC_tuo)txNewVal_uo;
            Com_SetRamValue(TXSIGGRP,_TRIGONCHNG,txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_TRUE);
        }
#endif
    }
    else
    {
        /* Copy the *signalDataPtr_pcv to all the Bytes of this signal in Shadow buffer */
        SchM_Enter_Com_TxGrpSigBuffer();

#ifdef COM_EffectiveSigGrpTOC
        if (Com_GetValue(TXGRPSIG,_IS_TOC,constByteValue_u16))
        {
            /* MR12 DIR 1.1 VIOLATION: Explicit cast is provided in line with COM_MEMCOMPARE definition.
            * But the void pointer is always deferenced to a type, based on the pointers alignment.
            * Hence the alignment warning can safely be ignored. */
            if ((COM_MEMCOMPARE(
                    &COM_GET_TXGRPSIGTYPEU8_BUFF(idTxMainFunc_uo,txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo),
                    signalDataPtr_pcv, (uint32)txGrpSigConstPtr_pcst->bitSize_uo) != 0x00))
            {

                /* MR12 RULE 11.5 VIOLATION: signalDataPtr_pcv is defined as void pointer in AUTOSAR specification,
                * so suppressing warning "Cast from a pointer to void to a pointer to object". */
                Com_ByteCopy(&COM_GET_TXGRPSIGTYPEU8_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo),
                             (const uint8 * )signalDataPtr_pcv,(uint32)txGrpSigConstPtr_pcst->bitSize_uo);

                Com_SetRamValue(TXSIGGRP,_TRIGONCHNG,txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,
                                                                                                            COM_TRUE);
            }
            else
            {
                /* do nothing */
            }
        }
        else
#endif
        {
            /* Copy the Signal when the transfer property is "triggered" or "pending" */
            /* MR12 RULE 11.5 VIOLATION: signalDataPtr_pcv is defined as void pointer in AUTOSAR specification,
            * so suppressing warning "Cast from a pointer to void to a pointer to object". */
            Com_ByteCopy(&COM_GET_TXGRPSIGTYPEU8_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo),
                         (const uint8 * )signalDataPtr_pcv,(uint32)txGrpSigConstPtr_pcst->bitSize_uo);

        }

        SchM_Exit_Com_TxGrpSigBuffer();
    }
}

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifdef COM_TX_SIGNALGROUP */

