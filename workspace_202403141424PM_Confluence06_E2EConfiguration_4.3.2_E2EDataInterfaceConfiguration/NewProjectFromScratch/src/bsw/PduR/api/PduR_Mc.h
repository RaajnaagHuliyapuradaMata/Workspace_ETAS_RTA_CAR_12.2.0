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
/* Multicast support functions */

#ifndef PDUR_MC_H
#define PDUR_MC_H
/**
\defgroup PDUR_MC_H    PDUR - Protocol Data Unit Router
*/
#include "PduR_Cfg_Internal.h"
#include "PduR_Types.h"
#if defined(PDUR_MULTICAST_TO_IF_SUPPORT) && (PDUR_MULTICAST_TO_IF_SUPPORT == 1)
#include "PduR_Multicast_TxConfirmation.h"
#endif
/* ------------------------------------------------------------------------ */
/**
 * @ingroup PDUR_MC_H
 *
 * Begin section for code
 */


#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/*begin */

/**
 * @ingroup PDUR_MC_H
 *
 *  This function is Support function to handle multicast from upper layers to lower layers \n
 *
 *  @param  In:      upToLoTable - This parameter represents Multicast routing table \n
 *  @param  In:      mcId - This parameter represents ID of I-PDU to be transmitted \n
 *  @param  Out:     info - This parameter represents a pointer to PduInfoStructure containing SDU data pointer and
 *                          SDU length of a transmit buffer. \n
 *
 *
 *  @return          E_OK: if the request is accepted\n
 *                   E_NOT_OK: if the request is not accepted  just for testing\n
 *
 */
extern Std_ReturnType PduR_MF_UpToLo( const PduR_MT_UpToLo * upToLoTable,
                                                         PduIdType mcId,
                                                         const PduInfoType * info);

/**
 * @ingroup PDUR_MC_H
 *
 *      Support function to handle Cancel Transmission request for multicast routes \n
 *
 * @param  In:      PduR_MT_UpToLo upToLoTable: Multicast routing table \n
 * @param  In:      PduIdType mcId            : ID of I-PDU to be transmitted \n
 *
 * @return          E_OK: if the request is accepted \n
 *                  E_NOT_OK: if the request is not accepted  just for testing \n
 *
 **************************************************************************************************
 */
extern Std_ReturnType PduR_MF_CancelTransmit_UpToLo(const PduR_MT_UpToLo * upToLoTable,
                                                 PduIdType mcId);
/**
 * @ingroup PDUR_MC_H
 *
 *  Provide the support functions for mapping TpTx callbacks to the upper layer \n
 *
 *  @param  In:      mcId - This parameter represents the multicast ID that has to be transmitted \n
 *  @param  In:      len - This parameter represents the length of the requested transmit buffer \n
 *  @param  Out:     ptr- This parameter represents the pointer to PduInfoStructure containing SDU data pointer and
 *                        SDU length of a transmit buffer. \n
 *
 *  @return         BUFREQ_OK- Buffer request accomplished successful \n
 *  @return         BUFREQ_E_BUSY - Currently no buffer of the requested size is available \n
 *  @return         BUFREQ_E_NOT_OK - Buffer request not successful, no buffer provided \n
 */
extern BufReq_ReturnType PduR_MF_McTpTx_CopyTxData( PduIdType mcId,
                                                                             const PduInfoType * info,
                                                                             const RetryInfoType * retry,
                                                                             PduLengthType * availableDataPtr );

/**
 * @ingroup PDUR_MC_H
 *
 *  Provide the support functions for mapping TpTx callbacks to the upper layer \n
 *
 *  @param  In:      mcId - This parameter represents the multicast ID that has to be transmitted \n
 *  @param  In:      layerId - This parameter represents the layer ID \n
 *  @param  In:      result- Result of the TP reception. \n
 *
 * @return None \n
 */
extern void PduR_MF_McTpTx_TxConfirmation( PduR_LayerType layerId,
                                                              PduIdType mcId,
                                                              Std_ReturnType result);

/**
 * @ingroup PDUR_MC_H
 *
 *  Provide support for multicast transmission via CanTp \n
 *
 *  @param  In:      mcId - multicast ID that has been transmitted \n
 *  @param  Out:     result- Result of the Tx confirmation. \n
 *
 *  @return None \n
 */
extern void PduR_MF_McTpTx_CanTp_TxConfirmation_Func(PduIdType mcId, Std_ReturnType result);

/**
 * @ingroup PDUR_MC_H
 *
 *  Provide support for multicast transmission via FrTp \n
 *
 *  @param  In:      mcId - multicast ID that has been transmitted \n
 *  @param  Out:     result- Result of the Tx confirmation. \n
 *
 *  @return None \n
 */
extern void PduR_MF_McTpTx_FrTp_TxConfirmation_Func( PduIdType mcId, Std_ReturnType result);

/**
 * @ingroup PDUR_MC_H
 *
 *  Provide support for multicast transmission via LinTp \n
 *
 *  @param  In:      mcId - multicast ID that has been transmitted \n
 *  @param  Out:     result- Result of the Tx confirmation. \n
 *
 *  @return None \n
 */
extern void PduR_MF_McTpTx_LinTp_TxConfirmation_Func(PduIdType mcId, Std_ReturnType result);

/**
 * @ingroup PDUR_MC_H
 *
 *  Provide support for multicast transmission via J1939Tp \n
 *
 *  @param  In:      mcId - multicast ID that has been transmitted \n
 *  @param  Out:     result- Result of the Tx confirmation. \n
 *
 *  @return None \n
 */
extern void PduR_MF_McTpTx_J1939Tp_TxConfirmation_Func( PduIdType mcId, Std_ReturnType result);

/**
 * @ingroup PDUR_MC_H
 *
 * CanTp Copy Tx Data
 */
#define PduR_MF_McTpTx_CanTp_CopyTxData_Func  PduR_MF_McTpTx_CopyTxData

/**
 * @ingroup PDUR_MC_H
 *
 * FrTp Copy Tx Data
 */
#define PduR_MF_McTpTx_FrTp_CopyTxData_Func   PduR_MF_McTpTx_CopyTxData

/**
 * @ingroup PDUR_MC_H
 *
 * LinTp Copy Tx Data
 */
#define PduR_MF_McTpTx_LinTp_CopyTxData_Func  PduR_MF_McTpTx_CopyTxData

/*end */

/* ------------------------------------------------------------------------ */
/**
 * @ingroup PDUR_MC_H
 *
 * End section for code
 */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_MC_H */

