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
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_FrIfTriggerTransmit
 *            Functions to be invoked when an invalid ID is encountered
 *
 *
 * \param          PduIdType id:ID of L-PDU that is requested to be transmitted.
 *                 uint8 *  ptr:Pointer to place inside the transmit buffer of the L-PDU where data shall be copied to.
 *
 *
 * \retval          None
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: The api is deprecated hence no functional impact.
   The api will be removed in upcoming release.*/
Std_ReturnType PduR_invId_IfTriggerTransmit( PduIdType id, PduInfoType * data )
{
    /* nothing - DET not enabled */
    /* to remove warning */
    /* data = data; Introduced for MISRA sake. Will be removed by Optimiser anyway */
    (void)id;
    (void)data;
    return E_NOT_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
 #include "PduR_MemMap.h"
