/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2016, all rights reserved
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


/* Com_SendSignalGroupArray() API shall only be available when the configuration switch
 *  ComEnableSignalGroupArrayApi is enabled */
#ifdef COM_TX_SIGNALGROUP_ARRAY

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

# define COM_START_SEC_CODE
# include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 * Function name    : Com_SendSignalGroupArray
 * Description      : The service Com_SendSignalGroupArray copies the content of the provided SignalGroupArrayPtr to
 *                    the associated I-PDU. The provided data shall correspond to the array representation of
 *                    the signal group.
 * Parameters    IN : idSignalGroup_u16        - Id of signal group to be sent
 *                    signalGroupArrayPtr_pcu8 - Reference to the signal group array
 * Sync/Async       : Asynchronous
 * Reentrancy       : Non Reentrant for the same signal group. Reentrant for different signal groups.
 * Return value     : E_OK                      : service has been accepted
 *                    COM_SERVICE_NOT_AVAILABLE : corresponding I-PDU group was stopped
 *                                                (or service failed due to development error)
 *                    COM_BUSY                  : in case the TP-Buffer is locked for large data types handling
 **********************************************************************************************************************
*/
uint8 Com_SendSignalGroupArray(Com_SignalGroupIdType idSignalGroup_u16, const uint8 * signalGroupArrayPtr_pcu8)
{
    Com_TxIpduCfg_tpcst                 txIpduConstPtr_pcst;
# ifdef COM_TX_TP_IPDUTYPE
    Com_TxIpduRam_tpst                  txIpduRamPtr_pst;
# endif
    Com_TxSigGrpCfg_tpcst               txSigGrpConstPtr_pcst;
    Com_TxSigGrpArrayCfg_tpcst          txSigGrpArrayConstPtr_pcst;
    uint8                               constxSignalGrpFields_u8;
# ifdef COM_EffectiveSigGrpTOC
    uint8                               transferProperty_u8;
# endif
    uint8                               returnValue_u8;

    returnValue_u8 = COM_SERVICE_NOT_AVAILABLE;

# if (COM_PRV_ERROR_HANDLING == STD_ON)
    if (Com_InitStatus_en == COM_UNINIT)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArray, COM_E_UNINIT);
    }
    else if (signalGroupArrayPtr_pcu8 == NULL_PTR)
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArray, COM_E_PARAM_POINTER);
    }
    else if (!Com_Prv_IsValidTxSignalGroupId(idSignalGroup_u16))
    {
        COM_DET_REPORT_ERROR(COMServiceId_SendSignalGroupArray, COM_E_PARAM);
    }
    else
# endif /* end of COM_PRV_ERROR_HANDLING */
    {
        /* If PB variant is selected, then PduId which is passed to this function will be changed
         * to internal Id which is generated through configuration
         * If PC variant is selected, then no mapping table will be used. */
        idSignalGroup_u16           = COM_GET_TXSIGNALGRP_ID( idSignalGroup_u16 );
        txSigGrpConstPtr_pcst       = COM_GET_TXSIGGRP_CONSTDATA( idSignalGroup_u16 );
        constxSignalGrpFields_u8    = txSigGrpConstPtr_pcst->txSignalGrpFields_u8;

        /* Proceed only if Tx-SignalGroup is enabled with Array Access */
        if ( Com_GetValue(TXSIGGRP,_ARRAYACCESS,constxSignalGrpFields_u8) )
        {
            Com_IpduId_tuo idIpdu_uo;
            idIpdu_uo    = txSigGrpConstPtr_pcst->idComIPdu_uo;

            txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA( idIpdu_uo );
# ifdef COM_TX_TP_IPDUTYPE
            txIpduRamPtr_pst = txIpduConstPtr_pcst->ipduRam_pst;
# endif

            /* Proceed only if Large Data tranmission is not in Progress */
# ifdef COM_TX_TP_IPDUTYPE
            if ( Com_GetRamValue(TXIPDU,_LARGEDATAINPROG, txIpduRamPtr_pst->txFlags_u16) )
            {
                returnValue_u8 = COM_BUSY;
            }
            else
# endif /*#ifdef COM_TX_TP_IPDUTYPE*/
            {
                txSigGrpArrayConstPtr_pcst =
                                        COM_GET_TXSIGGRP_ARRAY_CONSTDATA( txSigGrpConstPtr_pcst->idxSigGrpArray_uo );

# ifdef COM_EffectiveSigGrpTOC
                transferProperty_u8 = Com_GetValue(TXSIGGRP,_TP,constxSignalGrpFields_u8);

                if (COM_SIG_TP_IS_TRIGGERED_ON_CHANGE(transferProperty_u8))
                {
                    /* Unpack & check, if any of the GroupSignal data has changed whose transfer property is
                     * configured with TRIGGERED_ON_CHANGE */
                    Com_Prv_InternalSigGrpArrayTrigOnChange(idSignalGroup_u16, signalGroupArrayPtr_pcu8);
                }
                else
                {
                    /* Do-nothing */
                }
# endif /* #  ifdef COM_EffectiveSigGrpTOC */

                SchM_Enter_Com_TxIpduProtArea(SENDSIGNALGRP);

                /* The service Com_SendSignalGroupArray() shall copy the content designated by the
                * signalGroupArrayPtr_pcu8 to the associated I-PDU Buffer */
                Com_ByteCopy( ( uint8 * )( txIpduConstPtr_pcst->buffPtr_pu8 +
                                                                        txSigGrpArrayConstPtr_pcst->firstByteNo_uo ),
                              ( const uint8 * )( signalGroupArrayPtr_pcu8 ),
                              ( uint32  )( txSigGrpArrayConstPtr_pcst->length_uo ) );

                SchM_Exit_Com_TxIpduProtArea(SENDSIGNALGRP);

                returnValue_u8 = Com_Prv_InternalSendSignalGroup( idSignalGroup_u16 );
            }
        }
    }
   return returnValue_u8;
}


