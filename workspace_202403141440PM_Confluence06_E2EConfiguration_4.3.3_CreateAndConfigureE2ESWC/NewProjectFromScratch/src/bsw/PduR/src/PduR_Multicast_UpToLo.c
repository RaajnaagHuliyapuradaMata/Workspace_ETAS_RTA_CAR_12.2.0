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

#include "PduR_Prv.h"
#include "PduR_Mc.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"
/**
 **************************************************************************************************
 * PduR_MF_UpToLo
 *      Support function to handle multicast from upper layers to lower layers
 *
 * \param           PduR_MT_UpToLo upToLoTable: Multicast routing table
 *                  PduIdType mcId            :ID of I-PDU to be transmitted.
 *                  PduInfoType ** ptr:Pointer to pointer to PduInfoStructure containing SDU data pointer and
 *                                             SDU length of a transmit buffer.
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

Std_ReturnType PduR_MF_UpToLo(const PduR_MT_UpToLo * upToLoTable, PduIdType mcId, const PduInfoType * info)
{
    /* Start: Variable declaration */
    PduR_GT_TpPtr      gt;
    /* Fetch the fixed Buffer details */
    const PduR_FIFO_Tp_fixed * fifo;
    /* Fetch the RAM structure details  */
    PduR_TpSessDynPtr  SessionPtr;
    PduR_TpConnDynPtr ConnectionPtr;
    PduInfoType pduInfo_uo;

    /*Start :  Variable Initialization*/
    const PduR_MT_UpToLo * tablePtr = upToLoTable;
    Std_ReturnType rtn = E_NOT_OK, result = E_NOT_OK;

#if(PDUR_MULTICAST_TO_IF_SUPPORT == 1)
    boolean flg = FALSE;
#endif
    if(info != NULL_PTR)
    {
        pduInfo_uo.SduDataPtr = info->SduDataPtr;
        pduInfo_uo.MetaDataPtr = info->MetaDataPtr;
    }
    /* check for tablePtr */
    if(upToLoTable != NULL_PTR)
    {
        tablePtr += mcId;
        while (tablePtr->upId == mcId)
        {
        #if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)

            /* Below check is to perform routing if it is configured and enabled. */
            if ((tablePtr->routingCntrl_Idx == PDUR_RPGID_NULL)   ||  /* Routing control is not required.*/
                  ((PDUR_RPG_ENROUTING_INFO(tablePtr->routingCntrl_Idx)) != FALSE ))   /* Routing is enabled or not */
            #endif
            {
                if(info != NULL_PTR)
                {
                    /*[SWS_PduR_00783], If Gateway Routing Path is non buffer and received length is greater than
                     * configured destpdulength then forward configured destPdu length else forward received length */
                    if((tablePtr->gwType_u8 == PduR_GwNonBuffer) && (info->SduLength > tablePtr->destPduLength_uo))
                    {
                        pduInfo_uo.SduLength = tablePtr->destPduLength_uo;
                    }
                    else
                    {
                        pduInfo_uo.SduLength = info->SduLength;
                    }
                    #if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
                    rtn = PDUR_BASE_RPTBL(loTxTablePtr)[tablePtr->upToLo.loTransmitID].PduR_loTransmitFunc
                           (tablePtr->upToLo.loId, &pduInfo_uo);
                    #else
                    rtn = PduR_loTransmitTable[tablePtr->upToLo.loTransmitID].PduR_loTransmitFunc
                           (tablePtr->upToLo.loId, &pduInfo_uo);
                    #endif
                }
                else
                {
                    rtn = PduR_loTransmitTable[tablePtr->upToLo.loTransmitID].PduR_loTransmitFunc
                           (tablePtr->upToLo.loId, info);
                }
                if (rtn == E_OK)
                {
                #if(PDUR_MULTICAST_TO_IF_SUPPORT == 1)

                    flg = TRUE;
                #endif
                    /* Below code is to support Mc Tp Gateway feature.
                     * Incrementing of Tx_E_OK_Count is done only for TpModule and on successful Transmission to
                     * lower layer module. Decrementing of the same is done on receiving TpTxConf.
                     * The second condition in "If" is useful when Mc Gw Tp tx is used involving upper layer as one
                     * of the destination. If dest is upper layer then valid macro is generated.
                     * Ex : <UpperLayerModName>_PDURBSWMODNAME in mcGwToLo table for Tp Upperlayer
                     */
                    if((tablePtr->isTpModule) && (tablePtr->upSrcLayerName == PDUR_NULL_BSWMODNAME))
                    {
                        gt = PDUR_GW_TP_BASE;
                        while(gt->loId != mcId)
                        {
                            gt++;
                        }
                        /* Initialize only in case of Tp
                         * Start: Variable Initialization */
                        fifo = (gt->GW_Tp_buffer);
                        ConnectionPtr = fifo->connectionTable;
                        /* Working on primary session by default*/
                        SessionPtr = ConnectionPtr->primarySession;
                        /* End: Variable Initialization */
                        SessionPtr->Tx_E_OK_Count++;
                    }

                    result = E_OK;
                }

            }
            tablePtr++;  /* Iterate for next table entry. */
        }  /* while loop ends */
        #if(PDUR_MULTICAST_TO_IF_SUPPORT == 1)
        if(flg == TRUE)
        {
            tablePtr--;  /* Iterate for previous table entry. */
            PduR_MC_SetTxConfirmationFlag(tablePtr->upLayerSymName ,tablePtr->upSrcLayerName);
        }
        #endif
    }
    return result;
}

