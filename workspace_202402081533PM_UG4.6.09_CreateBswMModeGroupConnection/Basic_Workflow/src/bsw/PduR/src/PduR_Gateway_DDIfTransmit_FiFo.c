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
/* Provides a FIFO for gatewaying to an IF layer which uses data direct transmission.  */

#include "PduR_Prv.h"

/* Check for Gatewaying support during pre-compile time */
#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
#include "PduR_Gw.h"
#include "Dem.h"           /* For Dem_SetEventStatus */
#if(PDUR_ENABLE_INTER_MODULE_CHECKS)
#if (!defined(DEM_AR_RELEASE_MAJOR_VERSION) || \
     (DEM_AR_RELEASE_MAJOR_VERSION != PDUR_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DEM_AR_RELEASE_MINOR_VERSION) || \
      (DEM_AR_RELEASE_MINOR_VERSION != PDUR_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched"
#endif
#endif /* #if(PDUR_ENABLE_INTER_MODULE_CHECKS) */

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/* The signature needs to be compatible with <Lo>_Transmit */

Std_ReturnType PduR_GF_DDIf_FIFO_Transmit_Func(PduIdType id, const PduInfoType * info)
{
    Std_ReturnType RetVal;
    PduInfoType infoBuf;
    const PduR_GT_If * gt = PDUR_GW_IF_BASE + id;
    /* MR12 RULE 11.5 VIOLATION : 'pointer-to-void' is being cast to a 'pointer-to-object' type and this
     * conversion will not result in any misalignment. Hence Supressed.*/
    PduR_FifoType        fifo   = (PduR_FifoType)(gt->GW_If_buffer);
    PduR_FifoDynIfPtr    dyn    = (PduR_FifoDynIfPtr)(fifo->dyn);


    uint16_least i;

    /*  MEDC17 Core Adaptation */
    /* Enter Critical Section */


    SchM_Enter_PduR_LockIFGatewayTx();

    /* At various points in this function we call <Lo>If_Transmit().
     * We do not need to keep the resource protecting the FIFO locked for the duration of the
     * call to <Lo>If_Transmit() since, for the following reasons, we can be sure that we will not
     * re-enter <Lo>If_Transmit() preemptively (for the same PDU ID) from another task:
     * - This function is not re-entrant for the same PDU ID, so it cannot cause a re-entrant call
     *   to <Lo>If_Tranmsit().
     * - The only other place where we could call <Lo>If_Transmit() is from PduR_GF_DDIf_FIFO_TxConfirmation_Func(),
     *   but this will only occur if the FIFO is non-empty, and, when we call <Lo>If_Transmit() from this function,
     *   we can be sure the FIFO *is* empty (and it cannot be re-filled, since this function is not re-entrant
     *   for the same PDU ID).
     * See Bugzilla 13110 for further discussion of these matters.
     */

     /**/

    if(info->SduLength > fifo->eltSize)
    {
        infoBuf.SduLength = fifo->eltSize;
    }
    else
    {
        infoBuf.SduLength = info->SduLength;
    }

    infoBuf.SduDataPtr = info->SduDataPtr;
    infoBuf.MetaDataPtr = info->MetaDataPtr;

    /* Check for Tx Confirmation pending*/
    if( 0 == dyn->txConfPending )
    {
        /* There is no TX confirmation pending, so forward the PDU. */
        #if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
        RetVal = PDUR_BASE_RPTBL(loTxTablePtr)[gt->transmitID].PduR_loTransmitFunc( gt->loId, &infoBuf );
        #else
        RetVal = PduR_loTransmitTable[gt->transmitID].PduR_loTransmitFunc( gt->loId, &infoBuf );
        #endif
        /* Retval gives the status about the transmitted data */
        if(RetVal == E_OK)
        {
            dyn->txConfPending = 1;
        }
        else
        {
            /* Error occured during transmission */
			#if defined(PDUR_DEM_PDU_INSTANCE_LOST) && (PDUR_DEM_PDU_INSTANCE_LOST != 0)
            /*BSW modules calling DemSetEventStatus can safely ignore the return value*/
            (void)Dem_SetEventStatus( PDUR_E_PDU_INSTANCE_LOST, DEM_EVENT_STATUS_FAILED );
            #if(PDUR_DSM_REINT_SUPPORT != STD_OFF)
            PduR_DsmReInitStruct.instanceLostErrorStatus =  DEM_EVENT_STATUS_FAILED;
            #endif
            #endif /* PDUR_DEM_PDU_INSTANCE_LOST */
        }
    }
    /* There is a TX confirmation pending. Is the FIFO not full yet? */
    else if( dyn->used != fifo->nrElts )
    {
        /* Update the Actual length in the Dyn*/
        dyn->actLen[dyn->writeIndex] = infoBuf.SduLength;
        /* Store our PDU in the FIFO. */
        /*  memcpy( dyn->writePtr, info->SduDataPtr, fifo->eltSize );        */
        /*  MEDC17 Core Adaptation, since we are not including string.h file */

        /* START: MEDC17 Core Adaptation, as memcpy( ) is present in string.h file which we are not including  */
        /* Copy the SduDataPtr into Temp */

        for(i = 0 ; i < infoBuf.SduLength ; i++)
        {
            *( (dyn->writePtr) + i) = *( (info->SduDataPtr) + i );

        }

        /* If metadataPtr is NULL_PTR and metadataTyperef is not configured, then metadata will not be copied */
        if((gt->metaDataIFInfo_pst != NULL_PTR) && (info->MetaDataPtr != NULL_PTR))
        {
            for(i = 0 ; i < gt->metaDataIFInfo_pst->metaDataBufLen ; i++)
            {
                *( (dyn->metaData) + i) = *( (info->MetaDataPtr) + i );
            }
        }

        /* END: MEDC17 Core Adaptation, as memcpy( ) is present in string.h file which we are not including  */
        /* FIFO Routines */
        PDUR_FIFO_WRITE_INC_WRAP( fifo, dyn );
        dyn->used++;
    }
    /* There is a TX confirmation pending, and the FIFO is full. */
    else
    {
        /* We assume that the TX confirmation which we are waiting for has got lost. Therefore
         * we flush the FIFO and force another transmission in an attempt to unblock things. */
        /* [SWS_PduR_00255] */

        PduR_InitializePduR_Gw_If_DynStruct(dyn,fifo);

        /* Report the loss of the FIFO contents to the DEM:.
        * See Bugzilla 12468 for reasons why we deviate from the spec by not checking the size of the
        * FIFO before calling the DEM. */
        #if defined(PDUR_DEM_PDU_INSTANCE_LOST) && (PDUR_DEM_PDU_INSTANCE_LOST != 0)
        /*BSW modules calling DemSetEventStatus can safely ignore the return value*/
        (void)Dem_SetEventStatus( PDUR_E_PDU_INSTANCE_LOST, DEM_EVENT_STATUS_FAILED );
		#if(PDUR_DSM_REINT_SUPPORT != STD_OFF)
        PduR_DsmReInitStruct.instanceLostErrorStatus =  DEM_EVENT_STATUS_FAILED;
        #endif
        #endif /* PDUR_DEM_PDU_INSTANCE_LOST */

        /* See note above about re-entrancy of this function call. */
        #if(PDUR_CONFIGURATION_VARIANT == PDUR_VARIANT_POSTBUILD_LOADABLE)
        (void)PDUR_BASE_RPTBL(loTxTablePtr)[gt->transmitID].PduR_loTransmitFunc( gt->loId, &infoBuf );
        #else
        (void)PduR_loTransmitTable[gt->transmitID].PduR_loTransmitFunc( gt->loId, &infoBuf );
        #endif
    }
    /* PDUR_RTA-BSW_end 258 */

    /* MEDC17 Core Adaptation */
    SchM_Exit_PduR_LockIFGatewayTx();
    /* Exit Critical Section */
    /* This return code will always be discarded by one of our callers. */
    return E_OK;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"
#endif /* PDUR_IFGATEWAY_SUPPORT */