/*
 **********************************************************************************************************************
 * Function name : Com_Prv_InternalSigGrpArrayTrigOnChange
 * Description   : Check if GroupSignal data has changed from the last update
 * Parameters    : (IN) idSignalGroup_u16 - Tx SignalGroup Id
 *                      signalGroupArrayPtr_pcu8 - Reference to the signal group array
 * Return Value  : None
 **********************************************************************************************************************
*/
# ifdef COM_EffectiveSigGrpTOC

void Com_Prv_InternalSigGrpArrayTrigOnChange(
                                             Com_SignalGroupIdType idSignalGroup_u16,
                                             const uint8 * signalGroupArrayPtr_pcu8
                                            )
{
    Com_TxIpduCfg_tpcst             txIpduConstPtr_pcst;
    Com_TxSigGrpCfg_tpcst           txSigGrpConstPtr_pcst;
    Com_TxGrpSigCfg_tpcst           txGrpSigConstPtr_pcst;
    Com_TxSigGrpArrayCfg_tpcst      txSigGrpArrayConstPtr_pcst;
    Com_SigMax_tuo                  txNewValGrpSig_uo;
    uint16_least                    idxTxGrpSig_qu16;
    Com_Bitposition_tuo             sigGrpBitOffset_uo;
    uint16                          constByteValue_u16;
    Com_MainFunc_tuo                idTxMainFunc_uo;
    uint8                           grpSigType_u8;

    txSigGrpConstPtr_pcst   = COM_GET_TXSIGGRP_CONSTDATA(idSignalGroup_u16);
    txSigGrpArrayConstPtr_pcst = COM_GET_TXSIGGRP_ARRAY_CONSTDATA( txSigGrpConstPtr_pcst->idxSigGrpArray_uo );

    txIpduConstPtr_pcst     = COM_GET_TX_IPDU_CONSTDATA(txSigGrpConstPtr_pcst->idComIPdu_uo);

    txGrpSigConstPtr_pcst   = COM_GET_TXGRPSIG_CONSTDATA(txSigGrpConstPtr_pcst->idFirstGrpSig_uo);

    sigGrpBitOffset_uo      = ((Com_Bitposition_tuo)(txSigGrpArrayConstPtr_pcst->firstByteNo_uo) * 8u);

    idTxMainFunc_uo = txIpduConstPtr_pcst->idMainFunc_uo;

    for ( idxTxGrpSig_qu16  = txSigGrpConstPtr_pcst->numOfGrpSig_uo; idxTxGrpSig_qu16 > COM_ZERO; idxTxGrpSig_qu16-- )
    {
        constByteValue_u16  = txGrpSigConstPtr_pcst->txGrpSigFields_u8;

        grpSigType_u8       = (uint8)Com_GetValue(TXGRPSIG,_TYPE,constByteValue_u16);

        /* Check if the group signal contributes to evaluation of transfer property of signal group */
        if ( Com_GetValue(TXGRPSIG,_IS_TOC,constByteValue_u16) )
        {
            /* Locks are not required here to protect TxIpdu buffer, as this function is called only from
             * Com_SendSignalGroupArray, which is non-reentrant for the same signalGroup. Hence GroupSignal
             * data remains unchanged while processing this internal function.
             * Unpack GroupSignal data from SignalGroup with ArrayAccess */

            /* COM_UINT8_DYN type of signal is not explicitly checked here as it can never be a part of
             * signalgroup enabled with array access */
            if (grpSigType_u8 == COM_UINT8_N)
            {
                PduLengthType startByteInIpduBuf_uo;
                PduLengthType startByteInApplBuf_uo;

                startByteInIpduBuf_uo = txGrpSigConstPtr_pcst->bitPos_uo / 8u;
                startByteInApplBuf_uo = txSigGrpArrayConstPtr_pcst->firstByteNo_uo - startByteInIpduBuf_uo;
                /* MR12 DIR 1.1 VIOLATION: Explicit cast is provided in line with rba_BswSrv_MemCopy() definition.
                 * But the void pointer is always deferenced to a type based on the pointers address alignment.
                 * Hence the alignment warning can safely be ignored. */
                if (COM_MEMCOMPARE( (txIpduConstPtr_pcst->buffPtr_pu8 + startByteInIpduBuf_uo),
                                    (signalGroupArrayPtr_pcu8 + startByteInApplBuf_uo),
                                    txGrpSigConstPtr_pcst->bitSize_uo) != 0x00)
                {
                    /* Set the global TRIGGERED_ON_CHANGE flag if GroupSignal data has changed */
                    Com_SetRamValue(TXSIGGRP,_TRIGONCHNG,
                                                txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_TRUE);
                }
            }
            else
            {
                Com_Bitposition_tuo     relativeSigBitPos_uo;

                /* The Buffer into which Signal Values needs to be packed will be available with starting address
                 * of the Tx-SignalGroup Array Buffer.
                 * But we have GroupSignal: Bit_Pos, i.e., Signal's actual LSB in the IPdu Buffer.
                 * So, Tx-SignalGroup BitOffset(sigGrpBitOffset_uo) is removed from each of the GroupSignal and
                 * is used as BitPosition to pack its value into the Tx-SignalGroup Array Buffer */
                relativeSigBitPos_uo = txGrpSigConstPtr_pcst->bitPos_uo - sigGrpBitOffset_uo;

                txNewValGrpSig_uo = Com_Prv_UnpackSignal( Com_GetValue(TXGRPSIG,_ENDIANESS,constByteValue_u16),
                                                          relativeSigBitPos_uo,
                                                          txGrpSigConstPtr_pcst->bitSize_uo,
                                                          signalGroupArrayPtr_pcu8,
                                                          grpSigType_u8);

                /* Check if GroupSignal data has changed */
                if (COM_GET_OLDVALTOC(idTxMainFunc_uo,(txGrpSigConstPtr_pcst->idxOldValue_uo))!=
                        (Com_OldValTOC_tuo)txNewValGrpSig_uo )
                {
                    /* Update the old value with the latest data */
                    COM_GET_OLDVALTOC(idTxMainFunc_uo,(txGrpSigConstPtr_pcst->idxOldValue_uo)) =
                            (Com_OldValTOC_tuo)txNewValGrpSig_uo;
                    /* Set the global TRIGGERED_ON_CHANGE flag if GroupSignal data has changed */
                    Com_SetRamValue(TXSIGGRP,_TRIGONCHNG,
                                                txSigGrpConstPtr_pcst->sigGrpRAM_pst->txSigGrpRAMFields_u8,COM_TRUE);
                }
                else
                {
                    /* If GroupSignal data is unchanged, then it is not stored into Old_Value Buffer */
                }
            }
        }
        else
        {
            /* GroupSignal configured with TRIGGERED_ON_CHANGE TP is not supported for COM_FLOAT64 Type */
        }
        txGrpSigConstPtr_pcst++;
    }
}


# endif /* # ifdef COM_EffectiveSigGrpTOC */

# define COM_STOP_SEC_CODE
# include "Com_MemMap.h"

#endif /* #ifdef COM_TX_SIGNALGROUP_ARRAY */

