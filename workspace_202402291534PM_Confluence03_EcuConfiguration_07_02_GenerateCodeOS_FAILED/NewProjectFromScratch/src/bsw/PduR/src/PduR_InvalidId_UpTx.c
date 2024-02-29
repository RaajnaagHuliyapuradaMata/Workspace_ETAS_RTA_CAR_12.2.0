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
 * PduR_invId_UpCancelTransmit
 *      Functions to be invoked when an invalid ID is encountered.
 *
 * \param           PduIdType id: ID of DCM I-PDU to be cancelled the transmission.
 *
 *
 * \retval          E_NOT_OK
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: AUTOSAR specifies that pointer should be a P2VAR as it can be changed during run time*/
Std_ReturnType PduR_invId_UpCancelTransmit(PduIdType id)
{
    /* to remove warning */
    (void)id;
    /* nothing - DET not enabled */
    return E_NOT_OK;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