/* ------------------------------------------------------------------------ */
/* End section for code */


/* ------------------------------------------------------------------------ */
/* Begin section for code */


/**
 **************************************************************************************************
 * PduR_MF_CancelTransmit_UpToLo
 *      Support function to handle Cancel Transmission request for multicast routes
 *
 * \param           PduR_MT_UpToLo upToLoTable: Multicast routing table
 *                  PduIdType mcId            :ID of I-PDU to be transmitted.
 *
 * \retval          E_OK, E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

Std_ReturnType PduR_MF_CancelTransmit_UpToLo(const PduR_MT_UpToLo * upToLoTable,
                                                 PduIdType mcId)
{
    const PduR_MT_UpToLo * tablePtr = upToLoTable;
    Std_ReturnType rtn = E_NOT_OK;
    uint8 cnt_Not_Ok = 0;

    /* check for tablePtr */
    if(upToLoTable != NULL_PTR)
    {
        /*Table ptr is incremented till indexes to the expected Mc_GwToLo table entry*/
        while(tablePtr->upId < mcId)
        {
            tablePtr++;
        }
        while (tablePtr->upId == mcId)
        {
        #if defined(PDUR_MODE_DEPENDENT_ROUTING) && (PDUR_MODE_DEPENDENT_ROUTING != 0)

        /* Below check is to perform routing if it is configured and enabled. */
            if ((tablePtr->routingCntrl_Idx == PDUR_RPGID_NULL)   ||  /* Routing control is not required.*/
                    ((PDUR_RPG_ENROUTING_INFO(tablePtr->routingCntrl_Idx)) != FALSE )) /* Routing is enabled or not */
        #endif
            {
            #if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
                rtn = PDUR_BASE_RPTBL(loCnclTxTablePtr)[tablePtr->upToLo.loCancelTransmitID].
                            PduR_loCancelTransmitFunc(tablePtr->upToLo.loId);
            #else
                rtn = PduR_loCancelTransmitTable[tablePtr->upToLo.loCancelTransmitID].
                            PduR_loCancelTransmitFunc(tablePtr->upToLo.loId);
            #endif
                if (rtn != E_OK)
                {
                    cnt_Not_Ok++;
                }
            }
            tablePtr++;  /* Iterate for next table entry. */
        }  /* while loop ends */

        if (cnt_Not_Ok > 0)
        {
            rtn = E_NOT_OK;
        }
    }
    return rtn;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

