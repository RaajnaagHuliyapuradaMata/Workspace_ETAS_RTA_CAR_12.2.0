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
/* Support for PduR_Comxxx() can be configured out entirely. */ /**/
#include "PduR_Prv.h"
#if defined(PDUR_IFGATEWAY_SUPPORT) && (PDUR_IFGATEWAY_SUPPORT != STD_OFF)
#include "PduR_Gw.h"

#if defined(PDUR_COM_SUPPORT) && (PDUR_COM_SUPPORT != 0)
#include "PduR_Com.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_Com_Transmit - Support function for combined gateway and reception
 *
 * \param           PduIdType id: ID of AUTOSAR COM I-PDU to be transmitted.
 *                  PduR_TxPduInfoPtr info: A pointer to a structure with I-PDU related data that shall be transmitted
 *                                         data length and pointer to I-SDU buffer
 *
 * \retval          E_OK Transmit request has been accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_Com_Transmit_Func( PduIdType id, const PduInfoType * info)
{
    Com_RxIndication(id, info);
    return E_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_COM_SUPPORT */

/*
**********************************************************************************************************************
*
* COPYRIGHT RESERVED, ETAS GmbH, 2014. All rights reserved.
* The reproduction, distribution and utilization of this document as well as the communication of its contents to
* others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
**********************************************************************************************************************
*/


/* Support for PduR_Ipdumxxx() can be configured out entirely. */ /**/

/* Chek for IPDUM and Gateway support during pre-compile time*/
#if defined(PDUR_IPDUM_SUPPORT) && (PDUR_IPDUM_SUPPORT != 0)
#include "PduR_IpduM.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_IpdumTransmit - Support function for combined gateway and reception
 *
 * \param           PduIdType id: ID of IPDUM I-PDU to be transmitted.
 *                  const PduInfoType * info: A pointer to a structure with I-PDU related data
 *                                            that shall be transmitted:
 *                                            data length and pointer to I-SDU buffer
 *
 *
 * \retval          E_OK Transmit request has been accepted
 *
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_IpduM_Transmit_Func(PduIdType id, const PduInfoType * info)
{
    IpduM_RxIndication(id, info);
    return E_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_IPDUM_SUPPORT */

/* Support for PduR_SecOCxxx() can be configured out entirely. */

#if defined(PDUR_SECOC_SUPPORT) && (PDUR_SECOC_SUPPORT != 0)
#include "PduR_SecOC.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_SecOC_Transmit - Support function for combined gateway and reception
 *
 * \param           PduIdType id: ID of AUTOSAR SecOC I-PDU to be transmitted.
 *                  PduR_TxPduInfoPtr info: A pointer to a structure with I-PDU related data that shall be transmitted
 *                                         data length and pointer to I-SDU buffer
 *
 * \retval          E_OK Transmit request has been accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_SecOC_Transmit_Func( PduIdType id, const PduInfoType * info)
{
    SecOC_RxIndication(id, info);
    return E_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"


#endif /* PDUR_SECOC_SUPPORT */

#if defined(PDUR_LDCOM_SUPPORT) && (PDUR_LDCOM_SUPPORT != 0)
#include "PduR_LdCom.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_Com_Transmit - Support function for combined gateway and reception
 *
 * \param           PduIdType id: ID of AUTOSAR COM I-PDU to be transmitted.
 *                  PduR_TxPduInfoPtr info: A pointer to a structure with I-PDU related data that shall be transmitted
 *                                         data length and pointer to I-SDU buffer
 *
 * \retval          E_OK Transmit request has been accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_LdCom_Transmit_Func( PduIdType id, const PduInfoType * info)
{
    LdCom_RxIndication(id, info);
    return E_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_LDCOM_SUPPORT */

#if defined(PDUR_SD_SUPPORT) && (PDUR_SD_SUPPORT != 0)
#include "PduR_Sd.h"

/* ------------------------------------------------------------------------ */
/* Begin section for code */

#define PDUR_START_SEC_CODE
#include "PduR_MemMap.h"

/**
 **************************************************************************************************
 * PduR_GF_Sd_Transmit - Support function for combined gateway and reception
 *
 * \param           PduIdType id: ID of AUTOSAR SD I-PDU to be transmitted.
 *                  PduR_TxPduInfoPtr info: A pointer to a structure with I-PDU related data that shall be transmitted
 *                                         data length and pointer to I-SDU buffer
 *
 * \retval          E_OK Transmit request has been accepted
 *
 * \seealso
 * \usedresources
 **************************************************************************************************
 */
Std_ReturnType PduR_GF_Sd_Transmit_Func( PduIdType id, const PduInfoType * info)
{
    Sd_RxIndication(id, info);
    return E_OK;
}
/* ------------------------------------------------------------------------ */
/* End section for code */

#define PDUR_STOP_SEC_CODE
#include "PduR_MemMap.h"

#endif /* PDUR_SD_SUPPORT */

#endif /* PDUR_IFGATEWAY_SUPPORT */


