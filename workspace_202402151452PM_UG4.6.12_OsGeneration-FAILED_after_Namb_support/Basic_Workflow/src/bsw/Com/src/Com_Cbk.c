/*
 **********************************************************************************************************************
 *
 * COPYRIGHT (c) ETAS GmbH 2023, all rights reserved
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
#if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE)

#define COM_START_SEC_CODE
#include "Com_MemMap.h"

/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIPduAckCallbacks
 Description      : calls individual signal, signal-group or IPDU notification in case of post-build
 Parameter        : idIpdu_uo -> Id of the Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
# ifdef COM_TxIPduNotification
void Com_Prv_TxIPduAckCallbacks(Com_IpduId_tuo idIpdu_uo)
{
    Com_TxIpduCfg_tpcst txIpduConstPtr_pcst;
    const Com_Prv_xTxIpduVarCfg_tst * txIpduVarCfg_pcst;
    uint16_least index_qu16;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    txIpduVarCfg_pcst = COM_GET_TX_IPDU_VAR_CFG(idIpdu_uo);

    if (txIpduConstPtr_pcst->notification_pfct != NULL_PTR)
    {
        txIpduConstPtr_pcst->notification_pfct();
    }

    if (Com_GetValue(TXIPDUVAR,_IS_SIG_ACKCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigCfg_tpcst txSigConstPtr_pcst;
#  ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigVarCfg_tst * txSigVarCfg_pcst;
#  endif

        txSigConstPtr_pcst = COM_GET_TXSIG_CONSTDATA(txIpduConstPtr_pcst->idTxSig_uo);
#  ifdef COM_CALLBACK_TX
        txSigVarCfg_pcst = COM_GET_TXSIG_VAR_CFG(txIpduConstPtr_pcst->idTxSig_uo);
#  endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSig_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigConstPtr_pcst->notification_pfct != NULL_PTR)
            {
                txSigConstPtr_pcst->notification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigVarCfg_pcst->idxAckCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigVarCfg_pcst->idxAckCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigConstPtr_pcst->idHandle_u16);
            }
            txSigVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigConstPtr_pcst++;
        }
    }

#  ifdef COM_TX_SIGNALGROUP
    if (Com_GetValue(TXIPDUVAR,_IS_SIGGRP_ACKCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigGrpCfg_tpcst txSigGrpConstPtr_pcst;
#   ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigGrpVarCfg_tst * txSigGrpVarCfg_pcst;
#   endif

        txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   ifdef COM_CALLBACK_TX
        txSigGrpVarCfg_pcst = COM_GET_TXSIGGRP_VAR_CFG(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSigGroups_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigGrpConstPtr_pcst->notification_pfct != NULL_PTR)
            {
                txSigGrpConstPtr_pcst->notification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigGrpVarCfg_pcst->idxAckCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigGrpVarCfg_pcst->idxAckCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigGrpConstPtr_pcst->idHandle_u16);
            }
            txSigGrpVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigGrpConstPtr_pcst++;
        }
    }
#  endif /* end of COM_TX_SIGNALGROUP */

}
# endif /* end of COM_TxIPduNotification */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIPduErrCallbacks
 Description      : calls individual signal, signal-group error notification in case of post-build
 Parameter        : idIpdu_uo -> Id of the Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
# ifdef COM_ERRORNOTIFICATION
void Com_Prv_TxIPduErrCallbacks(Com_IpduId_tuo idIpdu_uo)
{
    Com_TxIpduCfg_tpcst txIpduConstPtr_pcst;
    const Com_Prv_xTxIpduVarCfg_tst * txIpduVarCfg_pcst;
    uint16_least index_qu16;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    txIpduVarCfg_pcst = COM_GET_TX_IPDU_VAR_CFG(idIpdu_uo);

    if (Com_GetValue(TXIPDUVAR,_IS_SIG_ERRCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigCfg_tpcst txSigConstPtr_pcst;
#  ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigVarCfg_tst * txSigVarCfg_pcst;
#  endif

        txSigConstPtr_pcst = COM_GET_TXSIG_CONSTDATA(txIpduConstPtr_pcst->idTxSig_uo);
#  ifdef COM_CALLBACK_TX
        txSigVarCfg_pcst = COM_GET_TXSIG_VAR_CFG(txIpduConstPtr_pcst->idTxSig_uo);
#  endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSig_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigConstPtr_pcst->errorNotification_pfct != NULL_PTR)
            {
                txSigConstPtr_pcst->errorNotification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigVarCfg_pcst->idxErrCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigVarCfg_pcst->idxErrCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigConstPtr_pcst->idHandle_u16);
            }
            txSigVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigConstPtr_pcst++;
        }
    }

