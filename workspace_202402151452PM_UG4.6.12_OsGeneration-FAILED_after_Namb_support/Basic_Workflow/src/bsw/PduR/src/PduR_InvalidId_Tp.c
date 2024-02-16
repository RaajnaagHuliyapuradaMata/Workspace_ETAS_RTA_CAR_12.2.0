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
/* Functions to be invoked when an invalid ID is encountered.
 */

#include "PduR_Prv.h"
/* Include appropriate header files to declare the
 * prototypes with which we must comply
 */
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_invId_UpCancelReceive
 *      Functions to be invoked when an invalid ID is encountered.
 *
 * \param           PduIdType id: ID of UpTp I-PDU to be cancelled the reception.
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: As per AUTOSAR SWS specification signature is as made as P2VAR.
 * So this warning can be ignored. */
Std_ReturnType PduR_invId_UpCancelReceive(PduIdType id)
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

