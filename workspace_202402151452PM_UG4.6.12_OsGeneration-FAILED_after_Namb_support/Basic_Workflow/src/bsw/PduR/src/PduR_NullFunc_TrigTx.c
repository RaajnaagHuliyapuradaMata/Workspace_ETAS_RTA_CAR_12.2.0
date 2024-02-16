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
 * PduR_NF_TriggerTransmit_Func
 *      Null function to handle unwanted PduR_xxxIfTriggerTransmit
 *
 * \param           PduIdType gwayId  :ID of N-PDU that shall be received
 *                  uint8*    data    : Data pointer for transmit buffer
 *
 * \retval          E_NOT_OK
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: The api is internally used by PduR when upper layer Trigger Transmit api is not configured
   hence there is no functional impact with the warning */
Std_ReturnType PduR_NF_TriggerTransmit_Func( PduIdType id,
                                                      PduInfoType * data)
{
    /* data = data; Introduced for MISRA sake. Will be removed by Optimiser */
    (void)id;
    (void)data;

    return E_NOT_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