#  ifdef COM_TX_SIGNALGROUP
    if (Com_GetValue(TXIPDUVAR,_IS_SIGGRP_ERRCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigGrpCfg_tpcst txSigGrpConstPtr_pcst;
#   ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigGrpVarCfg_tst * txSigGrpVarCfg_pcst;
#   endif

        txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   ifdef COM_CALLBACK_TX
        txSigGrpVarCfg_pcst = COM_GET_TXSIGGRP_VAR_CFG(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSigGroups_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigGrpConstPtr_pcst->errorNotification_pfct != NULL_PTR)
            {
                txSigGrpConstPtr_pcst->errorNotification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigGrpVarCfg_pcst->idxErrCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigGrpVarCfg_pcst->idxErrCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigGrpConstPtr_pcst->idHandle_u16);
            }
            txSigGrpVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigGrpConstPtr_pcst++;
        }
    }
#  endif /* end of COM_TX_SIGNALGROUP */

}
# endif /* end of COM_ERRORNOTIFICATION */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_TxIPduToutCallbacks
 Description      : calls individual signal, signal-group timeout notification in case of post-build
 Parameter        : idIpdu_uo -> Id of the Tx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
# ifdef COM_TxIPduTimeOutNotify
void Com_Prv_TxIPduToutCallbacks(Com_IpduId_tuo idIpdu_uo)
{
    Com_TxIpduCfg_tpcst txIpduConstPtr_pcst;
    const Com_Prv_xTxIpduVarCfg_tst * txIpduVarCfg_pcst;
    uint16_least index_qu16;

    txIpduConstPtr_pcst = COM_GET_TX_IPDU_CONSTDATA(idIpdu_uo);
    txIpduVarCfg_pcst = COM_GET_TX_IPDU_VAR_CFG(idIpdu_uo);

    if (txIpduConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
    {
        txIpduConstPtr_pcst->timeoutNotification_pfct();
    }

    if (Com_GetValue(TXIPDUVAR,_IS_SIG_TOUTCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigCfg_tpcst txSigConstPtr_pcst;
#  ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigVarCfg_tst * txSigVarCfg_pcst;
#  endif

        txSigConstPtr_pcst = COM_GET_TXSIG_CONSTDATA(txIpduConstPtr_pcst->idTxSig_uo);
#  ifdef COM_CALLBACK_TX
        txSigVarCfg_pcst = COM_GET_TXSIG_VAR_CFG(txIpduConstPtr_pcst->idTxSig_uo);
#  endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSig_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
            {
                txSigConstPtr_pcst->timeoutNotification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigVarCfg_pcst->idxToutCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigVarCfg_pcst->idxToutCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigConstPtr_pcst->idHandle_u16);
            }
            txSigVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigConstPtr_pcst++;
        }
    }

#  ifdef COM_TX_SIGNALGROUP
    if (Com_GetValue(TXIPDUVAR,_IS_SIGGRP_TOUTCBK,txIpduVarCfg_pcst->fields_u8))
    {
        Com_TxSigGrpCfg_tpcst txSigGrpConstPtr_pcst;
#   ifdef COM_CALLBACK_TX
        const Com_Prv_xTxSigGrpVarCfg_tst * txSigGrpVarCfg_pcst;
#   endif

        txSigGrpConstPtr_pcst = COM_GET_TXSIGGRP_CONSTDATA(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   ifdef COM_CALLBACK_TX
        txSigGrpVarCfg_pcst = COM_GET_TXSIGGRP_VAR_CFG(txIpduConstPtr_pcst->idFirstTxSigGrp_uo);
#   endif
        for (index_qu16 = txIpduConstPtr_pcst->numOfSigGroups_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (txSigGrpConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
            {
                txSigGrpConstPtr_pcst->timeoutNotification_pfct();
            }
#  ifdef COM_CALLBACK_TX
            else if (txSigGrpVarCfg_pcst->idxToutCbk_uo != COM_PRV_CBK_INDEX_NULL_PTR)
            {
                Com_Prv_UserCbk_acst[txSigGrpVarCfg_pcst->idxToutCbk_uo].
                userCbk_pfct((CbkHandleIdType)txSigGrpConstPtr_pcst->idHandle_u16);
            }
            txSigGrpVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_TX */
            txSigGrpConstPtr_pcst++;
        }
    }
#  endif /* end of COM_TX_SIGNALGROUP */

}
# endif /* end of COM_TxIPduTimeOutNotify */


/*
 **********************************************************************************************************************
 Function name    : Com_Prv_RxIPduToutCallbacks
 Description      : calls individual signal, signal-group timeout notification in case of post-build
 Parameter        : idIpdu_uo -> Id of the Rx Ipdu
 Return value     : None
 **********************************************************************************************************************
*/
# ifdef COM_RxIPduTimeoutNotify
void Com_Prv_RxIPduToutCallbacks(Com_IpduId_tuo idIpdu_uo)
{
    Com_RxIpduCfg_tpcst rxIpduConstPtr_pcst;
    const Com_Prv_xRxIpduVarCfg_tst * rxIpduVarCfg_pcst;
    uint16_least index_qu16;

    rxIpduConstPtr_pcst = COM_GET_RX_IPDU_CONSTDATA(idIpdu_uo);
    rxIpduVarCfg_pcst = COM_GET_RX_IPDU_VAR_CFG(idIpdu_uo);

    if (rxIpduConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
    {
        rxIpduConstPtr_pcst->timeoutNotification_pfct();
    }

    if (Com_GetValue(RXIPDUVAR,_IS_SIG_TOUTCBK,rxIpduVarCfg_pcst->fields_u8))
    {
        Com_RxSigCfg_tpcst rxSigConstPtr_pcst;
#  ifdef COM_CALLBACK_RX
        const Com_Prv_xRxSigVarCfg_tst * rxSigVarCfg_pcst;
#  endif

        rxSigConstPtr_pcst = COM_GET_RXSIG_CONSTDATA(rxIpduConstPtr_pcst->idRxSig_uo);
#  ifdef COM_CALLBACK_RX
        rxSigVarCfg_pcst = COM_GET_RXSIG_VAR_CFG(rxIpduConstPtr_pcst->idRxSig_uo);
#  endif

        for (index_qu16 = rxIpduConstPtr_pcst->numOfSig_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (rxSigConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
            {
                rxSigConstPtr_pcst->timeoutNotification_pfct();
            }
#  ifdef COM_CALLBACK_RX
            else
            {
#   ifdef COM_RxSigUpdateBit
                if (Com_GetValue(GEN,_UPDBITCONF,rxSigConstPtr_pcst->general_u8) == COM_FALSE)
#   endif
                {
                    if (rxSigVarCfg_pcst->toutCbk_pcauo != NULL_PTR)
                    {
                        Com_Prv_InvokeRxUserCbks(rxSigVarCfg_pcst->toutCbk_pcauo, rxSigConstPtr_pcst->idHandle_u16);
                    }
                }
            }
            rxSigVarCfg_pcst++;
#  endif /* end of COM_CALLBACK_RX */
            rxSigConstPtr_pcst++;
        }
    }

#  ifdef COM_RX_SIGNALGROUP
    if (Com_GetValue(RXIPDUVAR,_IS_SIGGRP_TOUTCBK,rxIpduVarCfg_pcst->fields_u8))
    {
        Com_RxSigGrpCfg_tpcst rxSigGrpConstPtr_pcst;
#   ifdef COM_CALLBACK_RX
        const Com_Prv_xRxSigGrpVarCfg_tst * rxSigGrpVarCfg_pcst;
#   endif

        rxSigGrpConstPtr_pcst = COM_GET_RXSIGGRP_CONSTDATA(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);
#   ifdef COM_CALLBACK_RX
        rxSigGrpVarCfg_pcst = COM_GET_RXSIGGRP_VAR_CFG(rxIpduConstPtr_pcst->idFirstRxSigGrp_uo);
#   endif

        for (index_qu16 = rxIpduConstPtr_pcst->numOfSigGrp_u16; index_qu16 > COM_ZERO; --index_qu16)
        {
            if (rxSigGrpConstPtr_pcst->timeoutNotification_pfct != NULL_PTR)
            {
                rxSigGrpConstPtr_pcst->timeoutNotification_pfct();
            }
#   ifdef COM_CALLBACK_RX
            else
            {
#    ifdef COM_RxSigGrpUpdateBit
                if (Com_GetValue(RXSIGGRP,_UPDBITCONF,rxSigGrpConstPtr_pcst->rxSignalGrpFields_u8) == COM_FALSE)
#    endif
                {
                    if (rxSigGrpVarCfg_pcst->toutCbk_pcauo != NULL_PTR)
                    {
                        Com_Prv_InvokeRxUserCbks(rxSigGrpVarCfg_pcst->toutCbk_pcauo,
                                                    rxSigGrpConstPtr_pcst->idHandle_u16);
                    }
                }
            }
            rxSigGrpVarCfg_pcst++;
#   endif /* end of COM_CALLBACK_RX */

            rxSigGrpConstPtr_pcst++;
        }
    }
#  endif /* end of COM_RX_SIGNALGROUP */

}
# endif /* end of COM_RxIPduTimeoutNotify */

#define COM_STOP_SEC_CODE
#include "Com_MemMap.h"

#endif /* #if (COM_CONFIGURATION_VARIANT != COM_VARIANT_PRE_COMPILE) */

