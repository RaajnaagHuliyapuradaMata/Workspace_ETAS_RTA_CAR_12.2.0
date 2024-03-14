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

#ifdef COM_TxGrpSigInvalid

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_InternalInvalidateShadowSignal
 Description      : Service for Invalidating the shadow signal (i.e Group signal)
                    This is an internal function to invalidate a group signal
                    This function doesnt do DET checks, these checks are done in the API itself.
 Parameter        : idSignal_u16 -> Internal Id of the group signal. (Relevant for post-build case)
 Return value     : None
 **********************************************************************************************************************
*/
uint8 Com_Prv_InternalInvalidateShadowSignal(Com_SignalIdType idSignal_u16)
{
    Com_TxGrpSigCfg_tpcst       txGrpSigConstPtr_pcst;
    Com_SigMax_tuo              sigValue_uo;
    uint16                      constByteValue_u16;
    uint16                      type_u16;
    Com_MainFunc_tuo            idTxMainFunc_uo;
    uint8                       status_u8;

    /* [SWS_Com_00643] Com_InvalidateSignal shall return COM_SERVICE_NOT_AVAILABLE
    * in case no ComSignalDataInvalidValue is configured for the signal with the given SignalId. */
    status_u8 = COM_SERVICE_NOT_AVAILABLE;

    txGrpSigConstPtr_pcst = COM_GET_TXGRPSIG_CONSTDATA(idSignal_u16);
    constByteValue_u16    = txGrpSigConstPtr_pcst->txGrpSigFields_u8;

    /**
    * TRACE[SWS_Com_00099] By a call to Com_InvalidateSignal, the AUTOSAR COM module shall
    * perform internally a Com_SendSignal with the configured ComSignalDataInvalidValue
    * TRACE[note] If Com_SendSignal or Com_InvalidateSignal is called for a signal that belongs to a
    * signal group, then the AUTOSAR COM will only update the shadow buffer of this
    * signal group. There is no need for any further I-PDU processing like TMS evaluation,
    * unless the I-PDU contents changed.
    */
    if (Com_GetValue(TXGRPSIG,_INVACTION,constByteValue_u16))
    {
        Com_TxSigGrpCfg_tpcst txSigGrpConstPtr_pcst;
        status_u8  = E_OK;

        txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txGrpSigConstPtr_pcst->idSigGrp_uo);
        type_u16           = Com_GetValue(TXGRPSIG,_TYPE,constByteValue_u16);
        sigValue_uo = txGrpSigConstPtr_pcst->dataInvalidVal_u32;

        /* Fetch the Tx-MainFunction internal Id */
        idTxMainFunc_uo = (COM_GET_TX_IPDU_CONSTDATA(txSigGrpConstPtr_pcst->idComIPdu_uo))->idMainFunc_uo;

        switch (type_u16)
        {
            case COM_UINT8_N:
            {
                /* MR12 DIR 1.1 VIOLATION: Explicit cast has no impact, as it is de-referenced with
                * same type-qualifier */
                Com_Prv_UpdateShadowSignal(idSignal_u16,
                        (const void *)COM_GET_BYTE_ARRAY_SIG_VAL(sigValue_uo).sigByteArray_pau8);

            }
            break;
#ifdef COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT
            case COM_UINT8_DYN:
            {
                /* MR12 DIR 1.1 VIOLATION: Explicit cast has no impact, as it is de-referenced with
                * same type-qualifier */
                (void)Com_Prv_CopyDynGroupSignal(idSignal_u16,
                        (const void *)COM_GET_BYTE_ARRAY_SIG_VAL(sigValue_uo).sigByteArray_pau8,
                        (uint16)COM_GET_BYTE_ARRAY_SIG_VAL(sigValue_uo).sigLength_uo );

            }
            break;
#endif /* end of COM_TX_DYNAMIC_GROUPSIGNAL_SUPPORT */
#ifdef COM_TXGRPSIG_FLOAT64SUPP
            case COM_FLOAT64:
            {
                SchM_Enter_Com_TxGrpSigBuffer();

                COM_GET_TXGRPSIGTYPEFLOAT64_BUFF(idTxMainFunc_uo, txGrpSigConstPtr_pcst->idxTxGrpSigBuff_uo) =
                                                                                Com_getfloat64SigValue(sigValue_uo);

                SchM_Exit_Com_TxGrpSigBuffer();
            }
            break;
#endif /* #ifdef COM_TXGRPSIG_FLOAT64SUPP */
            default:
            {
#ifdef COM_TXGRPSIG_INT64
                if (COM_SIG_IS_INT64_TYPE(type_u16))
                {
                    sigValue_uo = (Com_SigMax_tuo)(COM_GET_INT64_SIG_VALUE(sigValue_uo));
                }
#endif /* end of COM_TXGRPSIG_INT64 */

                Com_Prv_CopyTxGroupSignalData(idSignal_u16, sigValue_uo, idTxMainFunc_uo);

#ifdef COM_EffectiveSigGrpTOC
                /* Check if the group signal contributes to evaluation of transfer property of signal group */
                if ((Com_GetValue(TXGRPSIG,_IS_TOC,constByteValue_u16))  &&
                    (COM_GET_OLDVALTOC(idTxMainFunc_uo,(txGrpSigConstPtr_pcst->idxOldValue_uo)) !=
                     (Com_OldValTOC_tuo)sigValue_uo))
                {

                    /* Update the old value */
                    COM_GET_OLDVALTOC(idTxMainFunc_uo,(txGrpSigConstPtr_pcst->idxOldValue_uo)) =
                                                                                (Com_OldValTOC_tuo)sigValue_uo;
                    Com_SetRamValue(TXSIGGRP,_TRIGONCHNG,
                                    txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_TRUE);
                }
#endif
            }
            break;
        }
    }
    return(status_u8);
}

#endif /* #ifdef COM_TxGrpSigInvalid */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #ifdef COM_TX_SIGNALGROUP */

