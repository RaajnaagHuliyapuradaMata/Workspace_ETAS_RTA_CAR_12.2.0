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


#if defined(PDUR_J1939RM_SUPPORT) && (PDUR_J1939RM_SUPPORT != 0)
#include "PduR_J1939Rm.h"
#include "PduR_J1939Rm_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_null_J1939RmTxConfirmation
 *        Null functions - to discard any unnecessary notification
 *
 * \param           PduIdType gwayId  :ID of multiplexed I-PDU that has been transmitted.
 *
 * \retval          None
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
void PduR_null_J1939RmTxConfirmation( PduIdType id , Std_ReturnType result)
{
    /* Nothing */
    (void)id;  /* To remove warning */
    (void)result;

}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_J1939RM_SUPPORT */

#if defined(PDUR_SD_SUPPORT) && (PDUR_SD_SUPPORT != 0)
#include "PduR_Sd.h"
#include "PduR_Sd_Up.h"
/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_null_SdTxConfirmation
 *        Null functions - to discard any unnecessary notification
 *
 * \param           PduIdType gwayId  :ID of multiplexed I-PDU that has been transmitted.
 *
 * \retval          None
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */

/*The parameter 'Std_ReturnType result' is used only when Autosar version is higher than AR42*/
void PduR_null_SdTxConfirmation( PduIdType id , Std_ReturnType result)
{
    /* Nothing */
    (void)id;  /* To remove warning */
    (void)result;
}

/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_SD_SUPPORT */
