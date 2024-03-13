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
/** Handle TxConfirmation from a lower interface layer which uses
  * data direct transmission and which is not configured with a DD-FIFO.
  */

#include "PduR_Prv.h"
#include "PduR_Gw.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

void PduR_GF_DDIf_NoBuf_TxConfirmation_Func(PduIdType gwayId,Std_ReturnType result)
{
    /* We do not need this confirmation: simply ignore it */
    (void)gwayId;
    (void)result;

}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

